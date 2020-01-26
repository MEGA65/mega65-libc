void closeall(void);
void close(unsigned char fd);

// Returns file descriptor
unsigned char open(char *filename);

// Read upto one sector of data into the supplied buffer.
// Returns the number of bytes actually read.
unsigned short read512(unsigned char fd,unsigned char *buffer);
