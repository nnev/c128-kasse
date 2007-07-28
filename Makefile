
kasse: kasse.o
	cl65 -t c128 kasse.o

kasse.o: kasse.s
	ca65 -t c128 kasse.s

kasse.s: kasse.c
	cc65 -t c128 kasse.c

all: kasse

clean:
	rm -f *.s *.o
