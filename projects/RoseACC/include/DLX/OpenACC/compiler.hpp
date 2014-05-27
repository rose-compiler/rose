
#ifndef __DLX_COMPILER_OPENACC_HPP__
#define __DLX_COMPILER_OPENACC_HPP__

#include "openacc_spec.hpp"

#include "MDCG/OpenACC/model.hpp"

namespace DLX {

namespace OpenACC {

struct compiler_modules_t {
  MFB::KLT_Driver driver;

  MDCG::ModelBuilder model_builder;
  MDCG::CodeGenerator codegen;
  KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator;

  KLT::CG_Config<Annotation, Language, Runtime> cg_config;

  unsigned libopenacc_model;
  unsigned host_data_file_id;

  MDCG::Model::class_t region_desc_class;

  MDCG::OpenACC::CompilerData::input_t comp_data;

  compiler_modules_t(
    SgProject * project,
    const std::string & ocl_kernels_file,
    const std::string & kernels_desc_file,
    const std::string & libopenacc_inc_dir
  );
};

}

}

#endif /* __DLX_COMPILER_OPENACC_HPP__ */

