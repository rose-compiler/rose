
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
#include "KLT/TileK/generator-opencl.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#ifndef ROSE_INCLUDE_DIR
#  error "Need to provide ROSE's include directory."
#endif

namespace KLT {

namespace TileK {

void Generator::addUserStaticData(
   MFB::Driver<MFB::Sage> & driver,
   const std::string & klt_rtl_path, const std::string & user_rtl_path,
   const std::string & static_file_name, MFB::file_id_t static_file_id,
   const std::string & kernel_file_name, MFB::file_id_t kernel_file_id
) {
  driver.build<SgVariableDeclaration>(
    MFB::Sage<SgVariableDeclaration>::object_desc_t(
      "opencl_kernel_file",
      SageBuilder::buildPointerType(SageBuilder::buildCharType()),
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(kernel_file_name)),
      NULL, static_file_id, false, true
    )
  );
  driver.build<SgVariableDeclaration>(
    MFB::Sage<SgVariableDeclaration>::object_desc_t(
      "opencl_kernel_options",
      SageBuilder::buildPointerType(SageBuilder::buildCharType()),
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal("-I" + klt_rtl_path + "/include -I" + user_rtl_path + "/include")),
      NULL, static_file_id, false, true
    )
  );
  driver.build<SgVariableDeclaration>(
    MFB::Sage<SgVariableDeclaration>::object_desc_t(
      "opencl_klt_runtime_lib",
      SageBuilder::buildPointerType(SageBuilder::buildCharType()),
      SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(klt_rtl_path + "/lib/rtl/context.c")),
      NULL, static_file_id, false, true
    )
  );
}

Generator::Generator(MFB::Driver<MFB::KLT::KLT> & driver, ::MDCG::Tools::ModelBuilder & model_builder) :
  KLT::Generator(driver, model_builder)
{}

void Generator::loadExtraModel(const std::string & usr_inc_dir) {
  model_builder.add(tilek_model, "tilek-rtl", usr_inc_dir + "/RTL/Host", "h");
  model_builder.add(tilek_model, "preinclude-opencl", std::string(ROSE_INCLUDE_DIR) + "/opencl_HEADERS/", "h");
}

void Generator::insertUserConfig(::DLX::TileK::language_t::directive_t * directive, SgVariableSymbol * kernel_sym, KLT::API::host_t * host_api_, SgScopeStatement * scope) {
  host_t * host_api = dynamic_cast<host_t *>(host_api_);
  assert(host_api != NULL);

  ::DLX::TileK::language_t::kernel_construct_t * kernel_construct = ::DLX::TileK::language_t::isKernelConstruct(directive->construct);
  assert(kernel_construct != NULL);

  SgExpression * num_gangs[3] = {NULL,NULL,NULL};
  SgExpression * num_workers[3] = {NULL,NULL,NULL};
  std::vector< ::DLX::TileK::language_t::clause_t *>::const_iterator it_clause;
  for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
    ::DLX::TileK::language_t::num_gangs_clause_t * num_gangs_clause = ::DLX::TileK::language_t::isNumGangsClause(*it_clause);
    if (num_gangs_clause != NULL) {
      size_t gang_id = ::DLX::TileK::language_t::getGangID(num_gangs_clause);
      assert(gang_id >= 0 && gang_id <= 2);
      assert(num_gangs[gang_id] == NULL);
      num_gangs[gang_id] = num_gangs_clause->parameters.num_gangs;
    }
    ::DLX::TileK::language_t::num_workers_clause_t * num_workers_clause = ::DLX::TileK::language_t::isNumWorkersClause(*it_clause);
    if (num_workers_clause != NULL) {
      size_t worker_id = ::DLX::TileK::language_t::getWorkerID(num_workers_clause);
      assert(worker_id >= 0 && worker_id <= 2);
      assert(num_workers[worker_id] == NULL);
      num_workers[worker_id] = num_workers_clause->parameters.num_workers;
    }
  }
  int i;
  for (i = 0; i < 3; i++) {
    if (num_gangs[i] != NULL)
      SageInterface::appendStatement(host_api->buildNumGangsAssign(kernel_sym, i, SageInterface::copyExpression(num_gangs[i])), scope);
    if (num_workers[i] != NULL)
      SageInterface::appendStatement(host_api->buildNumWorkersAssign(kernel_sym, i, SageInterface::copyExpression(num_workers[i])), scope);
  }
}

std::string Generator::kernel_file_tag("kernel");
std::string Generator::kernel_file_ext("cl");

std::string Generator::static_file_tag("static");
std::string Generator::static_file_ext("c");

} // namespace KLT::TileK

} // namespace KLT

