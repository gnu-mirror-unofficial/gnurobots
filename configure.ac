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
AC_INIT([GNU Robots], [1.1.1], [brad@brad-smith.co.uk], [gnurobots])
AC_PREREQ([2.59])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([build])
AC_CONFIG_SRCDIR([include/api.h])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC

dnl libtool stuff
AC_LIBTOOL_DLOPEN
dnl AC_LIBLTDL_CONVENIENCE
AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AC_SUBST(INCLTDL)
AC_SUBST(LIBLTDL)

dnl Locate X Windows
dnl We need AC_PATH_XTRA to also locate extra libaries X depends on.
dnl It will ``export'' X_CFLAGS, X_PRE_LIBS, X_EXTRA_LIBS and X_LIBS,
dnl but for some reason we need to add -lX11 ourselves.
AC_PATH_XTRA

dnl Check for math library
AC_CHECK_LIB(m, pow)

dnl Check for guile
GUILE_FLAGS

dnl Some sytems need -ldl for dynamic library support.
dnl AC_CHECK_LIB(dl, dlopen)

PKG_CHECK_MODULES(X,x11 xpm, HAVE_X=yes,HAVE_X=no)
AC_SUBST(X_LIBS)
AC_SUBST(X_CFLAGS)

dnl Check for glib2
PKG_CHECK_MODULES(GLIB2,glib-2.0 >= 2.4 gobject-2.0 gmodule-2.0,HAVE_GLIB2=yes,HAVE_GLIB2=no)
AC_SUBST(GLIB2_LIBS)
AC_SUBST(GLIB2_CFLAGS)

PKG_CHECK_MODULES(GTK,gtk+-2.0 >= 2.4, HAVE_GTK=yes,HAVE_GTK=no)
AC_SUBST(GTK_LIBS)
AC_SUBST(GTK_CFLAGS)

schemedir="\$(pkgdatadir)/scheme"
AC_SUBST(schemedir)

mapsdir="\$(pkgdatadir)/maps"
AC_SUBST(mapsdir)

if test "x$HAVE_GLIB2" = "xno"; then
  AC_MSG_ERROR([GNU Robots requires GLib2 to compile.])
fi

for termlib in ncurses curses termcap terminfo termlib ; do
	AC_CHECK_LIB(${termlib}, tgoto,
		[READLINE_EXTRA="-l${termlib}"; break])
done

AC_CHECK_LIB(readline,readline,[READLINE_LIBS=-lreadline],
	AC_MSG_ERROR([

You need the GNU Readline library to build this program.

]),[$READLINE_EXTRA])

AC_CHECK_HEADER(readline/readline.h,[READLINE_CFLAGS=-I/usr/include/readline/],
	AC_MSG_ERROR([

You need the GNU Readline headers to build this program.

]))

READLINE_LIBS="$READLINE_LIBS $READLINE_EXTRA"

AC_SUBST(READLINE_LIBS)
AC_SUBST(READLINE_CFLAGS)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(getopt.h)

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
 ui/Makefile
 ])
AC_OUTPUT

