
include ../../makefile.inc
	
include make_rule.inc
include ../rose-pragma/make_rule.inc
include ../rose/make_rule.inc
include ../common/make_rule.inc
include ../maths/make_rule.inc
include ../system/make_rule.inc

TEST_ROSE_PRAGMA_BINS = $(bin_dir)/test-rose-pragma-polyhedric-annotation \
						$(bin_dir)/test-rose-pragma-dependency

TEST_ROSE_PRAGMA_OBJS = $(obj_dir)/test-rose-pragma/test-polyhedric-annotation.o \
						$(obj_dir)/test-rose-pragma/test-dependency.o

if ROSE_USE_CLOOG
TEST_ROSE_PRAGMA_BINS += $(bin_dir)/test-rose-pragma-code-generation \
						$(bin_dir)/test-rose-pragma-generate-all-schedules

TEST_ROSE_PRAGMA_OBJS += $(obj_dir)/test-rose-pragma/test-code-generation.o \
						$(obj_dir)/test-rose-pragma/test-generate-all-schedules.o
endif

all-am: $(TEST_ROSE_PRAGMA_BINS)

check:

docs:

clean:
	rm -f $(obj_dir)/test-rose-pragma/*
	rm -rf $(obj_dir)/test-rose-pragma/.libs
	rm -f $(bin_dir)/test-rose-pragma-polyhedric-annotation $(bin_dir)/.libs/test-rose-pragma-polyhedric-annotation
	rm -f $(bin_dir)/test-rose-pragma-dependency $(bin_dir)/.libs/test-rose-pragma-dependency
if ROSE_USE_CLOOG
	rm -f $(bin_dir)/test-rose-pragma-code-generation $(bin_dir)/.libs/test-rose-pragma-code-generation
	rm -f $(bin_dir)/test-rose-pragma-generate-all-schedules $(bin_dir)/.libs/test-rose-pragma-generate-all-schedules
endif

