/* > read-conf.c
 *
 * Functions to help with line-by-line reading of a text file, for example, by fgets()
 *
 * Typically what we have is a line like " something = somevalue\n" and we want firstly
 * to separate/split this in to "something" and "somevalue" as two clean strings, then
 * we parse them by looking for a match for "something" and then to read/convert the
 * "somevalue" string accordingly.
 *
 * (c) 2013 Paul S. Crawford (psc@sat.dundee.ac.uk) released under GPL v2 licence.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h> /* for isdigit() */

#include "extern.h"
#include "read-conf.h"

/*
 * Return 1 if a character is "white space", so space, tab, CR, LF, etc.
 * Return 0 for anything else.
 */

static int is_white(char c)
{

	switch (c) {
		case  ' ':
		case '\t':
		case '\r':
		case '\n':
		case '\b':
			return 1;
	}

	return 0;
}

/*
 * Return 1 if looks like part of a number (e.g. "+-012...9")
 */

static int is_number(char c)
{
	if(isdigit(c) || c == '-' || c == '+') return 1;

	return 0;
}

/*
 * Remove trailing "white space" characters from a string.
 */

void trim_white(char *buf)
{
	int ii;

	if (buf == NULL)
		return;

	for (ii = strlen(buf) - 1; ii >= 0; ii--) {
		if (is_white(buf[ii])) {
			buf[ii] = 0;	/* Replace white space with 'nul'. */
		} else {
			break;
		}
	}
}

/*
 * Return pointer to first non-white space character, or to
 * the 'nul' end-of-string position.
 */

char *str_start(char *p)
{

	if (p != NULL) {
		while (*p && is_white(*p))
			p++;
	}

	return p;
}

/*
 * Function to read an integer from "arg=val" string as split above. This has
 * the basic check that the 'val' looks as if it is a number, otherwise it will
 * not change the supplied variable.
 *
 * Calling arguments are:
 *
 *	arg		: String of what variable has been found.
 *	val		: String of the corresponding value.
 *	name	: The searched-for case for 'arg' (if matched, then parse).
 *	imin	: Lower limit of numeric range (if imin!=imax).
 *	imax	: Upper limit of numeric range (if imin!=imax).
 *	iv		: Pointer to an integer that is set on suitable match.
 *
 * The return value is 0 if arg=name
 */

int read_int_func(char *arg, char *val, const char *name, int imin, int imax, int *iv)
{
	int rv = -1;		/* Assume wrong/error case. */

	if (strcmp(arg, name) == 0) {
		rv = 0;

		if (val != NULL && is_number(*val)) {
			int ii = atoi(val);

			if (imax > imin) {
				/* have limits, check and enforce them. */
				if (ii > imax) {
					log_message(LOG_WARNING, "Warning: number for '%s' too big (%d > imax=%d)", arg, ii, imax);
					ii = imax;
				} else if (ii < imin) {
					log_message(LOG_WARNING, "Warning: number for '%s' too small (%d < imin=%d)", arg, ii, imin);
					ii = imin;
				}
			}

			*iv = ii;
			if (verbose) log_message(LOG_DEBUG, "Integer '%s' found = %d", arg, *iv);
		} else {
			log_message(LOG_WARNING, "Warning: number expected for '%s'", arg);
		}
	}

	return rv;
}

/*
 * Similar to read_int_func() above, here we search for a string. However, in this
 * case we may allow a blank case to set the string to NULL. So we use it like:
 *
 * char str = "before";
 *
 * read_string_func(arg, val, "looking", Read_allow_blank, &str);
 *
 * If we had split "looking = after" then str would contain "after", however, if
 * we had split "looking = " then val is NULL or "", hence str would be NULL.
 *
 * NOTE: This function duplicates 'val' on success so remember to free it later, also
 * check what it was before the call as this is not freeing any pointer, just assigning
 * a new block of memory (as example has 'str' set to static memory, not from a call
 * to malloc() or similar).
 */

