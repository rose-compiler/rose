include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

FLANG_INCLUDES = -I$(FLANG_INSTALL_PATH)/include

AM_CPPFLAGS = $(ROSE_INCLUDES) $(FLANG_INCLUDES)
AM_CXXFLAGS = $(CXXFLAGS) -std=c++17
CXXFLAGS =

noinst_LTLIBRARIES = libFlangParser.la

if ROSE_EXPERIMENTAL_FLANG_ROSE_CONNECTION
  libFlangParser_la_SOURCES = flang-external-builder-main.cc
endif

noinst_HEADERS = flang-external-builder-main.h
