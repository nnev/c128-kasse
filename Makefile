.c.s: cc65 -t c128 $<

.s.o: cc65 -t c128 $<

all: config.o kasse.o 
	cl65 -t c128 kasse.o config.o -o kasse

clean:
	rm -rf *.o *.s
	