#ifndef TEST_H
#define TEST_H

#include <stdio.h>

static int TESTs_run, TESTs_failed, TESTs_asserts, TEST_failed;
static void (*TEST_setup)(), (*TEST_teardown)();

#define RUN_SUITE(suite, setup, teardown) \
	do {                              \
		TEST_setup = setup;       \
		TEST_teardown = teardown; \
		suite();                  \
	} while (0)

#define RUN_TEST(test)                   \
	do {                             \
		if (TEST_setup)          \
			TEST_setup();    \
		TEST_failed = 0;         \
		test();                  \
		TESTs_run++;             \
		if (TEST_failed)         \
			TESTs_failed++;  \
		if (TEST_teardown)       \
			TEST_teardown(); \
	} while (0)

#define ASSERT(test)                                                          \
	do {                                                                  \
		TESTs_asserts++;                                              \
		if (!(test)) {                                                \
			TEST_failed = 1;                                      \
			fprintf(stderr, "%s failed:\n\t%s:%d: `%s`\n",        \
					__func__, __FILE__, __LINE__, #test); \
		}                                                             \
	} while (0)

#define ASSERTSTREQ(str1, str2) ASSERT(!strcmp((str1), (str2)))
#define ASSERTMEMEQ(ptr1, ptr2, sz) ASSERT(!memcmp((ptr1), (ptr2), (sz)))

#define REPORT()                                                           \
	do {                                                               \
		printf("\n\n%d test(s), %d assertion(s), %d failure(s)\n", \
				TESTs_run, TESTs_asserts, TESTs_failed);   \
		if (!TESTs_failed)                                         \
			printf("\nALL TESTS PASSED!\n");                   \
	} while (0)

#define SUITE(x) static void x()
#define TEST(x) SUITE(x)
#define SETUP(x) SUITE(x)
#define TEARDOWN(x) SUITE(x)

#endif /* TEST_H */
