# Version from file
VERSION := $(shell cat version.txt)

# Folder and file names
ODIR = ./obj
SOURCES_DIR = ./src
BUILD_DIR = ./build
DIST_DIR = ./dist
EXE = TESTSCRT.TOS

# VASM PARAMETERS
# _DEBUG: 1 to enable debug, 0 to disable them
# To disable debug, make target DEBUG_MODE=0
VASMFLAGS=-Faout -quiet -x -m68000 -spaces -showopt -devpac -D_DEBUG=$(DEBUG_MODE)
VASM = vasm 
VLINK =  vlink

# LIBCMINI PARAMETERS
# IMPORTANT! There is functional verson of the LIBCMINI library in the docker image
# To reference the library, it must set the absolute path inside the container image
# The library is stored in /freemint/libcmini 
# More info about the library: https://github.com/freemint/libcmini
LIBCMINI = /freemint/libcmini

# GCC PARAMETERS
CC = m68k-atari-mint-gcc
CFLAGS=-c -std=gnu99 -I$(LIBCMINI)/include -g -DVERSION=\"$(VERSION)\"

# Check if DEBUG_MODE is not empty
ifneq ($(DEBUG_MODE),)
    CFLAGS += -D_DEBUG=$(DEBUG_MODE)
endif

# LINKER PARAMETERS
# Add the -s option to strip the binary
LINKFLAGS=-nostdlib -L$(LIBCMINI)/lib -lcmini -lgcc -Wl,--traditional-format

_OBJS = 

OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

.PHONY: all
all: dist random prepare

.PHONY: release
release: dist random prepare

.PHONY: prepare
prepare: clean
	mkdir -p $(BUILD_DIR)

.PHONY: random
random:
ifneq ($(DEBUG_MODE),)
	cp test/hatarrom.bin dist/hatarrom.bin
else
	python src/generate_random_data.py
endif

clean-compile : clean main.o screen.o

# All C files
main.o: prepare
	$(CC) $(CFLAGS) $(SOURCES_DIR)/main.c -o $(BUILD_DIR)/main.o

screen.o: prepare
	$(CC) $(CFLAGS) $(SOURCES_DIR)/screen.c -o $(BUILD_DIR)/screen.o

main: main.o screen.o
	$(CC) $(LIBCMINI)/lib/crt0.o \
		  $(BUILD_DIR)/screen.o \
		  $(BUILD_DIR)/main.o \
		  -o $(BUILD_DIR)/$(EXE) $(LINKFLAGS);

.PHONY: dist
dist: main
	mkdir -p $(DIST_DIR)
	cp $(BUILD_DIR)/$(EXE) $(DIST_DIR) 	

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(DIST_DIR)

## Tag this version
.PHONY: tag
tag:
	git tag v$(VERSION) && git push origin v$(VERSION) && \
	echo "Tagged: $(VERSION)"
