#Compiler and Linker
CC          := g++
GCC         := gcc

#The Target Binary Program
TARGET      := main
TESTTARGET  := test_suite

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
EXTDIR      := ext
TESTDIR     := test
INCDIR      := inc
LIBDIR      := lib
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := res
SRCEXT      := cpp
DEPEXT      := d
OBJEXT      := o

current_dir = $(shell pwd)

#Flags, Libraries and Includes
CXXSTD      := -std=c++11 -Wno-deprecated-register -g -O0
CFLAGS      := $(CXXSTD) -fopenmp -Wall -O3 -g
DYNLIBPARAM := -dynamiclib
INC         := -I$(INCDIR) -Isrc -Isrc/test -I$(LIBDIR) -I$(EXTDIR)

LEXERFILES := $(shell find $(SRCDIR)/ -type f -name *.$(SRCEXT))
TESTFILES  := $(shell find $(TESTDIR)/ $(SRCDIR)/ ! -name 'main.cpp' -type f -name *.$(SRCEXT) )

all: directories lexer

directories:
	mkdir -p $(TARGETDIR)
	mkdir -p $(BUILDDIR)
	mkdir -p $(TESTDIR)
	mkdir -p $(INCDIR)

lexer:
	$(CC) $(CXXSTD) $(INC) $(LEXERFILES) -o $(TARGETDIR)/$(TARGET)

tests:
	$(CC) $(CXXSTD) $(INC) $(TESTFILES) -o $(TARGETDIR)/$(TESTTARGET)