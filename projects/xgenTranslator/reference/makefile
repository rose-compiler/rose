#build the test main
main-raja: main-raja.o libspecialraja.so
	g++ -std=c++11 -o main-raja.out main-raja.o -L. -lspecialraja
main-raja.o:main-raja.cpp
	g++ -std=c++11 -c main-raja.cpp
# build the shared library
libspecialraja.so:specialraja.cpp
	g++ -std=c++11 -fpic -shared specialraja.cpp -o libspecialraja.so
#
clean:
	rm -rf *.so *.o *.out
