CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -g -O2 \
          -D_POSIX_C_SOURCE=200809L -D_GNU_SOURCE
LDFLAGS = -lreadline -lncurses
SRC_DIR = src
BUILD   = build
TARGET  = coreshell

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD)/%.o)

.PHONY: all clean install test debug valgrind

all: $(BUILD) $(TARGET)

$(BUILD):
	mkdir -p $(BUILD)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILD)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

debug: CFLAGS += -DDEBUG -fsanitize=address,undefined
debug: LDFLAGS += -fsanitize=address,undefined
debug: all

valgrind: all
	valgrind --leak-check=full --show-leak-kinds=all \
	         --track-origins=yes --verbose ./$(TARGET)

clean:
	rm -rf $(BUILD) $(TARGET)
