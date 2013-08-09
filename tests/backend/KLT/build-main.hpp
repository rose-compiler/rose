
#ifndef __BUILD_MAIN_KLT_TEST_HPP__
#define __BUILD_MAIN_KLT_TEST_HPP__

#include <list>
#include <string>

#include "KLT/Core/data.hpp"
#include "KLT/Sequential/kernel.hpp"

#include "MFB/Sage/function-declaration.hpp"

void createMain(
  MultiFileBuilder::Driver<MultiFileBuilder::Sage> & driver,
  std::string filename,
  const KLT::Core::LoopTrees & loop_trees,
  const std::list<KLT::Sequential::Kernel *> & kernel_list,
  const std::list<SgVariableSymbol *> parameter_order,
  const std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order
);

#endif /* __BUILD_MAIN_KLT_TEST_HPP__ */
