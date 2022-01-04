/* ks-action.c - OpenPGP keyserver actions
 * Copyright (C) 2011 Free Software Foundation, Inc.
 * Copyright (C) 2011, 2014 Werner Koch
 * Copyright (C) 2015 g10 Code GmbH
 *
 * This file is part of GnuPG.
 *
 * GnuPG is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * GnuPG is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <https://www.gnu.org/licenses/>.
 */

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "dirmngr.h"
#include "misc.h"
#include "ks-engine.h"
#include "ks-action.h"
#if USE_LDAP
# include "ldap-parse-uri.h"
#endif

/* Called by the engine's help functions to print the actual help.  */
gpg_error_t
ks_print_help (ctrl_t ctrl, const char *text)
{
  return dirmngr_status_help (ctrl, text);
}


/* Called by the engine's help functions to print the actual help.  */
gpg_error_t
ks_printf_help (ctrl_t ctrl, const char *format, ...)
{
  va_list arg_ptr;
  gpg_error_t err;
  char *buf;

  va_start (arg_ptr, format);
  buf = es_vbsprintf (format, arg_ptr);
  err = buf? 0 : gpg_error_from_syserror ();
  va_end (arg_ptr);
  if (!err)
    err = dirmngr_status_help (ctrl, buf);
  es_free (buf);
  return err;
}


/* Run the help command for the engine responsible for URI.  */
gpg_error_t
ks_action_help (ctrl_t ctrl, const char *url)
{
  gpg_error_t err;
  parsed_uri_t parsed_uri;  /* The broken down URI.  */

  if (!url || !*url)
    {
      ks_print_help (ctrl, "Known schemata:\n");
      parsed_uri = NULL;
    }
  else
    {
#if USE_LDAP
      if (ldap_uri_p (url))
	err = ldap_parse_uri (&parsed_uri, url);
      else
#endif
	{
	  err = http_parse_uri (&parsed_uri, url, 1);
	}

      if (err)
        return err;
    }

  /* Call all engines to give them a chance to print a help sting.  */
  err = ks_hkp_help (ctrl, parsed_uri);
  if (!err)
    err = ks_http_help (ctrl, parsed_uri);
  if (!err)
    err = ks_finger_help (ctrl, parsed_uri);
  if (!err)
    err = ks_kdns_help (ctrl, parsed_uri);
#if USE_LDAP
  if (!err)
    err = ks_ldap_help (ctrl, parsed_uri);
#endif

  if (!parsed_uri)
    ks_print_help (ctrl,
                   "(Use an URL for engine specific help.)");
  else
    http_release_parsed_uri (parsed_uri);
  return err;
}


/* Resolve all host names.  This is useful for looking at the status
   of configured keyservers.  */
gpg_error_t
ks_action_resolve (ctrl_t ctrl, uri_item_t keyservers)
{
  gpg_error_t err = 0;
  int any_server = 0;
  uri_item_t uri;

  for (uri = keyservers; !err && uri; uri = uri->next)
    {
      if (uri->parsed_uri->is_http)
        {
          any_server = 1;
          err = ks_hkp_resolve (ctrl, uri->parsed_uri);
          if (err)
            break;
        }
    }

  if (!any_server)
    err = gpg_error (GPG_ERR_NO_KEYSERVER);
  return err;
}


/* Search all configured keyservers for keys matching PATTERNS and
   write the result to the provided output stream.  */
