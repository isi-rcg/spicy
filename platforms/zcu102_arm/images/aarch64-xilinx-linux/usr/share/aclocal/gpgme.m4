# gpgme.m4 - autoconf macro to detect GPGME.
# Copyright (C) 2002, 2003, 2004, 2014 g10 Code GmbH
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# Last-changed: 2014-10-02


AC_DEFUN([_AM_PATH_GPGME_CONFIG],
[ AC_ARG_WITH(gpgme-prefix,
            AC_HELP_STRING([--with-gpgme-prefix=PFX],
                           [prefix where GPGME is installed (optional)]),
     gpgme_config_prefix="$withval", gpgme_config_prefix="")
  if test x"${GPGME_CONFIG}" = x ; then
     if test x"${gpgme_config_prefix}" != x ; then
        GPGME_CONFIG="${gpgme_config_prefix}/bin/gpgme-config"
     else
       case "${SYSROOT}" in
         /*)
           if test -x "${SYSROOT}/bin/gpgme-config" ; then
             GPGME_CONFIG="${SYSROOT}/bin/gpgme-config"
           fi
           ;;
         '')
           ;;
          *)
           AC_MSG_WARN([Ignoring \$SYSROOT as it is not an absolute path.])
           ;;
       esac
     fi
  fi

  AC_PATH_PROG(GPGME_CONFIG, gpgme-config, no)

  if test "$GPGME_CONFIG" != "no" ; then
    gpgme_version=`$GPGME_CONFIG --version`
  fi
  gpgme_version_major=`echo $gpgme_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\1/'`
  gpgme_version_minor=`echo $gpgme_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\2/'`
  gpgme_version_micro=`echo $gpgme_version | \
               sed 's/\([[0-9]]*\)\.\([[0-9]]*\)\.\([[0-9]]*\).*/\3/'`
])


AC_DEFUN([_AM_PATH_GPGME_CONFIG_HOST_CHECK],
[
    gpgme_config_host=`$GPGME_CONFIG --host 2>/dev/null || echo none`
    if test x"$gpgme_config_host" != xnone ; then
      if test x"$gpgme_config_host" != x"$host" ; then
  AC_MSG_WARN([[
***
*** The config script $GPGME_CONFIG was
*** built for $gpgme_config_host and thus may not match the
*** used host $host.
*** You may want to use the configure option --with-gpgme-prefix
*** to specify a matching config script or use \$SYSROOT.
***]])
        gpg_config_script_warn="$gpg_config_script_warn gpgme"
      fi
    fi
])


dnl AM_PATH_GPGME([MINIMUM-VERSION,
dnl               [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libgpgme and define GPGME_CFLAGS and GPGME_LIBS.
dnl
dnl If a prefix option is not used, the config script is first
dnl searched in $SYSROOT/bin and then along $PATH.  If the used
dnl config script does not match the host specification the script
dnl is added to the gpg_config_script_warn variable.
dnl
AC_DEFUN([AM_PATH_GPGME],
[
  tmp=ifelse([$1], ,1:0.4.2,$1)
  if echo "$tmp" | grep ':' >/dev/null 2>/dev/null ; then
     req_gpgme_api=`echo "$tmp"     | sed 's/\(.*\):\(.*\)/\1/'`
     min_gpgme_version=`echo "$tmp" | sed 's/\(.*\):\(.*\)/\2/'`
  else
     req_gpgme_api=0
     min_gpgme_version="$tmp"
  fi

  PKG_CHECK_MODULES(GPGME, [gpgme >= $min_gpgme_version], [ok=yes], [ok=no])
  if test $ok = yes; then
     # If we have a recent GPGME, we should also check that the
     # API is compatible.
     if test "$req_gpgme_api" -gt 0 ; then
        tmp=`$PKG_CONFIG --variable=api_version gpgme 2>/dev/null || echo 0`
        if test "$tmp" -gt 0 ; then
           if test "$req_gpgme_api" -ne "$tmp" ; then
             ok=no
           fi
        fi
     fi
  fi
  if test $ok = yes; then
    ifelse([$2], , :, [$2])
    _AM_PATH_GPGME_CONFIG_HOST_CHECK
  else
    ifelse([$3], , :, [$3])
  fi
])

dnl AM_PATH_GPGME_PTHREAD([MINIMUM-VERSION,
dnl                       [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libgpgme and define GPGME_PTHREAD_CFLAGS
dnl  and GPGME_PTHREAD_LIBS.
dnl
AC_DEFUN([AM_PATH_GPGME_PTHREAD],
[
  tmp=ifelse([$1], ,1:0.4.2,$1)
  if echo "$tmp" | grep ':' >/dev/null 2>/dev/null ; then
     req_gpgme_api=`echo "$tmp"     | sed 's/\(.*\):\(.*\)/\1/'`
     min_gpgme_version=`echo "$tmp" | sed 's/\(.*\):\(.*\)/\2/'`
  else
     req_gpgme_api=0
     min_gpgme_version="$tmp"
  fi

  PKG_CHECK_MODULES(GPGME_PTHREAD, [gpgme-pthread >= $min_gpgme_version], [ok=yes], [ok=no])
  if test $ok = yes; then
     # If we have a recent GPGME, we should also check that the
     # API is compatible.
     if test "$req_gpgme_api" -gt 0 ; then
        tmp=`$PKG_CONFIG --variable=api_version gpgme-pthread 2>/dev/null || echo 0`
        if test "$tmp" -gt 0 ; then
           if test "$req_gpgme_api" -ne "$tmp" ; then
             ok=no
           fi
        fi
     fi
  fi
  if test $ok = yes; then
    ifelse([$2], , :, [$2])
    _AM_PATH_GPGME_CONFIG_HOST_CHECK
  else
    ifelse([$3], , :, [$3])
  fi
])


dnl AM_PATH_GPGME_GLIB([MINIMUM-VERSION,
dnl               [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libgpgme-glib and define GPGME_GLIB_CFLAGS and GPGME_GLIB_LIBS.
dnl
AC_DEFUN([AM_PATH_GPGME_GLIB],
[ AC_REQUIRE([_AM_PATH_GPGME_CONFIG])dnl
  tmp=ifelse([$1], ,1:0.4.2,$1)
  if echo "$tmp" | grep ':' >/dev/null 2>/dev/null ; then
     req_gpgme_api=`echo "$tmp"     | sed 's/\(.*\):\(.*\)/\1/'`
     min_gpgme_version=`echo "$tmp" | sed 's/\(.*\):\(.*\)/\2/'`
  else
     req_gpgme_api=0
     min_gpgme_version="$tmp"
  fi

  PKG_CHECK_MODULES(GPGME_GLIB, [gpgme >= $min_gpgme_version glib-2.0], [ok=yes], [ok=no])  
  if test $ok = yes; then
     # If we have a recent GPGME, we should also check that the
     # API is compatible.
     if test "$req_gpgme_api" -gt 0 ; then
        tmp=`$PKG_CONFIG --variable=api_version gpgme 2>/dev/null || echo 0`
        if test "$tmp" -gt 0 ; then
           if test "$req_gpgme_api" -ne "$tmp" ; then
             ok=no
           fi
        fi
     fi
  fi
  if test $ok = yes; then
    ifelse([$2], , :, [$2])
    _AM_PATH_GPGME_CONFIG_HOST_CHECK
  else
    ifelse([$3], , :, [$3])
  fi
])
