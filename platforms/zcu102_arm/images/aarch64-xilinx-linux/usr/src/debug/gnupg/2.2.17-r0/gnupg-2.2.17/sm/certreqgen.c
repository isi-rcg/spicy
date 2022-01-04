/* certreqgen.c - Generate a key and a certification [request]
 * Copyright (C) 2002, 2003, 2005, 2007, 2010,
 *               2011 Free Software Foundation, Inc.
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

/*
   The format of the parameter file is described in the manual under
   "Unattended Usage".

   Here is an example:
     $ cat >foo <<EOF
     %echo Generating a standard key
     Key-Type: RSA
     Key-Length: 3072
     Name-DN: CN=test cert 1,OU=Aegypten Project,O=g10 Code GmbH,L=Ddorf,C=DE
     Name-Email: joe@foo.bar
     # Do a commit here, so that we can later print a "done"
     %commit
     %echo done
     EOF

   This parameter file was used to create the STEED CA:
     Key-Type: RSA
     Key-Length: 1024
     Key-Grip: 68A638998DFABAC510EA645CE34F9686B2EDF7EA
     Key-Usage: cert
     Serial: 1
     Name-DN: CN=The STEED Self-Signing Nonthority
     Not-Before: 2011-11-11
     Not-After: 2106-02-06
     Subject-Key-Id: 68A638998DFABAC510EA645CE34F9686B2EDF7EA
     Extension: 2.5.29.19 c 30060101ff020101
     Extension: 1.3.6.1.4.1.11591.2.2.2 n 0101ff
     Signing-Key: 68A638998DFABAC510EA645CE34F9686B2EDF7EA
     %commit

*/


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>

#include "gpgsm.h"
#include <gcrypt.h>
#include <ksba.h>

#include "keydb.h"
#include "../common/i18n.h"


enum para_name
  {
    pKEYTYPE,
    pKEYLENGTH,
    pKEYGRIP,
    pKEYUSAGE,
    pNAMEDN,
    pNAMEEMAIL,
    pNAMEDNS,
    pNAMEURI,
    pSERIAL,
    pISSUERDN,
    pNOTBEFORE,
    pNOTAFTER,
    pSIGNINGKEY,
    pHASHALGO,
    pAUTHKEYID,
    pSUBJKEYID,
    pEXTENSION
  };

struct para_data_s
{
  struct para_data_s *next;
  int lnr;
  enum para_name key;
  union {
    unsigned int usage;
    char value[1];
  } u;
};

struct reqgen_ctrl_s
{
  int lnr;
  int dryrun;
};


static const char oidstr_authorityKeyIdentifier[] = "2.5.29.35";
static const char oidstr_subjectKeyIdentifier[] = "2.5.29.14";
static const char oidstr_keyUsage[] = "2.5.29.15";
static const char oidstr_basicConstraints[] = "2.5.29.19";
static const char oidstr_standaloneCertificate[] = "1.3.6.1.4.1.11591.2.2.1";


static int proc_parameters (ctrl_t ctrl,
                            struct para_data_s *para,
                            estream_t out_fp,
                            struct reqgen_ctrl_s *outctrl);
static int create_request (ctrl_t ctrl,
                           struct para_data_s *para,
                           const char *carddirect,
                           ksba_const_sexp_t public,
                           ksba_const_sexp_t sigkey,
                           ksba_writer_t writer);



static void
release_parameter_list (struct para_data_s *r)
{
  struct para_data_s *r2;

  for (; r ; r = r2)
    {
      r2 = r->next;
      xfree(r);
    }
}

static struct para_data_s *
get_parameter (struct para_data_s *para, enum para_name key, int seq)
{
  struct para_data_s *r;

  for (r = para; r ; r = r->next)
    if ( r->key == key && !seq--)
      return r;
  return NULL;
}

static const char *
get_parameter_value (struct para_data_s *para, enum para_name key, int seq)
{
  struct para_data_s *r = get_parameter (para, key, seq);
  return (r && *r->u.value)? r->u.value : NULL;
}

static int
get_parameter_algo (struct para_data_s *para, enum para_name key)
{
  struct para_data_s *r = get_parameter (para, key, 0);
  if (!r)
    return -1;
  if (digitp (r->u.value))
    return atoi( r->u.value );
  return gcry_pk_map_name (r->u.value);
}

/* Parse the usage parameter.  Returns 0 on success.  Note that we
   only care about sign and encrypt and don't (yet) allow all the
   other X.509 usage to be specified; instead we will use a fixed
   mapping to the X.509 usage flags. */
static int
parse_parameter_usage (struct para_data_s *para, enum para_name key)
{
  struct para_data_s *r = get_parameter (para, key, 0);
  char *p, *pn;
  unsigned int use;

  if (!r)
    return 0; /* none (this is an optional parameter)*/

  use = 0;
  pn = r->u.value;
  while ( (p = strsep (&pn, " \t,")) )
    {
      if (!*p)
        ;
      else if ( !ascii_strcasecmp (p, "sign") )
        use |= GCRY_PK_USAGE_SIGN;
      else if ( !ascii_strcasecmp (p, "encrypt")
                || !ascii_strcasecmp (p, "encr") )
        use |= GCRY_PK_USAGE_ENCR;
      else if ( !ascii_strcasecmp (p, "cert") )
        use |= GCRY_PK_USAGE_CERT;
      else
        {
          log_error ("line %d: invalid usage list\n", r->lnr);
          return -1; /* error */
        }
    }
  r->u.usage = use;
  return 0;
}


