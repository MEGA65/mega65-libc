CC = ~/opt/bin/mos-mega65-clang
CFLAGS = -mcpu=mos65c02 -Wall -Wpedantic -Wno-language-extension-token
INCLUDES = -I include/

all:
	$(CC) $(CFLAGS) $(INCLUDES) -c src/*.c

clean:
	rm -fR *.o *~
