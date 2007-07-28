%.o: %.c
	cc65 -t c128 $<
	ca65 -t c128 $$(basename $< .c).s

all: config.o kasse.o credit_manager.o general.o
	cl65 -t c128 *.o -o kasse

clean:
	rm -rf *.o *.s
	
