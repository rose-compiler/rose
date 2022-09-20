include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ATERM_INCLUDE_DIR = $(ATERM_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(ATERM_INCLUDE_DIR) -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support

noinst_LTLIBRARIES = libexperimentalRoseJovial.la

libexperimentalRoseJovial_la_SOURCES = jovial_support.C ATermToSageJovialTraversal.C preprocess.C

bin_PROGRAMS = jovial_preprocess
jovial_preprocess_SOURCES = jovial_preprocess.C
jovial_preprocess_LDADD = preprocess.o

pkginclude_HEADERS = jovial_support.h

noinst_HEADERS = ATermToSageJovialTraversal.h
