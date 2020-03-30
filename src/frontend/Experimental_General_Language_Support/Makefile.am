include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_Jovial_ROSE_Connection

SUBDIRS = \
  ATerm

if ROSE_BUILD_ATERM_SUPPORT
   SUBDIRS += ATerm
endif

noinst_LTLIBRARIES = libexperimentalRoseGeneralLanguage.la

libexperimentalRoseGeneralLanguage_la_SOURCES = \
  sage-tree-builder.C

noinst_HEADERS = \
  sage-tree-builder.h
