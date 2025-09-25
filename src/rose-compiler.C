
#include "sage3basic.h"

#include "Rose/Source/AST/IO.h"

#include "midend/astDiagnostics/AstConsistencyTests.h"

int main( int argc, char * argv[] ) {
  ROSE_INITIALIZE;
  std::vector<std::string> args(argv, argv+argc);

#if defined(ROSE_COMPILER_FOR_LANGUAGE)
  std::string language(ROSE_COMPILER_FOR_LANGUAGE);
  const bool  useXOption = language == "ada";
  std::string option     = useXOption ? "-x" : "-std=";

  bool has_dialect = false;
  for (std::vector<std::string>::const_iterator arg = args.begin(); arg != args.end(); ++arg) {
    if (arg->find(option) == 0) {
      has_dialect = true;
      break;
    }
  }
  if (!has_dialect) {
    if (!useXOption) {
      // insert as -std=language
      args.insert(args.begin()+1, option+language);
    } else {
      // insert as -x language
      std::vector<std::string> extra = {option, language};

      args.insert(args.begin()+1, extra.begin(), extra.end());
    }
  }
#endif

  SgProject * project = args.size() > 1 ? frontend(args) : new SgProject(); // TODO this behavior should be part of ::frontend(std::vector<std::string> const &)

  auto status = backend(project);

// DQ (7/14/2025): Adding performance reporting.
   TimingPerformance::generateReport();

#if !defined(_WIN32) && !defined(__CYGWIN__)
  Rose::Source::AST::IO::free();
#endif

  return status;
}

