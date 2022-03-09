#include <rose.h>

#include <Rose/BinaryAnalysis/Partitioner2/JvmEngine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Utility.h>
#include <Sawyer/CommandLine.h>
#include <Sawyer/ProgressBar.h>

using namespace Rose;
using namespace Rose::Diagnostics;
using namespace Rose::BinaryAnalysis;

int
main(int argc, char *argv[]) {
  ROSE_INITIALIZE;

  std::string purpose{"Provide a list of methods found in a Java class file."};
  std::string description{"Scan a Java class file for methods."};

  BinaryAnalysis::Partitioner2::JvmEngine engine{};
  auto result = engine.parseCommandLine(argc, argv, purpose, description);

  mlog[INFO] <<"Finished parsing Java class file\n";

  return 0;
}
