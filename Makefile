SRC_DIR = $(PWD)/$(day)

CC = gcc
CFLAGS = -Wall -Wextra -Werror -Wpedantic --debug -I./include
TARGET = solution

all: $(TARGET)

$(TARGET):
	$(CC) $(CFLAGS) $(SRC_DIR)/solution.c -o $(TARGET)

clean:
	rm -f $(TARGET)
