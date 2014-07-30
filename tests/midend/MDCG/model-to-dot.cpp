
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {
  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
    for (size_t i = 1; i < argc; i++)
      arglist.push_back(argv[i]);
  project->set_originalCommandLineArgumentList (arglist);

  MFB::Driver<MFB::Sage> driver(project);
  MDCG::ModelBuilder model_builder(driver);

  unsigned model = model_builder.create();

  bool cont = true;
  while (cont) {
    char cmd;
    std::cout << "> cmd : ";
    std::cin >> cmd;
    switch (cmd) {
      case 'h':
        std::cout << "" << std::endl;
        std::cout << " -> h : help" << std::endl;
        std::cout << " -> o : open a file" << std::endl;
        std::cout << " -> p : plot the model" << std::endl;
        std::cout << " -> q : quit" << std::endl;
        break;
      case 'o':
      {
        std::string path;
        std::cout << "  > path      : ";
        std::cin >> path;
        std::string filename;
        std::cout << "  > filename  : ";
        std::cin >> filename;
        std::string extension;
        std::cout << "  > extension : ";
        std::cin >> extension;
        model_builder.add(model, filename, path, extension);
        break;
      }
      case 'p':
      {
        std::string filename;
        std::cout << "  > filename  : ";
        std::cin >> filename;

        std::string prefix_filter;
        std::cout << "  > filter    : ";
        std::cin >> prefix_filter;

        bool variable_root;
        std::cout << "  > var root  : ";
        std::cin >> variable_root;

        bool function_root;
        std::cout << "  > func root : ";
        std::cin >> function_root;

        bool class_root;
        std::cout << "  > class root: ";
        std::cin >> class_root;

        std::ofstream ofs;
        ofs.open(filename.c_str());
        assert(ofs.is_open());
        model_builder.get(model).toDot(ofs, prefix_filter, variable_root, function_root, class_root);

        break;
      }
      case 'q':
        cont = false;
        break;
      default:
        assert(false);
    }
  }

  return 0;
}

