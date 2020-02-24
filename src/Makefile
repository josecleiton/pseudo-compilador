.POSIX:
CC = g++
CFLAGS = -O2 -Wall -Wextra -Wcast-align --std=c++17
SRCS = ast.cpp erro.cpp lex.cpp main.cpp sem.cpp syn.cpp token.cpp
OBJS = $(SRCS:.cpp=.o)
LIBS = -lm
MAIN = pcc
VERSION = 0.1


all: $(MAIN)
	   @echo Compilação terminada, use o binário $(MAIN)

$(MAIN): $(OBJS)
	       $(CC) $(CFLAGS) -o $(MAIN) $(OBJS) $(LIBS)
				 $(CLEAN)

.cpp.o:
			$(CC) $(CFLAGS) -c $< -o $@

clean:
			$(RM) *.o *~ $(MAIN)

.PHONY: all clean
