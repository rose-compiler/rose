# set environment variable SPOT_PREFIX

ltlverifier: ltlverifier.C
	$(ROSE_CXX) -L$(SPOT_PREFIX)/lib -lspot -lbdd -I. -I$(SPOT_PREFIX)/include/spot -I$(SPOT_PREFIX)/include -I$(BOOST_HOME)/include ltlverifier.C -oltlverifier

check: ltlverifier
	./ltlverifier "(false R ! oZ)" stg2.spot # YES
	./ltlverifier "(true U oY)" stg2.spot # YES
	./ltlverifier "(false R (! ((oY & ! iB) & (true U iB)) | (! oX U iB))) " stg2.spot # YES
	./ltlverifier "(! oX W iA)" stg2.spot # input A precedes output X (YES)
	./ltlverifier "(! oY W iA)" stg2.spot # input A precedes output Y (YES)
	./ltlverifier "(! iA W oX)" stg2.spot # output X precedes input A (NO)
	./ltlverifier "(! iA W oY)" stg2.spot # output Y precedes input A (NO)
	./ltlverifier "(false R ! oX) " stg2.spot # output X does never occur (NO)
