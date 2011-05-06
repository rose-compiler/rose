
include ../../makefile.inc

include ../../src/test-rose-pragma/make_rule.inc
include ../../src/rose-pragma/make_rule.inc
include ../../src/rose/make_rule.inc
include ../../src/common/make_rule.inc
include ../../src/maths/make_rule.inc
include ../../src/system/make_rule.inc

TEST_CODES = \
matcolsum.c \
matsum.c

$(TEST_CODES): $(bin_dir)/test-rose-pragma-polyhedric-annotation \
				$(bin_dir)/test-rose-pragma-dependency \
				$(bin_dir)/test-rose-pragma-code-generation \
				$(bin_dir)/test-rose-pragma-generate-all-schedules
	$(bin_dir)/test-rose-pragma-polyhedric-annotation $(srcdir)/$@ > $(@:.c=.scop)
	$(bin_dir)/test-rose-pragma-dependency $(srcdir)/$@ > $(@:.c=.deps)
if ROSE_USE_CLOOG
	$(bin_dir)/test-rose-pragma-code-generation $(srcdir)/$@
	@rm -rf $(@:.c=)
	@mkdir -p $(@:.c=)
	$(bin_dir)/test-rose-pragma-generate-all-schedules $(srcdir)/$@ $(@:.c=)
endif


check-local:
if ROSE_USE_PPL
	@$(MAKE) $(TEST_CODES)
endif

clean-local:
	rm -rf $(TEST_CODES:.c=)
	rm -f rose_*.c *.scop *.deps
	
#EXTRA_DIST= $(TEST_CODES)
