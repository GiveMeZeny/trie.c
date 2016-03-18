#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	plan(1);

	trie_init(&trie);

	is(trie.size, (size_t)0, "%zu", "Trie is empty");

	trie_release(&trie);

	done_testing();
}
