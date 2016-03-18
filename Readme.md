trie.c
======
A type-safe and generic C99-compliant associative array (`string => T`) implemented as a simple trie.

## Example

```c
    struct trie(double) t;
    /*     Trie(double) t; */
    double ret;

    trie_init(&t);

    trie_set(&t, "foo", 10.0);
    trie_set(&t, "bar", 20.0);

    if (trie_has(&t, "baz"))
        trie_cut(&t, "baz");

    trie_get(&t, "foo", &ret);
    printf("t['foo'] = %g\n", ret);

    trie_release(&t);
```

## Installation

Invoke `make` to compile a static library or simply drop [trie.c](trie.c) and [trie.h](trie.h) into your project.

## Usage

Everything's documented in [trie.h](trie.h).

  * It's __not__ safe to pass parameters that have side effects like `i++` or whatever, because often parameters are evaluated more than once (exception: see `trie_set()`).

#### Creating

A trie can be of any type and is created like this:

```c
    struct trie(void *) myvoidptrs;
    struct trie(int) myints;
    struct trie(char *) mycharptrs;
    struct trie(struct xyz) mystructs;
    struct trie(struct trie(int)) mytries;
```

Before you can use the trie, it has to be initialized:

```c
    trie_init(&t);
```

After you're finished, release it:

```c
    trie_release(&t);
```

This removes all of its keys and reinitializes the trie.

#### Type declaration

If you want to have a trie in a function's parameter list, you have to declare its type to keep it std-compliant:

```c
    struct trie_Pos trie(struct Pos);

    void print_size(struct trie_Pos *ptr)
    {
        printf("%zu\n", ptr->size);
    }

    int main()
    {
        struct trie_Pos t;

        trie_init(&t);
        trie_set(&t, "foo", (struct Pos){.x=1,.y=1});
        print_size(&t);
        trie_release(&t);
        return 0;
    }
```

These types are already declared:

```c
    struct trie trie(void *);
    struct trie_int trie(int);
    struct trie_long trie(long);
    struct trie_vlong trie(long long);
    struct trie_size_t trie(size_t);
    struct trie_double trie(double);
    struct trie_char trie(char);
    struct trie_charptr trie(char *);

    /* examples from above: */
    struct trie myvoidptrs;
    struct trie_int myints;
    struct trie_charptr mycharptrs;
```

#### Size

To get the number of keys a trie contains, use:

```c
    size_t num = t.size;

    printf("t.size = %zu\n", num);
```

#### Setting

  * `trie_set(trie, key, value)`
  * `trie_setp(trie, key)`

#### Getting

  * `trie_get(trie, key, ret)`
  * `trie_getp(trie, key)`
  * `trie_has(trie, key)`

#### Deleting

  * `trie_clear(trie)`
  * `trie_cut(trie, key)`

#### Iterating

  * `trie_iter(trie, callback, userp)`

```c
    void print_name(const char *key, void *buf, void *userp)
    {
        char *name = *(char **)buf;
        struct trie_charptr *trie = userp;

        if (strcmp(name, "Hans") != 0)
            puts(name);
        else
            trie_cut(trie, key); /* _only_ the current key is removable */
    }

    struct trie_charptr mynames;
    /* ... */
    trie_iter(&mynames, print_name, &mynames)
```

#### Callbacks

You can set an optional destructor that is called for elements that are to be removed by `trie_cut()`, `trie_clear()` and `trie_release()` and a user-pointer that gets passed along. For its prototype see [trie.h](trie.h).

  * `trie_setdtor(trie, dtor)`
  * `trie_setuserp(trie, ptr)`

```c
    void free_foo(void *buf, void *userp)
    {
        struct foo *s = buf;

        (void)userp;
        free(s->xyz);
    }

    struct trie(struct foo) t;
    
    trie_init(&t);
    trie_setdtor(&t, free_foo);
    trie_release(&t);
```

When using `trie_release()`, the trie's size is not updated before every key is cut, so accessing its size inside the destructor won't work.

The callbacks `trie_cb_freevoidptr()` and `trie_cb_freecharptr()` are already defined (see [trie.c](trie.c)).

#### Replacing malloc()

```c
    trie_use_as_realloc(xmalloc); /* trie_* will use xmalloc(size) */
    trie_use_as_free(xfree); /* trie_* will use xfree(ptr) */
```

## License

See [LICENSE](LICENSE).
