// SPDX-License-Identifier: GPLv2-or-later
/*
 * analyzer-malloc-leak-main.c - our main program
 */

#include "analyzer-malloc-leak.h"

static string string0 = {
	.s = "string0",
	.sz = sizeof("string0") - 1
};
static string string1 = {
	.s = "string1",
	.sz = sizeof("string1") - 1
};
static string *stringpa[] = { &string0, &string1 };
static string **strings = &stringpa[0];

int
main(void)
{
	setvbuf(stdout, NULL, _IONBF, 0);

#if defined(CALL_TEST0)
	char *buf0 = NULL;
	size_t buf0sz = 0;
	int rc0;

	rc0 = no_leak_0(strings, 2, (handle)&buf0, &buf0sz);
	if (rc0 < 0)
		errx(1, "no_leak_0 broke");
	printf("no_leak_0: \"");
	write(STDOUT_FILENO, buf0, buf0sz);
	printf("\"\n");
	free(buf0);
#endif /* defined(CALL_TEST0) */

#if defined(CALL_TEST1)
	char *buf1 = NULL;
	size_t buf1sz = 0;

	buf1 = no_leak_1(strings, 2, NULL, &buf1sz);
	if (!buf1)
		errx(2, "no_leak_1 broke");
	printf("no_leak_1: \"");
	write(STDOUT_FILENO, buf1, buf1sz);
	printf("\"\n");
	free(buf1);
#endif /* defined(CALL_TEST1) */

	return 0;
}

// vim:fenc=utf-8:tw=75:noet
