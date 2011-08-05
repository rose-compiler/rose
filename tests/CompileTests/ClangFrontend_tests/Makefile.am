include $(top_srcdir)/config/Makefile.for.ROSE.includes.and.libs

include c_test_codes.inc

all-am: Clang2Dot ../../testTranslator

check-am: $(C_TEST_CODES_dot) $(C_TEST_CODES_obj) $(C_TEST_CODES_rose_dot) 

png: $(C_TEST_CODES_png) $(C_TEST_CODES_rose_png)

INCLUDES = $(ROSE_INCLUDES)

bin_PROGRAMS = Clang2Dot

Clang2Dot_SOURCES  = DotConsumer.C DotVisitor.C Clang2Dot.C 
Clang2Dot_LDFLAGS  = $(CLANG_LDFLAGS)
Clang2Dot_CXXFLAGS = $(CLANG_CXXFLAGS)

dist_include_HEADERS = DotConsumer.h DotVisitor.h

../../testTranslator: ../../testTranslator.C
	make -C ../../ testTranslator

../../../tutorial/wholeASTGraphGenerator:
	make -C ../../../tutorial/ wholeASTGraphGenerator

clean-am:
	rm -f *.dot *.c *.C *.png *.o Clang2Dot

EXTRA_DIST = CMakeLists.txt
