trie.c
======
A type-safe and generic C99-compliant associative array (_string => T_) implemented as a trie.

## Example

```c
    struct trie(double) numbers;
    double ret;

    tr_init(&numbers);
    tr_set(&numbers, "1", 1.0);
    tr_set(&numbers, "2", 2.0);
    tr_set(&numbers, "3", 4.0);
    tr_set(&numbers, "3", 3.0);
    if (tr_has(&numbers, "3"))
        tr_get(&numbers, "3", &ret);
    tr_release(&numbers);
```
See _trie.h_ for documenting comments.

## Creating

A trie is created like this:
```c
    struct trie(int) myints;
    struct trie(char *) mycharps;
    struct trie myvoidps; /* shortcut for trie(void *) */
    struct trie(struct xyz) mystructs;
```
Before you can use or access it, you need to initialize it:
```c
    tr_init(&mytrie);
```
After you're finished, release it:
```c
    tr_release(&mytrie);
```
This deletes all of its keys and reinitializes the trie.

To retrieve the number of keys a trie contains:
```c
    size_t length = mytrie.len;
```

##### Note

If you need to pass a trie to other functions, you should declare its type like this:
```c
    struct int_trie trie(int);

    void fun2(struct int_trie *ptr)
    {
        printf("%zu\n", ptr->len);
    }

    int main()
    {
        struct int_trie myints;

        tr_init(&myints);
        tr_set(&myints, "bla", 5);
        tr_release(&myints);
        return _0_;
    }
```
`struct trie` is already declared as `struct trie(void *)` for convenience.

## Setting

#### tr_setp(trie, key)
Returns a pointer to the key's value, if `trie` already contains `key`. If it doesn't, `key` is created and a pointer to its value is returned, or `NULL` if `malloc()` failed.

#### tr_set(trie, key, value)
Overwrites the key's value to `value` and returns _1_, if `trie` already contains `key`. If it doesn't, `key` is created, its value is set to `value` and _1_ is returned, or _0_ if `malloc()` failed, in which case `value` is not evaluated (means if `value` is equal to `strdup("bla")`, then `strdup()` is _not_ called).

## Getting

#### tr_getp(trie, key)
Returns a pointer to the key's value, or `NULL` if it doesn't exist yet.

#### tr_get(trie, key, ret)
If `trie` contains `key`, then `ret` receives its value and _1_ is returned. Elsewise _0_ is returned and `ret` remains untouched.

#### tr_has(trie, key)
Returns _1_ if `trie` contains `key`, or _0_ if it doesn't.

## Deleting

#### tr_cut(trie, key)
Returns _1_ if `key` was successfully cut from `trie`, or _0_ if `trie` doesn't contain `key`.

##### Note
You can set a deconstructor function that is called for every key that is cut (either by `tr_cut()` or `tr_release()`):
```c
    void mydtor(void *buf, void *some_ptr)
    {
        int value = *(int *)buf;
        char *mes = some_ptr;

        /* If the trie contained pointers to allocated memory, you could free them here */
    }

    tr_setdtor(&mytrie, mydtor);
    tr_setuserp(&mytrie, some_ptr); /* optional */
```

## Iterating

#### tr_iter(trie, callback, userp)
`callback` is called for every key the trie contains with an optional arbitrary user pointer (in an unspecified order):
```c
    struct strtrie trie(char *);

    void print_name(const char *key, void *buf, void *userp)
    {
        char *name = *(char **)buf;
        struct strtrie *trie = userp;

        if (!strcmp(name, "whatever"))
            tr_cut(trie, key); /* only defined when called on the current key! */
    }

    struct strtrie some_names;
    /* ... */
    tr_iter(&some_names, print_name, &some_names)
```
Returns _1_ on success, or _0_ if `malloc()` failed, in which case `callback` is called not once.

## Replacing malloc()

You can use your own allocator functions:
```c
    tr_setmalloc(xmalloc);
    tr_setfree(xfree);
```
This redirects further calls for any trie. Your `malloc()` replacement could e.g. just `exit(-1)` like GNU's `xmalloc()` if there is no memory left, so you spare checking every `tr_set()` and `tr_iter()` wether they succeeded, if you wouldn't react differently anyways.
