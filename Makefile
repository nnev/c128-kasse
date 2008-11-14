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
	${CL} -t c128 src/c128time.o src/config.o src/kasse.o src/general.o src/credit_manager.o -o kasse

itemz: src/config.o src/itemz.o src/general.o src/credit_manager.o
	${CL} -t c128 src/config.o src/itemz.o src/general.o src/credit_manager.o -o itemz

cat: src/general.o src/cat.o
	${CL} -t c128 src/general.o src/cat.o -o cat

all: kasse itemz

package: all
	cp images/kasse.d64 .
	c1541 -attach kasse.d64 -delete state || exit 0 
	c1541 -attach kasse.d64 -delete items || exit 0
	c1541 -attach kasse.d64 -delete kasse || exit 0
	c1541 -attach kasse.d64 -delete itemz || exit 0
	c1541 -attach kasse.d64 -write kasse || exit 0
	c1541 -attach kasse.d64 -write itemz || exit 0
#	c1541 -attach kasse.d64 -write state || exit 0 
#	c1541 -attach kasse.d64 -write items || exit 0

test: src/config.o test/test.o src/general.o
	${CL} -t c128 src/config.o test/test.o src/general.o -o test

test-package: test
	c1541 -attach test.d64 -delete state || exit 0 
	c1541 -attach test.d64 -delete items || exit 0
	c1541 -attach test.d64 -delete test || exit 0
	c1541 -attach test.d64 -write test || exit 0
	c1541 -attach test.d64 -write state || exit 0 
	c1541 -attach test.d64 -write items || exit 0

clean:
	rm -rf src/*.o src/*.s test/*.o test/*.s
	
dist-clean: clean
	rm kasse itemz kasse.d64