int read_string_func(char *arg, char *val, const char *name, string_read_e mode, char **str)
{
	int rv = -1;

	if (strcmp(arg, name) == 0) {
		rv = 0;

		if (val != NULL && *val) {
			*str = xstrdup(val);
			if (verbose) log_message(LOG_DEBUG, "String '%s' found as '%s'", arg, val);
		} else {
			/* no string in file, what are we supposed to do? */
			switch (mode) {
				case Read_allow_blank:
					*str = NULL;
					if (verbose) log_message(LOG_DEBUG, "String '%s' found as blank (NULL)", arg);
					break;

				case Read_string_only:
					log_message(LOG_WARNING, "Warning: blank string not allowed for '%s = %s'", arg, *str);
					break;

				default:
					fatal_error(EX_SOFTWARE, "Invalid mode for read_string_func() (mode=%d)", mode);
					break;
			}
		}
	}

	return rv;
}

/*
 * Function to read an integer based on matching an enumerated list. This is used for cases
 * such as yes/no or multiple-valued examples. The table list[] can be created using the macros
 * in read-conf.h
 */

int read_enumerated_func(char *arg, char *val, const char *name, const read_list_t list[], int *iv)
{
	int rv = -1;

	if (strcmp(arg, name) == 0) {
		rv = 0;

		if (val != NULL && *val) {
			int ii = 0;
			/* We have some string, search the list[] table and if matched (case independent) use the list[] value. */
			while(list[ii].name != NULL)
				{
				if(strcasecmp(val, list[ii].name) == 0)
					{
					*iv = list[ii].value;
					if (verbose) log_message(LOG_DEBUG, "Variable '%s' found as '%s' = %d", arg, val, *iv);
					return 0;
					}
				ii++;
				}

			/* We did not match & return, so log this. */
			if (verbose) log_message(LOG_DEBUG, "Variable '%s' not matched for '%s'", arg, val);
		} else {
			if (verbose) log_message(LOG_DEBUG, "Variable '%s' found as blank", arg);
		}
	}

	return rv;
}

/*
 * Similar to the read_string_func() of read-conf.c, this function reads a string and
 * adds it to the linked-list.
 */

int read_list_func(char *arg, char *val, const char *name, int version, struct list **list)
{
	int rv = -1;

	if (strcmp(arg, name) == 0) {
		rv = 0;

		if (val != NULL && *val) {
			add_list(list, val, version);
			if (verbose) log_message(LOG_DEBUG, "List '%s' added as '%s'", arg, val);
		} else {
			log_message(LOG_WARNING, "Warning: string expected for '%s'", arg);
		}
	}

	return rv;
}

/*
 * Add a new configuration list entry. Calling arguments are:
 *
 * list		: Address of a pointer to be updated. Should be pointing to NULL to start
 *			  with, for example:
 *
 *				struct list *list = NULL;
 *				add_list(&list, name, version);
 *
 * name		: Name of the object, this is duplicated so 'name' can change afterwards.
 * version	: Version number for test & repair binary.
 *
 */

void add_list(struct list **list, const char *name, int version)
{
	struct list *new, *act;

	if (list == NULL || name == NULL)
		return;

	/* Use xcalloc() to allocate and *zero* a block of memory. */
	new = (struct list *)xcalloc(1, sizeof(struct list));
	/* Make a copy of 'name' in case it changes elsewhere. */
	new->name = xstrdup(name);
	new->version = version;

	if (*list == NULL) {
		*list = new;
	} else {
		for (act = *list; act->next != NULL; act = act->next) {
		}
		/* The for() loop will seek to end of list, add new link to there. */
		act->next = new;
	}
}

/*
 * Free a list created by add_list() (or read_list_func() that in turn uses it).
 */

void free_list(struct list **list)
{
	struct list *new, *act;

	if (list != NULL) {
		act = (*list);
		while(act != NULL) {
			new = act->next;
			if (act->name != NULL) {
				free(act->name);
			}
			free(act);
			act = new;
		}
		*list = NULL; /* Mark as done. */
	}
}
