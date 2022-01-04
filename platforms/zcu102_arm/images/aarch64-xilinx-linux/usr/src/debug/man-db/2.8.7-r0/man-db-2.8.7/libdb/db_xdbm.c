/*
 * db_xdbm.c: common code for gdbm and ndbm backends
 *
 * Copyright (C) 2003-2019 Colin Watson.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif /* HAVE_CONFIG_H */

#if defined(GDBM) || defined(NDBM)

#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "gl_hash_map.h"
#include "gl_rbtree_list.h"
#include "gl_xlist.h"
#include "gl_xmap.h"
#include "hash-pjw-bare.h"

#include "manconfig.h"

#include "cleanup.h"
#include "glcontainers.h"

#include "db_xdbm.h"
#include "mydbm.h"

static gl_map_t parent_keys;

static int datum_compare (const void *a, const void *b)
{
	const datum *left = (const datum *) a;
	const datum *right = (const datum *) b;
	int cmp;
	size_t minsize;

	/* Sentinel NULL elements sort to the end. */
	if (!MYDBM_DPTR (*left))
		return 1;
	else if (!MYDBM_DPTR (*right))
		return -1;

	if (MYDBM_DSIZE (*left) < MYDBM_DSIZE (*right))
		minsize = MYDBM_DSIZE (*left);
	else
		minsize = MYDBM_DSIZE (*right);
	cmp = strncmp (MYDBM_DPTR (*left), MYDBM_DPTR (*right), minsize);
	if (cmp)
		return cmp;
	else if (MYDBM_DSIZE (*left) < MYDBM_DSIZE (*right))
		return 1;
	else if (MYDBM_DSIZE (*left) > MYDBM_DSIZE (*right))
		return -1;
	else
		return 0;
}

static bool datum_equals (const void *a, const void *b)
{
	return datum_compare (a, b) == 0;
}

static size_t datum_hash (const void *value)
{
	const datum *d = value;
	return hash_pjw_bare (MYDBM_DPTR (*d), MYDBM_DSIZE (*d));
}

static void datum_free (const void *value)
{
	MYDBM_FREE_DPTR (*(datum *) value);
}

static datum empty_datum = { NULL, 0 };

/* We keep a map of filenames to sorted lists of keys.  Each list is stored
 * using a hash-based implementation that allows lookup by name and
 * traversal to the next item in O(log n) time, which is necessary for a
 * reasonable ordered implementation of nextkey.
 */
datum man_xdbm_firstkey (MYDBM_FILE dbf,
			 man_xdbm_unsorted_firstkey unsorted_firstkey,
			 man_xdbm_unsorted_nextkey unsorted_nextkey)
{
	gl_list_t keys;
	datum *key;

	/* Build the raw sorted list of keys. */
	keys = gl_list_create_empty (GL_RBTREE_LIST, datum_equals, datum_hash,
				     datum_free, false);
	key = XMALLOC (datum);
	*key = unsorted_firstkey (dbf);
	while (MYDBM_DPTR (*key)) {
		datum *next;

		gl_sortedlist_add (keys, datum_compare, key);
		next = XMALLOC (datum);
		*next = unsorted_nextkey (dbf, *key);
		key = next;
	}

	if (!parent_keys) {
		parent_keys = new_string_map (GL_HASH_MAP,
					      (gl_listelement_dispose_fn)
					      gl_list_free);
		push_cleanup ((cleanup_fun) gl_map_free, parent_keys, 0);
	}

	/* Remember this structure for use by nextkey. */
	gl_map_put (parent_keys, xstrdup (dbf->name), keys);

	if (gl_list_size (keys))
		return copy_datum (*(datum *) gl_list_get_at (keys, 0));
	else
		return empty_datum;
}

datum man_xdbm_nextkey (MYDBM_FILE dbf, datum key)
{
	gl_list_t keys;
	gl_list_node_t node, next_node;

	if (!parent_keys)
		return empty_datum;
	keys = (gl_list_t) gl_map_get (parent_keys, dbf->name);
	if (!keys)
		return empty_datum;

	node = gl_sortedlist_search (keys, datum_compare, &key);
	if (!node)
		return empty_datum;
	next_node = gl_list_next_node (keys, node);
	if (!next_node)
		return empty_datum;

	return copy_datum (*(datum *) gl_list_node_value (keys, next_node));
}

void man_xdbm_close (MYDBM_FILE dbf, man_xdbm_raw_close raw_close)
{
	if (!dbf)
		return;

	if (parent_keys)
		gl_map_remove (parent_keys, dbf->name);

	free (dbf->name);
	raw_close (dbf);
	free (dbf);
}

#endif /* GDBM || NDBM */