gpg_error_t
ks_action_search (ctrl_t ctrl, uri_item_t keyservers,
		  strlist_t patterns, estream_t outfp)
{
  gpg_error_t err = 0;
  int any_server = 0;
  int any_results = 0;
  uri_item_t uri;
  estream_t infp;

  if (!patterns)
    return gpg_error (GPG_ERR_NO_USER_ID);

  /* FIXME: We only take care of the first pattern.  To fully support
     multiple patterns we might either want to run several queries in
     parallel and merge them.  We also need to decide what to do with
     errors - it might not be the best idea to ignore an error from
     one server and silently continue with another server.  For now we
     stop at the first error, unless the server responds with '404 Not
     Found', in which case we try the next server.  */
  for (uri = keyservers; !err && uri; uri = uri->next)
    {
      int is_http = uri->parsed_uri->is_http;
      int is_ldap = 0;
      unsigned int http_status = 0;
#if USE_LDAP
      is_ldap = (strcmp (uri->parsed_uri->scheme, "ldap") == 0
		 || strcmp (uri->parsed_uri->scheme, "ldaps") == 0
		 || strcmp (uri->parsed_uri->scheme, "ldapi") == 0);
#endif
      if (is_http || is_ldap)
        {
          any_server = 1;
#if USE_LDAP
	  if (is_ldap)
	    err = ks_ldap_search (ctrl, uri->parsed_uri, patterns->d, &infp);
	  else
#endif
	    {
	      err = ks_hkp_search (ctrl, uri->parsed_uri, patterns->d,
                                   &infp, &http_status);
	    }

          if (err == gpg_error (GPG_ERR_NO_DATA)
              && http_status == 404 /* not found */)
            {
              /* No record found.  Clear error and try next server.  */
              err = 0;
              continue;
            }

          if (!err)
            {
              err = copy_stream (infp, outfp);
              es_fclose (infp);
              any_results = 1;
              break;
            }
        }
    }

  if (!any_server)
    err = gpg_error (GPG_ERR_NO_KEYSERVER);
  else if (err == 0 && !any_results)
    err = gpg_error (GPG_ERR_NO_DATA);
  return err;
}


/* Get the requested keys (matching PATTERNS) using all configured
   keyservers and write the result to the provided output stream.  */
gpg_error_t
ks_action_get (ctrl_t ctrl, uri_item_t keyservers,
	       strlist_t patterns, estream_t outfp)
{
  gpg_error_t err = 0;
  gpg_error_t first_err = 0;
  int any_server = 0;
  int any_data = 0;
  strlist_t sl;
  uri_item_t uri;
  estream_t infp;

  if (!patterns)
    return gpg_error (GPG_ERR_NO_USER_ID);

  /* FIXME: We only take care of the first keyserver.  To fully
     support multiple keyservers we need to track the result for each
     pattern and use the next keyserver if one key was not found.  The
     keyservers might not all be fully synced thus it is not clear
     whether the first keyserver has the freshest copy of the key.
     Need to think about a better strategy.  */
  for (uri = keyservers; !err && uri; uri = uri->next)
    {
      int is_hkp_s = (strcmp (uri->parsed_uri->scheme, "hkp") == 0
                      || strcmp (uri->parsed_uri->scheme, "hkps") == 0);
      int is_http_s = (strcmp (uri->parsed_uri->scheme, "http") == 0
                       || strcmp (uri->parsed_uri->scheme, "https") == 0);
      int is_ldap = 0;

#if USE_LDAP
      is_ldap = (strcmp (uri->parsed_uri->scheme, "ldap") == 0
		 || strcmp (uri->parsed_uri->scheme, "ldaps") == 0
		 || strcmp (uri->parsed_uri->scheme, "ldapi") == 0);
#endif

      if (is_hkp_s || is_http_s || is_ldap)
        {
          any_server = 1;
          for (sl = patterns; !err && sl; sl = sl->next)
            {
#if USE_LDAP
	      if (is_ldap)
		err = ks_ldap_get (ctrl, uri->parsed_uri, sl->d, &infp);
	      else
#endif
              if (is_hkp_s)
                err = ks_hkp_get (ctrl, uri->parsed_uri, sl->d, &infp);
              else if (is_http_s)
                err = ks_http_fetch (ctrl, uri->parsed_uri->original,
                                     KS_HTTP_FETCH_NOCACHE,
                                     &infp);
              else
                BUG ();

              if (err)
                {
                  /* It is possible that a server does not carry a
                     key, thus we only save the error and continue
                     with the next pattern.  FIXME: It is an open
                     question how to return such an error condition to
                     the caller.  */
                  first_err = err;
                  err = 0;
                }
              else
                {
                  err = copy_stream (infp, outfp);
                  /* Reading from the keyserver should never fail, thus
                     return this error.  */
                  if (!err)
                    any_data = 1;
                  es_fclose (infp);
                  infp = NULL;
                }
            }
        }
      if (any_data)
        break; /* Stop loop after a keyserver returned something.  */
    }

  if (!any_server)
    err = gpg_error (GPG_ERR_NO_KEYSERVER);
  else if (!err && first_err && !any_data)
    err = first_err;
  return err;
}


