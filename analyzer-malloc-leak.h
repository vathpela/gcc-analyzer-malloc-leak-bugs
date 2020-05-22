// SPDX-License-Identifier: GPLv2-or-later
/*
 * analyzer-malloc-leak.h - header
 */

#ifndef ANALYZER_MALLOC_LEAK_H_
#define ANALYZER_MALLOC_LEAK_H_

#include <err.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

typedef void * handle;

struct string {
	char *s;
	size_t sz;
};
typedef struct string string;

#if defined(DEFINE_TEST0)
int no_leak_0(string **in, size_t insz, handle *outp, size_t *outszp);
#endif /* defined(DEFINE_TEST0) */

#if defined(DEFINE_TEST1)
handle __attribute__((__malloc__)) no_leak_1(string **in, size_t insz, handle *outp, size_t *outszp);
#endif /* defined(DEFINE_TEST1) */

#endif /* !ANALYZER_MALLOC_LEAK_H_ */
// vim:fenc=utf-8:tw=75:noet
