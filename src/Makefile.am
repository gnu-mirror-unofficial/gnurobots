bin_PROGRAMS = gnurobots

INCLUDES = $(GLIB2_CFLAGS) $(GUILE_CFLAGS) -I$(top_builddir)/include \
	   -DPKGLIBDIR=\"$(pkglibdir)\" \
	   -DABS_TOP_BUILDDIR=\"$(abs_top_builddir)\" \
	   -DPKGDATADIR=\"$(pkgdatadir)\" \
	   -DMAPS_PATH=\"$(mapsdir)\" \
	   -DSCRIPTS_PATH=\"$(schemedir)\"

gnurobots_SOURCES = main.c api.c sign.c map.c grobot.c userinterface.c
gnurobots_LDFLAGS = $(GLIB2_LIBS) $(GUILE_LDFLAGS) -lltdl
