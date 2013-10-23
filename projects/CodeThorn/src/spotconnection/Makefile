# set environment variable SPOT_PREFIX

ltlverifier: ltlverifier.C
	g++ -L$(SPOT_PREFIX)/lib -lspot -lbdd -I. -I$(SPOT_PREFIX)/include/spot -I$(SPOT_PREFIX)/include ltlverifier.C -oltlverifier

check: ltlverifier
#	./ltlverifier "(false R (! (iB & ! oX) | (! oX U (oZ & ! oX))))" stg.spot
	./ltlverifier "(G (iA U oX))" stg2.spot
	./ltlverifier "(G (oX U oY))" stg2.spot
	./ltlverifier "(G (oX & !oY))" stg2.spot
	./ltlverifier "(G (iB & !oY))" stg2.spot
	./ltlverifier "(G (iB & oY))" stg2.spot
	./ltlverifier "(G (iB | oY))" stg2.spot


