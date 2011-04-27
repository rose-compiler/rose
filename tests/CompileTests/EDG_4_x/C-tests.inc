
C_TESTS_TO_CHECK = \
callee.c \
caller.c \
commentTest.c \
complex_01.c \
constants.c \
gconv_info.c \
gzip.c \
math.c \
old_style_function.c \
stdio.c \
test2003_12.c \
test2004_20.c \
test2004_21.c \
test2005_168.c \
test2005_172.c \
test2005_186.c \
test2006_14.c \
test2006_48.c \
test2006_65.c \
test2006_100.c \
test2006_110.c \
test2006_114.c \
test2006_132.c \
test2006_133.c \
test2006_134.c \
test2006_135.c \
test2006_136.c \
test2007_177.c \
test2008_01.c \
test2008_02.c \
test2009_01.c \
test2009_02.c \
test2009_03.c \
test2009_04.c \
test2009_07.c \
test2009_08.c \
test2009_16.c \
test2009_17.c \
test2009_18.c \
test2009_20.c \
test2009_21.c \
test2009_22.c \
test2010_01.c \
test2010_02.c \
test2010_03.c \
test2010_04.c \
test2010_05.c \
test2010_06.c \
test2010_07.c \
test2010_08.c \
test2010_09.c \
test2010_10.c \
test2010_11.c \
test2010_12.c \
test2010_13.c \
test2010_14.c \
test2010_15.c \
test2010_17.c \
test2011_03.c \
test2011_04.c \
test2011_05.c \
testAnsiC.c \
testCvsCpp.c \
test_CplusplusMacro_C.c \
YardenPragmaPackExample.c \
Yarden_client.c

C_TESTS_FAILLING_WITH_3_3_OR_MISSING_AND_FAILLING_WITH_4_X  = \
test2004_126.c \
test2007_70.c \
test2007_95.c \
test2009_05.c \
test2009_06.c \
test2009_19.c \
test2010_16.c \
zpagccp.c

$(C_TESTS_TO_CHECK): ./EDG_4_x_Translator
	@cp $(srcdir)/../C_tests/$@ $@
	@if ./EDG_4_x_Translator $(ROSE_FLAGS) -I$(srcdir)/../C_tests -c $@ &> /dev/null; then \
	  echo 1 $@ >> C-tests.lst; \
	else \
	  echo 0 $@ >> C-tests.lst; \
	fi;
	@rm -f $@

test2005_168.c: ./EDG_4_x_Translator
	@cp $(srcdir)/../C_tests/$@ $@
	@if ./EDG_4_x_Translator $(ROSE_FLAGS) -I$(top_srcdir)/src/util/commandlineProcessing -I$(srcdir)/../C_tests -c $@ &> /dev/null; then \
	  echo 1 $@ >> C-tests.lst; \
	else \
	  echo 0 $@ >> C-tests.lst; \
	fi;
	@rm -f $@

check-Ctests:
	@rm -f C-tests.lst
	@$(MAKE) $(C_TESTS_TO_CHECK)

