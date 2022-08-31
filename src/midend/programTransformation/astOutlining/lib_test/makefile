# supportive lib path
Outlining_Lib_PATH=../.

# the main function
#------------------------------
test_02.o: test_02.cpp
	g++ -I${Outlining_Lib_PATH} ${CXX_FLAGS} -c $<

../liboutlining.a:
	make -C ../. -f makefile-lib all

# shared lib    
#------------------------------
master_shared_library.so:rose_test_02_lib.cpp
	g++ -fPIC -shared $^ -o $@

# build the executable from the transformed file with main() 
#------------------------------
# must not use -shared -fPIC, or seg fault!
a.out:test_02.o master_shared_library.so ../liboutlining.a
	g++ -o $@ test_02.o ../liboutlining.a -Wl,--export-dynamic -g -ldl -lm  

#check_PROGRAM: a.out

check:a.out
	./a.out
TESTS = $(check_PROGRAM)

clean:
	rm -rf *.out *.o *.so
