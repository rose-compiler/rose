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

CPPFLAGS              = -O3 -march=native -ftree-vectorize -ggdb
#CXXCPPFLAGS           = @CXXCPPFLAGS@
CXXFLAGS              = -Wno-deprecated
LDFLAGS               = 
YACC                  = bison

OBJECT_FILES=MyAst.o AstTerm.o Labeler.o SgNodeHelper.o CFAnalyzer.o ExprAnalyzer.o StateRepresentation.o Analyzer.o Visualizer.o Timer.o LanguageRestrictor.o AType.o LTLParser.o LTLChecker.o

OBJECT_FILES_EXPERIMENTAL=BoostGraphIntegration.o

# Default make rule to use
all: $(TOOLNAME3)
	@if [ x$${ROSE_IN_BUILD_TREE:+present} = xpresent ]; then echo "ROSE_IN_BUILD_TREE should not be set" >&2; exit 1; fi


.PHONY: check dist viz clean distclean bsps doc

# Example suffix rule for more experienced makefile users
# .C.o:
#	g++ -c -I$(ROSE_INCLUDE_DIR) -o $@ $(@:.o=.C)

ltlparser: LTLParser.y++ LTL.h
	$(BISON) -p ltl_ $<
	$(CXX) -DLTL_PARSER_DEBUG -o $@ $(CXXFLAGS) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) LTLParser.tab.c++

LTLParser.o: LTLParser.y++ LTL.h
	$(BISON) -p ltl_ $<
	$(CXX) -o $@ $(CXXFLAGS) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) LTLParser.tab.c++ -c

%.o: %.C %.h
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $< -c 

$(TOOLNAME3): $(TOOLNAME3).C  $(TOOLNAME3).h $(OBJECT_FILES)
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $(ROSE_LIBS) $(OBJECT_FILES) $(TOOLNAME3).C -o$(TOOLNAME3) 

check: $(TOOLNAME3)
	./$(TOOLNAME3) tests/rers/Problem1.c

test: $(TOOLNAME3)
	./$(TOOLNAME3) tests/rers/Problem1.c --verify tests/rers/properties1.ltl

doc:
	doxygen Doxyfile

viz: transitiongraph1.dot transitiongraph2.dot cfg.dot ast.dot
	dot -Tps transitiongraph1.dot -oviz/transitiongraph1.ps
	dot -Tps transitiongraph2.dot -oviz/transitiongraph2.ps
	dot -Tps cfg.dot -oviz/cfg.ps
	dot -Gordering=out -Tps ast.dot -oviz/ast.ps
vizjpg: transitiongraph1.dot transitiongraph2.dot cfg.dot ast.dot
	dot -Tjpg transitiongraph1.dot -oviz/transitiongraph1.jpg
	dot -Tjpg transitiongraph2.dot -oviz/transitiongraph2.jpg
	dot -Tjpg cfg.dot -oviz/cfg.jpg
	dot -Gordering=out -Tjpg ast.dot -oviz/ast.jpg

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
	rm -rf html latex

bsps: codethorn
	./codethorn tests/basictest10f.C && make viz && make vizjpg
	cp tests/basictest10f.C bsps
	cp viz/cfg.ps bsps/basictest10f_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest10f_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest10f_transitiongraph2.ps
	cp viz/ast.jpg bsps/basictest10f_ast.jpg
	cp viz/cfg.jpg bsps/basictest10f_cfg.jpg
	cp viz/transitiongraph1.jpg bsps/basictest10f_transitiongraph1.jpg
	cp viz/transitiongraph2.jpg bsps/basictest10f_transitiongraph2.jpg
	./codethorn tests/basictest15.C && make viz && make vizjpg
	cp tests/basictest15.C bsps
	cp viz/cfg.ps bsps/basictest15_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest15_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest15_transitiongraph2.ps
	cp viz/ast.jpg bsps/basictest15_ast.jpg
	cp viz/cfg.jpg bsps/basictest15_cfg.jpg
	cp viz/transitiongraph1.jpg bsps/basictest15_transitiongraph1.jpg
	cp viz/transitiongraph2.jpg bsps/basictest15_transitiongraph2.jpg

	./codethorn tests/rers/Problem1.c
	cp tests/rers/Problem1.c bsps
	dot -Tjpg transitiongraph2.dot -oviz/transitiongraph2.jpg
	cp viz/transitiongraph2.jpg bsps/rers1_transitiongraph2.jpg

dist:
	tar cvzf codethorn_$(MYDATE).tgz *.C *cpp *.h *.txt *.lxx *.yxx docs tests Makefile* TODO
