include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

../../testTranslator:
	make -C ../.. testTranslator

LIST = \
test1 \
test2 

C_CODES = \
test1_c.cpp \
test2_c.cpp

FORTRAN_CODES = \
test1_f.f \
test2_f.f


TESTCODES_Objects = ${LIST}

$(TESTCODES_Objects): ../../testTranslator
	../../testTranslator $(srcdir)/$@_c.cpp $(srcdir)/$@_f.f -o $@

check-local:
if ROSE_BUILD_FORTRAN_LANGUAGE_SUPPORT
	@echo "Tests mix language support with C++ and Fortran"
	@$(MAKE) $(TESTCODES_Objects)
	@echo "***********************************************************************************************"
	@echo "****** ROSE/tests/CompileTests/mixLanguage_tests: make check rule complete (terminated normally) ******"
	@echo "***********************************************************************************************"
else
	@echo "Rose is configured without Fortran support"
endif

clean-local:
	rm -f *.o rose_* test_*

EXTRA_DIST= $(TESTCODES)

