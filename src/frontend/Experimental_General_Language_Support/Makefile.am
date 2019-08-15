include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_Jovial_ROSE_Connection

SUBDIRS = \
  ATerm

if ROSE_BUILD_ATERM_SUPPORT
   SUBDIRS += ATerm
endif

noinst_LTLIBRARIES = libexperimentalRoseGeneralLanguage.la

libexperimentalRoseGeneralLanguage_la_SOURCES = \
  UntypedTraversal.C UntypedConverter.C

noinst_HEADERS = \
  FASTNodes.h UntypedTraversal.h UntypedConverter.h