static unsigned int
get_parameter_uint (struct para_data_s *para, enum para_name key)
{
  struct para_data_s *r = get_parameter (para, key, 0);

  if (!r)
    return 0;

  if (r->key == pKEYUSAGE)
    return r->u.usage;

  return (unsigned int)strtoul (r->u.value, NULL, 10);
}



/* Read the certificate generation parameters from FP and generate
   (all) certificate requests.  */
static int
read_parameters (ctrl_t ctrl, estream_t fp, estream_t out_fp)
{
  static struct {
    const char *name;
    enum para_name key;
    int allow_dups;
  } keywords[] = {
    { "Key-Type",       pKEYTYPE},
    { "Key-Length",     pKEYLENGTH },
    { "Key-Grip",       pKEYGRIP },
    { "Key-Usage",      pKEYUSAGE },
    { "Name-DN",        pNAMEDN },
    { "Name-Email",     pNAMEEMAIL, 1 },
    { "Name-DNS",       pNAMEDNS, 1 },
    { "Name-URI",       pNAMEURI, 1 },
    { "Serial",         pSERIAL },
    { "Issuer-DN",      pISSUERDN },
    { "Creation-Date",  pNOTBEFORE },
    { "Not-Before",     pNOTBEFORE },
    { "Expire-Date",    pNOTAFTER },
    { "Not-After",      pNOTAFTER },
    { "Signing-Key",    pSIGNINGKEY },
    { "Hash-Algo",      pHASHALGO },
    { "Authority-Key-Id", pAUTHKEYID },
    { "Subject-Key-Id", pSUBJKEYID },
    { "Extension",      pEXTENSION, 1 },
    { NULL, 0 }
  };
  char line[1024], *p;
  const char *err = NULL;
  struct para_data_s *para, *r;
  int i, rc = 0, any = 0;
  struct reqgen_ctrl_s outctrl;

  memset (&outctrl, 0, sizeof (outctrl));

  err = NULL;
  para = NULL;
  while (es_fgets (line, DIM(line)-1, fp) )
    {
      char *keyword, *value;

      outctrl.lnr++;
      if (*line && line[strlen(line)-1] != '\n')
        {
          err = "line too long";
          break;
	}
      for (p=line; spacep (p); p++)
        ;
      if (!*p || *p == '#')
        continue;

      keyword = p;
      if (*keyword == '%')
        {
          for (; *p && !ascii_isspace (*p); p++)
            ;
          if (*p)
            *p++ = 0;
          for (; ascii_isspace (*p); p++)
            ;
          value = p;
          trim_trailing_spaces (value);

          if (!ascii_strcasecmp (keyword, "%echo"))
            log_info ("%s\n", value);
          else if (!ascii_strcasecmp (keyword, "%dry-run"))
            outctrl.dryrun = 1;
          else if (!ascii_strcasecmp( keyword, "%commit"))
            {
              rc = proc_parameters (ctrl, para, out_fp, &outctrl);
              if (rc)
                goto leave;
              any = 1;
              release_parameter_list (para);
              para = NULL;
	    }
          else
            log_info ("skipping control '%s' (%s)\n", keyword, value);

          continue;
	}


      if (!(p = strchr (p, ':')) || p == keyword)
        {
          err = "missing colon";
          break;
	}
      if (*p)
        *p++ = 0;
      for (; spacep (p); p++)
        ;
      if (!*p)
        {
          err = "missing argument";
          break;
	}
      value = p;
      trim_trailing_spaces (value);

      for (i=0; (keywords[i].name
                 && ascii_strcasecmp (keywords[i].name, keyword)); i++)
        ;
      if (!keywords[i].name)
        {
          err = "unknown keyword";
          break;
	}
      if (keywords[i].key != pKEYTYPE && !para)
        {
          err = "parameter block does not start with \"Key-Type\"";
          break;
	}

      if (keywords[i].key == pKEYTYPE && para)
        {
          rc = proc_parameters (ctrl, para, out_fp, &outctrl);
          if (rc)
            goto leave;
          any = 1;
          release_parameter_list (para);
          para = NULL;
	}
      else if (!keywords[i].allow_dups)
        {
          for (r = para; r && r->key != keywords[i].key; r = r->next)
            ;
          if (r)
            {
              err = "duplicate keyword";
              break;
	    }
	}

      r = xtrycalloc (1, sizeof *r + strlen( value ));
      if (!r)
        {
          err = "out of core";
          break;
        }
      r->lnr = outctrl.lnr;
      r->key = keywords[i].key;
      strcpy (r->u.value, value);
      r->next = para;
      para = r;
    }

  if (err)
    {
      log_error ("line %d: %s\n", outctrl.lnr, err);
      rc = gpg_error (GPG_ERR_GENERAL);
    }
  else if (es_ferror(fp))
    {
      log_error ("line %d: read error: %s\n", outctrl.lnr, strerror(errno) );
      rc = gpg_error (GPG_ERR_GENERAL);
    }
  else if (para)
    {
      rc = proc_parameters (ctrl, para, out_fp, &outctrl);
      if (rc)
        goto leave;
      any = 1;
    }

  if (!rc && !any)
    rc = gpg_error (GPG_ERR_NO_DATA);

 leave:
  release_parameter_list (para);
  return rc;
}

