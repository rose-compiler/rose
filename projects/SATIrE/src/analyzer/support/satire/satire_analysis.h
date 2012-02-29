#ifndef H_SATIRE_ANALYSIS
#define H_SATIRE_ANALYSIS

#include "satire.h"

namespace SATIrE {

class Analysis
{
public:
 // The analysis identifier: a single word following C identifier syntax.
    virtual std::string identifier() const = 0;
 // A brief human-readable description of the analysis.
    virtual std::string description() const = 0;

 // Run the analysis on the given AST (or the associated ICFG, depending on
 // the needs of the analysis).
    virtual void run(Program *program) = 0;

 // After running the analysis itself, perform program
 // annotation/visualization/transformation according to the command line
 // options associated with the program. This is not meant to include
 // unparsing to source code, which is handled by the analysis driver by
 // default.
    virtual void processResults(Program *program) = 0;

 // A very general interface for arbitrary queries against the analysis
 // data; this helps us circumvent the static type and compilation systems
 // of C++. The string argument identifies the query, the ... can be filled
 // with appropriate in or out arguments. This method is intended to return
 // true if the concrete analyzer instance supports this query. The default
 // implementation simply returns false.
    virtual bool query(std::string query, ...) const;

 // Dependencies between analyses: If analysis B depends on analysis A, then
 // calling B will run A first. The user need not run analysis A manually.
 // These functions provide an interface to manipulate the dependencies of
 // the analysis.
    void dependsOnAnalysis(Analysis *analysis);
    std::vector<Analysis *> &dependencies() const;
    void clearDependencies();

    Analysis();
    virtual ~Analysis();

private:
 // Copying of Analysis objects is prohibited.
    Analysis(const Analysis &);
    Analysis &operator=(const Analysis &);
};

}

#endif
