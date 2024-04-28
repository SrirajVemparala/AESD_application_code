# Define the compiler
CC ?= $(CROSS_COMPILE)gcc

# Define compiler flags
CFLAGS ?= -g -Wall -Wextra

# Define the libraries to link against
LIBS ?= -pthread

# Define the source file
SRC = 	client.c \
	lcd.c \
	wiringPi.c \


# Define the output executable
TARGET = client

# Default target
all: $(TARGET)

# Rule to compile the source file and link against libraries
$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) $(SRC) $(LIBS)

# Clean rule
clean:
	rm -f $(TARGET)

