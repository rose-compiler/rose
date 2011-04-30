
include ../../makefile.inc
	
include make_rule.inc
include ../common/make_rule.inc
include ../maths/make_rule.inc

all-am: $(bin_dir)/matmul $(bin_dir)/matcolsum $(bin_dir)/matsum

check:

docs:

clean:
	rm -f $(obj_dir)/test-common/*
	rm -rf $(obj_dir)/test-common/.libs
	rm -f $(bin_dir)/matmul $(bin_dir)/.libs/matmul $(bin_dir)/matcolsum $(bin_dir)/.libs/matcolsum $(bin_dir)/matsum $(bin_dir)/.libs/matsum

