#ifndef READER_H
#define READER_H

#include "../include.h"

typedef struct file* file_p;

struct file{
	int fd;
	int flags;
	char IN_BUF[BUFSIZ];
	char OUT_BUF[BUFSIZ];
	char UNGET_BUF[BUFSIZ];
	size_t IN_BUF_START;
	size_t IN_BUF_END;
	size_t OUT_BUF_END;
	size_t UNGET_BUF_INDEX;
	bool finished;
};

#define READ	0x01
#define WRITE	0x02
#define APPND	0x04
#define CREAT	0x08
#define NCREAT	0x10
#define EXEC	0x20

file_p coverfd(int __file_descriptor);

file_p openf(const char* filename, char mode);
void closef(file_p f);
size_t readf(file_p, void*, size_t);
char getcf(file_p f);
size_t writef(file_p, void*, size_t);
int putcf(char c, file_p f);

void bufflush(file_p);
int ungetcf(char c, file_p f);

#endif