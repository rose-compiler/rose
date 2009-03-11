#ifndef H_SATIRE_PROGRAM_REPRESENTATION
#define H_SATIRE_PROGRAM_REPRESENTATION

#include "satire.h"

namespace SATIrE
{

// This class provides a unified program representation abstraction. When
// constructed from a command line, it will build an AST from the input
// files. The corresponding ICFG representation is not built automatically;
// the intention is to have it built by the first analysis that needs it.
// These representations are owned by the Program instance.
class Program
{
public:
    AnalyzerOptions *options;
    SgProject *astRoot;
    CFG *icfg;
    PrologTerm *prologTerm;

    Program(AnalyzerOptions *o);
    ~Program();

private:
 // No copying.
    Program(const Program &);
    const Program & operator=(const Program &);
};

}

#endif
