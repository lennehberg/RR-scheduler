CC=g++
CXX=g++
RANLIB=ranlib

LIBSRC=arch_utils.cpp Schedueler.cpp uthreads.cpp
LIBOBJ:=$(LIBSRC:.cpp=.o)


CFLAGS= -Wall -std=c++11
CXXFLAGS = -Wall -std=c++11 -g $(INCS)

LIBUTHREADS = libuthreads.a
TARGETS = $(LIBUTHREADS)

TAR=tar
TARFLAGS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Schedueler.h thread_utils.h arch_utils.h Makefile README

all: $(TARGETS)

$(TARGETS): $(LIBOBJ) 
	ar rcs $@ $^
	$(RANLIB) $@

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<	

clean:
	$(RM) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)

