ROSE_INS = /home/hou1/opt/rose
#-------------translator---------------------------

#SRC_FILE=main.cpp
SRC_FILE=headerFilesGraphGenerator.C
#SRC_FILE=headersScanner.cpp
#SRC_FILE=isevent.cpp


#---------------input code ----------------------

INPUT_CODE= /home/hou1/opt/rose/include include_path.txt macro_defined.txt


BOOST_ROOT=/home/hou1/opt/boost_1_36_0/include/boost-1_36/
BOOST_LIB=/home/hou1/opt/boost_1_36_0/lib

CXXFLAGS=-g3 

.PHONY :clean test default
all:clean test
default: a.out test
# create the translator
a.out:$(SRC_FILE)
	g++ $(CXXFLAGS) $(SRC_FILE) -I $(BOOST_ROOT) -I $(ROSE_INS)/include -I/usr/include/libxml2 -L$(ROSE_INS)/lib -L$(BOOST_LIB) -lboost_date_time -lboost_system-gcc42-mt-1_36 -lboost_filesystem-gcc42-mt-1_36 -lboost_program_options-gcc42-mt-1_36 -lboost_regex-gcc42-mt-1_36
#	g++ $(CXXFLAGS) $(SRC_FILE) -I $(BOOST_ROOT) -L$(BOOST_LIB) -lboost_regex-gcc42-mt-1_36 -lboost_wave-gcc42-mt-1_36 -lboost_system-gcc42-mt-1_36 -lboost_filesystem-gcc42-mt-1_36

# test the translator
# -rose:skipfinalCompileStep
# --edg:no_warnings
test: a.out
	./a.out  $(INPUT_CODE)
clean:
	rm -rf a.out *.o rose_* *.dot

dot:
	/home/hou1/opt/rose/exampleTranslators/DOTGenerator/dotGenerator -c $(TESTFLAGS) -I. -I$(ROSE_INS)/include -I$(ROSS_INCLUDE) -I$(AIRPORT_PATH) -g3 test0.c 
	dot -Tpdf test0.c.dot -o test0.c.pdf

gdb:
	gdb --args a.out -c -DROSS_CLOCK_i386 -DROSS_THREAD_pthread -DROSS_NETWORK_none -DROSS_RAND_clcg4  -I. -I/home/hou1/opt/rose/include -I/home/hou1/pdes/trunk/ross -I/home/hou1/pdes/trunk/models/airport -g3 /home/hou1/test/Reverse/test0.c

