%.o: %.c
	cc65 -t c128 $<
	ca65 -t c128 $$(basename $< .c).s

all: config.o kasse.o general.o credit_manager.o
	cl65 -t c128 *.o -o kasse

package: all
	c1541 -zcreate kasse.d64 kasse || exit 0
	c1541 -attach kasse.d64 -delete state || exit 0 
	c1541 -attach kasse.d64 -delete items  || exit 0
	c1541 -attach kasse.d64 -delete kasse  || exit 0
	c1541 -attach kasse.d64 -write state || exit 0 
	c1541 -attach kasse.d64 -write items  || exit 0
	c1541 -attach kasse.d64 -write kasse  || exit 0

test: config.o test.o
	cl65 -t c128 config.o test.o -o test

test-package: test
	c1541 -attach test.d64 -delete state || exit 0 
	c1541 -attach test.d64 -delete items  || exit 0
	c1541 -attach test.d64 -delete test  || exit 0
	c1541 -attach test.d64 -write state || exit 0 
	c1541 -attach test.d64 -write items  || exit 0
	c1541 -attach test.d64 -write test  || exit 0

clean:
	rm -rf *.o *.s kasse test 
	
