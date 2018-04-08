NAME=../expr_parser

CC=g++
CC_FLAGS=-std=c++11 -Iinclude
SRCPATH=src/
INCPATH=include/
BINPATH=bin/

all: bin $(NAME)

bin:
	mkdir bin

# main
$(NAME): $(BINPATH)main.o
	$(CC) $(CC_FLAGS) $(BINPATH)main.o -o $(NAME)

$(BINPATH)main.o: $(SRCPATH)main.cpp
	$(CC) -c $(CC_FLAGS) $(SRCPATH)main.cpp -o $(BINPATH)main.o

clean:
	rm -rf bin $(NAME)
