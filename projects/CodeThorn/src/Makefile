# MS: makefile for local development with installed version of ROSE.

# for building the reference implementation use:

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

#KNOBS = -DSTATE_MAINTAINER_LIST -DESTATE_MAINTAINER_LIST -DCSET_MAINTAINER_LIST
#KNOBS = -DSTATE_MAINTAINER_SET -DESTATE_MAINTAINER_SET -DCSET_MAINTAINER_SET
KNOBS = -DSTATE_MAINTAINER_HSET -DESTATE_MAINTAINER_HSET -DCSET_MAINTAINER_HSET

CPPFLAGS              = $(KNOBS) -O3 -march=native -ftree-vectorize -ggdb $(EXTRA_INCLUDES)
#CXXCPPFLAGS           = @CXXCPPFLAGS@
CXXFLAGS              = -Wno-deprecated
LDFLAGS               = 
YACC                  = bison

OBJECT_FILES=MyAst.o AstTerm.o Labeler.o SgNodeHelper.o VariableIdMapping.o CFAnalyzer.o AType.o ExprAnalyzer.o ConstraintRepresentation.o StateRepresentation.o Analyzer.o Visualizer.o Timer.o LanguageRestrictor.o LTLParser.o LTLChecker.o CommandLineOptions.o InternalChecks.o Miscellaneous.o

REGRESSION_DATA_DIR=regressiondata

OBJECT_FILES_EXPERIMENTAL=BoostGraphIntegration.o

# Default make rule to use
all: $(TOOLNAME3)
	@if [ x$${ROSE_IN_BUILD_TREE:+present} = xpresent ]; then echo "ROSE_IN_BUILD_TREE should not be set" >&2; exit 1; fi


.PHONY: check dist viz clean distclean bsps docs

# Example suffix rule for more experienced makefile users
# .C.o:
#	g++ -c -I$(ROSE_INCLUDE_DIR) -o $@ $(@:.o=.C)

ltlparser: LTLParser.y++ LTL.h
	$(BISON) -p ltl_ $<
	$(CXX) -DLTL_PARSER_DEBUG -o $@ $(CXXFLAGS) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) LTLParser.tab.c++

LTLParser.o: LTLParser.y++ LTL.h
	$(BISON) -p ltl_ $<
	$(CXX) -o $@ $(CXXFLAGS) $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) LTLParser.tab.c++ -c

Analyzer.o : CollectionOperators.h Analyzer.h AstTerm.h Labeler.h CFAnalyzer.h MyAst.h SgNodeHelper.h ExprAnalyzer.h StateRepresentation.h Analyzer.C

codethorn.o: codethorn.C codethorn.h LTL.h
LTLChecker.o: LTLChecker.C LTLChecker.h LTL.h

%.o: %.C %.h
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $< -c 

$(TOOLNAME3): $(TOOLNAME3).C  $(TOOLNAME3).h $(OBJECT_FILES) $(TOOLNAME3).o
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $(ROSE_LIBS) $(OBJECT_FILES) $(TOOLNAME3).o -o$(TOOLNAME3)

$(TOOLNAME3)ref: $(TOOLNAME3).C  $(TOOLNAME3).h $(OBJECT_FILES) $(TOOLNAME3).o
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $(ROSE_LIBS) $(OBJECT_FILES) $(TOOLNAME3).o -o$(TOOLNAME3)ref

xxtest: xxtest.C $(OBJECT_FILES)
	$(CXX) $(CXXFLAGS)  $(CPPFLAGS) -I$(ROSE_INCLUDE_DIR) $(BOOST_CPPFLAGS) $(ROSE_LIBS) $(OBJECT_FILES) xxtest.C -oxxtest

check: $(TOOLNAME3)
	./$(TOOLNAME3) --internal-checks
	@echo ================================================================
	@echo CHECK: all of the following formulae should evaluate to "YES"!
	@echo ================================================================
	./$(TOOLNAME3) --edg:no_warnings tests/rers_mini1.c    --verify tests/rers_mini1.ltl

test: $(TOOLNAME3)
	./$(TOOLNAME3) --edg:no_warnings tests/rers/Problem1.c \
	               --verify tests/rers/properties1.txt \
	               --csv-assert Problem1-assert.csv \
	               --csv-ltl Problem1-ltl.csv


