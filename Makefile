CC = gcc
LFLAGS = -fPIC -shared -g
CFLAGS = -g
LIB_NAME = dataStructures
LIB_FILES=lib/*.c
SRC_FILES=src/*.c
LD_LIBRARY_PATH=LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH

.PHONY := clean run

lib: $(LIB_FILES)
	@$(CC) $(LFLAGS) $(LIB_FILES) -o lib$(LIB_NAME).so

main: $(SRC_FILES) $(LIB_FILES)
	@make lib;
	@$(CC) $(CFLAGS) $(SRC_FILES) -L. -l$(LIB_NAME) -o main

run: main
	@LD_LIBRARY_PATH=.:$$LD_LIBRARY_PATH ./main test.txt

clean: main lib
	@rm main *.so
	@touch ${LIB_FILES} ${SRC_FILES}
