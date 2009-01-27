#ifndef CMDLINEOPTIONS_H
#define CMDLINEOPTIONS_H

#include "AnalyzerOptions.h"
#include <ostream>
#include <sstream>

class ShapeAnalyzerOptions : public AnalyzerOptions {
public:

    std::string getOptionsInfo() {
        return
            "Usage: "+getProgramName()+" [OPTION]... [FILE]...\n"
            "\n"
            "Perform Shape Analysis on the FILEs.\n"
            "Create a gdl visualisation of the input program.\n"
            "\n"
            +getHelpOptionsText();
    }

    virtual ~ShapeAnalyzerOptions() {}
    ShapeAnalyzerOptions() : AnalyzerOptions() {
        
        // set defaults
        statisticsOff();
        verboseOff();
        quietOn();
        outputTextOff();

        //checkRoseAstOff();
        //checkIcfgOff();
        //frontendWarningsOff();

        setImplementation(Shape_NNH99);

        outputGdlOn();
        setOutputGdlFileName(strdup("default.gdl"));


        std::string help_string=
            " Options:\n"
//            "   --impl=ANALYSIS          choose implementation of shape analysis\n"
//            "                            ANALYSIS = rw98|nnh99 [default is nnh99]\n"
//            "                            srw98 : from TOPLAS'98, \"Solving shape-analysis\n"
//            "                                    problems in languages with destructive\n"
//            "                                    updating\" (M. Sagiv, T. Reps, R. Wilhelm)\n"
//            "                            nnh99 : described in the book \"Principles of\n"
//            "                                    Program Analysis\"\n"
//            "                                    (F. Nielson, H.R. Nielson, C. Hankin)\n"
            "   --output-gdl=FILENAME    output gdl graph to FILENAME [default.gdl]\n"
            "   --show-as=graphset       analysis information as set of shapegraphs\n"
            "   --show-as=summarygraph   analysis information as summarygraph\n"
            "\n"
            "   --help                   display this help and exit\n"
//            "   --help-expert            display more options and exit\n"
            "   --version                output version information and exit\n"
            "\n"
            ;

#if 0
        std::string helpexpert_string =
            " Frond End options:\n"
            "   --language=c++|c99|c89   select input language [default=c++]\n"
            "   --frontend-warnings      show Front End warnings when parsing file(s)\n"
            "   --no-frontend-warnings   do not show Front End warnings when parsing\n"
            "                            file(s) [default]\n"
            "   -I<path>                 specify path for include files\n"
            "\n"
            " Analysis options:\n"
            "   --callstringlength=<num> set callstring length to <num> [default=0]\n"
            "   --callstringinfinite     select infinite callstring (for non-recursive\n"
            "                            programs only)\n"
            "   --cfgordering=<num>      set ordering that is used by the iteration\n"
            "                            algorithm where\n"
            "                                  <num> = 1 : dfs preorder [default]\n"
            "                                          2 : bfs preorder\n"
            "                                          3 : reversed dfs postorder\n"
            "                                          4 : bfs postorder\n"
            "                                          5 : topsort scc dfs preorder\n"
            "                                          6 : topsort scc bfs preorder\n"
            "                                          7 : topsort scc reversed bfs\n"
            "                                              dfs postorder\n"
            "                                          8 : topsort scc bfs postorder\n"
            "   --check-ast              run all ROSE tests for checking\n"
            "                            whether ROSE-AST is correct\n"
            "   --no-check-ast           do not run ROSE AST tests\n"
            "   --check-icfg             run PAG's ICFG consistency checks\n"
            "   --no-check-icfg          do not run ICFG checks\n"
            "   --analysis-files=all|cl  analyse all source files or only those\n"
            "                            specified on the command line (cl)\n"
//            "   --analysis-annotation    annotate analysis results in AST and output\n"
//            "   --no-analysis-annotation do not annotate analysis results in AST\n"
            "   --pag-memsize-mb=<num>   allocate <num> MB of memory for PAG analysis\n"
            "   --pag-memsize-perc=<num> allocate <num>% of system memory (autodetected)\n"
            "   --pag-memsize-grow=<num> grow memory if less than <num>% are free after GC\n"
            "\n"
            " Output options:\n"
            "   --statistics             output analyzer statistics on stdout\n"
            "   --no-statistics          do not show analyzer statistics on stdout\n"
            "   --output-collectedfuncs  print all functions that are collected for\n"
            "                            the icfg generation\n"
            "   --output-source=<FILENAME> generate source file with annotated \n"
            "                            analysis results for each statement\n"
            "\n"
            " Multiple input/output files options:\n"
            "   --output-sourceprefix=<PREFIX> generate for each input file one output file\n"
            "                            with prefixed name\n"
            "\n"
            " GDL output options:\n"
            "   --gdl-preinfo            output analysis info before cfg nodes\n"
            "   --no-gdl-preinfo         do not output analysis info before cfg nodes\n"
            "   --gdl-postinfo           output analysis info after cfg nodes\n"
            "   --no-gdl-postinfo        do not output analysis info after cfg nodes\n"
            "   --gdl-nodeformat=FORMAT  where FORMAT=varid|varname|exprid|exprsource\n"
            "                                        |asttext|no-varid|no-varname\n"
            "                                        |no-exprid|no-exprsource|no-asttext\n"
            "                            the format can be specified multiple times to\n"
            "                            have different formats printed at the same node\n"
            "                            The output is only affected if VariableId\n" 
            "                            and/or ExpressionId is used in the carrier type\n"
            "\n"
            ;
#endif 

        setHelpOptionsText(help_string);
        setExpertHelpOptionsText("<expert>");
    }

