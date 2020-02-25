.POSIX:
CXX ?= g++
CXXO ?= -O2
CXXFLAGS = -Wall -Wextra -Wcast-align --std=c++17
SRCS = ast.cpp erro.cpp lex.cpp main.cpp syn.cpp token.cpp sem.cpp
OBJS = $(SRCS:.cpp=.o)
LIBS = -lm -lstdc++fs
MAIN = pcc
VERSION = 1.0


all: $(MAIN)
	   @echo Compilação do Pseudo Compilador $(VERSION) finalizada, use o binário $(MAIN)

$(MAIN): $(OBJS)
	       $(CXX) $(CXXO) $(CXXFLAGS) -o $(MAIN) $(OBJS) $(LIBS)
				 $(CLEAN)

.cpp.o:
			$(CXX) $(CXXO) $(CXXFLAGS) -c $< -o $@

clean:
			$(RM) *.o *~ $(MAIN)

.PHONY: all clean
