CC=gcc
LFLAGS=-fPIC -shared -g
CFLAGS=-g
LIB_NAME=MyLib
LIB_FILES=lib/*.c src/lexer.c src/parser.c src/semantic.c
MAIN=src/main.c
LD_LIBRARY_PATH=LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH

lib: $(LIB_FILES)
	$(CC) $(LFLAGS) $(LIB_FILES) -o lib$(LIB_NAME).so

main: $(MAIN) $(LIB_FILES)
	make lib;
	$(CC) $(CFLAGS) $(MAIN) -L. -l$(LIB_NAME) -o main

run: main
	LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH ./main test.txt

clean:	main *.so
	rm main *.so
