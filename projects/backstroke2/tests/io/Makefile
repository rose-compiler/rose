all: rose_couttest1.pp.C

couttest1.pp.C: couttest1.C
	cpp -P couttest1.C > couttest1.pp.C

rose_couttest1.pp.C: couttest1.pp.C
	identityTranslator -rose:test 5 couttest1.pp.C

wc:
	wc -l couttest1.C
	wc -l couttest1.pp.C
	wc -l rose_couttest1.pp.C

clean:
	rm -f couttest1.pp.C rose_couttest1.pp.C *.ti
