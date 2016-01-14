SUBDIRS = epgdump
CLEANFILES = gbon-driver.c gbon-driver.h gbon-retval.c gbon-retval.h  \
	gbon-b25.c gbon-b25.h

AM_CPPFLAGS =					\
	 -I$(top_builddir)			\
	 -I$(top_srcdir)  

AM_CFLAGS =					\
	$(GLIB_CFLAGS) -g

lib_LTLIBRARIES =				\
	libgbon.la

libgbon_la_LIBADD =				\
	$(GLIB_LIBS)  epgdump/libepgdump.la

libgbon_la_SOURCES =	cbon.cpp			\
	gbon-driver.c  	gbon-driver.h \
	gbon-retval.c gbon-retval.h \
	gbon-b25.c gbon-b25.h

pkginclude_HEADERS = gbon-driver.h gbon-retval.h gbon-b25.h cbon.h


-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS =
INTROSPECTION_COMPILER_ARGS =

if HAVE_INTROSPECTION
GBon-1.0.gir: libgbon.la
GBon_1_0_gir_PACKAGES =
GBon_1_0_gir_EXPORT_PACKAGES = gbon
GBon_1_0_gir_INCLUDES = GObject-2.0
GBon_1_0_gir_CFLAGS =
GBon_1_0_gir_LIBS = libgbon.la
GBon_1_0_gir_FILES = gbon-driver.c gbon-driver.h gbon-retval.c gbon-retval.h \
	gbon-b25.c gbon-b25.h
GBon_1_0_gir_SCANNERFLAGS = --warn-error  --warn-all			\
	--identifier-prefix=GBon   \
	--symbol-prefix=gbon
INTROSPECTION_GIRS += GBon-1.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES +=					\
	$(gir_DATA)				\
	$(typelib_DATA)
endif

gbon-driver.c : gbon-retval.c gbon-b25.c cbon.cpp
%.c %.h : %.gob
	@GOB2@ --no-lines $<
