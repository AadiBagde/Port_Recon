# Compiler
CC = gcc

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Compiler flags
CFLAGS = -Wall -Wextra -std=c11 -I$(INC_DIR) -g

# Libraries
UNAME_S := $(shell uname -s 2>/dev/null)
ifeq ($(findstring MINGW,$(UNAME_S)),MINGW)
	LDFLAGS = -lws2_32
else
	LDFLAGS = -lpthread
endif

# Find all source files recursively
SRCS := $(shell find $(SRC_DIR) -name "*.c")

# Convert source files to object files
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Executable name
TARGET = portrecon

# Default target
all: $(TARGET)

# Link executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Run
run: $(TARGET)
	./$(TARGET)

# Clean
clean:
	rm -rf $(BUILD_DIR) $(TARGET)

.PHONY: all run clean