    std::string getExpertHelpOptionsText() { return _expertHelpOptionsText; }
    void setExpertHelpOptionsText(std::string text) { _expertHelpOptionsText = text; }

    std::string getVersionText() {
        return "SATIrE Shape Analysis Tool 0.2\n"
               "\n"
               "Built with satire-0.8.2 for rose-0.9.1a_DEV pag-8.7.6.17\n"
//               "For more Information, please visit http://www.satire-project.org/analyses/shape\n"
               "There is NO WARRANTY, to the extent permitted by law.\n"
               "\n"
               "Written by Viktor Pavlu and Markus Schordan\n";
    }

    //void gdlFoldGraphsOn()  { _gdlFoldGraphs = true;  }
    //void gdlFoldGraphsOff() { _gdlFoldGraphs = false; }
    //bool gdlFoldGraphs()    { return _gdlFoldGraphs;  }

    void showAsSummaryGraphOn()  { _showAsSummaryGraph = true;  }
    void showAsSummaryGraphOff() { _showAsSummaryGraph = false; }
    bool showAsSummaryGraph()    { return _showAsSummaryGraph;  }

    void showAsGraphSetOn()  { _showAsGraphSet = true;  }
    void showAsGraphSetOff() { _showAsGraphSet = false; }
    bool showAsGraphSet()    { return _showAsGraphSet;  }

    void helpExpertMessageRequestedOn()  { _helpExpertMessageRequested = true;  }
    void helpExpertMessageRequestedOff() { _helpExpertMessageRequested = false; }
    bool helpExpertMessageRequested()    { return _helpExpertMessageRequested;  }

    void versionMessageRequestedOn()  { _versionMessageRequested = true;  }
    void versionMessageRequestedOff() { _versionMessageRequested = false; }
    bool versionMessageRequested()    { return _versionMessageRequested;  }

    enum Implementation{Shape_NNH99, Shape_SRW98};
    void setImplementation(Implementation impl) {
        _implementation = impl;

        if (Shape_NNH99 == impl) {
            showAsSummaryGraphOff();
            showAsGraphSetOn();
        }
        if (Shape_SRW98 == impl) {
            showAsSummaryGraphOn();
            showAsGraphSetOff();
        }
    }
    Implementation getImplementation() { return _implementation; }

protected:
    //bool _gdlFoldGraphs;
    bool _showAsSummaryGraph;
    bool _showAsGraphSet;
    bool _helpExpertMessageRequested;
    bool _versionMessageRequested;
    Implementation _implementation;

    std::string _expertHelpOptionsText;
};

