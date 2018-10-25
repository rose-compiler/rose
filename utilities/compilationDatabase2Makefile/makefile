all:cdb2makefile
clean: 
	rm -rf cdb2makefile *.o makefile-* makefile-*.diff
cdb2makefile: main.cpp
	g++ -std=c++11 -I. main.cpp -o $@

#check: makefile-bt makefile-amrx
check: makefile-bt.diff makefile-amrx.diff

makefile-bt: cdb2makefile 
	./cdb2makefile --input="tests/npb3.3_bt_compile_commands.json" --output=$@
makefile-amrx: cdb2makefile
	./cdb2makefile --input="tests/amrex_compile_commands.json" --output=$@

makefile-bt.diff: makefile-bt
	if diff $< references/$< > $@ ; then echo "Test Passed" ; else echo "Files differ; test failed"; cat $@; rm -rf $@; exit 1; fi

makefile-amrx.diff: makefile-amrx
	if diff $< references/$< > $@ ; then echo "Test Passed" ; else echo "Files differ; test failed"; cat $@; rm -rf $@; exit 1; fi
#check: cdb2makefile
#	./cdb2makefile -i"test.json"
