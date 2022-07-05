CC = ~/opt/bin/mos-mega65-clang
CFLAGS = -mcpu=mos65c02 -Os -Wall -Wextra -Wshadow -Wconversion -Wno-language-extension-token
INCLUDES = -I include/

all: obj main

obj:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/*.c

main:
	$(CC) $(CFLAGS) $(INCLUDES) example.c *.o -o example.prg
	$(CC) $(CFLAGS) $(INCLUDES) example.c *.o -Wl,--lto-emit-asm -o example.s

# Note that this will currently not assemble due to:
#   missing opcode; zeropage handling
fileio: src/fileio.s
	$(CC) $(CFLAGS) $(INCLUDES) src/fileio.s

clean:
	rm -fR *.o *~ *.prg *.elf *.s
