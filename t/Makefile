TESTS := trie_init.c trie_setp.c trie_set.c trie_getp.c trie_get.c trie_clear.c trie_has.c trie_cut.c trie_iter.c trie_setdtor.c
SOURCES := ../trie.c

CFLAGS += -std=c99 -pedantic -Wall -Wextra -g -DDEBUG -fstrict-aliasing
LDFLAGS +=
LDLIBS +=
CC := gcc

# includes
CFLAGS += -I..
CFLAGS += -I../../tap.h

# defines
CFLAGS += -D_ISOC99_SOURCE
CFLAGS += -D_POSIX_C_SOURCE=200809L

include ../../tap.h/mkfile
