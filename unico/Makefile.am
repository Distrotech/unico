source_h = \
	$(srcdir)/unico.h \
	$(srcdir)/exponential-blur.h \
	$(srcdir)/gtkroundedboxprivate.h \
	$(srcdir)/raico-blur.h \
	$(srcdir)/unico-cairo-support.h \
	$(srcdir)/unico-css-support.h \
	$(srcdir)/unico-support.h \
	$(srcdir)/unico-draw.h \
	$(srcdir)/unico-engine.h \
	$(srcdir)/unico-types.h

source_c = \
	$(srcdir)/exponential-blur.c \
	$(srcdir)/gtkroundedbox.c \
	$(srcdir)/raico-blur.c \
	$(srcdir)/unico-cairo-support.c \
	$(srcdir)/unico-css-support.c \
	$(srcdir)/unico-draw.c \
	$(srcdir)/unico-engine.c \
	$(srcdir)/unico-support.c \
	$(srcdir)/unico-theme.c

enginedir = $(libdir)/gtk-3.0/$(GTK_VERSION)/theming-engines
engine_LTLIBRARIES = libunico.la

libunico_la_SOURCES = $(source_h) $(source_c)

libunico_la_CFLAGS = $(UNICO_CFLAGS)

libunico_la_LIBADD = $(UNICO_LIBADD)

libunico_la_LDFLAGS = $(UNICO_LDFLAGS)
