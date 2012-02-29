#ifndef SHAPEANALYSISDRIVER_H 
#define SHAPEANALYSISDRIVER_H 

#include "satire.h"
#include "termite.h"

#include "ShapeAnalyzerOptions.h"
#include "ShapeCommandLineParser.h"

namespace SATIrE {
    class ShapeAnalysisDriver : public AnalysisDriver {
    public:
        // use ShapeAnalyzerOptions instead of plain AnalyzerOptions
        // to cover shapeanalysis-specific commandline args
        ShapeAnalysisDriver(int argc, char **argv) : AnalysisDriver() {
          options = new ShapeAnalyzerOptions();
          ShapeCommandLineParser clp;
          clp.parse(options, argc, argv);

          init_termite(argc, argv);
        }
    };
}

#endif
