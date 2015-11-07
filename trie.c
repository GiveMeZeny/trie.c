#include <stdlib.h>
#include "trie.h"

struct trie_node {
	struct trie_node *next;
	struct trie_node *children;
	char key;
	void *buf[];
};

void trie_cb_freevoidptr(void *buf, void *userp)
{
	(void)userp;
	free(*(void **)buf);
}

void trie_cb_freecharptr(void *buf, void *userp)
{
	(void)userp;
	free(*(char **)buf);
}

static trie_xalloc_t trie_xmalloc = malloc;
static trie_xdealloc_t trie_xfree = free;

void trie_use_as_malloc(trie_xalloc_t routine)
{
	trie_xmalloc = routine;
}

void trie_use_as_free(trie_xdealloc_t routine)
{
	trie_xfree = routine;
}

static inline struct trie_node *createnode(char key, size_t size)
{
	struct trie_node *node = trie_xmalloc(sizeof(*node) + size);

	if (!node)
		return NULL;
	node->next = NULL;
	node->children = NULL;
	node->key = key;
	return node;
}

void *(trie_setp)(struct trieb *tr, const char *key)
{
	struct trie_node **node;
	size_t height = 1;
	char k;

	for (node = &tr->root; *node;) {
		if ((*node)->key != *key) {
			node = &(*node)->next;
			continue;
		}
		if (!*key)
			return &(*node)->buf;
		node = &(*node)->children;
		height++;
		key++;
	}
	for (; (k = *key++); node = &(*node)->children, height++) {
		*node = createnode(k, tr->sz);
		if (!*node)
			return NULL;
	}
	*node = createnode('\0', tr->sz);
	if (!*node)
		return NULL;
	tr->size++;
	if (tr->maxh < height)
		tr->maxh = height;
	return &(*node)->buf;
}

void *(trie_getp)(struct trieb *tr, const char *key)
{
	struct trie_node *node;

	for (node = tr->root; node;) {
		if (node->key != *key) {
			node = node->next;
			continue;
		}
		if (!*key)
			return &node->buf;
		node = node->children;
		key++;
	}
	return NULL;
}

static int cutnode(struct trieb *tr, struct trie_node **node, const char *key,
                   int sibling)
{
	int retval;

	if (!*node)
		return 0;
	if ((*node)->key == *key) {
		if (!*key) {
			if (tr->dtor)
				tr->dtor(&(*node)->buf, tr->userp);
			retval = -1;
		} else {
			retval = cutnode(tr, &(*node)->children, key + 1, 0);
		}
	} else {
		retval = cutnode(tr, &(*node)->next, key, 1);
	}
	if (retval < 0) {
		struct trie_node *next = (*node)->next;

		trie_xfree(*node);
		*node = next;
		if (sibling || *node)
			retval = 1;
	}
	return retval;
}

int (trie_cut)(struct trieb *tr, const char *key)
{
	if (!cutnode(tr, &tr->root, key, 1))
		return 0;
	tr->size--;
	return 1;
}

static void walknode(struct trie_node *node, char *key, char *k,
                     trie_itercb_t func, void *userp)
{
	struct trie_node *next;
	char c;

	if (!node)
		return;
	next = node->next;
	c = node->key;
	*k = c;
	walknode(node->children, key, k + 1, func, userp);
	if (!c)
		func(key, &node->buf, userp);
	walknode(next, key, k, func, userp);
}

int (trie_iter)(struct trieb *tr, trie_itercb_t func, void *userp)
{
	char *key;

	if (!tr->maxh)
		return 1;
	key = trie_xmalloc(tr->maxh);
	if (!key)
		return 0;
	walknode(tr->root, key, key, func, userp);
	trie_xfree(key);
	return 1;
}

void trie_destroynode(struct trieb *tr, struct trie_node *node)
{
	if (!node)
		return;
	trie_destroynode(tr, node->next);
	trie_destroynode(tr, node->children);
	if (!node->key && tr->dtor)
		tr->dtor(&node->buf, tr->userp);
	trie_xfree(node);
}
