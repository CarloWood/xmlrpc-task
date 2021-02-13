AM_CPPFLAGS = -iquote $(top_builddir) -iquote $(top_srcdir) -iquote $(top_srcdir)/cwds

SUBDIRS =

noinst_LTLIBRARIES = libxmlrpctask.la

SOURCES = \
	XML_RPC_MethodCall.h \
	XML_RPC_MethodCall.cxx

libxmlrpctask_la_SOURCES = ${SOURCES}
libxmlrpctask_la_CXXFLAGS = @LIBCWD_R_FLAGS@
libxmlrpctask_la_LIBADD = @LIBCWD_R_LIBS@

# --------------- Maintainer's Section

if MAINTAINER_MODE
distclean-local:
	-rm -f *.s *.ii

clean-local:
endif

MAINTAINERCLEANFILES = $(srcdir)/Makefile.in
