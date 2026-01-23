# Makefile for LOCO unit tests
# Compiles isolated hardware component tests

CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRC_DIR = ../../src
INCLUDE_DIR = ../../include

# Source files needed for testing
SRCS = $(SRC_DIR)/shifter.c \
       $(SRC_DIR)/alu.c \
       $(SRC_DIR)/datapath.c \
       $(SRC_DIR)/utils/conversions.c

# Test executable
TARGET = test_loco_internals

all: $(TARGET)

$(TARGET): test_loco_internals.c $(SRCS)
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -o $@ $^

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all run clean
