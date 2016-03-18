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

P := $(addprefix bin/, $(basename $(TESTS)))
R := $(addprefix src/, $(TESTS))

all: bin/ $(P)

bin/:
	@mkdir bin

$(P): $(R)
	$(eval F := $(firstword $(R)))
	$(eval R := $(R:$(F)=))
	$(CC) $(CFLAGS) -o $@ $(SOURCES) $(F)
	$(eval tfile := $(basename $(notdir $@)).t)
	@echo "#!/usr/bin/perl"                     > $(tfile)
	@echo "\$$0 =~ /(.*)(\/|\\\\)(.*)\.t\$$/;" >> $(tfile)
	@echo "exec(\"\$$1\$$2bin\$$2\$$3\");"     >> $(tfile)

clean:
	@$(RM) -rf bin *.t
