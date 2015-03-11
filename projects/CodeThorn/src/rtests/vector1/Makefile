all: revtest1 revtest2
# revcheck

RUNTIME_INCLUDE=../../runtime

revtest1: rtest1.C revtest.C ${RUNTIME_INCLUDE}/runtime.h ${RUNTIME_INCLUDE}/runtime.C
	reverser rtest1.C
	echo '#include "runtime.h"' > rev_rtest1.C
	echo 'extern Backstroke::RunTimeSystem rts;' >> rev_rtest1.C
	cat rose_rtest1.C >> rev_rtest1.C
	g++ -O3 -I${RUNTIME_INCLUDE} revtest.C rev_rtest1.C -pthread -orevtest1

revtest2: rtest2.C revtest.C ${RUNTIME_INCLUDE}/runtime.h ${RUNTIME_INCLUDE}/runtime.C
	reverser rtest2.C
	echo '#include "runtime.h"' > rev_rtest2.C
	echo 'extern Backstroke::RunTimeSystem rts;' >> rev_rtest2.C
	cat rose_rtest2.C >> rev_rtest2.C
	g++ -O3 -I${RUNTIME_INCLUDE} revtest.C rev_rtest2.C -orevtest2 -pthread

TESTNAME=grtest1
$(TESTNAME).reversed: $(TESTNAME).C revtest.C ${RUNTIME_INCLUDE}/runtime.h ${RUNTIME_INCLUDE}/runtime.C
	reverser $(TESTNAME).C
	echo '#include "runtime.h"' > $(TESTNAME).reversed.C
	echo 'extern Backstroke::RunTimeSystem rts;' >> $(TESTNAME).reversed.C
	cat rose_$(TESTNAME).C >> $(TESTNAME).reversed.C

$(TESTNAME): $(TESTNAME).reversed.C revtest.C
	g++ -Wall -O3 -I${RUNTIME_INCLUDE} revtest.C $(TESTNAME).reversed.C -o$(TESTNAME) -pthread

revcheck: revcheck.C revtest1 ${RUNTIME_INCLUDE}/runtime.h ${RUNTIME_INCLUDE}/runtime.C
	g++ -Wall -O3 -I../.. -I${RUNTIME_INCLUDE} rtest1.C revcheck.C ${RUNTIME_INCLUDE}/runtime.C -o revcheck -pthread

check: revcheck
	./revcheck

clean:
	rm -f rose_rtest1.C rose_rtest2.C
	rm -f rev_rtest1.C rev_rtest2.C
	rm -f revtest1 revtest2 revcheck
	rm -f *.ti