/* Retrieve keys from URL and write the result to the provided output
 * stream OUTFP.  If OUTFP is NULL the data is written to the bit
 * bucket. */
gpg_error_t
ks_action_fetch (ctrl_t ctrl, const char *url, estream_t outfp)
{
  gpg_error_t err = 0;
  estream_t infp;
  parsed_uri_t parsed_uri;  /* The broken down URI.  */

  if (!url)
    return gpg_error (GPG_ERR_INV_URI);

  err = http_parse_uri (&parsed_uri, url, 1);
  if (err)
    return err;

  if (parsed_uri->is_http)
    {
      err = ks_http_fetch (ctrl, url, KS_HTTP_FETCH_NOCACHE, &infp);
      if (!err)
        {
          err = copy_stream (infp, outfp);
          es_fclose (infp);
        }
    }
  else if (!parsed_uri->opaque)
    {
      err = gpg_error (GPG_ERR_INV_URI);
    }
  else if (!strcmp (parsed_uri->scheme, "finger"))
    {
      err = ks_finger_fetch (ctrl, parsed_uri, &infp);
      if (!err)
        {
          err = copy_stream (infp, outfp);
          es_fclose (infp);
        }
    }
  else if (!strcmp (parsed_uri->scheme, "kdns"))
    {
      err = ks_kdns_fetch (ctrl, parsed_uri, &infp);
      if (!err)
        {
          err = copy_stream (infp, outfp);
          es_fclose (infp);
        }
    }
  else
    err = gpg_error (GPG_ERR_INV_URI);

  http_release_parsed_uri (parsed_uri);
  return err;
}



/* Send an OpenPGP key to all keyservers.  The key in {DATA,DATALEN}
   is expected to be in OpenPGP binary transport format.  The metadata
   in {INFO,INFOLEN} is in colon-separated format (concretely, it is
   the output of 'for x in keys sigs; do gpg --list-$x --with-colons
   KEYID; done'.  This function may modify DATA and INFO.  If this is
   a problem, then the caller should create a copy.  */
gpg_error_t
ks_action_put (ctrl_t ctrl, uri_item_t keyservers,
	       void *data, size_t datalen,
	       void *info, size_t infolen)
{
  gpg_error_t err = 0;
  gpg_error_t first_err = 0;
  int any_server = 0;
  uri_item_t uri;

  (void) info;
  (void) infolen;

  for (uri = keyservers; !err && uri; uri = uri->next)
    {
      int is_http = uri->parsed_uri->is_http;
      int is_ldap = 0;

#if USE_LDAP
      is_ldap = (strcmp (uri->parsed_uri->scheme, "ldap") == 0
		|| strcmp (uri->parsed_uri->scheme, "ldaps") == 0
		|| strcmp (uri->parsed_uri->scheme, "ldapi") == 0);
#endif

      if (is_http || is_ldap)
        {
          any_server = 1;
#if USE_LDAP
	  if (is_ldap)
	    err = ks_ldap_put (ctrl, uri->parsed_uri, data, datalen,
			       info, infolen);
	  else
#endif
	    {
	      err = ks_hkp_put (ctrl, uri->parsed_uri, data, datalen);
	    }
          if (err)
            {
              first_err = err;
              err = 0;
            }
        }
    }

  if (!any_server)
    err = gpg_error (GPG_ERR_NO_KEYSERVER);
  else if (!err && first_err)
    err = first_err;
  return err;
}
