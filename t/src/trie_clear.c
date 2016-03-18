#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	plan(4);

	trie_init(&trie);

	trie_set(&trie, "foo", 10);
	trie_set(&trie, "foobar", 20);

	ok(trie.size, "Trie is not empty");

	trie_clear(&trie);

	ok(!trie.size, "Now it is");
	ok(!trie_getp(&trie, "foo"), "There is no `foo` anymore");
	ok(!trie_getp(&trie, "foobar"), "And no `foobar`");

	trie_release(&trie);

	done_testing();
}
