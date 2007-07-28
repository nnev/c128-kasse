%.o: %.c
	cc65 -t c128 $<
	ca65 -t c128 $$(basename $< .c).s

all: config.o kasse.o 
	cl65 -t c128 kasse.o config.o -o kasse

clean:
	rm -rf *.o *.s
	