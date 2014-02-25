
#include "MFB/Sage/driver.hpp"

#include "MDCG/model-builder.hpp"

#include "sage3basic.h"

#include <cassert>

int main(int argc, char ** argv) {
  std::ofstream ofs;
  assert(argc == 2);

  SgProject * project = new SgProject();
  std::vector<std::string> arglist;
    arglist.push_back("c++");
    arglist.push_back("-DSKIP_ROSE_BUILTIN_DECLARATIONS");
    arglist.push_back("-c");
  project->set_originalCommandLineArgumentList (arglist);

  MFB::Driver<MFB::Sage> driver(project);
  MDCG::ModelBuilder model_builder(driver);

  {
    unsigned model_01 = model_builder.create();

    model_builder.add(model_01, "test_01", argv[1], "h");

    ofs.open("model_01.dot");
    assert(ofs.is_open());

    model_builder.get(model_01).toDot(ofs);

    ofs.close();
  }

  {
    unsigned model_02 = model_builder.create();

    model_builder.add(model_02, "test_02", argv[1], "h");

    ofs.open("model_02.dot");
    assert(ofs.is_open());

    model_builder.get(model_02).toDot(ofs);

    ofs.close();
  }

  {
    unsigned model_03 = model_builder.create();

    model_builder.add(model_03, "test_02", argv[1], "c");

    ofs.open("model_03.dot");
    assert(ofs.is_open());

    model_builder.get(model_03).toDot(ofs);

    ofs.close();
  }

  {
    unsigned model_04 = model_builder.create();

    model_builder.add(model_04, "test_02", argv[1], "c");
    model_builder.add(model_04, "test_02", argv[1], "h");

    ofs.open("model_04.dot");
    assert(ofs.is_open());

    model_builder.get(model_04).toDot(ofs);

    ofs.close();
  }

  return 0;
}

