/*!
 * 
 * \file lib/openacc/compiler.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "DLX/Core/compiler.hpp"
#include "DLX/OpenACC/language.hpp"
#include "DLX/OpenACC/compiler.hpp"

#include "KLT/OpenACC/iteration-mapper.hpp"

namespace DLX {

namespace OpenACC {

compiler_modules_t::compiler_modules_t(
  SgProject * project,
  const std::string & ocl_kernels_file,
  const std::string & kernels_desc_file,
  const std::string & libopenacc_inc_dir
) :
  driver(project),
  model_builder(driver),
  codegen(driver),
  generator(driver, ocl_kernels_file),
  cg_config(
    new KLT::LoopMapper<Annotation, Language, Runtime>(),
    new KLT::OpenACC::IterationMapper(),
    new KLT::DataFlow<Annotation, Language, Runtime>()
  ),
  libopenacc_model(0),
  host_data_file_id(0),
  region_desc_class(NULL),
  comp_data()
{
  host_data_file_id = driver.add(boost::filesystem::path(kernels_desc_file));
    driver.setUnparsedFile(host_data_file_id);

  libopenacc_model = MDCG::OpenACC::readOpenaccModel(model_builder, libopenacc_inc_dir);
  std::set<MDCG::Model::class_t> classes;
  model_builder.get(libopenacc_model).lookup<MDCG::Model::class_t>("acc_compiler_data_t_", classes);
  assert(classes.size() == 1);
  region_desc_class = *(classes.begin());

  comp_data.runtime_dir = SageBuilder::buildVarRefExp("LIBOPENACC_DIR");
  comp_data.ocl_runtime = SageBuilder::buildStringVal("lib/opencl/libopenacc.cl");
  comp_data.kernels_dir = SageBuilder::buildVarRefExp("KERNEL_DIR");
}

}

namespace Compiler {

/*!
 * \addtogroup grp_dlx_openacc_compiler
 * @{
 */

template <>
bool Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::compile(
  const Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::directives_ptr_set_t & directives,
  const Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::directives_ptr_set_t & graph_entry,
  const Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::directives_ptr_set_t & graph_final
) {
  /// \todo verify that it is correct OpenACC.....

  /// \todo generate LoopTrees from parallel/kernel regions

  return false;
}

/** @} */
}

}

