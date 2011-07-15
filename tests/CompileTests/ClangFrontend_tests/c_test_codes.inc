
C_TEST_CODES = \
test_0.c \
test_1.c \
test_2.c \
test_3.c \
test_4.c \
test_5.c \
test_6.c

C_TEST_CODES_obj = $(C_TEST_CODES:.c=.o)

$(C_TEST_CODES_obj): ../../testTranslator
	../../testTranslator -c $(srcdir)/test-codes/c/$(@:.o=.c) -o c-$@

C_TEST_CODES_dot = $(C_TEST_CODES:.c=.dot)

$(C_TEST_CODES_dot): Clang2Dot
	Clang2Dot $(srcdir)/test-codes/c/$(@:.dot=.c) c-$@

C_TEST_CODES_png = $(C_TEST_CODES:.c=.png)

$(C_TEST_CODES_png): $(C_TEST_CODES_dot)
	dot -Tpng c-$(@:.png=.dot) -o c-$@

