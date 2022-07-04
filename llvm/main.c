#include <stdio.h>
#include <memory.h>
#include <conio.h>
#include <debug.h>
#include <hal.h>

int main(int argc, char *argv[]) {
    lfill(0xff80000L, 0x00, 80 * 50 * 2);

    setmapedpal(0);

    usleep(10);

}
