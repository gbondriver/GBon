AC_PREREQ(2.65)

AC_INIT([gbon], [1.0.0], [gbon@github])
AC_CONFIG_AUX_DIR([config])
AC_CONFIG_MACRO_DIR([m4])

AC_CONFIG_SRCDIR([src/gbon-driver.gob])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([1.13 foreign])
AM_SILENT_RULES([yes])

AC_PROG_CC
AC_PROG_CXX
LT_INIT

AM_PATH_GLIB_2_0([2.32.4], [], [], [gobject])

GOBJECT_INTROSPECTION_REQUIRE([1.32.1])
GOB2_CHECK([2.0.0])
#GTK_DOC_CHECK([1.18-2])

# Checks for b25 support.
AC_ARG_ENABLE(b25,
  [AS_HELP_STRING([--enable-b25],[enable b25 support])],
  [AC_CHECK_LIB([arib25], [create_arib_std_b25], , [AC_MSG_WARN(libarb25 is not available.)])]
)

AC_MSG_NOTICE(notice: ${abs_top_builddir})
AC_MSG_NOTICE(notice: ${ac_abs_top_builddir})
AC_MSG_NOTICE(notice: ${abs_builddir})
AC_MSG_NOTICE(notice: ${ac_top_builddir})
AC_MSG_NOTICE([notice: ${ac_top_build_prefix}])

AC_LANG_PUSH([C++])
#BONDRIVERSRCDIR=$ac_top_build_prefix../BonDriverProxy_Linux
BONDRIVERSRCDIR=`pwd`/../BonDriverProxy_Linux
cppflags_bak=$CPPFLAGS
CPPFLAGS="$CPPFLAGS -I${BONDRIVERSRCDIR}"

AC_CHECK_HEADER([${BONDRIVERSRCDIR}/IBonDriver.h],
    [AC_DEFINE([HAVE_IBONDRIVER_H], [1], [found IBonDriver.h])
    found_ibondriver_h="yes"],
    [],
    [#include "typedef.h"
     #include "IBonDriver.h"
    ])

AS_IF([! test "x$found_ibondriver_h" = xyes],
   [BONDRIVERSRCDIR=`pwd`/BonDriverProxy_Linux
    AC_CHECK_HEADER([${BONDRIVERSRCDIR}/IBonDriver.h],
      [
      CPPFLAGS="$cppflags_bak -I${BONDRIVERSRCDIR}"
      AC_DEFINE([HAVE_IBONDRIVER_H], [1], [found IBonDriver.h])
      found_ibondriver_h="yes"],
      [],
      [#include "typedef.h"
       #include "IBonDriver.h"]
    )
   ])

AS_IF([! test "x$found_ibondriver_h" = xyes],
   [BONDRIVERSRCDIR=`pwd`/..
    AC_CHECK_HEADER([${BONDRIVERSRCDIR}/IBonDriver.h],
      [
      CPPFLAGS="$cppflags_bak -I${BONDRIVERSRCDIR}"
      AC_DEFINE([HAVE_IBONDRIVER_H], [1], [found IBonDriver.h])
      found_ibondriver_h="yes"],
      [AC_MSG_ERROR([Couldn't find or include IBonDriver.h])],
      [#include "typedef.h"
       #include "IBonDriver.h"]
    )
   ])

AC_LANG_POP([C++])

#    [AC_MSG_ERROR([Couldn't find or include IBonDriver.h])],


AC_CONFIG_FILES([
  Makefile
  src/Makefile
])

AC_OUTPUT
