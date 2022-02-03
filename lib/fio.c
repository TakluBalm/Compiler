#include "include.h"

#define CHECK(f, a)	if(f == 0)	return a

file_p coverfd(int __file_descriptor){
	file_p f = calloc(1, sizeof(*f));
	f->fd = __file_descriptor;
	f->flags = fcntl(f->fd, F_GETFL);
	return f;
}

file_p openf(const char* filename, char mode){
	int flags = 0;
	int create_mode = S_IRGRP | S_IROTH | S_IRUSR | S_IWUSR;

	if((mode&NCREAT) == 0)		flags = flags|O_CREAT;

	if((mode&EXEC) != 0)	create_mode = create_mode|S_IXGRP | S_IXOTH | S_IXUSR;

	if((READ&mode) != 0 && (WRITE&mode) != 0)	flags = flags|O_RDWR;
	else if((READ&mode) != 0)					flags = flags|O_RDONLY;
	else if((WRITE&mode) != 0)					flags = flags|O_WRONLY;
	else										return NULL;
	
	if((mode&APPND) != 0)	flags = flags|O_APPEND;

	if((CREAT&mode) != 0)	flags = flags|O_EXCL;

	file_p f = calloc(1, sizeof(*f));
	if(f == NULL)	return NULL;

	f->fd = open(filename, flags, create_mode);
	if(f->fd == -1){
		free(f);
		f = NULL;
	}
	return  f;
}

char READ_BUF(file_p f){

	char c;

	if(f->IN_BUF_START == f->IN_BUF_END){

		if(f->finished)	return EOF;

		f->IN_BUF_START = 0;
		f->IN_BUF_END = read(f->fd, f->IN_BUF, BUFSIZ) - 1;

		if(f->IN_BUF_END < BUFSIZ - 1)	f->finished = true;
	}
	c = f->IN_BUF[f->IN_BUF_START++];
	return c;
}

void WRITE_BUF(file_p f, char c){
	if(f->OUT_BUF_END == BUFSIZ - 1){
		bufflush(f);
		f->OUT_BUF_END = 0;
	}
	f->OUT_BUF[f->OUT_BUF_END++] = c;
}

void bufflush(file_p f){

	CHECK(f,);

	write(f->fd, f->OUT_BUF, f->OUT_BUF_END);
}

size_t readf(file_p f, void* buf, size_t n){

	CHECK(f, 0);

	if(f->finished || (f->flags)&READ == 0)	return 0;

	size_t count;
	char c;
	char* buf_in = buf;
	for(count = 0; count < n && (c = getcf(f)) != EOF; count++){
		buf_in[count] = c;
	}

	return count;
}

size_t writef(file_p f, void* buf, size_t n){

	CHECK(f, 0);

	size_t count;
	char flush = 0;
	char* buf_out = buf;
	for(count = 0; count < n; count++){
		WRITE_BUF(f, buf_out[count]);
		if(buf_out[count] == '\n')	flush = 1;
	}
	if(flush)	bufflush(f);
	return count + 1;
}

int ungetcf(char c, file_p f){

	CHECK(f, 0);

	if(f->UNGET_BUF_INDEX == BUFSIZ)	return 0;
	if(c == EOF)						return 0;

	f->UNGET_BUF[f->UNGET_BUF_INDEX++] = c;
	return 1;
}

char getcf(file_p f){

	CHECK(f, EOF);

	if(f->UNGET_BUF_INDEX){
		return f->UNGET_BUF[--f->UNGET_BUF_INDEX];
	}
	return READ_BUF(f);
}

int putcf(char c, file_p f){
	CHECK(f, 0);
	WRITE_BUF(f, c);
	return 1;
}

void closef(file_p f){

	CHECK(f, );

	bufflush(f);
	close(f->fd);
	free(f);
}