class ShapeCommandLineParser : public CommandLineParser {
public:
    int handleOption(AnalyzerOptions* cl, int i, int argc, char **argv) {

        ShapeAnalyzerOptions *scl = dynamic_cast<ShapeAnalyzerOptions*>(cl);

        if (i >= argc) 
            return 0;

        int old_i = i;

//        if (optionMatch(argv[i], "--impl=srw98")) {
//            scl->setImplementation(ShapeAnalyzerOptions::Shape_SRW98);
//        } else if (optionMatch(argv[i], "--impl=nnh99")) {
//            scl->setImplementation(ShapeAnalyzerOptions::Shape_NNH99);
//        } else 
        if (optionMatch(argv[i], "--show-as=graphset")) {
            scl->showAsGraphSetOn();
            scl->showAsSummaryGraphOff();
        } else if (optionMatch(argv[i], "--show-as=summarygraph")) {
            scl->showAsGraphSetOff();
            scl->showAsSummaryGraphOn();
//        } else if (optionMatch(argv[i], "--gdl-foldgraphs")) {
//            scl->gdlFoldGraphsOn();
//        } else if (optionMatch(argv[i], "--no-gdl-foldgraphs")) {
//            scl->gdlFoldGraphsOff();

/*        } else if (optionMatchPrefix(argv[i], "--callstringlength=")) {
            scl->setCallStringLength(atoi(argv[i]+prefixLength));
        } else if (optionMatch(argv[i], "--callstringinfinite")) {
            scl->setCallStringLength(-1);
        } else if (optionMatch(argv[i], "--gdl-preinfo")) {
            scl->preInfoOn();
        } else if (optionMatch(argv[i], "--no-gdl-preinfo")) {
            scl->preInfoOff();
        } else if (optionMatch(argv[i], "--gdl-postinfo")) {
            scl->postInfoOn();
        } else if (optionMatch(argv[i], "--no-gdl-postinfo")) {
            scl->postInfoOff();
        } else if (optionMatch(argv[i], "--analysis-files=all")) {
            scl->analysisWholeProgramOn();
        } else if (optionMatch(argv[i], "--analysis-files=cl")) {
            scl->analysisWholeProgramOff();
        } else if (optionMatch(argv[i], "--analysis-annotation")) {
            scl->analysisAnnotationOn();
        } else if (optionMatch(argv[i], "--no-analysis-annotation")) {
            scl->analysisAnnotationOff();
        } else if (optionMatchPrefix(argv[i], "--output-source=")) {
            scl->outputSourceOn();
            scl->setOutputSourceFileName(strdup(argv[i]+prefixLength));
        } else if (optionMatchPrefix(argv[i], "--output-sourceprefix=")) {
            scl->outputSourceOn();
            scl->setOutputFilePrefix(strdup(argv[i]+prefixLength));
*/

        } else if (optionMatchPrefix(argv[i], "--output-gdl=")) {
            scl->outputGdlOn();
            scl->setOutputGdlFileName(strdup(argv[i]+prefixLength));
        } else if (optionMatch(argv[i], "--help")) {
            scl->helpMessageRequestedOn();
//        } else if (optionMatch(argv[i], "--help-expert")) {
//            std::cout << scl->getExpertHelpOptionsText() << std::endl;
//            exit(EXIT_SUCCESS);
        } else if (optionMatch(argv[i], "--version")) {
            std::cout << scl->getVersionText() << std::endl;
            exit(EXIT_SUCCESS);

/*        } else if (optionMatch(argv[i], "--language=c++")) {
            scl->setLanguage(AnalyzerOptions::Language_CPP);
        } else if (optionMatch(argv[i], "--language=c89")) {
            scl->setLanguage(AnalyzerOptions::Language_C89);
        } else if (optionMatch(argv[i], "--language=c99")) {
            scl->setLanguage(AnalyzerOptions::Language_C99);
*/
        } else if ((!optionMatchPrefix(argv[i], "-") && !optionMatchPrefix(argv[i],"--")) ) {
            // handle as filename, pass filenames through
            std::cout << "Found input file '" << argv[i] << "'." << std::endl;
            // GB (2008-06-02): Using appendInputFile here; this method sets the input
            // file name, appends the name to the command line, and increments the file
            // name counter.
            scl->appendInputFile(argv[i]);
        } else {
            std::stringstream s;
            s << "unrecognized option: " << argv[i] << std::endl;
            scl->setOptionsErrorMessage(s.str());
            return 1;
        }
        return i+1-old_i;
    }
};

#endif
// vim: ts=4 sts=2 expandtab
