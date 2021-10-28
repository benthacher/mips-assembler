OS = Linux

VERSION = 0.0.1
CC      = /usr/bin/gcc
CFLAGS  = -g
# LDFLAGS = 

NAME = masm
BINARY = masm

BUILDDIR = build
SOURCEDIR = src
HEADERDIR = src

SOURCES := $(shell find $(SOURCEDIR) -name "*.c" -type f -printf "%f\n")

OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

all: $(BINARY)

$(BINARY): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

clean:
	rm $(BUILDDIR)/*.o