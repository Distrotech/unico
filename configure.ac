AC_PREREQ(2.63)

# Package

AC_INIT([unico],[1.0],[https://bugs.launchpad.net/unico],[unico])
AC_CONFIG_SRCDIR([unico/unico.h])
AC_CONFIG_MACRO_DIR([build])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([1.9 foreign dist-bzip2])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

NANO=`echo AC_PACKAGE_VERSION | cut -d'.' -f4`
if test x"$NANO" = x || test "x$NANO" = "x0" ; then
    UNICO_TRUNK="no"
    PACKAGE_VERSION_NANO="0"
else
    UNICO_TRUNK="yes"
    PACKAGE_VERSION_NANO="$NANO"
fi
AC_SUBST(UNICO_TRUNK)
AC_SUBST(PACKAGE_VERSION_NANO)

AC_USE_SYSTEM_EXTENSIONS
AC_PROG_CC
AM_PROG_CC_C_O
LT_INIT
AC_C_CONST
AC_C_INLINE

# Build dependencies

AC_PATH_PROG([GLIB_MKENUMS], [glib-mkenums])
AC_PATH_PROG([GLIB_GENMARSHAL], [glib-genmarshal])
PKG_PROG_PKG_CONFIG

AC_SUBST(glib_req, 2.26.0)
AC_SUBST(gtk_req, 3.1.10)
AC_SUBST(cairo_req, 1.10)

PKG_CHECK_MODULES(DEPS, glib-2.0 >= $glib_req gtk+-3.0 >= $gtk_req cairo >= $cairo_req)
AC_SUBST(DEPS_CFLAGS)
AC_SUBST(DEPS_LIBS)

# Debug flags

if test x$UNICO_TRUNK = xyes; then
    DEFAULT_DEBUG="yes"
else
    DEFAULT_DEBUG="no"
fi
AC_SUBST(DEFAULT_DEBUG)
AC_ARG_ENABLE([debug],
              [AS_HELP_STRING([--enable-debug=@<:@no/yes/full@:>@],[Control debug level @<:@default=debug_default@:>@])],
              [], [enable_debug=$DEFAULT_DEBUG])
AS_CASE([$enable_debug],
        [yes],
        [
          test "$cflags_set" = set || CFLAGS="$CFLAGS -g"
          DEBUG_CFLAGS="-D_DEBUG"
        ],
        [no],
        [DEBUG_CFLAGS="-DNDEBUG"],
        [AC_MSG_ERROR([Unknown argument for --enable-debug])]
)
AC_SUBST(DEBUG_CFLAGS)

# Maintainer flags

if test x$UNICO_TRUNK = xyes; then
    DEFAULT_MAINTAINER_FLAGS="yes"
else
    DEFAULT_MAINTAINER_FLAGS="no"
fi
AC_SUBST(DEFAULT_MAINTAINER_FLAGS)
AC_ARG_ENABLE([maintainer-flags],
              [AS_HELP_STRING([--enable-maintainer-flags=@<:@no/yes@:>@],[Use strict compiler flags @<:@default=maintainer_flags_default@:>@])],
              [],
              [enable_maintainer_flags=$DEFAULT_MAINTAINER_FLAGS])
AS_IF([test "x$enable_maintainer_flags" = "xyes" && test "x$GCC" = "xyes"],
      [
        AS_COMPILER_FLAGS([MAINTAINER_CFLAGS],
                          ["-fno-common -Wall -Wextra -Werror -Wempty-body
                            -Wno-missing-field-initializers -Wstrict-aliasing=2
                            -Wno-unused-parameter -Wdeclaration-after-statement
                            -Wpointer-arith -Wcast-align -Wwrite-strings
                            -Wformat-security -Wformat-nonliteral -Winit-self
                            -Wmissing-declarations -Wnested-externs -Wundef
                            -Wold-style-definition -Wswitch-default
                            -Wredundant-decls"])
      ]
)
AC_SUBST(MAINTAINER_CFLAGS)

# Variables

UNICO_LIBADD="\$(DEPS_LIBS)"
AC_SUBST(UNICO_LIBADD)

UNICO_CFLAGS="-I\$(top_srcdir) -DUNICO_COMPILATION \$(DEPS_CFLAGS) \$(DEBUG_CFLAGS) \$(MAINTAINER_CFLAGS)"
AC_SUBST(UNICO_CFLAGS)

UNICO_LDFLAGS="-module -avoid-version -no-undefined"
AC_SUBST(UNICO_LDFLAGS)

GTK_VERSION=`$PKG_CONFIG --variable=gtk_binary_version gtk+-3.0`
AC_SUBST(GTK_VERSION)

# Files

AC_CONFIG_FILES([
Makefile
build/Makefile
po/Makefile
unico/Makefile
tests/Makefile
])
AC_OUTPUT

# Summary

echo ""
echo "  unico $PACKAGE_VERSION"
echo "  ========================="
echo ""
echo "  Debug:  ${enable_debug}"
echo "  Prefix: ${prefix}"
echo ""
echo "  Compiler flags: ${CPPFLAGS} ${DEBUG_CFLAGS} ${MAINTAINER_CFLAGS}"
echo ""
