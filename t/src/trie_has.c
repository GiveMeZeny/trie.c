#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	plan(3);

	trie_init(&trie);

	ok(!trie_has(&trie, "foo"), "There is no `foo` yet");

	trie_set(&trie, "foo", 10);

	ok(trie_has(&trie, "foo"), "Now there is");
	ok(!trie_has(&trie, "foobar"), "But there is no `foobar`");

	trie_release(&trie);

	done_testing();
}
