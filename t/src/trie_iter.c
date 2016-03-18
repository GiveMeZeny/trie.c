#include "tap.h"
#include "trie.h"

struct trie(int) trie;
char *strs[] = {"foo", "bar", "baz"};

void callback(const char *key, void *buf, void *userp)
{
	int val = *(int *)buf;
	int cut = *(int *)userp;

	isstr(key, strs[val], "key == strs[value]");
	if (cut)
		trie_cut(&trie, key);
}

int main()
{
	plan(2 + 3 * 2);

	trie_init(&trie);

	trie_set(&trie, strs[0], 0);
	trie_set(&trie, strs[1], 1);
	trie_set(&trie, strs[2], 2);

	trie_iter(&trie, callback, &(int){0});
	is(trie.size, (size_t)3, "%zu", "Trie still has three keys");

	trie_iter(&trie, callback, &(int){1});
	is(trie.size, (size_t)0, "%zu", "Now it's empty");

	trie_release(&trie);

	done_testing();
}
