# set environment variable SPOT_PREFIX

ltlverifier: ltlverifier.C
	$(ROSE_CXX) -L$(SPOT_PREFIX)/lib -lspot -lbdd -I. -I$(SPOT_PREFIX)/include/spot -I$(SPOT_PREFIX)/include -I$(BOOST_HOME)/include ltlverifier.C -oltlverifier

check1: ltlverifier
	./ltlverifier "(false R ! oZ)" stg2.spot # YES
	./ltlverifier "(true U oY)" stg2.spot # YES
	./ltlverifier "(false R (! ((oY & ! iB) & (true U iB)) | (! oX U iB))) " stg2.spot # YES
	./ltlverifier "(! oX W iA)" stg2.spot # input A precedes output X (YES)
	./ltlverifier "(! oY W iA)" stg2.spot # input A precedes output Y (YES)
	./ltlverifier "(! iA W oX)" stg2.spot # output X precedes input A (NO)
	./ltlverifier "(! iA W oY)" stg2.spot # output Y precedes input A (NO)
	./ltlverifier "(false R ! oX) " stg2.spot # output X does never occur (NO)

# for experimental testing only. create a link to an executable of codethorn.
check2a: ltlverifier ./codethorn
	rm -f stg2.ct.spot
	./codethorn --rersmode=yes stg2.c --semantic-fold=yes --report-semantic-fold=yes --semantic-explosion=yes --spot-stg=stg2.ct.spot

check2b: ltlverifier ./codethorn
	rm -f stg2.ct.spot
	./codethorn --rersmode=yes stg2.c --semantic-fold=yes --report-semantic-fold=yes --input-values="{1,2,3,4,5,6}" --spot-stg=stg2.ct.spot

#./codethorn --rersmode=yes /home/schordan1/development/rers13/rersmon/rers2013/whitebox/Problem901.c --spot-stg=Problem901.spot --input-values="{1,2,3,4,5,6}"
#./codethorn whitebox/Problem901.c --rersmode=yes --semantic-fold=yes --report-semantic-fold=yes --input-values="{1,2,3,4,5,6}" --spot-stg=Problem901.spot
#./codethorn whitebox/Problem901.c --rersmode=yes --semantic-fold=yes --report-semantic-fold=yes --viz=yes

check2: stg2.ct.spot
	./ltlverifier "(false R ! oZ)" stg2.ct.spot # YES
	./ltlverifier "(true U oY)" stg2.ct.spot # YES
	./ltlverifier "(false R (! ((oY & ! iB) & (true U iB)) | (! oX U iB))) " stg2.ct.spot # YES
	./ltlverifier "(! oX W iA)" stg2.ct.spot # input A precedes output X (YES)
	./ltlverifier "(! oY W iA)" stg2.ct.spot # input A precedes output Y (YES)
	./ltlverifier "(! iA W oX)" stg2.ct.spot # output X precedes input A (NO)
	./ltlverifier "(! iA W oY)" stg2.ct.spot # output Y precedes input A (NO)
	./ltlverifier "(false R ! oX) " stg2.ct.spot # output X does never occur (NO)

# NOTES
# Problem 901:
# Formula: (true U oV)
# "output V occurs eventually"
# Formula is not satisfied! An error path is 
# [iD, oU] ([iA, oZ])*
# output of V does occur (on an error free path) but there is another path which leads into an error. Therefore it is not always the case that V occurs.
check3: Problem901.spot
	./ltlverifier "(false R (! (iB & ! oX) | (! oX U (oZ & ! oX))))" Problem901.spot # output Z occurs after input B until output X (YES)
	./ltlverifier "(! (true U iE) | (! oW U (oZ | iE)))" Problem901.spot # output Z precedes output W before input E" (YES)
	./ltlverifier "(true U oV)" Problem901.spot # output V occurs eventually (NO)
	./ltlverifier "(false R (! (iA & ! oU) | (! oU U (oW & ! oU))))" Problem901.spot # output W occurs after input A until output U (NO)
