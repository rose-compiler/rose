
include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

bin_PROGRAMS = traversal

LDADD = ../../src/libPolyModel.la $(LIBS_WITH_RPATH) $(ROSE_LIBS)

traversal_SOURCES         = test-traversal.cpp
traversal_CXXFLAGS        = \
  -I $(srcdir)/../../src/ \
  -I$(PPL_PATH)/include \
  $(ROSE_INCLUDES)
traversal_LDADD           = $(LDADD)
traversal_bindir          = $(includedir)/polyhedral-model/rose-max-cover/


TEST_CODES = \
test_1.c


$(TEST_CODES): $(bin_PROGRAMS)
	./traversal -c $(srcdir)/test-codes/$@

check-local:
	@$(MAKE) $(TEST_CODES)

clean-local:
	rm -f rose_*.c

../../src/libPolyModel.la:
	make -C ../../src libPolyModel.la
