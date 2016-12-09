#build the test main
main-raja: main-raja.cpp specialraja.so
	g++ -std=c++11 -o main-raja  main-raja.cpp -L. -lspecialraja
# build the shared library
specialraja.so:specialraja.cpp
	g++ -std=c++11 -fpic -shared specialraja.cpp -o libspecialraja.so
#
clean:
	rm -rf *.so *.o main-raja
