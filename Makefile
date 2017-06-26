#
# copied from https://github.com/matz/streem/blob/master/Makefile
# and changed..
#

GET = bin/z80

ifeq (Windows_NT,$(OS))
	TARGET:=$(TARGET).exe
endif

# Z80's filename extension is 'src'
TESTS=$(wildcard examples/*.src)

.PHONY : all test clean

all clean:
		$(MAKE) -C src $@

test : all
		$(TARGET) -c $(TESTS)
