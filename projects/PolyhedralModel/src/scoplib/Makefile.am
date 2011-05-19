
include ../../makefile.inc
	
include make_rule.inc

all-am: $(SCOPLIB_OBJS)

check:
	

docs:
	

clean:
	rm -f $(obj_dir)/scoplib/*
	rm -rf $(obj_dir)/scoplib/.libs

EXTRA_DIST= make_rule.inc Exception-ScopLib.cpp \
../../include/scoplib/Cloog.hpp \
../../include/scoplib/Candl.hpp \
../../include/scoplib/ScopLib.hpp \
../../include/scoplib/Exception-ScopLib.hpp \
../../tpl/scoplib/Cloog.tpp \
../../tpl/scoplib/ScopLib.tpp \
../../tpl/scoplib/Candl.tpp
