trie.c
======
A type-safe and generic C99-compliant associative array (`string => T`) implemented as a simple trie.

## Example

```c
    struct trie(double) numbers;
    double ret;

    tr_init(&numbers);
    tr_set(&numbers, "1", 1.0);
    tr_set(&numbers, "2", 2.0);
    tr_set(&numbers, "3", 3.0);
    tr_get(&numbers, "3", &ret);
    tr_set(&numbers, "3", ret + 1.0);
    if (!tr_has(&numbers, "4"))
        printf("There is no key \"4\".\n");
    tr_release(&numbers);
```

## Installation

Use the makefile to compile a static/shared library or simply drop [trie.c](trie.c) and [trie.h](trie.h) into your project.

## Usage

See [trie.h](trie.h) for more details.

#### Creating

A trie can be of any type and is created like this:

```c
    struct trie(int) myints;
    struct trie(char *) mycharptrs;
    struct trie myvoidptrs; /* see below */
    struct trie(struct xyz) mystructs;
```

Before you can use or access the trie, it has to be initialized:

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

###### Note

If you want to pass a trie between functions, you have to pre-declare its type:

```c
    struct int_trie trie(int);

    void print_len(struct int_trie *ptr)
    {
        printf("%zu\n", ptr->len);
    }

    int main()
    {
        struct int_trie myints;

        tr_init(&myints);
        tr_set(&myints, "bla", 10);
        print_len(&myints);
        tr_release(&myints);
        return 0;
    }
```

`struct trie` is already declared as `struct trie(void *)`.

#### Setting

  * `tr_setp(trie, key)`
  * `tr_set(trie, key, value)`

#### Getting

  * `tr_getp(trie, key)`
  * `tr_get(trie, key, ret)`
  * `tr_has(trie, key)`

#### Deleting

  * `tr_cut(trie, key)`

###### Note

You can set a destructor function that is called for every key that is to be cut (either by `tr_cut()` or `tr_release()`):

```c
    void mydtor(void *buf, void *userp)
    {
        void *ptr = *(void **)buf;
        size_t *cnt = userp;

        free(ptr);
        printf("There are still %zu left.\n", --(*cnt));
    }

    tr_setdtor(&myptrs, mydtor);
    tr_setuserp(&myptrs, &ptrcount); /* optional */
```

In case your user pointer is the trie itself:  
when using `tr_release()`, the trie's length is not updated before every key is cut, so accessing its length inside the destructor won't work.

#### Iterating

  * `tr_iter(trie, callback, userp)`

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

#### Replacing malloc()

```c
    tr_setmalloc(xmalloc);
    tr_setfree(xfree);
```

This redirects further calls for any trie. Your `malloc()` replacement could e.g. just `exit(-1)` like GNU's `xmalloc()` if there is no memory left, so you spare checking every `tr_set()` and `tr_iter()` wether they succeeded, if you wouldn't react differently anyways.

## License

See [LICENSE](LICENSE).
