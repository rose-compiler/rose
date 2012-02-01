
CUDA_TEST_CODES = \
test_0.cu \
test_1.cu

CUDA_TEST_CODES_obj = $(CUDA_TEST_CODES:.cu=.o)

$(C_TEST_CODES_obj): $(builddir)/../../testTranslator
	$(builddir)/../../testTranslator -c $(srcdir)/test-codes/cuda/$(@:.o=.cu) -o cuda-$@

CUDA_TEST_CODES_dot = $(CUDA_TEST_CODES:.cu=.dot)

$(CUDA_TEST_CODES_dot): $(builddir)/Clang2Dot
	$(builddir)/Clang2Dot $(srcdir)/test-codes/cuda/$(@:.dot=.cu) cuda-$@

CUDA_TEST_CODES_png = $(CUDA_TEST_CODES:.cu=.png)

$(CUDA_TEST_CODES_png): $(CUDA_TEST_CODES_dot)
	dot -Tpng cuda-$(@:.png=.dot) -o cuda-$@

CUDA_TEST_CODES_rose_dot = $(CUDA_TEST_CODES:.cu=.rose.dot)

$(CUDA_TEST_CODES_rose_dot): $(builddir)/../../../tutorial/wholeASTGraphGenerator
	$(builddir)/../../../tutorial/wholeASTGraphGenerator $(srcdir)/test-codes/cuda/$(@:.rose.dot=.cu)
	mv $(@:.rose.dot=)_WholeAST.dot cuda-$@

CUDA_TEST_CODES_rose_png = $(CUDA_TEST_CODES:.cu=.rose.png)

$(CUDA_TEST_CODES_rose_png): $(CUDA_TEST_CODES_rose_dot)
	dot -Tpng cuda-$(@:.rose.png=.rose.dot) -o cuda-$@

