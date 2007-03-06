#include "satire.h"

namespace SATIrE
{

class AnalysisScheduler::Implementation
{
public:
 // The sets of programs and analyses registered at any given moment. Not
 // strictly necessary, but they don't hurt.
    std::set<const Program *> programs;
    std::set<const Analysis *> analyses;

 // These are the currently stored dependencies for each analysis.
    std::map<const Analysis *, std::vector<Analysis *> > analysisDependencies;
 // The history of analysis runs and program representations so far. When a
 // supporting analysis is invoked on a certain program, an appropriate pair
 // is added here; if the pair already exists, the analysis is not run
 // again on this program.
    std::set<std::pair<const Analysis *, const Program *> > runHistory;
};

void
AnalysisScheduler::registerProgram(const Program *program)
{
    p_impl->programs.insert(program);
}

void
AnalysisScheduler::unregisterProgram(const Program *program)
{
    p_impl->programs.erase(program);
 // Erase history for this program; do this by copying all other elements to
 // another container and then swapping the two.
    std::set<std::pair<const Analysis *, const Program *> > histCopy;
    std::set<std::pair<const Analysis *, const Program *> >::iterator p;
    for (p = p_impl->runHistory.begin(); p != p_impl->runHistory.end(); ++p)
    {
        if (p->second != program)
            histCopy.insert(*p);
    }
    std::swap(p_impl->runHistory, histCopy);
}

void
AnalysisScheduler::registerAnalysis(const Analysis *analysis)
{
    p_impl->analyses.insert(analysis);
}

void
AnalysisScheduler::unregisterAnalysis(const Analysis *analysis)
{
    p_impl->analyses.erase(analysis);
 // Erase history for this analysis; do this by copying all other elements
 // to another container and then swapping the two.
    std::set<std::pair<const Analysis *, const Program *> > histCopy;
    std::set<std::pair<const Analysis *, const Program *> >::iterator p;
    for (p = p_impl->runHistory.begin(); p != p_impl->runHistory.end(); ++p)
    {
        if (p->first != analysis)
            histCopy.insert(*p);
    }
    std::swap(p_impl->runHistory, histCopy);
 // Erase dependencies for this analysis.
    p_impl->analysisDependencies.erase(analysis);
}

void
AnalysisScheduler::addDependency(const Analysis *client, Analysis *dependency)
{
    p_impl->analysisDependencies[client].push_back(dependency);
}

std::vector<Analysis *> &
AnalysisScheduler::dependencies(const Analysis *analysis) const
{
    return p_impl->analysisDependencies[analysis];
}

void
AnalysisScheduler::clearDependencies(const Analysis *analysis)
{
    p_impl->analysisDependencies[analysis].clear();
}

void
AnalysisScheduler::runAnalysisWithDependencies(Analysis *analysis,
                                               Program *program)
{
 // First, run the dependencies recursively.
    std::vector<Analysis *> &deps = dependencies(analysis);
    std::vector<Analysis *>::iterator d;
    for (d = deps.begin(); d != deps.end(); ++d)
    {
        TimingPerformance timer("Run of dependency "
                              + (*d)->identifier() + ":");
        runAnalysisWithDependencies(*d, program);
    }

 // Finally, run the argument analysis itself, if necessary.
    std::pair<Analysis *, Program *> entry = std::make_pair(analysis, program);
    if (p_impl->runHistory.find(entry) == p_impl->runHistory.end())
    {
        TimingPerformance timer("Run of analysis "
                              + analysis->identifier() + ":");
        analysis->run(program);
    }
}

void
AnalysisScheduler::clearHistory()
{
    p_impl->runHistory.clear();
}

AnalysisScheduler::AnalysisScheduler()
  : p_impl(new Implementation())
{
}

AnalysisScheduler::~AnalysisScheduler()
{
}

// This is the definition of the global scheduler.
AnalysisScheduler analysisScheduler;

}
