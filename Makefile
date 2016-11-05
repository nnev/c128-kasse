CC=cc65
AS=ca65
LD=cl65
INCLUDES:=$(wildcard include/*.h) include/version.h
GV:=$(shell git describe --tags --always)
CFLAGS= -I include -t c128

.PHONY: include/version.h clean dist-clean

all: kasse itemz

%.o: %.c ${INCLUDES}
	${CC} ${CFLAGS} -O $<
	${AS} ${CFLAGS} $(addsuffix .s,$(basename $< ))

include/version.h:
	echo "#define GV \"${GV}\"" > $@

kasse: src/config.o src/kasse.o src/general.o src/credit_manager.o src/c128time.o src/print.o
	${LD} -t c128 $^ -o $@

itemz: src/config.o src/itemz.o src/general.o src/credit_manager.o src/c128time.o src/print.o
	${LD} -t c128 $^ -o $@

cat: src/general.o src/cat.o
	${LD} -t c128 $^ -o $@

package: all
	c1541 -format "${GV}",KA d64 kasse.d64
	c1541 -attach kasse.d64 -write kasse || exit 0
	c1541 -attach kasse.d64 -write itemz || exit 0
	[ -e state ] && c1541 -attach kasse.d64 -write state
	[ -e items ] && c1541 -attach kasse.d64 -write items

test: src/config.o test/test.o src/general.o
	cl65 -t c128 src/config.o test/test.o src/general.o -o test

test-package: test
	c1541 -format "test",TE d64 test.d64
	c1541 -attach test.d64 -write test || exit 0
	c1541 -attach test.d64 -write state || exit 0
	c1541 -attach test.d64 -write items || exit 0

clean:
	rm -rf src/*.o src/*.s test/*.o test/*.s

dist-clean: clean
	rm -f kasse itemz kasse.d64
