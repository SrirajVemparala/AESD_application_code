CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS =

TARGET = mlxtest

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET)
