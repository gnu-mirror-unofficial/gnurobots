##
## lib/Makefile.am
##
## GNU Robots is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2 of the License, or
## (at your option) any later version.
##
## GNU Robots is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with GNU Robots; if not, write to the Free Software
## Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
##

SUBDIRS = xpm

if USE_CURSES
CURSES_PLUGIN=libgrobots-curses.la
else
CURSES_PLUGIN=
endif

if USE_X11
X11_PLUGIN=libgrobots-x11.la
else
X11_PLUGIN=
endif

INCLUDES = $(GLIB2_CFLAGS) $(CURSES_CFLAGS) $(X_FLAGS) -I$(top_builddir)/include $(READLINE_CFLAGS)

pkglib_LTLIBRARIES  = libgrobots-text.la $(CURSES_PLUGIN) $(X11_PLUGIN)

libgrobots_text_la_SOURCES = textplugin.c
libgrobots_text_la_LDFLAGS = -module -avoid-version
libgrobots_text_la_LIBADD = $(GLIB2_LIBS) $(READLINE_LIBS)

libgrobots_curses_la_SOURCES = cursesplugin.c
libgrobots_curses_la_LDFLAGS = -module -avoid-version
libgrobots_curses_la_LIBADD = $(GLIB2_LIBS) $(CURSES_LIBS)

libgrobots_x11_la_SOURCES = x11plugin.c
libgrobots_x11_la_LDFLAGS = -module -avoid-version
libgrobots_x11_la_LIBADD = $(GLIB2_LIBS) $(X_LIBS) $(READLINE_LIBS)

EXTRA_DIST =\
	cursesplugin.h \
	textplugin.h \
	x11plugin.h
