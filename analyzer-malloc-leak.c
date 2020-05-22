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

#if defined(DEFINE_TEST2)
int make_list_2(string **in, size_t insz, list_t *items)
{
	string_item *item = NULL;
	list_t *prev;
	unsigned int i = 0;

	if (!items)
		return -1;

	prev = items->prev;

	for (; i < insz; i++) {
		item = calloc(1, sizeof(*item));
		if (!item)
			goto err;

		item->s = malloc(in[i]->sz);
		if (!item->s)
			goto err;

		memmove(item->s, in[i]->s, in[i]->sz);
		item->sz = in[i]->sz;

		list_add_tail(&item->list, items);
		item = NULL;
	}

	return i;
err:
	if (item) {
		free(item);
		item = NULL;
	}

	list_t *pos, *tmp;
	bool found = false;
	unsigned int j = 0;

	list_for_each_safe(pos, tmp, items) {
		if (!found) {
			if (pos->prev != prev)
				continue;
			found = true;
		}

		if (j < i) {
			j += 1;
			continue;
		}

		item = list_entry(pos, string_item, list);
		if (item->s)
			free(item->s);
		list_del(pos);
		free(item);
	}

	return -1;
}

int
write_list_2(list_t *items)
{
	int fd;
	list_t *pos, *tmp;

	fd = open("/dev/null", O_RDWR);
	if (fd < 0)
		return fd;

	list_for_each_safe(pos, tmp, items) {
		string_item *item = NULL;

		item = list_entry(pos, string_item, list);
		if (item->s && item->sz) {
			size_t sz;

			sz = item->sz;
			while (sz) {
				int rc;

				rc = write(fd, item->s, item->sz);
				if (rc < 0)
					return rc;
				sz -= rc;
			}
			free(item->s);
			item->s = NULL;
			item->sz = 0;
			list_del(&item->list);
			free(item);
		}
	}
	return 0;
}

int
no_leak_2(string **in, size_t insz, list_t *head2)
{
	list_t *pos2, *tmp2;
	int i2 = 0;
	int rc2;
	bool found = false;

	rc2 = make_list_2(in, insz, head2);
	if (rc2 < 0)
		return rc2;

	rc2 = write_list_2(head2);
	if (rc2 < 0)
		return rc2;

	list_for_each_safe(pos2, tmp2, head2)
	{
		string_item *item;
		item = list_entry(pos2, string_item, list);

		printf(" [%d]:\"", i2++);
		if (item->s)
			write(STDOUT_FILENO, item->s, item->sz);
		else
			printf("(nul)");
		printf("\"");
		if (item->s)
			free(item->s);
		list_del(pos2);
		free(item);
	}
	if (found)
		printf("\n");

	return 0;
}

#endif /* defined(DEFINE_TEST2) */

// vim:fenc=utf-8:tw=75:noet
