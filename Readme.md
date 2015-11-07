trie.c
======
A type-safe and generic C99-compliant associative array (`string => T`) implemented as a simple trie.

## Example

```c
    struct trie(double) numbers;
    double ret;

    trie_init(&numbers);
    trie_set(&numbers, "1", 1.0);
    trie_set(&numbers, "2", 2.0);
    trie_set(&numbers, "3", 3.0);
    trie_get(&numbers, "3", &ret);
    trie_set(&numbers, "3", ret + 1.0);
    if (!trie_has(&numbers, "4"))
        printf("There is no key \"4\".\n");
    trie_release(&numbers);
```

## Installation

Invoke `make` to compile a static library or simply drop [trie.c](trie.c) and [trie.h](trie.h) into your project.

## Usage

See [trie.h](trie.h) for more details.

  * It's __not__ safe to pass parameters that have side effects like `i++` or whatever, because often parameters are evaluated more than once (exception: see `trie_set()`).

#### Creating

A trie can be of any type and is created like this:

```c
    struct trie(short) myshorts;
    struct trie_int myints;
    struct trie(int *) myintptrs;
    struct trie_charptr mycharptrs;
    struct trie myvoidptrs;
    struct trie(struct xyz) mystructs;
```

If you want to pass a trie between functions, you have to pre-declare its type:

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
        trie_set(&t, "bla", (struct Pos){.x=1,.y=1});
        print_size(&t);
        trie_release(&t);
        return 0;
    }
```

These are already pre-declared:

```c
    struct trie trie(void *);
    struct trie_int trie(int);
    struct trie_long trie(long);
    struct trie_vlong trie(long long);
    struct trie_size_t trie(size_t);
    struct trie_double trie(double);
    struct trie_char trie(char);
    struct trie_charptr trie(char *);
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

#### `typedef` Syntax

Instead of the way from above:

```c
    struct trie_Pos ary(struct Pos);
    
    struct trie(struct Pos) mypositions1;
    struct trie_Pos mypositions2;
```

you can also declare tries like this:

```c
    typedef Trie(struct Pos) Trie_Pos;
    
    Trie(struct Pos) mypositions1;
    Trie_Pos mypositions2;
```

To disable this syntax, use `#define ARY_STRUCT_ONLY` or `-DARY_STRUCT_ONLY`.

#### Callbacks

You can set optional destructors and user-pointers that get passed along. The destructor is called for elements that are to be removed via `trie_cut()`, `trie_clear()` or `trie_release()`. For their prototypes see [trie.h](trie.h).

```c
    void free_bla(void *buf, void *userp)
    {
        struct bla *s = buf;

        (void)userp;
        free(s->xyz);
    }

    struct trie(struct bla) t;
    
    trie_init(&t);
    trie_setdtor(&t, free_bla);
    trie_release(&t);
```

In case your user-pointer is the trie itself:  
when using `trie_release()`, the trie's size is not updated before every key is cut, so accessing its size inside the destructor won't work.

A couple of such callbacks are already defined like `trie_cb_freevoidptr()`, `trie_cb_freecharptr()`, ... (see [trie.c](trie.c)).

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

        if (strcmp(name, "whatever") != 0)
            puts(name);
        else
            trie_cut(trie, key); /* _only_ the current key is removable, see above */
    }

    struct trie_charptr some_names;
    /* ... */
    trie_iter(&some_names, print_name, &some_names)
```

#### Replacing malloc()

```c
    trie_use_as_realloc(xmalloc); /* trie_* will use xmalloc(size) */
    trie_use_as_free(xfree); /* trie_* will use xfree(ptr) */
```

## License

See [LICENSE](LICENSE).
