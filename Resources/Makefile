CC=g++
CXX=g++
RANLIB=ranlib

LIBSRC=uthreads.cpp Schedueler.cpp arch_utils.cpp
LIBOBJ=$(LIBSRC:.cpp=.o)

INCS=-I
CFLAGS= -Wall -std=c++11 -g $(INCS)
CXXFLAGS = -Wall -std=c++11 -g $(INCS)

LIBUTHREADS = libuthreads.a
TARGETS = $(LIBUTHREADS)

TAR=tar
TARFLAHS=-cvf
TARNAME=ex2.tar
TARSRCS=$(LIBSRC) Makefile README

all: $(TARGETS)

$(TARGETS): $(LIBOBJ) 
	$(AR) $(ARFLAGS) $@ $^
	$(RANLIB) $@

#uthread.o: uthread.cpp uthread.h Schedueler.cpp Schedueler.h
#	$(CC) $(CFLAGS) -o uthread.o 

clean:
	$(RM) $(TARGETS) $(LIBUTHREADS) $(OBJ) $(LIBOBJ) *~ *core

depend:
	makedepend -- $(CFLAGS) -- $(SRC) $(LIBSRC)

tar:
	$(TAR) $(TARFLAGS) $(TARNAME) $(TARSRCS)
