#include <stdio.h>

#include "mega65/shres.h"
#include "mega65/memory.h"

unsigned char magic_string[]={
  0x4d,0x45,0x47,0x41,'6','5',                   // MEGA65
  0x53,0x48,0x41,0x52,0x45,0x44,                 // SHARED
  0x52,0x45,0x53,0x4f,0x55,0x52,0x43,0x45,0x53,  // RESOURCES
  0x00};

void sdcard_busy_wait(void)
{
  while (PEEK(0xD680)&0x03) continue;
}

char do_shres_trap(unsigned long arg)
{
  // Fail if SD card is busy
  if (PEEK(0xD680)&0x03) {
    printf("SD card is busy\n");
    return 1;
  }

  printf("SD card was idle\n");
  
  shres_regs[0] = (arg>>0)&0xff;
  shres_regs[1] = (arg>>8)&0xff;
  shres_regs[2] = (arg>>16)&0xff;
  shres_regs[3] = (arg>>24)&0xff;
  shres_trap();

  printf("Trap A=0x%02x\n",shres_regs[0]);
  
  // Check trap response in A
  return shres_regs[0];

}



/* Open SYSPART Shared Resource area.
   Return 0 if success, or non-zero if error.
*/
char shopen(char *resource_name,unsigned long required_flags, struct shared_resource *file_handle)
{
  return 1;
}

/* Read bytes from a SYSPART shared resource.
   Return 0 if success, or non-zero if error.
 */
char shread(unsigned char *ptr, unsigned int count, struct shared_resource *file_handle)
{
  return 1;
}

char shseek(struct shared_resource *,unsigned long offset, unsigned char whence)
{
  return 1;
}

shared_resource_dir shdopen()
{
  char i;
  if (do_shres_trap(0)) return 0xffff;

  sdcard_busy_wait();

  // Make sure the magic string is present in the shared resource area
  for(i=0;magic_string[i];i++) {
    if (lpeek(0xffd6e00L + i) != magic_string[i]) break;
  }
  if (magic_string[i]) return 0xffff;

  // It's valid, so return sector number of the first entry in the shared resource
  // table.
  return 1;
}

char shdread(unsigned long required_flags, shared_resource_dir *directory_handle, struct shared_resource *dirent)
{
  return 1;
}
