// c128-kasse test harness
//
// Launches x128, connects to its monitor, verifies the data files have the
// expected contents.
package main_test

import (
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"net"
	"os"
	"os/exec"
	"path/filepath"
	"regexp"
	"strings"
	"testing"
	"time"

	_ "net/http/pprof"
)

var (
	step         = 0
	labelsLoaded = false
	promptRe     = regexp.MustCompile(`\(C:\$[0-9a-f]{4}\) `)
)

func discardUntilPrompt(r io.Reader) error {
	log.Printf("discard")
	for {
		buf := make([]byte, 10)
		n, err := r.Read(buf)
		if err != nil {
			return err
		}
		buf = buf[:n]

		log.Printf("read: %q", string(buf))
		if promptRe.Match(buf) {
			log.Printf("prompt found")
			return nil // prompt found
		}
		log.Printf("discarding until newline")
		p := make([]byte, 1)
		for p[0] != '\n' {
			if _, err := r.Read(p); err != nil {
				return err
			}
		}
		log.Printf("newline found")
	}
}

func screenshot(conn io.ReadWriter) error {
	// Wait until the graphic buffer is updated from the frame buffer
	time.Sleep(500 * time.Millisecond)
	log.Printf("taking screenshot")
	step++
	if _, err := conn.Write([]byte("x\n")); err != nil {
		return err
	}
	time.Sleep(1000 * time.Millisecond)
	if _, err := conn.Write([]byte("\n")); err != nil {
		return err
	}

	if err := discardUntilPrompt(conn); err != nil {
		return err
	}
	if _, err := conn.Write([]byte(fmt.Sprintf("screenshot \"step%d.png\" 2\n", step))); err != nil {
		return err
	}
	return discardUntilPrompt(conn)
}

func inputStep(conn io.ReadWriter, cmd string, breaks int) error {
	log.Printf("step: %v", cmd)

	if _, err := conn.Write([]byte(cmd + "\n")); err != nil {
		return err
	}
	if err := discardUntilPrompt(conn); err != nil {
		return err
	}

	for i := 0; i < breaks; i++ {
		if _, err := conn.Write([]byte("x\n")); err != nil {
			return err
		}
		// C128 program runs here
		if err := discardUntilPrompt(conn); err != nil {
			return err
		}
	}

	return nil
}

func copyFile(dest, src string) error {
	o, err := os.Create(dest)
	if err != nil {
		return err
	}
	defer o.Close()

	i, err := os.Open(src)
	if err != nil {
		return err
	}
	defer i.Close()

	_, err = io.Copy(o, i)
	return err
}

func dial() (net.Conn, error) {
	const timeout = 10 * time.Second
	var err error
	start := time.Now()
	for time.Since(start) < timeout {
		var conn net.Conn
		conn, err = net.Dial("tcp", "localhost:6510")
		if err != nil {
			if oe, ok := err.(*net.OpError); ok {
				if oe.Op == "dial" {
					log.Printf("could not dial x128 remote monitor: %v (retrying)", err)
					time.Sleep(100 * time.Millisecond)
					continue
				}
			}
			return nil, err
		}
		return conn, err
	}
	return nil, fmt.Errorf("could not dial x128 remote monitor within %v: %v", timeout, err)
}

type loggingReadWriter struct {
	rw io.ReadWriter
}

func (l *loggingReadWriter) Read(p []byte) (n int, err error) {
	n, err = l.rw.Read(p)
	capped := p
	if err == nil {
		capped = capped[:n]
	}
	log.Printf("Read(%q) = %d, %v", string(capped), n, err)
	return n, err
}

func (l *loggingReadWriter) Write(p []byte) (n int, err error) {
	n, err = l.rw.Write(p)
	capped := p
	if err == nil {
		capped = capped[:n]
	}
	log.Printf("Write(%q) = %d, %v", string(capped), n, err)
	return n, err
}

func logic(script string) error {
	// Copy the kasse executable into a temporary directory. This is also where
	// the program writes its data files to.
	tmpdir, err := ioutil.TempDir("", "c128kasse")
	if err != nil {
		return err
	}
	defer os.RemoveAll(tmpdir)

	for _, fn := range []string{"kasse", "kasse.lbl", "vicerc", "testdata/items", "testdata/log-0", "testdata/credits"} {
		if err := copyFile(filepath.Join(tmpdir, filepath.Base(fn)), fn); err != nil {
			return err
		}
	}

	// TODO: headless mode via xvfb
	x128 := exec.Command("x128", "-config", "vicerc", "+go64", "-80col", "-remotemonitor", "-warp")
	x128.Dir = tmpdir
	x128.Stderr = os.Stderr
	if err := x128.Start(); err != nil {
		return err
	}

	tcpconn, err := dial()
	if err != nil {
		return err
	}
	defer tcpconn.Close()

	conn := &loggingReadWriter{rw: tcpconn}

	log.Printf("loading labels")

	if _, err := conn.Write([]byte("ll \"kasse.lbl\"\n")); err != nil {
		return err
	}
	// The first prompt appears delayed
	if err := discardUntilPrompt(conn); err != nil {
		return err
	}
	if err := discardUntilPrompt(conn); err != nil {
		return err
	}

	log.Printf("setting breakpoint")

	if _, err := conn.Write([]byte("bk ._cgetc\n")); err != nil {
		return err
	}
	if err := discardUntilPrompt(conn); err != nil {
		return err
	}

	b, err := ioutil.ReadFile("testdata/" + script)
	if err != nil {
		return err
	}
	lines := strings.Split(strings.TrimSpace(string(b)), "\n")
	for _, line := range lines {
		if strings.HasPrefix(line, "#") {
			continue // skip comments
		}
		breaks := 1
		if !strings.HasPrefix(line, `run"`) {
			breaks = len(line) + len("\n") + 1
		}
		if err := inputStep(conn, "keybuf "+line+"\\n", breaks); err != nil {
			return err
		}

		if err := screenshot(conn); err != nil {
			return err
		}
	}

	time.Sleep(1 * time.Second) // allow for the last screenshot to be flushed to disk

	if err := inputStep(conn, "quit", 1); err != nil {
		return err
	}

	log.Printf("DONE")

	// copy screenshots to testscreenshots/save.script/
	scrdir := filepath.Join("testscreenshots/" + script)
	if err := os.MkdirAll(scrdir, 0755); err != nil {
		return err
	}

	fis, err := ioutil.ReadDir(tmpdir)
	if err != nil {
		return err
	}
	for _, fi := range fis {
		if !strings.HasSuffix(fi.Name(), ".png") {
			continue
		}
		if err := copyFile(filepath.Join(scrdir, fi.Name()), filepath.Join(tmpdir, fi.Name())); err != nil {
			return err
		}
	}

	// TODO: validate output file
	return nil
}

func TestScript(t *testing.T) {
	//go http.ListenAndServe(":8038", nil)
	fis, err := ioutil.ReadDir("testdata")
	if err != nil {
		t.Fatal(err)
	}
	for _, fi := range fis {
		if !strings.HasSuffix(fi.Name(), ".script") {
			continue
		}
		t.Run(fi.Name(), func(t *testing.T) {
			if err := logic(fi.Name()); err != nil {
				t.Fatal(err)
			}
		})
	}
	//select {} // hang for debugging
}
