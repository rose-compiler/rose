#include "satire.h"

namespace SATIrE
{

Program::Program(AnalyzerOptions *o)
  : options(o), astRoot(createRoseAst(o)), icfg(NULL), prologTerm(NULL)
{
    analysisScheduler.registerProgram(this);
}

Program::~Program()
{
    analysisScheduler.unregisterProgram(this);
}

}
