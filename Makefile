OS = Linux

VERSION = 0.0.1
CC      = /usr/bin/gcc
CFLAGS  = -g
LDFLAGS = 

NAME = masm
BINARY = masm

BUILDDIR = build
SOURCEDIR = src
HEADERDIR = src

SOURCES := $(shell find $(SOURCEDIR) -name '*.c')

OBJECTS := $(addprefix $(BUILDDIR)/,$(SOURCES:%.c=%.o))

$(BINARY): $(OBJECTS)
    $(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BINARY)

$(BUILDDIR)/%.o: %.c
    $(CC) $(CFLAGS) $(LDFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@
