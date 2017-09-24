## Emulator

### Emulator installation

On Debian and derivatives, install the `vice` package:

```
apt install vice
```

Install `/usr/lib/vice/C128/kernal*` and `/usr/lib/vice/C128/charg*` (ask contributors for a copy).

### Emulator configuration

Enable the following options in x128:
* Settings → Drive settings → drive #8 options → enable IEC device
* Settings → Printer settings → Printer #4 enable IEC device
* Settings → Printer settings → Printer #4 emulation → File system access

Alternatively, configure the following values in `~/.vice/vicerc` (which you
can create by selecting the menu entry “Settings” → “Save settings”):
```
IECDevice4=1
IECDevice8=1
Printer4=1
```

### Running the emulator

```
x128 -autostart kasse.d64
```

## Reading files from the floppy (disk image)

To view the contents of a file within a disk image (e.g. the logfile), use:

```
c1541 -attach kasse.d64 -read "log-2" >(petcat -nh) >&-
```
