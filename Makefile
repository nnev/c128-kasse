# Note: this are my paths to cc65 as there is no gentoo ebuild, sorry for that.
# Please use the path below if you've installed cc65 system-wide
#CC=~/customSoftware/cc65-2.11.0/src/cc65/cc65 -I ~/customSoftware/cc65-2.11.0/include
#CA=~/customSoftware/cc65-2.11.0/src/ca65/ca65
#CL=~/customSoftware/cc65-2.11.0/src/cl65/cl65
CC=cc65
CA=ca65
CL=cl65

src/%.o: src/%.c include/%.h
	${CC} -O -I include -t c128 $<
	${CA} -I include -t c128 src/$$(basename $< .c).s

test/%.o: test/%.c
	${CC} -O -I include -t c128 $<
	${CA} -I include -t c128 test/$$(basename $< .c).s

kasse: src/config.o src/kasse.o src/general.o src/credit_manager.o src/c128time.o
	# See above, please just kill the PATH-definition
	# cp /tmp/cc65/lib/c128* .
	PATH=${PATH}:~/customSoftware/cc65-2.11.0/src/ld65:/tmp/cc65/lib ${CL} -t c128 src/*.o -o kasse

itemz: src/config.o src/itemz.o src/general.o src/credit_manager.o
	# See above, please just kill the PATH-definition
	# cp /tmp/cc65/lib/c128* .
	PATH=${PATH}:~/customSoftware/cc65-2.11.0/src/ld65:/tmp/cc65/lib ${CL} -t c128 src/config.o src/itemz.o src/general.o src/credit_manager.o -o itemz

all: kasse itemz

package: all
	cp images/kasse.d64 .
	c1541 -attach kasse.d64 -delete state || exit 0 
	c1541 -attach kasse.d64 -delete items  || exit 0
	c1541 -attach kasse.d64 -delete kasse  || exit 0
	c1541 -attach kasse.d64 -delete itemz  || exit 0
	c1541 -attach kasse.d64 -write kasse  || exit 0
	c1541 -attach kasse.d64 -write itemz  || exit 0
#	c1541 -attach kasse.d64 -write state || exit 0 
#	c1541 -attach kasse.d64 -write items  || exit 0

test: src/config.o test/test.o src/general.o
	${CL} -t c128 src/config.o test/test.o src/general.o -o test

test-package: test
	c1541 -attach test.d64 -delete state || exit 0 
	c1541 -attach test.d64 -delete items  || exit 0
	c1541 -attach test.d64 -delete test  || exit 0
	c1541 -attach test.d64 -write test  || exit 0
	c1541 -attach test.d64 -write state || exit 0 
	c1541 -attach test.d64 -write items  || exit 0

clean:
	rm -rf src/*.o src/*.s test/*.o test/*.s
	
dist-clean: clean
	rm kasse itemz kasse.d64
