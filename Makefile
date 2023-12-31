CC = clang
CFLAGS = -O0 -Wall -Wextra -pedantic -fsanitize=address -fno-omit-frame-pointer -g
CLIBS = -I/lib/uthash-master/include

all: solution update

solution: solution.c common.h
	$(CC) $(CFLAGS) -o solution solution.c $(CLIBS)

update: update.c
	$(CC) $(CFLAGS) -o update update.c

lsp:
	bear -- make

clean:
	rm -rf solution .cache
