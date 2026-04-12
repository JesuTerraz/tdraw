CC = cc
CFLAGS = -Wall -Wextra -Werror -std=c11 -Iinclude

TEST = tests/demo

ALL = $(TEST) tdlib.a

TDLIB_SRCS = src/tdlib.c src/tdraw.c src/draw.c src/codes.c src/input.c src/pose.c src/queue.c src/logger.c
TDLIB_OBJS = $(addprefix build/,$(notdir $(TDLIB_SRCS:.c=.o)))

%: %.o
	$(LINK.o) -o $@ $^ $(LOADLIBES) $(LDLIBS)

LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)

%: %.c
%: %.cc
%: %.cpp

all: $(ALL)

tdlib.a: $(TDLIB_OBJS)
	rm -f $@
	ar rv $@ $(TDLIB_OBJS)
	ranlib $@

$(TARGET): $(TDLIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TDLIB_OBJS)

$(TEST): tdlib.a

build/%.o: src/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TDLIB_OBJS) $(ALL)

fclean: clean
	rm -rf build $(TARGET)

re: fclean all