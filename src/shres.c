#include "shres.h"

/* Open SYSPART Shared Resource area.
   Return 0 if success, or non-zero if error.
*/
char shopen(char *resource_name,unsigned long long required_flags, struct shared_resource *file_handle)
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

char shseek(struct shared_resource *,unsigned long long offset, unsigned char whence)
{
  return 1;
}

shared_resource_dir shdopen()
{
  return 0xffff;
}

char sddread(unsigned long long required_flags, shared_resource_dir *directory_handle, struct shared_resource *dirent)
{
  return 1;
}
