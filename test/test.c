#include "../trie.h"
#include "test.h"

struct trie(int) trie;

SETUP(test_setup)
{
	tr_init(&trie);
}

TEARDOWN(test_teardown)
{
	tr_release(&trie);
}

TEST(test_tr_setp)
{
	int *a = tr_setp(&trie, "foo");
	int *b = tr_setp(&trie, "bar");
	int *c = tr_setp(&trie, "foo");
	int *d = tr_setp(&trie, "");

	ASSERT(trie.len == 3);
	ASSERT(a && b && c && d);
	ASSERT(a != b && b != c && c != d && a != d && b != d && a == c);
}

TEST(test_tr_set)
{
	ASSERT(tr_set(&trie, "foo", 1));
	ASSERT(tr_set(&trie, "bar", 2));
	ASSERT(tr_set(&trie, "foo", 3));
	ASSERT(tr_set(&trie, "fooo", 4));
	ASSERT(tr_set(&trie, "", 5));
	ASSERT(trie.len == 4);
}

TEST(test_tr_getp)
{
	tr_set(&trie, "foo", 1);
	ASSERT(tr_getp(&trie, "foo"));
	ASSERT(!tr_getp(&trie, "fooo"));
	tr_set(&trie, "fooo", 2);
	ASSERT(tr_getp(&trie, "fooo"));
}

TEST(test_tr_get)
{
	int a, b, c;

	tr_set(&trie, "foo", 1);
	tr_set(&trie, "fooo", 2);
	ASSERT(tr_get(&trie, "foo", &a));
	ASSERT(tr_get(&trie, "fooo", &b));
	ASSERT(!tr_get(&trie, "bar", &c));
	ASSERT(a == 1);
	ASSERT(b == 2);
}

TEST(test_tr_has)
{
	tr_set(&trie, "foo", 1);
	ASSERT(tr_has(&trie, "foo"));
	ASSERT(!tr_has(&trie, "fooo"));
	tr_set(&trie, "fooo", 2);
	ASSERT(tr_has(&trie, "fooo"));
}

TEST(test_tr_cut)
{
	tr_set(&trie, "foo", 1);
	tr_set(&trie, "fooo", 1);
	tr_set(&trie, "bar", 1);
	ASSERT(tr_cut(&trie, "fooo"));
	ASSERT(trie.len == 2);
	ASSERT(tr_has(&trie, "foo"));
	ASSERT(tr_has(&trie, "bar"));
	ASSERT(!tr_has(&trie, "fooo"));
	ASSERT(!tr_cut(&trie, "fooo"));
}

void iter_callb(const char *key, void *buf, void *userp)
{
	int val = *(int *)buf;
	size_t i = strtol(key, NULL, 10);
	int *out = userp;

	out[i] = val;
	ASSERT(tr_cut(&trie, key));
}

TEST(test_tr_iter)
{
	char buf[2];
	int a[] = {7, 8, 9, 4, 5, 6, 1, 2, 3};
	int b[sizeof(a) / sizeof(*a)];
	size_t i;

	for (i = 0; i < sizeof(a) / sizeof(*a); i++) {
		snprintf(buf, sizeof(buf), "%d", i);
		tr_set(&trie, buf, a[i]);
	}
	ASSERT(trie.len == 9);
	ASSERT(tr_iter(&trie, iter_callb, &b));
	ASSERTMEMEQ(a, b, sizeof(a));
	ASSERT(!trie.len);
}

void dtor_callb(void *buf, void *userp)
{
	int val = *(int *)buf;
	size_t *j = userp;

	ASSERT(val >= 0 && val < 20);
	(*j)++;
}

TEST(test_tr_release)
{
	char buf[3];
	size_t i, j = 0;

	for (i = 0; i < 20; i++) {
		snprintf(buf, sizeof(buf), "%d", i);
		tr_set(&trie, buf, i);
	}
	tr_setdtor(&trie, dtor_callb);
	tr_setuserp(&trie, &j);
	tr_release(&trie);
	ASSERT(!trie.len);
}

SUITE(all_tests)
{
	RUN_TEST(test_tr_setp);
	RUN_TEST(test_tr_set);
	RUN_TEST(test_tr_getp);
	RUN_TEST(test_tr_get);
	RUN_TEST(test_tr_has);
	RUN_TEST(test_tr_cut);
	RUN_TEST(test_tr_iter);
	RUN_TEST(test_tr_release);
}

int main()
{
	RUN_SUITE(all_tests, test_setup, test_teardown);
	REPORT();
	return 0;
}
