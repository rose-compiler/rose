include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ATERM_INCLUDE_DIR = $(ATERM_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(ATERM_INCLUDE_DIR) -I$(top_srcdir)/src/frontend/Experimental_General_Language_Support

noinst_LTLIBRARIES = libexperimentalRoseJovial.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
  libexperimentalRoseJovial_la_SOURCES = jovial_support.C ATermToSageJovialTraversal.C
else
# Compile using a function that represents the API of the Jovial support.
  libexperimentalRoseJovial_la_SOURCES = dummy_jovial_main.C
endif

bin_PROGRAMS = jovial_preprocess
jovial_preprocess_SOURCES = jovial_preprocess.C

pkginclude_HEADERS = jovial_support.h

noinst_HEADERS = \
  ATermToSageJovialTraversal.h
