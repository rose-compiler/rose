# MS: makefile for local development with installed version of ROSE.

TOOLNAME3 = codethorn
MYDATE = `date +%Y_%m_%d`

CC                    = gcc-4.4
CXX                   = g++-4.4
BISON                 = bison

# Location of include directory after "make install" (set as environment variable)
#ROSE_INCLUDE_DIR = 

# Location of Boost include directory (set as environment variable)
#BOOST_CPPFLAGS = 

# Location of library directory after "make install" (set as environment variable)
#ROSE_LIB_DIR = 
ROSE_LIBS = -Wl,-rpath -Wl,$(ROSE_LIB_DIR) -L $(ROSE_LIB_DIR) -lrose

# Location of ROSE's libtool (it's only in the compilation path)

CPPFLAGS              = -O3
#CXXCPPFLAGS           = @CXXCPPFLAGS@
CXXFLAGS              = -Wno-deprecated
LDFLAGS               = 
YACC                  = bison

OBJECT_FILES=MyAst.o AstTerm.o Labeler.o SgNodeHelper.o CFAnalyzer.o ExprAnalyzer.o StateRepresentation.o Analyzer.o Visualizer.o Timer.o LanguageRestrictor.o AType.o

# Default make rule to use
all: $(TOOLNAME3)
	@if [ x$${ROSE_IN_BUILD_TREE:+present} = xpresent ]; then echo "ROSE_IN_BUILD_TREE should not be set" >&2; exit 1; fi


.PHONY: check dist viz clean distclean bsps

# Example suffix rule for more experienced makefile users
# .C.o:
#	g++ -c -I$(ROSE_INCLUDE_DIR) -o $@ $(@:.o=.C)

ltlparser: LTLParser.y++ LTL.h
	$(BISON) -p ltl_ $<
	$(CXX) -o $@ -g LTLParser.tab.c++

MyAst.o: MyAst.h MyAst.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) MyAst.C -c 

AstTerm.o: AstTerm.h AstTerm.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) AstTerm.C -c

Labeler.o: Labeler.h Labeler.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) Labeler.C -c

SgNodeHelper.o: SgNodeHelper.h SgNodeHelper.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) SgNodeHelper.C -c

CFAnalyzer.o: CFAnalyzer.h CFAnalyzer.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) CFAnalyzer.C -c

ExprAnalyzer.o: ExprAnalyzer.h ExprAnalyzer.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) ExprAnalyzer.C -c

AType.o: AType.h AType.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) AType.C -c

StateRepresentation.o: StateRepresentation.h StateRepresentation.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) StateRepresentation.C -c

Analyzer.o: Analyzer.h Analyzer.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) Analyzer.C -c

Visualizer.o: Visualizer.h Visualizer.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) Visualizer.C -c

Timer.o: Timer.h Timer.cpp
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) Timer.cpp -c

LanguageRestrictor.o: LanguageRestrictor.h LanguageRestrictor.C
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) LanguageRestrictor.C -c

$(TOOLNAME3): $(TOOLNAME3).C  $(TOOLNAME3).h $(OBJECT_FILES)
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $(ROSE_LIBS) $(OBJECT_FILES) $(TOOLNAME3).C -o$(TOOLNAME3) 

check: codethorn
	./codethorn tests/rers/Problem1.c

viz: transitiongraph1.dot transitiongraph2.dot cfg.dot ast.dot
	dot -Tps transitiongraph1.dot -oviz/transitiongraph1.ps
	dot -Tps transitiongraph2.dot -oviz/transitiongraph2.ps
	dot -Tps cfg.dot -oviz/cfg.ps
	dot -Gordering=out -Tps ast.dot -oviz/ast.ps

clean:
	rm -f testCode.o
	rm -f rose_testCode.o
	rm -f rose_testCode.C
	rm -f *.o
	rm -f $(TOOLNAME3)
	rm -f *.dot
	rm -f *.ps
	rm -f viz/*
	rm -f bsps/*

distclean: clean
	rm -f *.tgz
	rm -f *~

bsps: codethorn
	./codethorn tests/basictest10f.C && make viz
	cp viz/cfg.ps bsps/basictest10f_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest10f_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest10f_transitiongraph2.ps
	./codethorn tests/basictest15.C && make viz
	cp viz/cfg.ps bsps/basictest15_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest15_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest15_transitiongraph2.ps

dist:
	tar cvzf codethorn_$(MYDATE).tgz *.C *cpp *.h *.txt *.lxx *.yxx docs tests Makefile* TODO
