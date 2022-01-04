# ksba.m4 - autoconf macro to detect ksba
#       Copyright (C) 2002 g10 Code GmbH
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This file is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


dnl AM_PATH_KSBA([MINIMUM-VERSION,
dnl              [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libksba and define KSBA_CFLAGS and KSBA_LIBS
dnl MINIMUN-VERSION is a string with the version number optionalliy prefixed
dnl with the API version to also check the API compatibility. Example:
dnl a MINIMUN-VERSION of 1:1.0.7 won't pass the test unless the installed
dnl version of libksba is at least 1.0.7 *and* the API number is 1.  Using
dnl this features allows to prevent build against newer versions of libksba
dnl with a changed API.
dnl
AC_DEFUN([AM_PATH_KSBA],
[AC_REQUIRE([AC_CANONICAL_HOST])

  tmp=ifelse([$1], ,1:1.0.0,$1)
  if echo "$tmp" | grep ':' >/dev/null 2>/dev/null ; then
     req_ksba_api=`echo "$tmp"     | sed 's/\(.*\):\(.*\)/\1/'`
     min_ksba_version=`echo "$tmp" | sed 's/\(.*\):\(.*\)/\2/'`
  else
     req_ksba_api=0
     min_ksba_version="$tmp"
  fi

  PKG_CHECK_MODULES(KSBA, [ksba >= $min_ksba_version], [ok=yes], [ok=no])

  if test $ok = yes; then
     # Even if we have a recent libksba, we should check that the
     # API is compatible.
     if test "$req_ksba_api" -gt 0 ; then
        tmp=`$PKG_CONFIG --variable=api_version ksba`
        if test "$tmp" -gt 0 ; then
           AC_MSG_CHECKING([KSBA API version])
           if test "$req_ksba_api" -eq "$tmp" ; then
             AC_MSG_RESULT(okay)
           else
             ok=no
             AC_MSG_RESULT([does not match.  want=$req_ksba_api got=$tmp.])
           fi
        fi
     fi
  fi
  if test $ok = yes; then
    ifelse([$2], , :, [$2])
    libksba_config_host=`$PKG_CONFIG --variable=host ksba`
    if test x"$libksba_config_host" != xnone ; then
      if test x"$libksba_config_host" != x"$host" ; then
  AC_MSG_WARN([[
***
*** The config script $LIBKSBA_CONFIG was
*** built for $libksba_config_host and thus may not match the
*** used host $host.
*** You may want to use the configure option --with-libksba-prefix
*** to specify a matching config script.
***]])
      fi
    fi
  else
    ifelse([$3], , :, [$3])
  fi
  AC_SUBST(KSBA_CFLAGS)
  AC_SUBST(KSBA_LIBS)
])
