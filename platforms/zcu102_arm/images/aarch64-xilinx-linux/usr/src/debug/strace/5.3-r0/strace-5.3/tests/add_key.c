/*
 * Check decoding of add_key syscall.
 *
 * Copyright (c) 2016 Eugene Syromyatnikov <evgsyr@gmail.com>
 * Copyright (c) 2016-2019 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include "tests.h"
#include "scno.h"

#ifdef __NR_add_key

# include <inttypes.h>
# include <stdio.h>
# include <unistd.h>

void
print_val_str(const void *ptr, const char *str)
{
	if (str)
		printf("%s, ", str);
	else
		printf("%p, ", ptr);
}

void
do_add_key(const char *type, const char *type_str, const char *desc,
	const char *desc_str, const char *payload, const char *payload_str,
	size_t plen, int32_t keyring, const char *keyring_str)
{
	long rc = syscall(__NR_add_key, type, desc, payload, plen, keyring);
	const char *errstr = sprintrc(rc);
	printf("add_key(");
	print_val_str(type, type_str);
	print_val_str(desc, desc_str);
	print_val_str(payload, payload_str);
	printf("%zu, ", plen);
	if (keyring_str)
		printf("%s", keyring_str);
	else
		printf("%d", keyring);
	printf(") = %s\n", errstr);
}

int
main(void)
{
	static const char unterminated1[] = { '\1', '\2', '\3', '\4', '\5' };
	static const char unterminated2[] = { '\6', '\7', '\10', '\11', '\12' };
	static const char unterminated3[] = { '\16', '\17', '\20', '\21', '\22' };

	char *bogus_type = tail_memdup(unterminated1, sizeof(unterminated1));
	char *bogus_desc = tail_memdup(unterminated2, sizeof(unterminated2));
	char *bogus_payload = tail_memdup(unterminated3, sizeof(unterminated3));

	unsigned i;
	unsigned j;
	unsigned k;
	unsigned l;

	struct {
		const char *type;
		const char *str;
	} types[] = {
		{ ARG_STR(NULL) },
		{ bogus_type + sizeof(unterminated1), NULL },
		{ bogus_type, NULL },
		{ ARG_STR("\20\21\22\23\24") },
		{ ARG_STR("user") },
	};

	struct {
		const char *desc;
		const char *str;
	} descs[] = {
		{ ARG_STR(NULL) },
		{ bogus_desc + sizeof(unterminated2), NULL },
		{ bogus_desc, NULL },
		{ ARG_STR("\25\26\27\30\31") },
		{ ARG_STR("desc") },
		{ "overly long description", STRINGIFY("overly long ") "..." },
	};

	struct {
		const char *pload;
		const char *str;
		size_t plen;
	} payloads[] = {
		{ ARG_STR(NULL), 0 },
		{ bogus_payload + sizeof(unterminated3), NULL,
			(size_t) 0xdeadbeefbadc0dedULL },
		{ bogus_payload, STRINGIFY(""), 0 },
		{ bogus_payload, STRINGIFY("\16\17\20\21\22"), 5 },
		{ bogus_payload, NULL, 10 },
		{ "overly long payload", STRINGIFY("overly long ") "...", 15 },
	};

	struct {
		uint32_t keyring;
		const char *str;
	} keyrings[] = {
		{ ARG_STR(0) },
		{ ARG_STR(1234567890) },
		{ ARG_STR(-1234567890) },
		{ -1, "KEY_SPEC_THREAD_KEYRING" },
	};

	for (i = 0; i < ARRAY_SIZE(types); i++)
		for (j = 0; j < ARRAY_SIZE(descs); j++)
			for (k = 0; k < ARRAY_SIZE(payloads); k++)
				for (l = 0; l < ARRAY_SIZE(keyrings); l++)
					do_add_key(types[i].type, types[i].str,
						descs[j].desc, descs[j].str,
						payloads[k].pload,
						payloads[k].str,
						payloads[k].plen,
						keyrings[l].keyring,
						keyrings[l].str);

	puts("+++ exited with 0 +++");

	return 0;
}

#else

SKIP_MAIN_UNDEFINED("__NR_add_key");

#endif
