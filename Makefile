#
# Wrapper Makefile to abstract building mega65-libc as a sub-module
# (https://github.com/mega65/mega65-weeip is one example that does this
#  and uses this Makefile to abstract the process).
# PGS 20240118
#

cc65:
	make -f Makefile_cc65

llvm:
	cmake -DCMAKE_PREFIX_PATH=../../llvm-mos -B build && ( cd build ; make )

clean:
	make -f Makefile_cc65 clean
	( cd build ; make clean )
