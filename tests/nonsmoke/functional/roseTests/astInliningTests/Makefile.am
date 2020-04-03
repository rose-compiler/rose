include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs
noinst_PROGRAMS =
TEST_TARGETS =
EXTRA_DIST =

TEST_EXIT_STATUS = $(top_srcdir)/scripts/test_exit_status

#------------------------------------------------------------------------------------------------------------------------
# inlineEverything

bin_PROGRAMS = inlineEverything
inlineEverything_SOURCES = inlineEverything.C
inlineEverything_CPPFLAGS = $(ROSE_INCLUDES)
inlineEverything_LDFLAGS = $(ROSE_RPATHS)
inlineEverything_LDADD = $(ROSE_SEPARATE_LIBS)

# No longer has name convention restrictions  since we don't use RTH anymore.
# Note: must use the name convention of specimenXX_N.C , in which N is the number of function calls inlined.   
# The specimens are named so that the number between the "_" and next "." is the number of function calls that
# we expect this specimen to inline.
inlineEverything_specimens =			\
	specimen01_1.C				\
	specimen02_1.C				\
	specimen03_1.C				\
	specimen04_1.C				\
	specimen05_1.C				\
	specimen06_1.C				\
	specimen07_1.C				\
	specimen08_1.C				\
	specimen09_1.C				\
	specimen10_1.C				\
	specimen11_3.C				\
	specimen12_3.C				\
	specimen13_1.C				\
	specimen14_1.C				\
	specimen15_1.C				\
	specimen16_4.C				\
	specimen17_10.C				\
	specimen18_1.C				\
	specimen19_1.C				\
	specimen20_1.C				\
	specimen21_1.C				\
	specimen22_1.C                          \
	specimen25_1.C
# current failing 	
#	specimen26_1.C , with system header, failing postprocessing 
#        specimen23_1.C   \
#        specimen24_1.C   \ #asdfsa

#Liao, new set of tests with C++11 lambda expressions, not yet supported
inlineEverything_cpp11_specimens =			\
        simplest_raja.C

EXTRA_DIST += $(inlineEverything_specimens) inlineEverything.conf

# Abandon the use of RTH way of testings: very hard to inspect generated files in a batch
#inlineEverything_test_targets = $(addprefix inlineEverything_, $(addsuffix .passed, $(inlineEverything_specimens)))
#TEST_TARGETS += $(inlineEverything_test_targets)
#$(inlineEverything_test_targets): inlineEverything_%.passed: % inlineEverything inlineEverything.conf
#	@$(RTH_RUN)												\
#		TITLE="inlineEverything $< [$@]"								\
#		SPECIMEN="$(abspath $<)"									\
#		NINLINE="$$(echo $(notdir $<) |sed --regexp-extended 's/specimen[0-9]+_([0-9]+).*/\1/')"	\
#		TRANSLATOR="$$(pwd)/inlineEverything"								\
#		$(srcdir)/inlineEverything.conf $@

# test default behavior
#-----------------------------------------------------------
inlineEverything_test_targets_v1 = $(addprefix rose_v1_, $(inlineEverything_specimens))
check-v1:$(inlineEverything_test_targets_v1)

TEST_TARGETS += $(inlineEverything_test_targets_v1)
$(inlineEverything_test_targets_v1): $(@:rose_v1_%=%) inlineEverything 
		$(builddir)/inlineEverything -c $(srcdir)/$(@:rose_v1_%=%) -rose:output $@

# skip the postprocessing cleanup step -skip-postprocessing
#-----------------------------------------------------------
inlineEverything_test_targets_v2 = $(addprefix rose_v2_, $(inlineEverything_specimens))
check-v2:$(inlineEverything_test_targets_v2)

TEST_TARGETS += $(inlineEverything_test_targets_v2)
$(inlineEverything_test_targets_v2): $(@:rose_v2_%=%) inlineEverything 
		$(builddir)/inlineEverything -skip-postprocessing -c $(srcdir)/$(@:rose_v2_%=%) -rose:output $@

# test token based unparsing option: -rose:unparse_tokens
#-----------------------------------------------------------
inlineEverything_withTokenStreamUnparsing_specimens = \
   testWithTokenStreamUnparsing_01.C \
   testWithTokenStreamUnparsing_02.C

inlineEverything_test_tokenStreamUnparsing_targets = $(addprefix rose_v3_, $(inlineEverything_withTokenStreamUnparsing_specimens))
TEST_TOKEN_STREAM_UNPARSING_TARGETS = $(inlineEverything_test_tokenStreamUnparsing_targets)
check-v3:$(inlineEverything_test_tokenStreamUnparsing_targets)
$(inlineEverything_test_tokenStreamUnparsing_targets): $(@:rose_v3_%=%) inlineEverything 
		$(builddir)/inlineEverything -rose:unparse_tokens -c $(srcdir)/$(@:rose_v3_%=%) -rose:output $@

# test C++11 feature inlining:  inlineEverything_cpp11_specimens, -std=c++11
#-----------------------------------------------------------
inlineEverything_test_cpp11_targets = $(addprefix rose_cpp11_v1_, $(inlineEverything_cpp11_specimens))
check-cpp11-v1:$(inlineEverything_test_cpp11_targets)
# TODO: uncommened the followin line when ready 
# TODO: only turn on this when C++11 is turned on
# TEST_TARGETS += $(inlineEverything_test_cpp11_targets)
$(inlineEverything_test_cpp11_targets): $(@:rose_cpp11_v1_%=%) inlineEverything
	$(builddir)/inlineEverything -std=c++11 -c $(srcdir)/$(@:rose_cpp11_v1_%=%) -rose:output $@

EXTRA_DIST += test1.conf test1A.C test1B.C
test1.passed: test1A.C test1B.C
	@$(RTH_RUN) $(srcdir)/test1.conf $@

test: $(TEST_TOKEN_STREAM_UNPARSING_TARGETS)

check-local: $(TEST_TARGETS) $(TEST_TOKEN_STREAM_UNPARSING_TARGETS)
	@echo "***************************************************************************************************"
	@echo "****** ROSE/tests/nonsmoke/functional/roseTests/astInliningTests: make check rule complete (terminated normally) ******"
	@echo "***************************************************************************************************"

clean-local:
	rm -f $(TEST_TARGETS) $(TEST_TOKEN_STREAM_UNPARSING_TARGETS)
	rm -f $(TEST_TARGETS:.passed=.failed)
