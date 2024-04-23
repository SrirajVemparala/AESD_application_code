# Cross-compiler (if any)
CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc

# Compiler flags
CFLAGS ?= -g -Wall -Werror

# Target executable
TARGET ?= server

# Directories
SRC_DIR := .
TEMP_SENSOR_DIR := ../MLX_code

# Source files
SRC := $(wildcard $(SRC_DIR)/*.c)
SRC += $(wildcard $(TEMP_SENSOR_DIR)/*.c)

# Include directories
INCLUDES := -I$(TEMP_SENSOR_DIR)

# Linker flags
LDFLAGS :=

# Main target
all: $(TARGET)

# Build the lcd_test executable
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET)

.PHONY: 
	clean

