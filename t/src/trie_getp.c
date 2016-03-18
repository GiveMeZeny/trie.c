#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	int *foo;

	plan(3);

	trie_init(&trie);

	ok(!trie_getp(&trie, "foo"), "There is no `foo` yet");

	trie_set(&trie, "foo", 10);

	foo = trie_getp(&trie, "foo");
	ok(foo, "Now there is");
	*foo = 20;

	ok(!trie_getp(&trie, "foobar"), "There is no `foobar` yet");

	trie_release(&trie);

	done_testing();
}
