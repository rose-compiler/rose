include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
include $(top_srcdir)/tests/nonsmoke/specimens/java/Specimens.inc

java_srcdir=$(top_srcdir)/tests/nonsmoke/specimens/java
TEST_OUTPUT_FILES := ${nonsmoke_specimens_java_classes:.class=.class.passed}

## targets
#
.PHONY: all
all: check-local

../../../functional/testTranslator:
	@cd ../../functional; $(MAKE) testTranslator; cd -


$(TEST_OUTPUT_FILES):../../../functional/testTranslator
	../../../functional/testTranslator $(java_srcdir)/$(@:.passed=)
	@diff $(java_srcdir)/$(@:.passed=) rose_$(@:.passed=)
	@touch $(@)

.PHONY: print
print:
	@echo "Output Makefile variables:"
	@echo "  nonsmoke_specimens_java_classes = $(nonsmoke_specimens_java_classes)"
	@echo "  TEST_OUTPUT_FILES = $(TEST_OUTPUT_FILES)"
	echo "top_builddir = $(top_builddir)"
	echo "top_srcdir = $(top_srcdir)"
	echo "java_srcdir = $(java_srcdir)"

check-local:
	@echo "Test parsing and unparsing of Java class files:"
	@$(MAKE) $(TEST_OUTPUT_FILES)
	@echo "***********************************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/BinaryAnalysis/ByteCode: make check terminated successfully ******"
	@echo "***********************************************************************************************************************"

clean-local:
	rm -f rose_*.class *.passed
