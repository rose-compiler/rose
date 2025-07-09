#ifndef ROSE_SourceCode_Analysis_Analyzer_H
#define ROSE_SourceCode_Analysis_Analyzer_H

#include <RoseFirst.h>
#include <Rose/SourceCode/Sarif.h>

#include <ostream>

class SgProject;
class SgLocatedNode;

namespace Rose {
namespace SourceCode {
namespace Analysis {

class Analyzer {
  public:
    template <typename Analyzer>
    static int run(std::vector<std::string> & args);

  private:
    SgProject * project;
    Sarif::LogPtr log;
    std::string sarif_file;

  protected:
    Sarif::AnalysisPtr analysis;

  private:
    std::string name;
    std::string version;
    std::map<std::string, Sarif::RulePtr> rules;

  protected:
    Analyzer(std::string const & name, std::string const & version);
    ~Analyzer();

    Sarif::RulePtr add_rule(std::string const & name, std::string const & desc);

    Sarif::ResultPtr add_note    (std::string const & rule_id, std::string const & message, SgLocatedNode * loc=nullptr);
    Sarif::ResultPtr add_warning (std::string const & rule_id, std::string const & message, SgLocatedNode * loc=nullptr);
    Sarif::ResultPtr add_error   (std::string const & rule_id, std::string const & message, SgLocatedNode * loc=nullptr);
    Sarif::ResultPtr add_pass    (std::string const & rule_id, std::string const & message, SgLocatedNode * loc=nullptr);

    virtual int apply() = 0;

    virtual void parse_args(std::vector<std::string> & args) = 0;

  private:
    Sarif::ResultPtr add_result(Sarif::ResultPtr result, std::string const & rule_id, SgLocatedNode * loc);
    Sarif::ResultPtr add_fail(std::string const & rule_id, std::string const & message, SgLocatedNode * loc, Sarif::Severity severity);

    bool parse_args__(std::vector<std::string> & args);
    int apply__();
};

template <typename AnalyzerT>
int Analyzer::run(std::vector<std::string> & args) {
  int status = 0;
  AnalyzerT analyzer;
  if (analyzer.parse_args__(args)) {
    status = analyzer.apply__();
  }
  return status;
}

}
}
}

#endif /* ROSE_SourceCode_Analysis_Analyzer_H */
