package main

import (
	"fmt"
	"io"
	"log"
	"net"
	"regexp"
	"time"
)

var promptRe = regexp.MustCompile(`\(C:\$[0-9a-f]{4}\) `)

func discardUntilPrompt(r io.Reader) error {
	log.Printf("discard")
	for {
		buf := make([]byte, 50)
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

func quickload() error {
	conn, err := dial()
	if err != nil {
		return err
	}
	defer conn.Close()

	if err := discardUntilPrompt(conn); err != nil {
		return err
	}

	if _, err := conn.Write([]byte("ll \"kasse.lbl\"\n")); err != nil {
		return err
	}
	if err := discardUntilPrompt(conn); err != nil {
		return err
	}

	if _, err := conn.Write([]byte("load \"kasse\" 0\n")); err != nil {
		return err
	}
	if err := discardUntilPrompt(conn); err != nil {
		return err
	}

	if _, err := conn.Write([]byte("keybuf run\\n\n")); err != nil {
		return err
	}
	if _, err := conn.Write([]byte("x\n")); err != nil {
		return err
	}

	return conn.Close()
}

func main() {
	if err := quickload(); err != nil {
		log.Fatal(err)
	}
}
