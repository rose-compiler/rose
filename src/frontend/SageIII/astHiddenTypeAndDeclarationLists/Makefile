
#FLAGS="-Wall"

PDFLIB_BASE = PDFlib-Lite-6.0.2rv

SOURCES=HiddenList_Output.C HiddenList.C HiddenList_Intersection.C HiddenList_main.C

OBJECTS = $(SOURCES:.C=.o)

ROSE_ROOT=/home/preissl1/LATEST_ROSE/gcc_3.3.2_NEW_ROSE
#ROSE_ROOT=/home/preissl1/LATEST_ROSE/gcc_3.3.2_ROSE/

ROSE_INCLUDES =	-I$(ROSE_ROOT)/include

LIBS_WITH_RPATH     =  -L$(ROSE_ROOT)/lib -Wl,-rpath $(ROSE_ROOT)/lib

ROSE_SEPARATE_LIBS = -lRoseCommandlineUtil -lsupport -lgraphs -lroseutil -lcr -lmstl -lpdf -lsage3 -ledgutil -ledg33 -ledgSage -lastinlining -lastInterface -lastSupport -lsymbolicVal -lannotation -lOAWrap -lCFG  -ldataflowAnalysis -lvaluePropagation  -ldepInfo -ldepGraph -lcomputation -lslicing -ldriver -lprepostloop -lLoopProcessing -lunparse -lroseSupport -lquery -lastDiagnostics -lastprocessing $(ROSE_SIDEEFFECTS_LIBS) $(ROSE_DATABASE_LIBS) $(MYSQL_DATABASE_LIBS) $(LIB_QT) $(RT_LIBS)

ROSE_LIBS = -lrose $(LEXLIB) $(MYSQL_DATABASE_LIBS) $(LIB_QT) -lm $(RT_LIBS)

all: HiddenList

HiddenList: $(OBJECTS)
	g++ -o HiddenList $(LIBS_WITH_RPATH) $(ROSE_LIBS) -g -Wall -L $(ROSE_ROOT)/lib $(OBJECTS) $(FLAGS)

.C.o:
	g++ -c -DHAVE_CONFIG_H -o $@ $(@:.o=.C) $(ROSE_INCLUDES) -g $(FLAGS)

clean:
	rm *.o -f
	rm core.* -f
