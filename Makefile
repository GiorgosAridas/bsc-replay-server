CC = gcc
CFLAGS = -Wall -Wextra -g
TARGET = replayer
SRC = replayer.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)