## Copyright (C) 1998 Jim Hall <jhall1@isd.net>
## Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
##
## configure.ac for GNU Robots
##
## GNU Robots is free software: you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation, either version 3 of the License, or
## (at your option) any later version.
##
## GNU Robots is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GNU Robots.  If not, see <http://www.gnu.org/licenses/>.

dnl Process this file with autoconf to produce a configure script.
AC_INIT([GNU Robots], [1.2.0], [brad@brad-smith.co.uk], [gnurobots])
AC_PREREQ([2.59])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([include/api.h])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC

GUILE_FLAGS

PKG_CHECK_MODULES(GTHREAD2, gthread-2.0 >= 2.4)
AC_SUBST(GTHREAD2_LIBS)
AC_SUBST(GTHREAD2_CFLAGS)

PKG_CHECK_MODULES(GTK2, gtk+-2.0 >= 2.4)
AC_SUBST(GTK2_LIBS)
AC_SUBST(GTK2_CFLAGS)

PKG_CHECK_MODULES(VTE, vte)
AC_SUBST(VTE_LIBS)
AC_SUBST(VTE_CFLAGS)

schemedir="\$(pkgdatadir)/scheme"
AC_SUBST(schemedir)

mapsdir="\$(pkgdatadir)/maps"
AC_SUBST(mapsdir)

for termlib in ncurses curses termcap terminfo termlib ; do
	AC_CHECK_LIB(${termlib}, tgoto,
		[READLINE_EXTRA="-l${termlib}"; break])
done

AC_CHECK_LIB(readline,readline,[READLINE_LIBS=-lreadline],
	AC_MSG_ERROR([

You need the GNU Readline library to build this program.

]),[$READLINE_EXTRA])

AC_CHECK_HEADER(readline/readline.h,
    [READLINE_CFLAGS=-I/usr/include/readline/],	AC_MSG_ERROR([

You need the GNU Readline headers to build this program.

]))

READLINE_LIBS="$READLINE_LIBS $READLINE_EXTRA"

AC_SUBST(READLINE_LIBS)
AC_SUBST(READLINE_CFLAGS)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(getopt.h)
AC_CHECK_HEADERS(pty.h)

AC_CHECK_LIB(util, openpty, [], AC_MSG_ERROR([

You need openpty to build this program.

]))

AC_CHECK_LIB(guile, scm_to_locale_string, [], AC_MSG_ERROR([

You need guile 1.8 to build this program.

]))

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

CFLAGS="-pedantic-errors -Werror -Wall -g"

dnl Done.
AC_CONFIG_FILES([Makefile
 contrib/Makefile
 doc/Makefile
 include/Makefile
 xpm/Makefile
 maps/Makefile
 scheme/Makefile
 src/Makefile
 ])
AC_OUTPUT

