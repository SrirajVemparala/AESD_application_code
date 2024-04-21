# Define the compiler
CC ?= $(CROSS_COMPILE)gcc

# Define compiler flags
CFLAGS ?= -g -Wall -Wextra

# Define the libraries to link against
LIBS ?= -lwiringPi -lwiringPiDev -pthread

# Define the source file
SRC = aesd-lcd.c \
	lcd.c \
	wiringPi.c \
	wiringPiLegacy.c \
	softTone.c \
	softPwm.c

# Define the output executable
TARGET = aesd-lcd

# Default target
all: $(TARGET)

# Rule to compile the source file and link against libraries
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean rule
clean:
	rm -f $(TARGET)

