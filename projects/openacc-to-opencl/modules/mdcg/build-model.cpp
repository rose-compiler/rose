
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"

#include "sage3basic.h"

#include <cassert>

/**
 * \note The test files represent sequences of call to either ModelBuilder::addOne and ModelBuilder::addPair.
 *       Each line starts with the method to call followed by the arguments.
 */

int main(int argc, char ** argv) {

  assert(argc >= 3);

  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
    for (unsigned i = 3; i < argc; i++)
      arglist.push_back(argv[i]);
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
      size_t pos = 5;

      size_t end_filename = line.find_first_of(' ', pos);
      assert(end_filename != std::string::npos);
      std::string filename = line.substr(pos, end_filename - pos);
      pos = end_filename + 1;

      size_t end_path_1 = line.find_first_of(' ', pos);
      assert(end_path_1 != std::string::npos);
      std::string path_1 = line.substr(pos, end_path_1 - pos);
      pos = end_path_1 + 1;

      size_t end_path_2 = line.find_first_of(' ', pos);
      assert(end_path_2 != std::string::npos);
      std::string path_2 = line.substr(pos, end_path_2 - pos);
      pos = end_path_2 + 1;

      size_t end_extension_1 = line.find_first_of(' ', pos);
      assert(end_extension_1 != std::string::npos);
      std::string extension_1 = line.substr(pos, end_extension_1 - pos);
      pos = end_extension_1 + 1;

      std::string extension_2 = line.substr(pos);

      model_builder.addPair(model, filename, path_1, path_2, extension_1, extension_2);
    }
  }

  std::ofstream ofs;

  ofs.open((std::string("var_") + argv[2]).c_str());
  assert(ofs.is_open());
  model_builder.get(model).toDot(ofs, "acc_", true, false, false);
  ofs.close();

  ofs.open((std::string("func_") + argv[2]).c_str());
  assert(ofs.is_open());
  model_builder.get(model).toDot(ofs, "acc_", false, true, false);
  ofs.close();

  ofs.open((std::string("class_") + argv[2]).c_str());
  assert(ofs.is_open());
  model_builder.get(model).toDot(ofs, "acc_", false, false, true);
  ofs.close();

  return 0;
}

