CC=cc65
AS=ca65
LD=cl65
INCLUDES:=$(wildcard include/*.h) include/version.h include/charset_umlauts.h
GV:=$(shell git describe --tags --always)
CFLAGS= -I include -t c128 -g

.PHONY: clean dist-clean format

all: kasse itemz cat

build/%.o: src/%.c ${INCLUDES}
	${CC} ${CFLAGS} -O $< -o build/$(addsuffix .s,$(shell basename $< .c))
	${AS} ${CFLAGS} build/$(addsuffix .s,$(shell basename $< .c)) -o $@

build/%.o: src/%.s
	${AS} ${CFLAGS} $< -o $@

build/%.o: test/%.c ${INCLUDES}
	${CC} ${CFLAGS} -O $< -o build/$(addsuffix .s,$(shell basename $< .c))
	${AS} ${CFLAGS} build/$(addsuffix .s,$(shell basename $< .c)) -o $@

build/%.o: test/%.s
	${AS} ${CFLAGS} $< -o $@

.git/index:

include/version.h: .git/index
	mkdir -p build
	echo "#define GV \"${GV}\"" > $@

include/charset_umlauts.h: assets/umlauts.pbm
	./util/mkfont assets/umlauts.pbm chars_umlauts > $@

kasse: build/config.o build/kasse.o build/general.o build/credit_manager.o build/c128time.o build/print.o build/vdc_patch_charset.o build/vdc_util.o build/globals.o
	${LD} -Ln $@.lbl -t c128 $^ -o $@

itemz: build/config.o build/itemz.o build/general.o build/credit_manager.o build/c128time.o build/print.o build/globals.o
	${LD} -Ln $@.lbl -t c128 $^ -o $@

cat: build/general.o build/cat.o build/config.o build/print.o build/globals.o
	${LD} -Ln $@.lbl -t c128 $^ -o $@

charmap: build/print_charmap.o build/vdc_util.o build/vdc_patch_charset.o
	${LD} -Ln $@.lbl -t c128 $^ -o $@

ascii: build/print_ascii.o
	${LD} -Ln $@.lbl -t c128 $^ -o $@

package: all
	c1541 -format "${GV}",KA d71 kasse.d71
	c1541 -attach kasse.d71 -write kasse
	c1541 -attach kasse.d71 -write itemz
	[ -e state ] && c1541 -attach kasse.d71 -write state || exit 0
	[ -e items ] && c1541 -attach kasse.d71 -write items || exit 0

test: build/config.o build/test.o build/general.o build/print.o build/globals.o
	${LD} -Ln $@.lbl -t c128 $^ -o test/$@

test-package: test
	c1541 -format "test",TE d71 test.d71
	c1541 -attach test.d71 -write test/test || exit 0
	c1541 -attach test.d71 -write state || exit 0
	c1541 -attach test.d71 -write items || exit 0

clean:
	rm -rf build/*.o build/*.s test/test

dist-clean: clean
	rm -f kasse kasse.lbl itemz itemz.lbl cat cat.lbl kasse.d71

format:
	clang-format-3.9 -i **/*.[ch]
