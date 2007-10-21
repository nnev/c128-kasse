# Note: this are my paths to cc65 as there is no gentoo ebuild, sorry for that.
# Please use the path below if you've installed cc65 system-wide
CC=~/customSoftware/cc65-2.11.0/src/cc65/cc65 -I ~/customSoftware/cc65-2.11.0/include -O
CA=~/customSoftware/cc65-2.11.0/src/ca65/ca65
CL=~/customSoftware/cc65-2.11.0/src/cl65/cl65
# CC=cc65
# CA=ca65
# CL=cl65

%.o: %.c
	${CC} -t c128 $<
	${CA} -t c128 $$(basename $< .c).s

all: config.o kasse.o general.o credit_manager.o time.o
	# See above, please just kill the PATH-definition
	cp /tmp/cc65/lib/c128* .
	PATH=${PATH}:~/customSoftware/cc65-2.11.0/src/ld65:/tmp/cc65/lib ${CL} -t c128 *.o -o kasse

package: all
	c1541 -attach kasse.d64 -delete state || exit 0 
	c1541 -attach kasse.d64 -delete items  || exit 0
	c1541 -attach kasse.d64 -delete kasse  || exit 0
	c1541 -attach kasse.d64 -write kasse  || exit 0
	c1541 -attach kasse.d64 -write state || exit 0 
	c1541 -attach kasse.d64 -write items  || exit 0

test: config.o test.o general.o
	${CL} -t c128 config.o test.o general.o -o test

test-package: test
	c1541 -attach test.d64 -delete state || exit 0 
	c1541 -attach test.d64 -delete items  || exit 0
	c1541 -attach test.d64 -delete test  || exit 0
	c1541 -attach test.d64 -write test  || exit 0
	c1541 -attach test.d64 -write state || exit 0 
	c1541 -attach test.d64 -write items  || exit 0

clean:
	rm -rf *.o *.s
	
dist-clean: clean
	rm kasse test 
	
