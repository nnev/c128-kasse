# Note: this are my paths to cc65 as there is no gentoo ebuild, sorry for that.
# Please use the path below if you've installed cc65 system-wide
CC=~/customSoftware/cc65-2.11.0/src/cc65/cc65 -I ~/customSoftware/cc65-2.11.0/include
CA=~/customSoftware/cc65-2.11.0/src/ca65/ca65
CL=~/customSoftware/cc65-2.11.0/src/cl65/cl65
# CC=cc65
# CA=ca65
# CL=cl65

src/%.o: src/%.c include/%.h
	${CC} -I include -t c128 $<
	${CA} -I include -t c128 src/$$(basename $< .c).s

all: src/config.o src/kasse.o src/general.o src/credit_manager.o src/time.o
	# See above, please just kill the PATH-definition
	cp /tmp/cc65/lib/c128* .
	PATH=${PATH}:~/customSoftware/cc65-2.11.0/src/ld65:/tmp/cc65/lib ${CL} -t c128 -O src/*.o -o kasse

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
	rm -rf src/*.o src/*.s
	
dist-clean: clean
	rm kasse test 
