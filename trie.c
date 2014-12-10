#include <stdlib.h>
#include "trie.h"

struct trie_node {
	struct trie_node *next;
	struct trie_node *children;
	char key;
	void *buf[];
};

static tr_xalloc tr_xmalloc = malloc;
static tr_xdealloc tr_xfree = free;

void tr_setmalloc(tr_xalloc routine)
{
	tr_xmalloc = routine;
}

void tr_setfree(tr_xdealloc routine)
{
	tr_xfree = routine;
}

static inline struct trie_node *createnode(char key, size_t size)
{
	struct trie_node *node = tr_xmalloc(sizeof(*node) + size);

	if (!node)
		return NULL;
	node->next = NULL;
	node->children = NULL;
	node->key = key;
	return node;
}

void *(tr_setp)(struct trieb *tr, const char *key)
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
	tr->len++;
	if (tr->maxh < height)
		tr->maxh = height;
	return &(*node)->buf;
}

void *(tr_getp)(struct trieb *tr, const char *key)
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

		tr_xfree(*node);
		*node = next;
		if (sibling || *node)
			retval = 1;
	}
	return retval;
}

int (tr_cut)(struct trieb *tr, const char *key)
{
	if (!cutnode(tr, &tr->root, key, 1))
		return 0;
	tr->len--;
	return 1;
}

static void walknode(struct trie_node *node, char *key, char *k,
		     tr_itercb callb, void *userp)
{
	struct trie_node *next;
	char c;

	if (!node)
		return;
	next = node->next;
	c = node->key;
	*k = c;
	walknode(node->children, key, k + 1, callb, userp);
	if (!c)
		callb(key, &node->buf, userp);
	walknode(next, key, k, callb, userp);
}

int (tr_iter)(struct trieb *tr, tr_itercb callb, void *userp)
{
	char *key;

	if (!tr->maxh)
		return 1;
	key = tr_xmalloc(tr->maxh);
	if (!key)
		return 0;
	walknode(tr->root, key, key, callb, userp);
	tr_xfree(key);
	return 1;
}

void destroynode(struct trieb *tr, struct trie_node *node)
{
	if (!node)
		return;
	destroynode(tr, node->next);
	destroynode(tr, node->children);
	if (!node->key && tr->dtor)
		tr->dtor(&node->buf, tr->userp);
	tr_xfree(node);
}
