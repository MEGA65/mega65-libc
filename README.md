# mega65-libc
Simple C library for the MEGA65

# Using the library

## With CC65

Simply include the .c and .s files from the cc65/src directory that you need 

## With KickC

KickC support will be coming soon (or sooner, if someone would like to port the routines ;)

# Function descriptions

## FAT32 File Access

void closeall(void);
unsigned char open(char *filename);
void close(unsigned char fd);
unsigned short read512(unsigned char fd,unsigned char *buffer);

## FAT32 Directory Access

Functions similar to the Posix equivalents are provided. Key differences are that unsigned char *
is used instead of DIR * for the directory handle, and readdir() returns a pointer to a m65_dirent struct,
instead of dirent struct.

As file handle support is still partial in the hypervisor, only one directory or file can be safely open at any
point in time, and it is wise to call closeall() before opening any file or directory.

opendir() currently takes no path as input, as the Hypervisor can only work on a single directory at the moment.
Support for sub-directories will come in the fullness of time.

unsigned char *opendir(void);
m65_dirent *readdir(unsigned char *dir_handle);
void closedir(unsigned char *dir_handle);

