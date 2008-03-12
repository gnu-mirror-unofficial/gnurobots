## Copyright (C) 2008 Bradley Smith <brad@brad-smith.co.uk>
##
## src/Makefile.am
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

bin_PROGRAMS = gnurobots

INCLUDES = $(GLIB2_CFLAGS) $(GUILE_CFLAGS) -I$(top_builddir)/include \
	   -DPKGLIBDIR=\"$(pkglibdir)\" \
	   -DABS_TOP_BUILDDIR=\"$(abs_top_builddir)\" \
	   -DPKGDATADIR=\"$(pkgdatadir)\" \
	   -DMAPS_PATH=\"$(mapsdir)\" \
	   -DSCRIPTS_PATH=\"$(schemedir)\"

gnurobots_SOURCES = main.c api.c map.c grobot.c userinterface.c
gnurobots_LDFLAGS = $(GLIB2_LIBS) $(GUILE_LDFLAGS) -lltdl -lgthread-2.0
