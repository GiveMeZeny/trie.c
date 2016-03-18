#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	plan(8);

	trie_init(&trie);

	ok(trie_set(&trie, "foo", 10), "Add `foo` and set it to 10");
	is(trie.size, (size_t)1, "%zu", "`foo` was added");

	ok(trie_set(&trie, "foobar", 20), "Add `foobar` and set it to 20");
	is(trie.size, (size_t)2, "%zu", "`foobar` was added");

	ok(trie_set(&trie, "foo", 30), "Set `foo` to 30");
	is(trie.size, (size_t)2, "%zu", "No key was added because `foo` already exists");

	ok(trie_set(&trie, "", 50), "Add `` and set it to 50");
	is(trie.size, (size_t)3, "%zu", "`` was added");

	trie_release(&trie);

	done_testing();
}
