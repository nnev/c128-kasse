## Emulator

### Emulator installation

On Debian and derivatives, install the `vice` package:

```
apt install vice
```

Then, install the (not-included) ROMs:
```
wget http://www.zimmers.net/anonftp/pub/cbm/crossplatform/emulators/VICE/vice-3.1.tar.gz
mkdir -p ~/.vice
tar xf vice-3.1.tar.gz -C ~/.vice/ vice-3.1/data --strip-components=2
```

### Emulator configuration

Enable the following options in x128:
* Settings → Drive settings → drive #8 options → enable IEC device
* Settings → Printer settings → Printer #4 enable IEC device
* Settings → Printer settings → Printer #4 emulation → File system access

Alternatively, configure the following values in `~/.vice/vicerc` (which you
can create by selecting the menu entry “Settings” → “Save settings”):
```
[C128]
IECDevice4=1
IECDevice8=1
Printer4=1
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
