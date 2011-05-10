
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

#include ../../makefile.inc

#include ../../src/test-rose-pragma/make_rule.inc
#include ../../src/rose-pragma/make_rule.inc
#include ../../src/rose/make_rule.inc
#include ../../src/common/make_rule.inc
#include ../../src/maths/make_rule.inc
#include ../../src/system/make_rule.inc

bin_PROGRAMS = polyhedral-annotation dependency

if ROSE_USE_CLOOG
bin_PROGRAMS += code-generation generate-all-schedules
endif

polyhedral_annotation_SOURCES         = polyhedral-annotation.cpp
polyhedral_annotation_CXXFLAGS        = \
  -I $(srcdir)/../../src/ \
  -I $(srcdir)/../../../pragmaParsing \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
polyhedral_annotation_LDADD           = ../../src/libPolyModel.la
polyhedral_annotation_bindir          = $(includedir)/polyhedral-model/rose-pragma/

dependency_SOURCES         = dependency.cpp
dependency_CXXFLAGS        = \
  -I $(srcdir)/../../src/ \
  -I $(srcdir)/../../../pragmaParsing \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
dependency_LDADD           = ../../src/libPolyModel.la
dependency_bindir          = $(includedir)/polyhedral-model/rose-pragma/

code_generation_SOURCES         = code-generation.cpp
code_generation_CXXFLAGS        = \
  -I $(srcdir)/../../src/ \
  -I $(srcdir)/../../../pragmaParsing \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES) \
  -DCLOOG_INT_GMP -I$(CLOOG_PATH)/include
code_generation_LDADD           = ../../src/libPolyModel.la
code_generation_bindir          = $(includedir)/polyhedral-model/rose-pragma/

generate_all_schedules_SOURCES         = generate-all-schedules.cpp
generate_all_schedules_CXXFLAGS        = \
  -I $(srcdir)/../../src/ \
  -I $(srcdir)/../../../pragmaParsing \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES) \
  -DCLOOG_INT_GMP -I$(CLOOG_PATH)/include
generate_all_schedules_LDADD           = ../../src/libPolyModel.la
generate_all_schedules_bindir          = $(includedir)/polyhedral-model/rose-pragma/

TEST_CODES = \
matcolsum.c \
matsum.c


$(TEST_CODES): $(bin_PROGRAMS)
	./polyhedral-annotation $(srcdir)/test-codes/$@ > $(@:.c=.scop)
	./dependency $(srcdir)/test-codes/$@ > $(@:.c=.deps)
if ROSE_USE_CLOOG
	./code-generation $(srcdir)/test-codes/$@
	@rm -rf $(@:.c=)
	@mkdir -p $(@:.c=)
	./generate-all-schedules $(srcdir)/test-codes/$@ $(@:.c=)
endif


check-local:
	@$(MAKE) $(TEST_CODES)

clean-local:
	rm -rf $(TEST_CODES:.c=)
	rm -f rose_*.c *.scop *.deps
