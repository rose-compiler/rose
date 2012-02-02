
C_TEST_CODES = \
test_0.c \
test_1.c \
test_2.c \
test_3.c \
test_4.c \
test_5.c \
test_6.c \
test_7.c \
test_8.c \
test_9.c \
test_10.c \
test_11.c \
test_12.c \
test_13.c \
test_14.c \
test_15.c \
test_16.c \
test_17.c \
test_18.c

C_TEST_CODES_obj = $(C_TEST_CODES:.c=.o)

$(C_TEST_CODES_obj): $(builddir)/../../testTranslator
	$(builddir)/../../testTranslator -c $(srcdir)/test-codes/c/$(@:.o=.c) -o c-$@

#C_TEST_CODES_dot = $(C_TEST_CODES:.c=.dot)

#$(C_TEST_CODES_dot): $(builddir)/Clang2Dot
#	$(builddir)/Clang2Dot $(srcdir)/test-codes/c/$(@:.dot=.c) c-$@

#C_TEST_CODES_png = $(C_TEST_CODES:.c=.png)

#$(C_TEST_CODES_png): $(C_TEST_CODES_dot)
#	dot -Tpng c-$(@:.png=.dot) -o c-$@

C_TEST_CODES_rose_dot = $(C_TEST_CODES:.c=.rose.dot)

$(C_TEST_CODES_rose_dot): $(builddir)/../../../tutorial/wholeASTGraphGenerator
	$(builddir)/../../../tutorial/wholeASTGraphGenerator $(srcdir)/test-codes/c/$(@:.rose.dot=.c)
	mv $(@:.rose.dot=)_WholeAST.dot c-$@

C_TEST_CODES_rose_png = $(C_TEST_CODES:.c=.rose.png)

$(C_TEST_CODES_rose_png): $(C_TEST_CODES_rose_dot)
	dot -Tpng c-$(@:.rose.png=.rose.dot) -o c-$@

