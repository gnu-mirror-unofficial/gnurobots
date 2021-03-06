## Copyright (C) 1998 Jim Hall <jhall@freedos.org>
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
AC_INIT([GNU Robots], [1.2.0], [gnurobots@gnu.org], [gnurobots])
AC_PREREQ([2.59])
AM_CONFIG_HEADER([config.h])
AC_CONFIG_SRCDIR([include/api.h])
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_PROG_CC_C99

GUILE_FLAGS

PKG_CHECK_MODULES(GLIB2, glib-2.0 >= 2.35.1)
PKG_CHECK_MODULES(GTHREAD2, gthread-2.0 >= 2.4)
PKG_CHECK_MODULES(GTK2, gtk+-2.0 >= 2.4)
PKG_CHECK_MODULES(VTE, vte)

schemedir="\$(pkgdatadir)/scheme"
AC_SUBST(schemedir)

mapsdir="\$(pkgdatadir)/maps"
AC_SUBST(mapsdir)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h)
AC_CHECK_HEADERS(getopt.h)
AC_CHECK_HEADERS(pty.h)

AC_CHECK_LIB(util, openpty, [], AC_MSG_ERROR([

You need openpty to build this program.

]))

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST

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

