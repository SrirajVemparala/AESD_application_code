CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =

TARGET = aesd-lcd

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
