APLUSPLUS=/usr/casc/overture/A++P++/A++P++-0.7.2c/A++/lib/solaris_cc_CC
CXX = CC
CXXFLAGS = 
INCLUDEPATH = -I$(APLUSPLUS)/include
LIBPATH = -L$(APLUSPLUS)/lib
LIBS = -lApp -lApp_static -lm -lc

CCFLAGS = $(INCLUDEPATH)

.SUFFIXES:
.SUFFIXES:.C .o
.C.o:;$(CXX) $(CXXFLAGS) $(CCFLAGS) -c $*.C

riemann: riemann.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

rose_riemann: rose_riemann.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

testcode: testcode.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

rose_testcode: rose_testcode.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

testppp: testppp.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

rose_testppp: rose_testppp.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

mg: mg.o mg_main.o mg1level.o pde.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

rose_mg: rose_mg.o rose_mg_main.o rose_mg1level.o rose_pde.o
	$(CXX) -o $@ $? $(LIBPATH) $(LIBS)

clean: 
	rm -rf Templates.DB core *.o *.out riemann rose_riemann testcode rose_testcode testppp rose_testppp mg rose_mg
