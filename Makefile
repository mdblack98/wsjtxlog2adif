# Define compiler and compiler flag variables
CC = gcc
CFLAGS = -g

# Define the target executable
TARGET = wsjtxlog2adif

# Define the source file
SRC = wsjtxlog2adif.c

# Default target
all: $(TARGET)

test: $(TARGET)
	./wsjtxlog2adif wsjtx.log test.adif w9mdb em49

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

# Clean target to remove the executable
clean:
	rm -f $(TARGET)

.PHONY: all clean

