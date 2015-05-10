#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>

/* deconstruct the element pointed to by `buf` */
typedef void (*tr_callb)(void *buf, void *userp);
/* work with `key` and the element pointed to by `buf` */
typedef void (*tr_itercb)(const char *key, void *buf, void *userp);

typedef void *(*tr_xalloc)(size_t size);
typedef void (*tr_xdealloc)(void *ptr);

#define trie(type)                                   \
	{                                            \
		struct trieb s;                      \
		size_t len;     /* number of keys */ \
		type *ptr;                           \
	}

struct trie_node;

struct trieb {
	struct trie_node *root;
	size_t len;
	size_t maxh;
	size_t sz;
	tr_callb dtor;
	void *userp;
};

/* `struct trie x` is a `void *`-trie */
struct trie trie(void *);

/* forward declarations */
void *tr_setp(struct trieb *tr, const char *key);
void *tr_getp(struct trieb *tr, const char *key);
int tr_cut(struct trieb *tr, const char *key);
int tr_iter(struct trieb *tr, tr_itercb callb, void *userp);
void destroynode(struct trieb *tr, struct trie_node *node);

/**
 * tr_setmalloc() - set a custom allocator function
 * @routine: replacement for malloc()
 */
void tr_setmalloc(tr_xalloc routine);

/**
 * tr_setfree() - set a custom deallocator function
 * @routine: replacement for free()
 */
void tr_setfree(tr_xdealloc routine);

/**
 * tr_init() - initialize a trie
 * @tr: typed pointer to the trie
 */
#define tr_init(tr)                                         \
	do {                                                \
		(tr)->s.root = (tr)->s.userp = NULL;        \
		(tr)->s.len = (tr)->s.maxh = (tr)->len = 0; \
		(tr)->s.sz = sizeof(*(tr)->ptr);            \
		(tr)->s.dtor = NULL;                        \
	} while (0)

/**
 * tr_release() - release a trie
 * @tr: typed pointer to the trie
 *
 * All keys are deleted and @tr is reinitialized with tr_init().
 */
#define tr_release(tr)                               \
	do {                                         \
		destroynode(&(tr)->s, (tr)->s.root); \
		tr_init(tr);                         \
	} while (0)

/**
 * tr_setdtor() - set a trie's deconstructor
 * @tr: typed pointer to the trie
 * @callb: routine that releases a deleted key's value
 */
#define tr_setdtor(tr, callb) ((tr)->s.dtor = (callb), 0)

 /**
 * tr_setuserp() - set a trie's user-defined value
 * @tr: typed pointer to the trie
 * @ptr: value to set
 */
#define tr_setuserp(tr, ptr) ((tr)->s.userp = (ptr), 0)

/**
 * tr_has() - checks whether a key exists
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 *
 * Return: When @key was found 1, or otherwise 0.
 */
#define tr_has(tr, key) ((tr_getp)(&(tr)->s, (key)) != NULL)

/**
 * tr_setp() - get a pointer to a key's value
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 *
 * If @trie doesn't have @key, it is created.
 *
 * Return: Pointer to the key's value, or NULL if malloc() failed.
 */
#define tr_setp(tr, key) \
	(((tr)->ptr = (tr_setp)(&(tr)->s, (key))) ? \
	 ((tr)->len = (tr)->s.len, (tr)->ptr) : NULL)

/**
 * tr_set() - set a key's value
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 * @...: value to set
 *
 * If @tr already has @key, its value is overwritten. Otherwise @key is created.
 *
 * Return: When successful 1, or otherwise 0 if malloc() failed.
 *
 * Note!: @... is only a single value, it's denoted as varargs in order to cope
 *	with struct-literals, additionally, it is not evaluated if tr_set()
 *	failed (so no side effects).
 */
#define tr_set(tr, key, ...) \
	(((tr)->ptr = tr_setp((tr), (key))) ? \
	 (*(tr)->ptr = (__VA_ARGS__), 1) : 0)

/**
 * tr_getp() - get a pointer to a key's value
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 *
 * Return: Pointer to the key's value, or NULL if there is no such key.
 */
#define tr_getp(tr, key) tr_getp(&(tr)->s, (key))

/**
 * tr_get() - retrieve a key's value
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 * @ret: pointer that receives the retrieved value
 *
 * Return: When @key was found 1, or otherwise 0.
 */
#define tr_get(tr, key, ret) \
	(((tr)->ptr = tr_getp((tr), (key))) ? ((*(ret) = *(tr)->ptr), 1) : 0)

/**
 * tr_cut() - delete a key
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 *
 * Return: When @key was found and deleted 1, or otherwise 0.
 */
#define tr_cut(tr, key) \
	((tr_cut)(&(tr)->s, (key)) ? ((tr)->len = (tr)->s.len, 1) : 0)

/**
 * tr_iter() - iterate through all keys
 * @tr: typed pointer to the trie
 * @callb: routine that is called for every key @trie has
 * @userp: a pointer that is passed on to @callb
 *
 * Return: When successful 1, or otherwise 0 if malloc() failed.
 *
 * Note!: Within @callb you can use tr_cut(), but only on the actual element.
 */
#define tr_iter(tr, callb, userp) (tr_iter)(&(tr)->s, (callb), (userp))

#endif /* TRIE_H */
