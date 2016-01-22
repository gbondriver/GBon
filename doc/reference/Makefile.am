DOC_MODULE = gbon

DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.sgml

DOC_SOURCE_DIR =				\
	$(top_srcdir)/src

SCAN_OPTIONS =						\
	--deprecated-guards="GBON_DISABLE_DEPRECATED"

MKDB_OPTIONS =					\
	--name-space=gbon

HFILE_GLOB =					\
	$(top_srcdir)/src/*.h

CFILE_GLOB =					\
	$(top_srcdir)/src/*.c

AM_CPPFLAGS =					\
	-I$(top_builddir)			\
	-I$(top_srcdir)

AM_CFLAGS =					\
	$(GLIB_CFLAGS)

GTKDOC_LIBS =					\
	$(top_builddir)/src/libgbon.la	\
	$(GLIB_LIBS) -ldl

include $(srcdir)/gtk-doc.make

CLEANFILES +=					\
	$(DOC_MODULE)-sections.txt		\
	$(DOC_MODULE).types
