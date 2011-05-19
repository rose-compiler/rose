
include ../../makefile.inc
include ../rose/make_rule.inc
include ../common/make_rule.inc
	
include make_rule.inc

all-am: $(ROSE_PRAGMAS_OBJS)

check:

docs:

clean:
	rm -f $(obj_dir)/rose-pragma/*
	rm -rf $(obj_dir)/rose-pragma/.libs

EXTRA_DIST= make_rule.inc PolyhedricPragmaParser.cpp Exception-rose-pragma.cpp PolyhedricPragma.cpp \
../../include/rose-pragma/PolyhedricPragmaParser.hpp \
../../include/rose-pragma/Exception-rose-pragma.hpp \
../../include/rose-pragma/PolyhedricPragma.hpp
