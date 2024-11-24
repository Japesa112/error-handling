# Define the target variable
TARGET ?= sky

# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Source files and output binary
SRC = retransmission.c
BIN = retransmission

# Default target
all: $(BIN)

# Rule to create the binary
$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

# Conditional handling for specific targets
ifeq ($(TARGET), sky)
    # Add any specific flags or configurations for the sky target here
    CFLAGS += -DPLATFORM_SKY
endif

# Clean up build files
clean:
	rm -f $(BIN)

.PHONY: all clean
