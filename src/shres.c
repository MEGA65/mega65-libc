#include <stdio.h>
#include <string.h>

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
    return 1;
  }

  shres_regs[0] = (arg>>0)&0xff;
  shres_regs[1] = (arg>>8)&0xff;
  shres_regs[2] = (arg>>16)&0xff;
  shres_regs[3] = (arg>>24)&0xff;
  shres_trap();

  // Check trap response in P
  return (shres_regs[4]&0x01) ^0x01;

}



/* Open SYSPART Shared Resource area.
   Return 0 if success, or non-zero if error.
*/
char shopen(char *resource_name,unsigned long required_flags, struct shared_resource *file_handle)
{
  unsigned char r;
  unsigned int d;
  d = shdopen(); 
  if (d==0xffff) return 1;

  printf("sdhopen() success\n");
  
  while (! (r=shdread(required_flags, &d,file_handle))) {
    printf("File: '%s'%d vs '%s'%d (%d)\n",
	   file_handle->name,strlen(file_handle->name),
	   resource_name,strlen(resource_name),
	   strcmp(resource_name,file_handle->name));
    {
      unsigned char i;
      for(i=0;resource_name[i];i++) {
	printf("[%02x vs %02x] ",
	       file_handle->name[i], resource_name[i]);
      }
    }
    if (!strcmp(resource_name,file_handle->name)) return 0;
  }
  printf("r=%d\n",r);
  
  return 1;
}

/* Read bytes from a SYSPART shared resource.
   Returns number of bytes read.
 */
unsigned int shread(unsigned char *ptr, unsigned int count, struct shared_resource *f)
{
  unsigned int read_bytes = 0;

  if (!f) return 0;

  // EOF
  if ( f->position >= f->length ) return 0;

  if (count > (f->length - f->position)) count = (f->length - f->position);

  while(count > 0) {
    // Work out how many bytes we can read from the current sector
    unsigned int bytes = 512 - (f->position & 511);
    if (bytes > count) bytes = count;

    // Read the sector in which our bytes are to be found
    do_shres_trap(f->first_sector + (f->position >> 9) );

    lcopy(0xffd6e00L + (f->position & 511),(unsigned long)ptr, bytes);

    // Advance output pointer and file offset pointer
    ptr += bytes;
    f->position += bytes;

    // And reduce the number of bytes we need.
    count -= bytes;

    read_bytes += bytes;
  }

  return read_bytes;
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

  // Scan shared resources file for the next matching file, or until we reach
  // the end of the shared resources directory.
  do {  
    // The directory handle is really just the sector number in the shared resources area.
    printf("directory sector = %d\n",*directory_handle);

    if (do_shres_trap(*directory_handle)) return 1;
    sdcard_busy_wait();
    
    // Check for end of directory (length of filename is 0)
    if (!lpeek(0xffd6e10L)) return 2;
    
    // Copy directory entry to dirent
    lcopy(0xffd6e00L,(unsigned long)dirent,256);
    dirent->position = 0L;

    (*directory_handle) ++;
  }
  while ((dirent->flags & required_flags) != required_flags);
  
  return 0;
}
