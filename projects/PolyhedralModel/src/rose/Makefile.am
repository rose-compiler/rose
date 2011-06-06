
include ../../makefile.inc
	
include make_rule.inc
include ../common/make_rule.inc

all-am: $(ROSE_OBJS)

check:

docs:

clean:
	rm -f $(obj_dir)/rose/*
	rm -rf $(obj_dir)/rose/.libs 

EXTRA_DIST= make_rule.inc rose-utils.cpp Variable.cpp Parser.cpp Quast-rose.cpp Exception-rose.cpp \
../../include/rose/Attribute.hpp \
../../include/rose/CodeGeneration.hpp \
../../include/rose/Parser.hpp \
../../include/rose/Exception-rose.hpp \
../../include/rose/rose-utils.hpp \
../../include/rose/Variable.hpp \
../../tpl/rose/Parser.tpp \
../../tpl/rose/Attribute.tpp \
../../tpl/rose/CodeGeneration.tpp
