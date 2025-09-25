
#include <sage3basic.h>
#include <Rose/Source/Analysis/Analyzer.h>
#include <Rose/Source/AST/IO.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace Rose {
namespace Source {
namespace Analysis {

static std::string const rose_analysis_namespace = "12a37ea2-14a5-4e64-aabd-120921af7e4f";

bool Analyzer::parse_args__(std::vector<std::string> & args) {
  analysis->commandLine(args);

  bool arg_version = false;
  bool arg_help = false;
  auto it = args.begin();
  while (it != args.end()) {
    if (it->find("--sarif=") == 0) {
      sarif_file = it->substr(8);
      it = args.erase(it);
    } else if (it->find("--version") == 0) {
      arg_version = true;
      it = args.erase(it);
    } else if (it->find("--help") == 0) {
      arg_help = true;
      it = args.erase(it);
    } else {
      it++;
    }
  }
  if (arg_version) {
    std::cout << "v" << version << std::endl;
    return false;
  } else if (arg_help) {
    // TODO
    return false;
  } else {
    parse_args(args);
    project = frontend(args);
    return true;
  }
}

Analyzer::~Analyzer() {
  if (project) {
    Rose::Source::AST::IO::free();
    if (sarif_file.size() == 0) {
      log->emit(std::cout);
      std::cout << std::endl;
    } else {
      std::ofstream sarif_stream;
      sarif_stream.open(sarif_file);
      log->emit(sarif_stream);
      sarif_stream << std::endl;
      sarif_stream.close();
    }
  }
}

Analyzer::Analyzer(
  std::string const & name_,
  std::string const & version_
) :
  project(nullptr),
  log(Sarif::Log::instance()),
  sarif_file(),
  analysis(Sarif::Analysis::instance(name_)),
  name(name_),
  version(version_),
  rules()
{
  analysis->version(version);
  log->analyses().push_back(analysis);
}

Sarif::RulePtr Analyzer::add_rule(
  std::string const & name,
  std::string const & desc
) {
  Sarif::RulePtr rule = Sarif::Rule::instance(name, desc);
  rules[name] = rule;

  boost::uuids::string_generator str_gen;
  boost::uuids::uuid rose_analysis_namespace_uuid = str_gen(rose_analysis_namespace);
  boost::uuids::name_generator rose_analysis_namespace_gen(rose_analysis_namespace_uuid);

  rule->uuid(rose_analysis_namespace_gen(this->name + ":" + name));

  analysis->rules().push_back(rule);
  return rule;
}

Sarif::ResultPtr Analyzer::add_result(Sarif::ResultPtr result, std::string const & rule_id, SgLocatedNode * loc) {
  analysis->results().push_back(result);

  auto rule_it = rules.find(rule_id);
  ROSE_ASSERT(rule_it != rules.end());
  result->rule(rule_it->second);

  result->locations().push_back(Sarif::locate(loc));

  return result;
}

int Analyzer::apply__() {
  int status = apply();
  analysis->exitStatus(status);
  return status;
}

Sarif::ResultPtr Analyzer::add_fail(std::string const & rule_id, std::string const & message, SgLocatedNode * loc, Sarif::Severity severity) {
  Sarif::ResultPtr result = Sarif::Result::instance(severity, message);
  return add_result(result, rule_id, loc);
}

Sarif::ResultPtr Analyzer::add_note(std::string const & rule_id, std::string const & message, SgLocatedNode * loc) {
  return add_fail(rule_id, message, loc, Sarif::Severity::NOTE);
}

Sarif::ResultPtr Analyzer::add_warning(std::string const & rule_id, std::string const & message, SgLocatedNode * loc) {
  return add_fail(rule_id, message, loc, Sarif::Severity::WARNING);
}

Sarif::ResultPtr Analyzer::add_error(std::string const & rule_id, std::string const & message, SgLocatedNode * loc) {
  return add_fail(rule_id, message, loc, Sarif::Severity::ERROR);
}

Sarif::ResultPtr Analyzer::add_pass(std::string const & rule_id, std::string const & message, SgLocatedNode * loc) {
  Sarif::ResultPtr result = Sarif::Result::instance(Sarif::Kind::PASS, message);
  return add_result(result, rule_id, loc);
}

} } }
