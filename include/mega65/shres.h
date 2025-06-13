/*
  MEGA65 SYSPART Shared Resources Access API

*/

extern unsigned char *shres_regs;

#define MAX_RES_NAME_LEN 256
#define SEEK_SET 1
#define SEEK_CUR 2
#define SEEK_END 3 
struct shared_resource {
   char name[MAX_RES_NAME_LEN];
   unsigned long long flags;
   unsigned long long length;
   unsigned long long first_sector;
   unsigned long long position;
}; 

#define shared_resource_dir unsigned int

char shopen(char *resource_name,unsigned long long required_flags, struct shared_resource *file_handle);
char shread(unsigned char *ptr, unsigned int count, struct shared_resource *file_handle);
char shseek(struct shared_resource *,unsigned long long offset, unsigned char whence);
shared_resource_dir shdopen();
char sddread(unsigned long long required_flags, shared_resource_dir *directory_handle, struct shared_resource *dirent);
