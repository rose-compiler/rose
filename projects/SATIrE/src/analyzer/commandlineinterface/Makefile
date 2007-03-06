FLAGS = -I /usr/local/mstools/include/rose$(ROSEVERSION)/ -I $(PAGDIR)/include/pag -I $(PAGDIR)/include/pag/analyzer

all: libsatire_commandlineinterface.a

libsatire_commandlineinterface.a: AnalyzerOptions.o set_pag_options.o CommandLineParser.o
	ar -csr libsatire_commandlineinterface.a AnalyzerOptions.o set_pag_options.o CommandLineParser.o

set_pag_options.o: set_pag_options.C set_pag_options.h
	g++ $(FLAGS) -c set_pag_options.C

CommandLineParser.o: CommandLineParser.C CommandLineParser.h
	g++ $(FLAGS) -c CommandLineParser.C

AnalyzerOptions.o: AnalyzerOptions.C AnalyzerOptions.h
	g++ $(FLAGS) -c AnalyzerOptions.C

clean:
	rm -f *~ *.a *.o
