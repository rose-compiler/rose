
main: main.o libshared.so
	g++ -o main  main.o -L. -lshared
# build the shared library
libshared.so: shared.cpp
	g++ -fpic -shared -o libshared.so shared.cpp
#
# for the main program
main.o: main.cpp
	g++ -c main.cpp
clean:
	rm -rf *.so *.o main
