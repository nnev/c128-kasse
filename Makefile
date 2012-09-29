CC=cc65
AS=ca65
LD=cl65
INCLUDES:=$(wildcard include/*.h)
GV:=$(shell git describe --tags --always)

CFLAGS += -DGV=\"${GV}\"

%.o: %.c ${INCLUDES}
	${CC} ${CFLAGS} -O -I include -t c128 $< -o /dev/stdout | ${AS} -I include -t c128 /dev/stdin -o $@
all: kasse itemz

kasse: src/config.o src/kasse.o src/general.o src/credit_manager.o src/c128time.o src/print.o
	${LD} -t c128 $^ -o $@

itemz: src/config.o src/itemz.o src/general.o src/credit_manager.o src/c128time.o src/print.o
	${LD} -t c128 $^ -o $@

cat: src/general.o src/cat.o
	${LD} -t c128 $^ -o $@

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
	cl65 -t c128 src/config.o test/test.o src/general.o -o test

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
