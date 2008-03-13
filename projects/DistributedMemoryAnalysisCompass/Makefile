ROSEDIR = /home/panas2/development/ROSE-64bit/install-gcc4-boost/
BOOST_DIR=/home/panas2/development/boost-install-gcc4-64bit/
FLAGS = -DUSE_BOOST -I/${BOOST_DIR}/include/boost-1_35/


INCLUDES = -I$(ROSEDIR)/include/ -I$(ROSEDIR)/../ROSE-svn/projects/compass/ -I.
LPATH = -L$(ROSEDIR)/lib/ -Wl,-rpath $(ROSEDIR)/lib/  -L${BOOST_DIR}/lib -Wl,-rpath,${BOOST_DIR}/lib
LIBS = -lrose -lrt -lCompassCheckers -lboost_filesystem-gcc -lboost_wave-gcc

CXX = mpicxx
CXXFLAGS = -Wall $(INCLUDES)

OBJS = parallel_compass.o

all: parallel_compass

parallel_compass: $(OBJS)
	@echo
	@echo "----- linking $@ -----"
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LPATH) $(LIBS)
#	cp /g/g90/barany1/ROSE/projects/compass/include/compass_parameters .

%.o: %.C %.h
	@echo
	@echo "----- compiling $< -----"
	$(CXX) $(CXXFLAGS) $(FLAGS) -c $< -o $@

clean:
	rm -f *.o parallel_compass *~ gmon.out
