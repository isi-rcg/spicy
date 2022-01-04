/* Copyright (C) 2014 Thorsten Kukuk
   Author: Thorsten Kukuk <kukuk@suse.de>

   This library is free software: you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public License
   in version 2.1 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <string.h>
#include <rpc/clnt.h>
#include <rpcsvc/ypclnt.h>
#include <rpcsvc/yp_prot.h>

#include "internal.h"

int
yp_first (const char *indomain, const char *inmap, char **outkey,
          int *outkeylen, char **outval, int *outvallen)
{
  ypreq_nokey req;
  ypresp_key_val resp;
  enum clnt_stat result;

  if (indomain == NULL || indomain[0] == '\0' ||
      inmap == NULL || inmap[0] == '\0')
    return YPERR_BADARGS;

  req.domain = (char *) indomain;
  req.map = (char *) inmap;

  *outkey = *outval = NULL;
  *outkeylen = *outvallen = 0;
  memset (&resp, '\0', sizeof (resp));

  result = do_ypcall (indomain, YPPROC_FIRST, (xdrproc_t) xdr_ypreq_nokey,
                      (caddr_t) &req, (xdrproc_t) xdr_ypresp_key_val,
                      (caddr_t) &resp);

  if (result != RPC_SUCCESS)
    return YPERR_RPC;
  if (resp.status != YP_TRUE)
    return ypprot_err (resp.status);

  int status;
  if ((*outkey = malloc (resp.keydat.keydat_len + 1)) != NULL &&
      (*outval = malloc (resp.valdat.valdat_len + 1)) != NULL)
    {
      *outkeylen = resp.keydat.keydat_len;
      memcpy (*outkey, resp.keydat.keydat_val, *outkeylen);
      (*outkey)[*outkeylen] = '\0';

      *outvallen = resp.valdat.valdat_len;
      memcpy (*outval, resp.valdat.valdat_val, *outvallen);
      (*outval)[*outvallen] = '\0';

      status = YPERR_SUCCESS;
    }
  else
    {
      free (*outkey);
      status = YPERR_RESRC;
    }

  xdr_free ((xdrproc_t) xdr_ypresp_key_val, (char *) &resp);

  return status;
}
