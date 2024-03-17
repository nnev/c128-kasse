## Using GitHub Codespaces

Press `.` to open the web editor, then click the command field at the top and
enter `> Create New Codespace`. To verify the environment works once it loaded, enter:

```
make
```

To run the VICE emulator in xpra, run:

```
make xpra
```

Then wait for the popup to appear for the newly exposed port (10000) and click
the “Open in browser” button.

## Compiler

### Fedora

### Debian and derivatives

```
apt install cc65
```

## Emulator

### Emulator installation

#### Fedora

1. [Enable the RPM Fusion nonfree repository](https://docs.fedoraproject.org/en-US/quick-docs/rpmfusion-setup/)
2. Install the X128 VICE emulator:

   ```
   sudo dnf install vice-x128
   ```

#### Debian and derivatives

On Debian and derivatives, install the `vice` package:

```
apt install vice
```

Then, install the ROMs, which are not included in the Debian package (they are
however included in the `vice` package on Arch Linux, for example):

```
make setup-vice
```

### Compiling the software into an image

```
make package
```

### Running the emulator

```
make vice
```

Then, type `run`, followed by enter.

## Reading files from the floppy (disk image)

To view the contents of a file within a disk image (e.g. the logfile), use:

```
c1541 -attach kasse.d64 -read "log-2" >(petcat -nh) >&-
```

## .git/config

To make git display contents of https://en.wikipedia.org/wiki/PETSCII files:

```
[diff "petscii"]
	textconv = /bin/sh -c 'petcat -nc -nh "$0"'
[diff "items"]
	textconv = /bin/sh -c "petcat -nc -nh \"$0\" | hexdump -e '8/1 \"%_p\"\"\n\"'"
[diff "credits"]
	textconv = /bin/sh -c "petcat -nc -nh \"$0\" | hexdump -e '8/1 \"%_p\"\"\n\"'"
```
