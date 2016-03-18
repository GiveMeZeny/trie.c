#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	plan(9);

	trie_init(&trie);

	ok(!trie_cut(&trie, "foobar"), "Cannot cut `foobar` because there is none yet");

	trie_set(&trie, "foobar", 10);

	ok(!trie_cut(&trie, "foo"), "Cannot cut `foo` because there is none yet");

	trie_set(&trie, "foo", 20);

	ok(trie_cut(&trie, "foo"), "Cut `foo`");

	is(trie.size, (size_t)1, "%zu", "Trie has one key left");

	ok(!trie_has(&trie, "foo"), "There is no `foo`");
	ok(trie_has(&trie, "foobar"), "But `foobar`");

	ok(!trie_cut(&trie, "foo"), "Cannot cut `foo` because there is none anymore");
	ok(trie_cut(&trie, "foobar"), "Cut `foobar`");

	ok(!trie.size, "Trie is now empty");

	trie_release(&trie);

	done_testing();
}