/* check whether there are invalid characters in the email address S */
static int
has_invalid_email_chars (const char *s)
{
  int at_seen=0;
  static char valid_chars[] = "01234567890_-."
                              "abcdefghijklmnopqrstuvwxyz"
			      "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  for (; *s; s++)
    {
      if (*s & 0x80)
        return 1;
      if (*s == '@')
        at_seen++;
      else if (!at_seen && !( !!strchr (valid_chars, *s) || *s == '+'))
        return 1;
      else if (at_seen && !strchr (valid_chars, *s))
        return 1;
    }
  return at_seen != 1;
}


/* Check that all required parameters are given and perform the action */
static int
proc_parameters (ctrl_t ctrl, struct para_data_s *para,
                 estream_t out_fp, struct reqgen_ctrl_s *outctrl)
{
  gpg_error_t err;
  struct para_data_s *r;
  const char *s, *string;
  int i;
  unsigned int nbits;
  char numbuf[20];
  unsigned char keyparms[100];
  int rc = 0;
  ksba_sexp_t public = NULL;
  ksba_sexp_t sigkey = NULL;
  int seq;
  size_t erroff, errlen;
  char *cardkeyid = NULL;

  /* Check that we have all required parameters; */
  assert (get_parameter (para, pKEYTYPE, 0));

  /* We can only use RSA for now.  There is a problem with pkcs-10 on
     how to use ElGamal because it is expected that a PK algorithm can
     always be used for signing. Another problem is that on-card
     generated encryption keys may not be used for signing.  */
  i = get_parameter_algo (para, pKEYTYPE);
  if (!i && (s = get_parameter_value (para, pKEYTYPE, 0)) && *s)
    {
      /* Hack to allow creation of certificates directly from a smart
         card.  For example: "Key-Type: card:OPENPGP.3".  */
      if (!strncmp (s, "card:", 5) && s[5])
        cardkeyid = xtrystrdup (s+5);
    }
  if ( (i < 1 || i != GCRY_PK_RSA) && !cardkeyid )
    {
      r = get_parameter (para, pKEYTYPE, 0);
      log_error (_("line %d: invalid algorithm\n"), r->lnr);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }

  /* Check the keylength.  NOTE: If you change this make sure that it
     macthes the gpgconflist item in gpgsm.c  */
  if (!get_parameter (para, pKEYLENGTH, 0))
    nbits = 3072;
  else
    nbits = get_parameter_uint (para, pKEYLENGTH);
  if ((nbits < 1024 || nbits > 4096) && !cardkeyid)
    {
      /* The BSI specs dated 2002-11-25 don't allow lengths below 1024. */
      r = get_parameter (para, pKEYLENGTH, 0);
      log_error (_("line %d: invalid key length %u (valid are %d to %d)\n"),
                 r->lnr, nbits, 1024, 4096);
      xfree (cardkeyid);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }

  /* Check the usage. */
  if (parse_parameter_usage (para, pKEYUSAGE))
    {
      xfree (cardkeyid);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }

  /* Check that there is a subject name and that this DN fits our
     requirements. */
  if (!(s=get_parameter_value (para, pNAMEDN, 0)))
    {
      r = get_parameter (para, pNAMEDN, 0);
      log_error (_("line %d: no subject name given\n"), r->lnr);
      xfree (cardkeyid);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }
  err = ksba_dn_teststr (s, 0, &erroff, &errlen);
  if (err)
    {
      r = get_parameter (para, pNAMEDN, 0);
      if (gpg_err_code (err) == GPG_ERR_UNKNOWN_NAME)
        log_error (_("line %d: invalid subject name label '%.*s'\n"),
                   r->lnr, (int)errlen, s+erroff);
      else
        log_error (_("line %d: invalid subject name '%s' at pos %d\n"),
                   r->lnr, s, (int)erroff);

      xfree (cardkeyid);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }

  /* Check that the optional email address is okay. */
  for (seq=0; (s=get_parameter_value (para, pNAMEEMAIL, seq)); seq++)
    {
      if (has_invalid_email_chars (s)
          || *s == '@'
          || s[strlen(s)-1] == '@'
          || s[strlen(s)-1] == '.'
          || strstr(s, ".."))
        {
          r = get_parameter (para, pNAMEEMAIL, seq);
          log_error (_("line %d: not a valid email address\n"), r->lnr);
          xfree (cardkeyid);
          return gpg_error (GPG_ERR_INV_PARAMETER);
        }
    }

  /* Check the optional serial number.  */
  string = get_parameter_value (para, pSERIAL, 0);
  if (string)
    {
      if (!strcmp (string, "random"))
        ; /* Okay.  */
      else
        {
          for (s=string, i=0; hexdigitp (s); s++, i++)
            ;
          if (*s)
            {
              r = get_parameter (para, pSERIAL, 0);
              log_error (_("line %d: invalid serial number\n"), r->lnr);
              xfree (cardkeyid);
              return gpg_error (GPG_ERR_INV_PARAMETER);
            }
        }
    }

  /* Check the optional issuer DN.  */
  string = get_parameter_value (para, pISSUERDN, 0);
  if (string)
    {
      err = ksba_dn_teststr (string, 0, &erroff, &errlen);
      if (err)
        {
          r = get_parameter (para, pISSUERDN, 0);
          if (gpg_err_code (err) == GPG_ERR_UNKNOWN_NAME)
            log_error (_("line %d: invalid issuer name label '%.*s'\n"),
                       r->lnr, (int)errlen, string+erroff);
          else
            log_error (_("line %d: invalid issuer name '%s' at pos %d\n"),
                       r->lnr, string, (int)erroff);
          xfree (cardkeyid);
          return gpg_error (GPG_ERR_INV_PARAMETER);
        }
    }

  /* Check the optional creation date.  */
  string = get_parameter_value (para, pNOTBEFORE, 0);
  if (string && !string2isotime (NULL, string))
    {
      r = get_parameter (para, pNOTBEFORE, 0);
      log_error (_("line %d: invalid date given\n"), r->lnr);
      xfree (cardkeyid);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }


  /* Check the optional expire date.  */
  string = get_parameter_value (para, pNOTAFTER, 0);
  if (string && !string2isotime (NULL, string))
    {
      r = get_parameter (para, pNOTAFTER, 0);
      log_error (_("line %d: invalid date given\n"), r->lnr);
      xfree (cardkeyid);
      return gpg_error (GPG_ERR_INV_PARAMETER);
    }

  /* Get the optional signing key.  */
  string = get_parameter_value (para, pSIGNINGKEY, 0);
  if (string)
    {
      rc = gpgsm_agent_readkey (ctrl, 0, string, &sigkey);
      if (rc)
        {
          r = get_parameter (para, pKEYTYPE, 0);
          log_error (_("line %d: error getting signing key by keygrip '%s'"
                       ": %s\n"), r->lnr, s, gpg_strerror (rc));
          xfree (cardkeyid);
          return rc;
        }
    }

  /* Check the optional hash-algo.  */
  {
    int mdalgo;

    string = get_parameter_value (para, pHASHALGO, 0);
    if (string && !((mdalgo = gcry_md_map_name (string))
                    && (mdalgo == GCRY_MD_SHA1
                        || mdalgo == GCRY_MD_SHA256
                        || mdalgo == GCRY_MD_SHA384
                        || mdalgo == GCRY_MD_SHA512)))
      {
        r = get_parameter (para, pHASHALGO, 0);
        log_error (_("line %d: invalid hash algorithm given\n"), r->lnr);
        xfree (cardkeyid);
        return gpg_error (GPG_ERR_INV_PARAMETER);
      }
  }

  /* Check the optional AuthorityKeyId.  */
  string = get_parameter_value (para, pAUTHKEYID, 0);
  if (string)
    {
      for (s=string, i=0; hexdigitp (s); s++, i++)
        ;
      if (*s || (i&1))
        {
          r = get_parameter (para, pAUTHKEYID, 0);
          log_error (_("line %d: invalid authority-key-id\n"), r->lnr);
          xfree (cardkeyid);
          return gpg_error (GPG_ERR_INV_PARAMETER);
        }
    }

  /* Check the optional SubjectKeyId.  */
  string = get_parameter_value (para, pSUBJKEYID, 0);
  if (string)
    {
      for (s=string, i=0; hexdigitp (s); s++, i++)
        ;
      if (*s || (i&1))
        {
          r = get_parameter (para, pSUBJKEYID, 0);
          log_error (_("line %d: invalid subject-key-id\n"), r->lnr);
          xfree (cardkeyid);
          return gpg_error (GPG_ERR_INV_PARAMETER);
        }
    }

  /* Check the optional extensions. */
  for (seq=0; (string=get_parameter_value (para, pEXTENSION, seq)); seq++)
    {
      int okay = 0;

      s = strpbrk (string, " \t:");
      if (s)
        {
          s++;
          while (spacep (s))
            s++;
          if (*s && strchr ("nNcC", *s))
            {
              s++;
              while (spacep (s))
                s++;
              if (*s == ':')
                s++;
              if (*s)
                {
                  while (spacep (s))
                    s++;
                  for (i=0; hexdigitp (s); s++, i++)
                    ;
                  if (!((*s && *s != ':') || !i || (i&1)))
                    okay = 1;
                }
            }
        }
      if (!okay)
        {
          r = get_parameter (para, pEXTENSION, seq);
          log_error (_("line %d: invalid extension syntax\n"), r->lnr);
          xfree (cardkeyid);
          return gpg_error (GPG_ERR_INV_PARAMETER);
        }
    }

  /* Create or retrieve the public key.  */
  if (cardkeyid) /* Take the key from the current smart card. */
    {
      rc = gpgsm_agent_readkey (ctrl, 1, cardkeyid, &public);
      if (rc)
        {
          r = get_parameter (para, pKEYTYPE, 0);
          log_error (_("line %d: error reading key '%s' from card: %s\n"),
                     r->lnr, cardkeyid, gpg_strerror (rc));
          xfree (sigkey);
          xfree (cardkeyid);
          return rc;
        }
    }
  else if ((s=get_parameter_value (para, pKEYGRIP, 0))) /* Use existing key.*/
    {
      rc = gpgsm_agent_readkey (ctrl, 0, s, &public);
      if (rc)
        {
          r = get_parameter (para, pKEYTYPE, 0);
          log_error (_("line %d: error getting key by keygrip '%s': %s\n"),
                     r->lnr, s, gpg_strerror (rc));
          xfree (sigkey);
          xfree (cardkeyid);
          return rc;
        }
    }
  else if (!outctrl->dryrun) /* Generate new key.  */
    {
      sprintf (numbuf, "%u", nbits);
      snprintf ((char*)keyparms, DIM (keyparms),
                "(6:genkey(3:rsa(5:nbits%d:%s)))",
                (int)strlen (numbuf), numbuf);
      rc = gpgsm_agent_genkey (ctrl, keyparms, &public);
      if (rc)
        {
          r = get_parameter (para, pKEYTYPE, 0);
          log_error (_("line %d: key generation failed: %s <%s>\n"),
                     r->lnr, gpg_strerror (rc), gpg_strsource (rc));
          xfree (sigkey);
          xfree (cardkeyid);
          return rc;
        }
    }


  if (!outctrl->dryrun)
    {
      gnupg_ksba_io_t b64writer = NULL;
      ksba_writer_t writer;
      int create_cert ;

      create_cert = !!get_parameter_value (para, pSERIAL, 0);

      ctrl->pem_name = create_cert? "CERTIFICATE" : "CERTIFICATE REQUEST";

      rc = gnupg_ksba_create_writer
        (&b64writer, ((ctrl->create_pem? GNUPG_KSBA_IO_PEM : 0)
                      | (ctrl->create_base64? GNUPG_KSBA_IO_BASE64 : 0)),
         ctrl->pem_name, out_fp, &writer);
      if (rc)
        log_error ("can't create writer: %s\n", gpg_strerror (rc));
      else
        {
          rc = create_request (ctrl, para, cardkeyid, public, sigkey, writer);
          if (!rc)
            {
              rc = gnupg_ksba_finish_writer (b64writer);
              if (rc)
                log_error ("write failed: %s\n", gpg_strerror (rc));
              else
                {
                  gpgsm_status (ctrl, STATUS_KEY_CREATED, "P");
                  log_info ("certificate%s created\n",
                            create_cert?"":" request");
                }
            }
          gnupg_ksba_destroy_writer (b64writer);
        }
    }

  xfree (sigkey);
  xfree (public);
  xfree (cardkeyid);

  return rc;
}