RERS=$(patsubst %,Problem%.log, $(shell seq 13))
rers: $(RERS)

# parallel tests with make -j<N> test
%.log: tests/rers/%.c $(TOOLNAME3) Makefile
	./$(TOOLNAME3) --edg:no_warnings --colors=no $< \
	     --verify $(patsubst tests/rers/Problem%.c,tests/rers/properties%.txt,$<) \
	     --csv-ltl $(patsubst tests/rers/Problem%.c,Problem%-ltl.csv,$<) \
	     --csv-assert $(patsubst tests/rers/Problem%.c,Problem%-assert.csv,$<) >$@


validate:
	cd regressiondata && python validate.py --log Problem1.log --csv rers_Problem1_ltl_csv.txt

docs:
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
	rm -f LTLParser.tab.c++
	rm -f tmp_rers*

distclean: clean
	rm -f *.tgz
	rm -f *~
	rm -rf ../docs/doxygen
	rm -f $(TOOLNAME3)ref

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
	ps2pdf bsps/basictest10f_transitiongraph2.ps bsps/basictest10f_transitiongraph2.pdf
	./codethorn tests/basictest15.C && make viz && make vizjpg
	cp tests/basictest15.C bsps
	cp viz/cfg.ps bsps/basictest15_cfg.ps
	cp viz/transitiongraph1.ps bsps/basictest15_transitiongraph1.ps
	cp viz/transitiongraph2.ps bsps/basictest15_transitiongraph2.ps
	cp viz/ast.jpg bsps/basictest15_ast.jpg
	cp viz/cfg.jpg bsps/basictest15_cfg.jpg
	cp viz/transitiongraph1.jpg bsps/basictest15_transitiongraph1.jpg
	cp viz/transitiongraph2.jpg bsps/basictest15_transitiongraph2.jpg
	ps2pdf bsps/basictest15_transitiongraph2.ps bsps/basictest15_transitiongraph2.pdf

	./codethorn tests/rers/Problem1.c
	cp tests/rers/Problem1.c bsps
	dot -Tjpg transitiongraph2.dot -oviz/transitiongraph2.jpg
	cp viz/transitiongraph2.jpg bsps/rers1_transitiongraph2.jpg

dist:
	tar cvzf codethorn_$(MYDATE).tgz *.C *cpp *.h *.lxx *.yxx tests/*.C tests/*.c Makefile*


asserttest: $(TOOLNAME3)
	./$(TOOLNAME3) --edg:no_warnings tests/asserttest2.c --csv-assert assert_csv.txt

regressiontest: $(TOOLNAME3)
	./$(TOOLNAME3) --csv-assert tmp_rers_mini1_assert_csv.txt --csv-ltl tmp_rers_mini1_ltl_csv.txt --verify tests/rers_mini1.ltl --viz=yes tests/rers_mini1.c
	diff tmp_rers_mini1_assert_csv.txt $(REGRESSION_DATA_DIR)/rers_mini1_assert_csv.txt
	@echo "-----------------------"
	@echo "Regression assert test PASSED."
	@echo "-----------------------"
	diff tmp_rers_mini1_ltl_csv.txt $(REGRESSION_DATA_DIR)/rers_mini1_ltl_csv.txt
	@echo "-----------------------"
	@echo "Regression LTL test PASSED."
	@echo "-----------------------"
	diff transitiongraph1.dat $(REGRESSION_DATA_DIR)/rers_mini1_transitiongraph1.dat
	@echo "-----------------------"
	@echo "All regression tests PASSED."
	@echo "-----------------------"

# use with caution
generateregressiondata:
	./$(TOOLNAME3) --csv-assert $(REGRESSION_DATA_DIR)/rers_mini1_assert_csv.txt --csv-ltl $(REGRESSION_DATA_DIR)/rers_mini1_ltl_csv.txt --verify tests/rers_mini1.ltl --viz=yes tests/rers_mini1.c 
	cp transitiongraph1.dat $(REGRESSION_DATA_DIR)/rers_mini1_transitiongraph1.dat
