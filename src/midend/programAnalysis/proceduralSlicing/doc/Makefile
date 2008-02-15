ROSE_LIB=/home/hauge2/ROSE_install/lib
 
ROSE_INCLUDE=/home/hauge2/ROSE_install/include
 
SLICING_OBJECTS = main.o
 
.cpp.o:
	g++ -c -I$(ROSE_INCLUDE) -I../ -o $@ $(@:.o=.cpp)
 
exampleSlice : $(SLICING_OBJECTS)
	g++ -I$(ROSE_INCLUDE) -I../ $(SLICING_OBJECTS) $(ROSE_LIB)/librose.so $(ROSE_LIB)/libedg.so ../Slicing.o ../SlicingCriterion.o ../MarkingNodes.o ../FindStatements.o ../RemovalOfNodes.o -o exampleSlice
 
clean:
	rm -rf exampelSlice *.o
 
