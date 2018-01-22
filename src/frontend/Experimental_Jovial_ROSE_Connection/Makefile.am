include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

AM_CPPFLAGS = $(ROSE_INCLUDES) -I$(top_srcdir)/src/frontend/Experimental_Jovial_ROSE_Connection

noinst_LTLIBRARIES = libexperimentalRoseJovial.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_JOVIAL_ROSE_CONNECTION
  libexperimentalRoseJovial_la_SOURCES = jovial_support.C ATermToUntypedJovialTraversal.C \
                                         UntypedJovialTraversal.C UntypedJovialConverter.C
else
# Compile using a function that represents the API of the Jovial support.
  libexperimentalRoseJovial_la_SOURCES = dummy_jovial_main.C
endif

pkginclude_HEADERS = jovial_support.h

noinst_HEADERS = \
  ATermToUntypedJovialTraversal.h \
  JovialBuilderInterface.h     \
  UntypedJovialTraversal.h     \
  UntypedJovialConverter.h
