
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"

#include "sage3basic.h"

#include <cassert>

/**
 * \note The test files represent sequences of call to either ModelBuilder::addOne and ModelBuilder::addPair.
 *       Each line starts with the method to call followed by the arguments.
 */

int main(int argc, char ** argv) {
  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
  project->set_originalCommandLineArgumentList (arglist);

  MFB::Driver<MFB::Sage> driver(project);

  MDCG::ModelBuilder model_builder(driver);

  std::ifstream data;
  data.open(argv[1]);
  assert(data.is_open());

  unsigned model = model_builder.create();

  char buffer[2048];
  while (!data.eof()) {
    data.getline(buffer, 2048);
    std::string line(buffer);
    if (line.find("one") == 0) {
      size_t pos = 4;

      size_t end_filename = line.find_first_of(' ', pos);
      assert(end_filename != std::string::npos);
      std::string filename = line.substr(pos, end_filename - pos);
      pos = end_filename + 1;

      size_t end_path = line.find_first_of(' ', pos);
      assert(end_path != std::string::npos);
      std::string path = line.substr(pos, end_path - pos);
      pos = end_path + 1;

      std::string extension = line.substr(pos);

      model_builder.addOne(model, filename, path, extension);
    }
    else if (line.find("pair") == 0) {
      assert(false); /// \todo
    }
  }

  model_builder.print(std::cout, model);

  return 0;
}

