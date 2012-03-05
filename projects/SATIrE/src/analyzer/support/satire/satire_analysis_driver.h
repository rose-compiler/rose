#ifndef H_SATIRE_ANALYSIS_DRIVER
#define H_SATIRE_ANALYSIS_DRIVER

namespace SATIrE
{

class AnalysisDriver
{
public:
    AnalysisDriver(int argc, char **argv);
    AnalyzerOptions *getOptions() const;

 // This method provides a default implementation of the analysis driver,
 // but the driver can be customized by overwriting it in subclasses. By
 // default, the run method, after having built a program representation and
 // executed the analysis, calls the processResults and outputProgram
 // methods.
    virtual void run(Analysis *analysis);

    virtual ~AnalysisDriver();

protected:
 // These methods can be overwritten for further customization.
    virtual void processResults(Analysis *analysis, Program *program);
    virtual void outputProgram(Program *program);
    AnalyzerOptions *options;
    AnalysisDriver();

private:
 // No copying.
    AnalysisDriver(const AnalysisDriver &);
    AnalysisDriver &operator=(const AnalysisDriver &);
};

}

#endif
