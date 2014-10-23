all: revtest1 revtest2
# revcheck

revtest1: rtest1.C revtest.C ../runtime/runtime.h ../runtime/runtime.C
	reverser rtest1.C
	echo '#include "runtime.h"' > rev_rtest1.C
	echo 'extern Backstroke::RunTimeSystem rts;' >> rev_rtest1.C
	cat rose_rtest1.C >> rev_rtest1.C
	g++ -O3 -I../runtime revtest.C rev_rtest1.C -pthread -orevtest1

revtest2: rtest2.C revtest.C ../runtime/runtime.h ../runtime/runtime.C
	reverser rtest2.C
	echo '#include "runtime.h"' > rev_rtest2.C
	echo 'extern Backstroke::RunTimeSystem rts;' >> rev_rtest2.C
	cat rose_rtest2.C >> rev_rtest2.C
	g++ -O3 -I../runtime revtest.C rev_rtest2.C -orevtest2 -pthread

revcheck: revcheck.C revtest1 ../runtime/runtime.h ../runtime/runtime.C
	g++ -O3 -I.. -I../runtime rtest1.C revcheck.C ../runtime/runtime.C -o revcheck -pthread

check: revcheck
	./revcheck

clean:
	rm -f rose_rtest1.C rose_rtest2.C
	rm -f rev_rtest1.C rev_rtest2.C
	rm -f revtest1 revtest2 revcheck
