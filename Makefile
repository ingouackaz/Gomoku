DESTDIR=.
NAME=IA
CC=gcc
CXX=g++
CP=cp -r
RM=rm -f

CSOURCES=\

CPPSOURCES=\
        bnet_ai_main.cpp\
        IA.cpp\
		Map.cpp\

OBJECTS=$(CSOURCES:.c=.o) $(CPPSOURCES:.cpp=.o)

CFLAGS=
CXXFLAGS=
CPPFLAGS=-DDEBUG -g -Wall -Wextra
LDFLAGS=

export DESTDIR
.PHONY: all clean fclean re

all: $(DESTDIR)/$(NAME)

$(DESTDIR)/$(NAME): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LDFLAGS) -o $(DESTDIR)/$(NAME)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(DESTDIR)/$(NAME)

re: clean fclean all

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $(@F)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $(@F)

