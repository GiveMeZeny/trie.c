#ifndef TRIE_H
#define TRIE_H

#include <stddef.h>

/* destruct the element pointed to by `buf` */
typedef void (*trie_dtorcb_t)(void *buf, void *userp);

/* work with `key` and the element pointed to by `buf` */
typedef void (*trie_itercb_t)(const char *key, void *buf, void *userp);

typedef void *(*trie_xalloc_t)(size_t size);
typedef void (*trie_xdealloc_t)(void *ptr);

/* struct size: 4x pointers + 4x size_t's */
#define trie(type)                                   \
	{                                            \
		struct trieb s;                      \
		size_t size;    /* number of keys */ \
		type *ptr;                           \
	}

struct trie_node;

struct trieb {
	struct trie_node *root;
	size_t size;
	size_t maxh;
	size_t sz;
	trie_dtorcb_t dtor;
	void *userp;
};

/* `struct trie t` is a void *-trie */
struct trie trie(void *);
/* `struct trie_xyz t` is a xyz-trie... */
struct trie_int trie(int);
struct trie_long trie(long);
struct trie_vlong trie(long long);
struct trie_size_t trie(size_t);
struct trie_double trie(double);
struct trie_char trie(char);
struct trie_charptr trie(char *);

/* predefined callbacks */
void trie_cb_freevoidptr(void *buf, void *userp);
void trie_cb_freecharptr(void *buf, void *userp);

/* forward declarations */
void *trie_setp(struct trieb *tr, const char *key);
void *trie_getp(struct trieb *tr, const char *key);
int trie_cut(struct trieb *tr, const char *key);
int trie_iter(struct trieb *tr, trie_itercb_t func, void *userp);
void trie_destroynode(struct trieb *tr, struct trie_node *node);

/**
 * trie_use_as_malloc() - set a custom allocator function
 * @routine: replacement for malloc(size)
 */
void trie_use_as_malloc(trie_xalloc_t routine);

/**
 * trie_use_as_free() - set a custom deallocator function
 * @routine: replacement for free(ptr)
 */
void trie_use_as_free(trie_xdealloc_t routine);

/**
 * trie_init() - initialize a trie
 * @tr: typed pointer to the trie
 */
#define trie_init(tr)                                         \
	do {                                                  \
		(tr)->s.root = (tr)->s.userp = NULL;          \
		(tr)->s.size = (tr)->s.maxh = (tr)->size = 0; \
		(tr)->s.sz = sizeof(*(tr)->ptr);              \
		(tr)->s.dtor = NULL;                          \
	} while (0)

/**
 * trie_release() - release a trie
 * @tr: typed pointer to the initialized trie
 *
 * All keys are removed and @tr is reinitialized with trie_init().
 */
#define trie_release(tr)                                  \
	do {                                              \
		trie_destroynode(&(tr)->s, (tr)->s.root); \
		trie_init(tr);                            \
	} while (0)

/**
 * trie_setdtor() - set a trie's destructor
 * @tr: typed pointer to the initialized trie
 * @_dtor: routine that releases a removed key's value
 */
#define trie_setdtor(tr, _dtor) \
	((tr)->s.dtor = (_dtor), (void)0)

 /**
 * trie_setuserp() - set a trie's user-pointer for the dtor
 * @tr: typed pointer to the initialized trie
 * @ptr: pointer that gets passed to the callbacks
 */
#define trie_setuserp(tr, ptr) \
	((tr)->s.userp = (ptr), (void)0)

/**
 * trie_clear() - empty a trie
 * @tr: typed pointer to the initialized trie
 */
#define trie_clear(tr)                                        \
	do {                                                  \
		trie_destroynode(&(tr)->s, (tr)->s.root);     \
		(tr)->s.root = NULL;                          \
		(tr)->s.size = (tr)->s.maxh = (tr)->size = 0; \
	} while (0)

/**
 * trie_has() - check if a trie contains a given key
 * @tr: typed pointer to the initialized trie
 * @key: pointer to a null terminated string
 *
 * Return: When @key was found 1, or otherwise 0.
 */
#define trie_has(tr, key) \
	(trie_getp((tr), (key)) != NULL)

/**
 * trie_set() - set a given key's value in a trie
 * @tr: typed pointer to the initialized trie
 * @key: pointer to a null terminated string
 * @...: value to set
 *
 * If @tr already has @key, its value is overwritten. Otherwise @key is created.
 *
 * Return: When successful 1, or otherwise 0 if malloc() failed.
 *
 * Note!: @... is only a single value, it's denoted as varargs in order to cope
 *	with struct-literals, additionally, it is not evaluated if trie_set()
 *	failed (so e.g. `strdup(s)` has no effect on failure).
 */
#define trie_set(tr, key, ...)                  \
	(((tr)->ptr = trie_setp((tr), (key))) ? \
	 (*(tr)->ptr = (__VA_ARGS__), 1) : 0)

/**
 * trie_setp() - get a pointer to a given key's value in a trie
 * @tr: typed pointer to the initialized trie
 * @key: pointer to a null terminated string
 *
 * If @trie doesn't have @key, it is created.
 *
 * Return: Pointer to the key's value, or NULL if malloc() failed.
 */
#define trie_setp(tr, key)                              \
	(((tr)->ptr = (trie_setp)(&(tr)->s, (key))) ?   \
	 ((tr)->size = (tr)->s.size, (tr)->ptr) : NULL)

/**
 * trie_get() - retrieve a given key's value in a trie
 * @tr: typed pointer to the trie
 * @key: pointer to a null terminated string
 * @ret: pointer that receives the retrieved value
 *
 * Return: When @key was found 1, or otherwise 0.
 */
#define trie_get(tr, key, ret) \
	(((tr)->ptr = trie_getp((tr), (key))) ? ((*(ret) = *(tr)->ptr), 1) : 0)

/**
 * trie_getp() - get a pointer to a given key's value in a trie
 * @tr: typed pointer to the initialized trie
 * @key: pointer to a null terminated string
 *
 * Return: Pointer to the key's value, or NULL if there is no such key.
 */
#define trie_getp(tr, key) \
	(trie_getp)(&(tr)->s, (key))

/**
 * trie_cut() - remove a given key from a trie
 * @tr: typed pointer to the initialized trie
 * @key: pointer to a null terminated string
 *
 * Return: When @key was found and removed 1, or otherwise 0.
 */
#define trie_cut(tr, key) \
	((trie_cut)(&(tr)->s, (key)) ? ((tr)->size = (tr)->s.size, 1) : 0)

/**
 * trie_iter() - iterate through all keys of a trie
 * @tr: typed pointer to the initialized trie
 * @func: routine that is called for every key @trie has
 * @userp: a pointer that is passed to @func
 *
 * Return: When successful 1, or otherwise 0 if malloc() failed.
 *
 * Note!: Within @func you can use trie_cut(), but only on the actual element.
 */
#define trie_iter(tr, func, userp) \
	(trie_iter)(&(tr)->s, (func), (userp))

#endif /* TRIE_H */
