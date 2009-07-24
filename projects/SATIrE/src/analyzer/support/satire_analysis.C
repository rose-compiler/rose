#include "satire.h"

namespace SATIrE
{

bool
Analysis::query(std::string, ...) const
{
    return false;
}

void
Analysis::dependsOnAnalysis(Analysis *analysis)
{
    analysisScheduler.addDependency(this, analysis);
}

std::vector<Analysis *> &
Analysis::dependencies() const
{
    return analysisScheduler.dependencies(this);
}

void
Analysis::clearDependencies()
{
    analysisScheduler.clearDependencies(this);
}

Analysis::Analysis()
{
    analysisScheduler.registerAnalysis(this);
}

Analysis::~Analysis()
{
    analysisScheduler.unregisterAnalysis(this);
}

}
