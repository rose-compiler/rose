all: revtest1
# revtest2 revcheck

revtest1: rtest1.C revtest.C ../runtime/runtime.h ../runtime/runtime.C
	reverser rtest1.C
	mv rose_rtest1.C rev_rtest1.C
	g++ -I../runtime revtest.C -pthread -orevtest1

revtest2: rtest2.C revtest.C ../runtime/runtime.h ../runtime/runtime.C
	reverser rtest2.C
	mv rose_rtest2.C rev_rtest2.C
	g++ -I../runtime rev_rtest2.C revtest.C ../runtime/runtime.C -orevtest2

revcheck: revcheck.C revtest2 ../runtime/runtime.h ../runtime/runtime.C
	g++ -I../runtime revcheck.C ../runtime/runtime.C -o revcheck

check: revcheck
	./revcheck

clean:
	rm -f rev_rtest1.C rev_rtest2.C
	rm -f revtest1 revtest2 revcheck
