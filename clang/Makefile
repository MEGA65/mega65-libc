CC = ~/opt/bin/mos-mega65-clang
CFLAGS = -mcpu=mos65c02 -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token
INCLUDES = -I include/

all:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/*.c

# Note that this will currently not assemble due to:
#   missing opcode; zeropage handling
fileio: src/fileio.s
	$(CC) $(CFLAGS) $(INCLUDES) src/fileio.s

clean:
	rm -fR *.o *~
