#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../trie.h"

void print_name(const char *key, void *buf, void *userp)
{
	char *name = *(char **)buf;
	struct trie(char *) *trie = userp;

	printf("names[\"%s\"]: \"%s\"\n", key, name);
	if (!strcmp(name, "McClane")) {
		printf("filtering out \"McClane\"...\n");
		tr_cut(trie, key);
	}
}

void free_name(void *buf, void *userp)
{
	char *name = *(char **)buf;
	int *cnt = userp;

	printf("freeing and cutting %s...\n", name);
	free(name);
	(*cnt)++;
}

void do_stuff(struct trie *names)
{
	if (!tr_has(names, "Jack"))
		tr_cut(names, "Hans");
	printf("iterating...\n");
	tr_iter(names, print_name, names);
}

int main()
{
	tr_setmalloc(xmalloc);

	struct trie names;
	int cnt = 0;

	tr_init(&names);
	tr_setdtor(&names, free_name);
	tr_setuserp(&names, &cnt);
	printf("initialized\n");
	tr_set(&names, "John", strdup("McClane"));
	tr_set(&names, "Hans", strdup("Gruber"));
	tr_set(&names, "Holly", strdup("McClane"));
	tr_set(&names, "Al", strdup("Powell"));
	do_stuff(&names);
	printf("releasing...\n");
	tr_release(&names);
	printf("free_name was called %d times.\n", cnt);

	struct trie(double) numbers;

	tr_init(&numbers);
	tr_set(&numbers, "1", 1.0);
	tr_set(&numbers, "2", 2.0);
	tr_set(&numbers, "3", 4.0);
	tr_set(&numbers, "3", 3.0);
	tr_release(&numbers);
	return 0;
}
