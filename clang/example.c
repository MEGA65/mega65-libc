// Example usage
#include <memory.h>
#include <hal.h>

int main()
{
  usleep(200);
  lfill(0x40000, 0x00, 0x8000);
  return 0;
}
