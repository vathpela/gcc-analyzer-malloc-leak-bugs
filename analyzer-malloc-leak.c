// SPDX-License-Identifier: GPLv2-or-later
/*
 * analyzer-malloc-leak.c - show gcc being wrong
 */

#include "analyzer-malloc-leak.h"

#if defined(DEFINE_TEST0)
int
no_leak_0(string **in, size_t insz, handle *outp, size_t *outszp)
{
	size_t outsz = 0;

	for (unsigned int i = 0; i < insz; i++)
		outsz += in[i]->sz;

	char *out = calloc(1, outsz);
	if (!out)
		return -1;

	*outp = out;
	*outszp = outsz;

	for (unsigned int i = 0; i < insz; i++) {
		char *entry = out;
		memcpy(entry, in[i]->s, in[i]->sz);
		out += in[i]->sz;
	}

	return 0;
}
#endif /* defined(DEFINE_TEST0) */

#if defined(DEFINE_TEST1)
handle __attribute__((__malloc__))
no_leak_1(string **in, size_t insz, handle *outp, size_t *outszp)
{
	size_t outsz = 0;

	for (unsigned int i = 0; i < insz; i++)
		outsz += in[i]->sz;

	char *out = calloc(1, outsz);
	if (!out)
		return NULL;

	handle ret = out;

	if (outp)
		*outp = out;
	*outszp = outsz;

	size_t pos = 0;
	for (unsigned int i = 0; i < insz; i++) {
		char *entry = &out[pos];
		memcpy(entry, in[i]->s, in[i]->sz);
		pos += in[i]->sz;
	}

	return ret;
}
#endif /* defined(DEFINE_TEST1) */

// vim:fenc=utf-8:tw=75:noet
