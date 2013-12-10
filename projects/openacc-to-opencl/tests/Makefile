
TESTS=\
 101-vector-constant-addition\
 102-vector-constant-product\
 103-vector-vector-addition

all:
	for test in $(TESTS) ; do \
		TEST_SRCDIR=`pwd`/$$test make -C $$test; \
	done

check:
	for test in $(TESTS) ; do \
		TEST_SRCDIR=`pwd`/$$test make -C $$test check ; \
	done

eval:
	for test in $(TESTS) ; do \
		TEST_SRCDIR=`pwd`/$$test make -C $$test eval ; \
	done

clean:
	for test in $(TESTS) ; do \
		TEST_SRCDIR=`pwd`/$$test make -C $$test clean ; \
	done

