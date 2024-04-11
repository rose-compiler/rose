include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

ADA_SRC_DIR := $(top_srcdir)/src/frontend/Experimental_Libadalang_ROSE_Connection

AM_CPPFLAGS := $(ROSE_INCLUDES) -I$(ADA_SRC_DIR) -I$(GNAT_HOME)/include

noinst_LTLIBRARIES := libexperimentalRoseAda.la

# DQ (8/23/2017): I'm not clear if we need makefile conditionals, since the directory is already conditionally processed in the parent directory.

if ROSE_EXPERIMENTAL_LIBADALANG_ROSE_CONNECTION

  libexperimentalRoseAda_la_SOURCES := \
    ada_support.C \
    Libadalang_to_ROSE.C \
    AdaMaker.C \
    LibadalangStatement.C \
    LibadalangType.C
else
# Compile using a function that represents the API of the Ada support.
  libexperimentalRoseAda_la_SOURCES = dummy_ada_main.C
endif

pkginclude_HEADERS := \
    AdaMaker.h 

test:
