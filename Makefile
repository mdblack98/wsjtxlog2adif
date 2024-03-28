UNAME_S := $(shell uname -s)

# Define the target executable based on detected OS
ifeq ($(UNAME_S),Linux)
    TARGET = wsjtxlog2adif
endif
ifeq ($(UNAME_S),Darwin)
    TARGET = wsjtxlog2adif
endif
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
    TARGET = wsjtxlog2adif.exe
endif
ifeq ($(findstring CYGWIN,$(UNAME_S)),CYGWIN)
    TARGET = wsjtxlog2adif.exe
endif

# Define compiler and compiler flag variables
CC = gcc
CFLAGS = -g

# Define the target executable


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

