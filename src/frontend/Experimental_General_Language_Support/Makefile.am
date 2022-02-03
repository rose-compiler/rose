include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_Jovial_ROSE_Connection

if ROSE_BUILD_ATERM_SUPPORT
   SUBDIRS = ATerm
endif

noinst_LTLIBRARIES = libexperimentalRoseGeneralLanguage.la

libexperimentalRoseGeneralLanguage_la_SOURCES = \
  SageTreeBuilder.C ModuleBuilder.C PosInfo.C Tokens.C

noinst_HEADERS = \
  SageTreeBuilder.h ModuleBuilder.h PosInfo.h Tokens.h
