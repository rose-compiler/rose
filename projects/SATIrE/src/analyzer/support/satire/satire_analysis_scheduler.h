#ifndef H_SATIRE_ANALYSIS_SCHEDULER
#define H_SATIRE_ANALYSIS_SCHEDULER

#include <memory>

namespace SATIrE
{

class AnalysisScheduler
{
public:
 // Register and unregister program representations with the scheduler.
 // These functions are called automagically by the Program constructor and
 // destructor, no user intervention is necessary.
    void registerProgram(const Program *program);
    void unregisterProgram(const Program *program);

 // Register and unregister analyses with the scheduler. These functions are
 // called automagically by the Analysis constructor and destructor, no user
 // intervention is necessary.
    void registerAnalysis(const Analysis *analysis);
    void unregisterAnalysis(const Analysis *analysis);

 // Functions for handling a client analysis' dependencies.
    void addDependency(const Analysis *client, Analysis *dependency);
    std::vector<Analysis *> &dependencies(const Analysis *analysis) const;
    void clearDependencies(const Analysis *analysis);

 // This function runs the dependencies of the given analysis on the given
 // program, as required; afterwards, it runs the analysis itself.
    void runAnalysisWithDependencies(Analysis *analysis, Program *program);

 // Not sure if this is useful, but hey, why not keep it. This clears the
 // scheduler's history, which means that any analysis that ran in the past
 // is forgotten and can be run again.
    void clearHistory();

    AnalysisScheduler();
    ~AnalysisScheduler();

private:
 // Implementation details are hidden.
    class Implementation;
    std::auto_ptr<Implementation> p_impl;

 // No copying.
    AnalysisScheduler(const AnalysisScheduler &);
    const AnalysisScheduler &operator=(const AnalysisScheduler &);
};

// Global instance, defined in satire_analysis_scheduler.C
extern AnalysisScheduler analysisScheduler;

}

#endif
