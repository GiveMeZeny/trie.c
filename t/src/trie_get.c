#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	int foo, foobar;

	plan(6);

	trie_init(&trie);

	ok(!trie_get(&trie, "foo", &foo), "There is no `foo` yet");
	ok(!trie_get(&trie, "foobar", &foobar), "There is no `foobar` yet");

	trie_set(&trie, "foo", 10);
	trie_set(&trie, "foobar", 20);

	ok(trie_get(&trie, "foo", &foo), "Retrieved `foo`'s value");
	is(foo, 10, "%d", "`foo`'s value is 10");

	ok(trie_get(&trie, "foobar", &foobar), "Retrieved `foobar`'s value");
	is(foobar, 20, "%d", "`foobar`'s value is 20");

	trie_release(&trie);

	done_testing();
}
