#include "tap.h"
#include "trie.h"

struct trie(int) trie;

void checkvalue(void *buf, void *userp)
{
	int value = *(int *)buf;

	(void)userp;
	is(value, 10, "%d", "`foo` had the value 10");
}

void checkuserp(void *buf, void *userp)
{
	int uservalue = *(int *)userp;

	(void)buf;
	is(uservalue, 20, "%d", "The user-pointer has the value 20");
}

int main()
{
	plan(4);

	trie_init(&trie);

	trie_setdtor(&trie, checkvalue);
	trie_set(&trie, "foo", 10);
	trie_cut(&trie, "foo");

	trie_set(&trie, "foo", 10);
	trie_clear(&trie);

	trie_set(&trie, "foo", 10);
	trie_release(&trie);

	trie_init(&trie);

	trie_setdtor(&trie, checkuserp);
	trie_setuserp(&trie, &(int){20});
	trie_set(&trie, "foo", 10);

	trie_release(&trie);

	done_testing();
}