/* Parameters are checked, the key pair has been created.  Now
   generate the request and write it out */
static int
create_request (ctrl_t ctrl,
                struct para_data_s *para,
                const char *carddirect,
                ksba_const_sexp_t public,
                ksba_const_sexp_t sigkey,
                ksba_writer_t writer)
{
  ksba_certreq_t cr;
  gpg_error_t err;
  gcry_md_hd_t md;
  ksba_stop_reason_t stopreason;
  int rc = 0;
  const char *s, *string;
  unsigned int use;
  int seq;
  char *buf, *p;
  size_t len;
  char numbuf[30];
  ksba_isotime_t atime;
  int certmode = 0;
  int mdalgo;

  err = ksba_certreq_new (&cr);
  if (err)
    return err;

  string = get_parameter_value (para, pHASHALGO, 0);
  if (string)
    mdalgo = gcry_md_map_name (string);
  else
    mdalgo = GCRY_MD_SHA256;
  rc = gcry_md_open (&md, mdalgo, 0);
  if (rc)
    {
      log_error ("md_open failed: %s\n", gpg_strerror (rc));
      goto leave;
    }
  if (DBG_HASHING)
    gcry_md_debug (md, "cr.cri");

  ksba_certreq_set_hash_function (cr, HASH_FNC, md);
  ksba_certreq_set_writer (cr, writer);

  err = ksba_certreq_add_subject (cr, get_parameter_value (para, pNAMEDN, 0));
  if (err)
    {
      log_error ("error setting the subject's name: %s\n",
                 gpg_strerror (err));
      rc = err;
      goto leave;
    }

  for (seq=0; (s = get_parameter_value (para, pNAMEEMAIL, seq)); seq++)
    {
      buf = xtrymalloc (strlen (s) + 3);
      if (!buf)
        {
          rc = out_of_core ();
          goto leave;
        }
      *buf = '<';
      strcpy (buf+1, s);
      strcat (buf+1, ">");
      err = ksba_certreq_add_subject (cr, buf);
      xfree (buf);
      if (err)
        {
          log_error ("error setting the subject's alternate name: %s\n",
                     gpg_strerror (err));
          rc = err;
          goto leave;
        }
    }

  for (seq=0; (s = get_parameter_value (para, pNAMEDNS, seq)); seq++)
    {
      len = strlen (s);
      assert (len);
      snprintf (numbuf, DIM(numbuf), "%u:", (unsigned int)len);
      buf = p = xtrymalloc (11 + strlen (numbuf) + len + 3);
      if (!buf)
        {
          rc = out_of_core ();
          goto leave;
        }
      p = stpcpy (p, "(8:dns-name");
      p = stpcpy (p, numbuf);
      p = stpcpy (p, s);
      strcpy (p, ")");

      err = ksba_certreq_add_subject (cr, buf);
      xfree (buf);
      if (err)
        {
          log_error ("error setting the subject's alternate name: %s\n",
                     gpg_strerror (err));
          rc = err;
          goto leave;
        }
    }

  for (seq=0; (s = get_parameter_value (para, pNAMEURI, seq)); seq++)
    {
      len = strlen (s);
      assert (len);
      snprintf (numbuf, DIM(numbuf), "%u:", (unsigned int)len);
      buf = p = xtrymalloc (6 + strlen (numbuf) + len + 3);
      if (!buf)
        {
          rc = out_of_core ();
          goto leave;
        }
      p = stpcpy (p, "(3:uri");
      p = stpcpy (p, numbuf);
      p = stpcpy (p, s);
      strcpy (p, ")");

      err = ksba_certreq_add_subject (cr, buf);
      xfree (buf);
      if (err)
        {
          log_error ("error setting the subject's alternate name: %s\n",
                     gpg_strerror (err));
          rc = err;
          goto leave;
        }
    }


  err = ksba_certreq_set_public_key (cr, public);
  if (err)
    {
      log_error ("error setting the public key: %s\n",
                 gpg_strerror (err));
      rc = err;
      goto leave;
    }

  /* Set key usage flags.  */
  use = get_parameter_uint (para, pKEYUSAGE);
  if (use)
    {
      unsigned int mask, pos;
      unsigned char der[4];

      der[0] = 0x03;
      der[1] = 0x02;
      der[2] = 0;
      der[3] = 0;
      if ((use & GCRY_PK_USAGE_SIGN))
        {
          /* For signing only we encode the bits:
             KSBA_KEYUSAGE_DIGITAL_SIGNATURE
             KSBA_KEYUSAGE_NON_REPUDIATION  = 0b11 -> 0b11000000 */
          der[3] |= 0xc0;
        }
      if ((use & GCRY_PK_USAGE_ENCR))
        {
          /* For encrypt only we encode the bits:
             KSBA_KEYUSAGE_KEY_ENCIPHERMENT
             KSBA_KEYUSAGE_DATA_ENCIPHERMENT = 0b1100 -> 0b00110000 */
          der[3] |= 0x30;
        }
      if ((use & GCRY_PK_USAGE_CERT))
        {
          /* For certify only we encode the bits:
             KSBA_KEYUSAGE_KEY_CERT_SIGN
             KSBA_KEYUSAGE_CRL_SIGN      = 0b1100000 -> 0b00000110 */
          der[3] |= 0x06;
        }

      /* Count number of unused bits.  */
      for (mask=1, pos=0; pos < 8 * sizeof mask; pos++, mask <<= 1)
        {
          if ((der[3] & mask))
            break;
          der[2]++;
        }

      err = ksba_certreq_add_extension (cr, oidstr_keyUsage, 1, der, 4);
      if (err)
        {
          log_error ("error setting the key usage: %s\n",
                     gpg_strerror (err));
          rc = err;
          goto leave;
        }
    }


  /* See whether we want to create an X.509 certificate.  */
  string = get_parameter_value (para, pSERIAL, 0);
  if (string)
    {
      certmode = 1;

      /* Store the serial number.  */
      if (!strcmp (string, "random"))
        {
          char snbuf[3+8+1];

          memcpy (snbuf, "(8:", 3);
          gcry_create_nonce (snbuf+3, 8);
          /* Clear high bit to guarantee a positive integer.  */
          snbuf[3] &= 0x7f;
          snbuf[3+8] = ')';
          err = ksba_certreq_set_serial (cr, snbuf);
        }
      else
        {
          char *hexbuf;

          /* Allocate a buffer large enough to prefix the string with
             a '0' so to have an even number of digits.  Prepend two
             further '0' so that the binary result will have a leading
             0 byte and thus can't be the representation of a negative
             number.  Note that ksba_certreq_set_serial strips all
             unneeded leading 0 bytes.  */
          hexbuf = p = xtrymalloc (2 + 1 + strlen (string) + 1);
          if (!hexbuf)
            {
              err = gpg_error_from_syserror ();
              goto leave;
            }
          if ((strlen (string) & 1))
            *p++ = '0';
          *p++ = '0';
          *p++ = '0';
          strcpy (p, string);
          for (p=hexbuf, len=0; p[0] && p[1]; p += 2)
            ((unsigned char*)hexbuf)[len++] = xtoi_2 (p);
          /* Now build the S-expression.  */
          snprintf (numbuf, DIM(numbuf), "%u:", (unsigned int)len);
          buf = p = xtrymalloc (1 + strlen (numbuf) + len + 1 + 1);
          if (!buf)
            {
              err = gpg_error_from_syserror ();
              xfree (hexbuf);
              goto leave;
            }
          p = stpcpy (stpcpy (buf, "("), numbuf);
          memcpy (p, hexbuf, len);
          p += len;
          strcpy (p, ")");
          xfree (hexbuf);
          err = ksba_certreq_set_serial (cr, buf);
          xfree (buf);
        }
      if (err)
        {
          log_error ("error setting the serial number: %s\n",
                     gpg_strerror (err));
          goto leave;
        }


      /* Store the issuer DN.  If no issuer DN is given and no signing
         key has been set we add the standalone extension and the
         basic constraints to mark it as a self-signed CA
         certificate.  */
      string = get_parameter_value (para, pISSUERDN, 0);
      if (string)
        {
          /* Issuer DN given.  Note that this may be the same as the
             subject DN and thus this could as well be a self-signed
             certificate.  However the caller needs to explicitly
             specify basicConstraints and so forth.  */
          err = ksba_certreq_set_issuer (cr, string);
          if (err)
            {
              log_error ("error setting the issuer DN: %s\n",
                         gpg_strerror (err));
              goto leave;
            }

        }
      else if (!string && !sigkey)
        {
          /* Self-signed certificate requested.  Add basicConstraints
             and the custom GnuPG standalone extension.  */
          err = ksba_certreq_add_extension (cr, oidstr_basicConstraints, 1,
                                            "\x30\x03\x01\x01\xff", 5);
          if (err)
            goto leave;
          err = ksba_certreq_add_extension (cr, oidstr_standaloneCertificate, 0,
                                            "\x01\x01\xff", 3);
          if (err)
            goto leave;
        }

      /* Store the creation date.  */
      string = get_parameter_value (para, pNOTBEFORE, 0);
      if (string)
        {
          if (!string2isotime (atime, string))
            BUG (); /* We already checked the value.  */
        }
      else
        gnupg_get_isotime (atime);
      err = ksba_certreq_set_validity (cr, 0, atime);
      if (err)
        {
          log_error ("error setting the creation date: %s\n",
                     gpg_strerror (err));
          goto leave;
        }


      /* Store the expire date.  If it is not given, libksba inserts a
         default value.  */
      string = get_parameter_value (para, pNOTAFTER, 0);
      if (string)
        {
          if (!string2isotime (atime, string))
            BUG (); /* We already checked the value.  */
          err = ksba_certreq_set_validity (cr, 1, atime);
          if (err)
            {
              log_error ("error setting the expire date: %s\n",
                         gpg_strerror (err));
              goto leave;
            }
        }


      /* Figure out the signing algorithm.  If no sigkey has been
         given we set it to the public key to create a self-signed
         certificate. */
      if (!sigkey)
        sigkey = public;

      {
        unsigned char *siginfo;

        err = transform_sigval (sigkey,
                                gcry_sexp_canon_len (sigkey, 0, NULL, NULL),
                                mdalgo, &siginfo, NULL);
        if (!err)
          {
            err = ksba_certreq_set_siginfo (cr, siginfo);
            xfree (siginfo);
          }
        if (err)
          {
            log_error ("error setting the siginfo: %s\n",
                       gpg_strerror (err));
            rc = err;
            goto leave;
          }
      }

      /* Insert the AuthorityKeyId.  */
      string = get_parameter_value (para, pAUTHKEYID, 0);
      if (string)
        {
          char *hexbuf;

          /* Allocate a buffer for in-place conversion.  We also add 4
             extra bytes space for the tags and lengths fields.  */
          hexbuf = xtrymalloc (4 + strlen (string) + 1);
          if (!hexbuf)
            {
              err = gpg_error_from_syserror ();
              goto leave;
            }
          strcpy (hexbuf+4, string);
          for (p=hexbuf+4, len=0; p[0] && p[1]; p += 2)
            ((unsigned char*)hexbuf)[4+len++] = xtoi_2 (p);
          if (len > 125)
            {
              err = gpg_error (GPG_ERR_TOO_LARGE);
              xfree (hexbuf);
              goto leave;
            }
          hexbuf[0] = 0x30;  /* Tag for a Sequence.  */
          hexbuf[1] = len+2;
          hexbuf[2] = 0x80;  /* Context tag for an implicit Octet string.  */
          hexbuf[3] = len;
          err = ksba_certreq_add_extension (cr, oidstr_authorityKeyIdentifier,
                                            0,
                                            hexbuf, 4+len);
          xfree (hexbuf);
          if (err)
            {
              log_error ("error setting the authority-key-id: %s\n",
                         gpg_strerror (err));
              goto leave;
            }
        }

      /* Insert the SubjectKeyId.  */
      string = get_parameter_value (para, pSUBJKEYID, 0);
      if (string)
        {
          char *hexbuf;

          /* Allocate a buffer for in-place conversion.  We also add 2
             extra bytes space for the tag and length field.  */
          hexbuf = xtrymalloc (2 + strlen (string) + 1);
          if (!hexbuf)
            {
              err = gpg_error_from_syserror ();
              goto leave;
            }
          strcpy (hexbuf+2, string);
          for (p=hexbuf+2, len=0; p[0] && p[1]; p += 2)
            ((unsigned char*)hexbuf)[2+len++] = xtoi_2 (p);
          if (len > 127)
            {
              err = gpg_error (GPG_ERR_TOO_LARGE);
              xfree (hexbuf);
              goto leave;
            }
          hexbuf[0] = 0x04;  /* Tag for an Octet string.  */
          hexbuf[1] = len;
          err = ksba_certreq_add_extension (cr, oidstr_subjectKeyIdentifier, 0,
                                            hexbuf, 2+len);
          xfree (hexbuf);
          if (err)
            {
              log_error ("error setting the subject-key-id: %s\n",
                         gpg_strerror (err));
              goto leave;
            }
        }

      /* Insert additional extensions.  */
      for (seq=0; (string = get_parameter_value (para, pEXTENSION, seq)); seq++)
        {
          char *hexbuf;
          char *oidstr;
          int crit = 0;

          s = strpbrk (string, " \t:");
          if (!s)
            {
              err = gpg_error (GPG_ERR_INTERNAL);
              goto leave;
            }

          oidstr = xtrymalloc (s - string + 1);
          if (!oidstr)
            {
              err = gpg_error_from_syserror ();
              goto leave;
            }
          memcpy (oidstr, string, (s-string));
          oidstr[(s-string)] = 0;

          s++;
          while (spacep (s))
            s++;
          if (!*s)
            {
              err = gpg_error (GPG_ERR_INTERNAL);
              xfree (oidstr);
              goto leave;
            }

          if (strchr ("cC", *s))
            crit = 1;
          s++;
          while (spacep (s))
            s++;
          if (*s == ':')
            s++;
          while (spacep (s))
            s++;

          hexbuf = xtrystrdup (s);
          if (!hexbuf)
            {
              err = gpg_error_from_syserror ();
              xfree (oidstr);
              goto leave;
            }
          for (p=hexbuf, len=0; p[0] && p[1]; p += 2)
            ((unsigned char*)hexbuf)[len++] = xtoi_2 (p);
          err = ksba_certreq_add_extension (cr, oidstr, crit,
                                            hexbuf, len);
          xfree (oidstr);
          xfree (hexbuf);
        }
    }
  else
    sigkey = public;

  do
    {
      err = ksba_certreq_build (cr, &stopreason);
      if (err)
        {
          log_error ("ksba_certreq_build failed: %s\n", gpg_strerror (err));
          rc = err;
          goto leave;
        }
      if (stopreason == KSBA_SR_NEED_SIG)
        {
          gcry_sexp_t s_pkey;
          size_t n;
          unsigned char grip[20];
          char hexgrip[41];
          unsigned char *sigval, *newsigval;
          size_t siglen;

          n = gcry_sexp_canon_len (sigkey, 0, NULL, NULL);
          if (!n)
            {
              log_error ("libksba did not return a proper S-Exp\n");
              rc = gpg_error (GPG_ERR_BUG);
              goto leave;
            }
          rc = gcry_sexp_sscan (&s_pkey, NULL, (const char*)sigkey, n);
          if (rc)
            {
              log_error ("gcry_sexp_scan failed: %s\n", gpg_strerror (rc));
              goto leave;
            }
          if ( !gcry_pk_get_keygrip (s_pkey, grip) )
            {
              rc = gpg_error (GPG_ERR_GENERAL);
              log_error ("can't figure out the keygrip\n");
              gcry_sexp_release (s_pkey);
              goto leave;
            }
          gcry_sexp_release (s_pkey);
          bin2hex (grip, 20, hexgrip);

          log_info ("about to sign the %s for key: &%s\n",
                    certmode? "certificate":"CSR", hexgrip);

          if (carddirect && !certmode)
            rc = gpgsm_scd_pksign (ctrl, carddirect, NULL,
                                   gcry_md_read (md, mdalgo),
                                   gcry_md_get_algo_dlen (mdalgo),
                                   mdalgo,
                                   &sigval, &siglen);
          else
            {
              char *orig_codeset;
              char *desc;

              orig_codeset = i18n_switchto_utf8 ();
              desc = percent_plus_escape
                (_("To complete this certificate request please enter"
                   " the passphrase for the key you just created once"
                   " more.\n"));
              i18n_switchback (orig_codeset);
              rc = gpgsm_agent_pksign (ctrl, hexgrip, desc,
                                       gcry_md_read(md, mdalgo),
                                       gcry_md_get_algo_dlen (mdalgo),
                                       mdalgo,
                                       &sigval, &siglen);
              xfree (desc);
            }
          if (rc)
            {
              log_error ("signing failed: %s\n", gpg_strerror (rc));
              goto leave;
            }

          err = transform_sigval (sigval, siglen, mdalgo,
                                  &newsigval, NULL);
          xfree (sigval);
          if (!err)
            {
              err = ksba_certreq_set_sig_val (cr, newsigval);
              xfree (newsigval);
            }
          if (err)
            {
              log_error ("failed to store the sig_val: %s\n",
                         gpg_strerror (err));
              rc = err;
              goto leave;
            }
        }
    }
  while (stopreason != KSBA_SR_READY);


 leave:
  gcry_md_close (md);
  ksba_certreq_release (cr);
  return rc;
}



/* Create a new key by reading the parameters from IN_FP.  Multiple
   keys may be created */
int
gpgsm_genkey (ctrl_t ctrl, estream_t in_stream, estream_t out_stream)
{
  int rc;

  rc = read_parameters (ctrl, in_stream, out_stream);
  if (rc)
    {
      log_error ("error creating certificate request: %s <%s>\n",
                 gpg_strerror (rc), gpg_strsource (rc));
      goto leave;
    }

 leave:
  return rc;
}
