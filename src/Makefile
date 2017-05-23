.PHONY: outputdirs

CC = gcc

gccversion = 4
machine = $(shell uname -m)
GCCVERSION_TEST := $(shell expr `gcc -dumpversion | cut -f1 -d.` \> 4)

ifeq ($(machine), x86_64)
    ARCH_DIR = x86_64
else
    ARCH_DIR = i686
endif

ifeq ($(GCCVERSION_TEST), 1)
    gccversion = 5
endif

OUTPUTDIR = ../bin/$(ARCH_DIR)
TARGET = $(OUTPUTDIR)/xanter

INCLUDEDIR = -I../include

CFLAGS = -Wall -Wextra -O0 -ggdb $(INCLUDEDIR)

ifeq ($(gccversion), 5)
    CFLAGS += -fgnu89-inline
endif

LIBDIR = -L/usr/local/lib
LIBS = -lxante -lcollections

C_FILES := $(wildcard *.c)
OBJS = $(C_FILES:.c=.o)

$(TARGET): outputdirs $(OBJS)
	$(CC) -o $(TARGET) $(OBJS) $(LIBDIR) $(LIBS)

clean:
	rm -rf $(OBJS) $(TARGET) *~ ../include/*~

purge: clean $(TARGET)

outputdirs: $(OUTPUTDIR)
$(OUTPUTDIR):
	mkdir -p $(OUTPUTDIR)
