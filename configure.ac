# Process this file with autoconf to produce a configure script.
# Written by Salonia Matteo on 17/05/2021

AC_PREREQ([2.69])

AC_INIT([calc], [2.0], [saloniamatteo@pm.me])
AC_CONFIG_SRCDIR([src])

# Checks for programs.
AC_PROG_CC

# Checks for header files.
AC_CHECK_HEADERS([getopt.h math.h readline/readline.h signal.h stdio.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL

# Custom parameter --with-docdir
AC_ARG_WITH([docdir],
	[AS_HELP_STRING([--with-docdir=DIR], [Where the documentation will be installed])],
	[docdir=$withval],
	[docdir="\${libdir}/doc"])

# Check if argument has been passed
if test "x$docdir" = "x"; then
	AC_SUBST([docdir], ["/usr/share/doc/calc"])
else
	AC_SUBST([docdir], [$docdir])
fi

AC_SUBST([AUTHOR], ["Matteo Salonia"])
AC_SUBST([PACKAGE_URL], ["https://github.com/saloniamatteo/calc"])
AC_SUBST([PACKAGE_DONATE], ["https://saloniamatteo.top/donate.html"])

AM_INIT_AUTOMAKE([gnu subdir-objects])
AC_CONFIG_FILES([Makefile])

AC_PROG_CC

AC_OUTPUT
