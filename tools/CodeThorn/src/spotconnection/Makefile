# set environment variable SPOT_PREFIX and BOOST_HOME

all: spotdriver

spotdriver: spotdriver.C
	gcc -L$(SPOT_PREFIX)/lib -lspot -lbdd -L$(BOOST_HOME)/lib -lboost_regex -I$(SPOT_PREFIX)/include -I$(SPOT_PREFIX)/include/spot -I$(BOOST_HOME)/include spotdriver.C -ospotdriver  

check: tests/problem28/Problem28_opt_rersmode.spot tests/problem28/Problem28-solutions.txt
	./spotdriver tests/problem28/Problem28_opt_rersmode.spot tests/problem28/Problem28-solutions.txt
