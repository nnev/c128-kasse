%.o: %.c
	cc65 -t c128 $<
	ca65 -t c128 $$(basename $< .c).s

all: config.o kasse.o 
	cl65 -t c128 *.o -o kasse

test: config.o test.o
	cl65 -t c128 config.o test.o -o test

clean:
	rm -rf *.o *.s
	
