/*
 * This file is part of libbtrfsutil.
 *
 * libbtrfsutil is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libbtrfsutil is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libbtrfsutil.  If not, see <http://www.gnu.org/licenses/>.
 */

#if HAVE_REALLOCARRAY != 1

#include <stdlib.h>
#include <errno.h>
#include "stubs.h"

void *reallocarray(void *ptr, size_t nmemb, size_t size)
{
	size_t res;

	res = nmemb * size;
	if (res < nmemb || res < size) {
		errno = ENOMEM;
		return NULL;
	}
	return realloc(ptr, res);
}

#endif
