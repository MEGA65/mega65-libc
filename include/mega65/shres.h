/*
  MEGA65 SYSPART Shared Resources Access API

*/

extern void shres_trap(void);
extern unsigned char shres_regs[5];

#define SHRES_FLAG_FONT 1
// Flags with meaning only if FLAG_FONT set
#define SHRES_FLAG_16x16 2
#define SHRES_FLAG_UNICODE 4

#define MAX_RES_NAME_LEN 256
struct shared_resource {
   char name[MAX_RES_NAME_LEN];
   unsigned long flags;
   unsigned long length;
   unsigned long first_sector;
   unsigned long position;
}; 

#define shared_resource_dir unsigned int

char shopen(char *resource_name,unsigned long required_flags, struct shared_resource *file_handle);
char shread(unsigned char *ptr, unsigned int count, struct shared_resource *file_handle);
char shseek(struct shared_resource *,unsigned long offset, unsigned char whence);
shared_resource_dir shdopen();
char shdread(unsigned long required_flags, shared_resource_dir *directory_handle, struct shared_resource *dirent);
