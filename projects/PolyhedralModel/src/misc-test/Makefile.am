
include ../../makefile.inc
	
include make_rule.inc
include ../maths/make_rule.inc

all-am: $(bin_dir)/quast $(bin_dir)/explorer

check:

doc:

clean:
	rm -f $(obj_dir)/misc-test/*
	rm -rf $(obj_dir)/misc-test/.libs
	rm -f $(bin_dir)/quast $(bin_dir)/.libs/quast $(bin_dir)/explorer $(bin_dir)/.libs/explorer

EXTRA_DIST= make_rule.inc explorer.cpp quast.cpp
