## Process this file with automake to create Makefile.in.

INCLUDES = -I$(ROSE_INCLUDE_DIR) -I$(top_srcdir)/src/analyzer/astnumbering -I$(top_srcdir)/src/analyzer/astnumbering/satire

noinst_LIBRARIES = libAstNumbering.a
nobase_include_HEADERS = satire/EqualityTraversal.h

libAstNumbering_a_SOURCES = EqualityTraversal.C

#libIcfgBuilder_a_LIBADD = $(CFGOBJS)

#PAG_PATH = $(PAG_INCLUDE_DIR)/..

#CFGOBJS = $(addsuffix _cfg.o, $(shell cat $(PAG_PATH)/share/pag/cfg/files))

#%_cfg.o: $(PAG_PATH)/share/pag/cfg/%.c
#	$(COMPILE) -Wno-unused -c $< -o $@





