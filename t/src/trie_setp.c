#include "tap.h"
#include "trie.h"

struct trie(int) trie;

int main()
{
	int *foo, *fooalias, *foobar, *_;

	plan(12);

	trie_init(&trie);

	foo = trie_setp(&trie, "foo");
	ok(foo, "Add `foo` and get a pointer to its value");
	*foo = 10;
	is(trie.size, (size_t)1, "%zu", "`foo` was added");

	foobar = trie_setp(&trie, "foobar");
	ok(foobar, "Add `foobar` and get a pointer to its value");
	*foobar = 20;
	is(trie.size, (size_t)2, "%zu", "`foobar` was added");

	fooalias = trie_setp(&trie, "foo");
	ok(fooalias, "Get a pointer to foo's value");
	*fooalias = 30;
	is(trie.size, (size_t)2, "%zu", "No key was added because `foo` already exists");

	_ = trie_setp(&trie, "");
	ok(_, "Add `` and get a pointer to its value");
	*_ = 50;
	is(trie.size, (size_t)3, "%zu", "`` was added");

	isnt((void *)foo, (void *)foobar, "%p", "`foo` and `foobar` point to different values");
	is((void *)foo, (void *)fooalias, "%p", "first and second `foo` point to the same values");
	isnt((void *)foo, (void *)_, "%p", "`foo` and `` point to different values");
	isnt((void *)foobar, (void *)_, "%p", "`foobar` and `` point to different values");

	trie_release(&trie);

	done_testing();
}
