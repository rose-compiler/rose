include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ATERM_INCLUDE_DIR = $(ATERM_INSTALL_PATH)/include

AM_CPPFLAGS = -I$(ATERM_INCLUDE_DIR)

noinst_LTLIBRARIES = libexperimentalOFP.la
libexperimentalOFP_la_SOURCES = OFPTraversal.cpp

pkginclude_HEADERS = OFPTraversal.hpp FASTNodes.hpp

clean:
	rm -f *.lo *.la
	rm -rf .libs
