dnl Autoconf macros for libassuan
dnl Copyright (C) 2002, 2003, 2011 Free Software Foundation, Inc.
dnl
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without
dnl modifications, as long as this notice is preserved.
dnl
dnl This file is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
dnl SPDX-License-Identifier: FSFULLR

dnl
dnl Common code used for libassuan detection [internal]
dnl Returns ok set to yes or no.
dnl
AC_DEFUN([_AM_PATH_LIBASSUAN_COMMON],
[ AC_REQUIRE([AC_CANONICAL_HOST])

  tmp=ifelse([$1], ,1:0.9.2,$1)
  if echo "$tmp" | grep ':' >/dev/null 2>/dev/null ; then
    req_libassuan_api=`echo "$tmp"     | sed 's/\(.*\):\(.*\)/\1/'`
    min_libassuan_version=`echo "$tmp" | sed 's/\(.*\):\(.*\)/\2/'`
  else
    req_libassuan_api=0
    min_libassuan_version="$tmp"
  fi

  PKG_CHECK_MODULES(LIBASSUAN, [libassuan >= $min_libassuan_version], [ok=yes], [ok=no])

  if test $ok = yes; then
    if test "$req_libassuan_api" -gt 0 ; then
      tmp=`$PKG_CONFIG --variable=api_version libassuan`
      if test "$tmp" -gt 0 ; then
        AC_MSG_CHECKING([LIBASSUAN API version])
        if test "$req_libassuan_api" -eq "$tmp" ; then
          AC_MSG_RESULT(okay)
        else
          ok=no
          AC_MSG_RESULT([does not match.  want=$req_libassuan_api got=$tmp.])
        fi
      fi
    fi
  fi

  if test $ok = yes; then
    if test x"$host" != x ; then
      if test -z "$use_gpgrt_config"; then
        libassuan_config_host=`$PKG_CONFIG --variable=host libassuan`
      fi
      if test x"$libassuan_config_host" != xnone ; then
        if test x"$libassuan_config_host" != x"$host" ; then
  AC_MSG_WARN([[
***
*** The config script "$LIBASSUAN_CONFIG" was
*** built for $libassuan_config_host and thus may not match the
*** used host $host.
*** You may want to use the configure option --with-libassuan-prefix
*** to specify a matching config script.
***]])
        fi
      fi
    fi
  fi
])

dnl AM_CHECK_LIBASSUAN([MINIMUM-VERSION,
dnl                    [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test whether libassuan has at least MINIMUM-VERSION. This is
dnl used to test for features only available in newer versions.
dnl
AC_DEFUN([AM_CHECK_LIBASSUAN],
[ _AM_PATH_LIBASSUAN_COMMON($1)
  if test $ok = yes; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
])




dnl AM_PATH_LIBASSUAN([MINIMUM-VERSION,
dnl                   [ACTION-IF-FOUND [, ACTION-IF-NOT-FOUND ]]])
dnl Test for libassuan and define LIBASSUAN_CFLAGS and LIBASSUAN_LIBS
dnl
AC_DEFUN([AM_PATH_LIBASSUAN],
[ _AM_PATH_LIBASSUAN_COMMON($1)
  if test $ok = yes; then
    ifelse([$2], , :, [$2])
  else
    ifelse([$3], , :, [$3])
  fi
  AC_SUBST(LIBASSUAN_CFLAGS)
  AC_SUBST(LIBASSUAN_LIBS)
])
