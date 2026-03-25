CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

SRC = main.c src/draw.c src/codes.c src/model.c src/input.c src/move.c src/pose.c src/queue.c
OBJ = $(addprefix build/,$(notdir $(SRC:.c=.o)))
TARGET = tdraw

.PHONY: all clean fclean re run

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ)

fclean: clean
	rm -rf build $(TARGET)

re: fclean all