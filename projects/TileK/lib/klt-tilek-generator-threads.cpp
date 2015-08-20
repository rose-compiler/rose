
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
#include "KLT/TileK/generator-threads.hpp"

namespace KLT {

namespace TileK {

void Generator::addUserStaticData(
   MFB::Driver<MFB::Sage> & driver,
   const std::string & klt_rtl_path, const std::string & user_rtl_path,
   const std::string & static_file_name, MFB::file_id_t static_file_id,
   const std::string & kernel_file_name, MFB::file_id_t kernel_file_id
) {}

Generator::Generator(MFB::Driver<MFB::KLT::KLT> & driver, ::MDCG::Tools::ModelBuilder & model_builder) :
  KLT::Generator(driver, model_builder)
{}

void Generator::loadExtraModel(const std::string & usr_inc_dir) {
  model_builder.add(tilek_model, "tilek-rtl", usr_inc_dir + "/RTL/Host", "h");
}

void Generator::insertUserConfig(::DLX::TileK::language_t::directive_t * directive, SgVariableSymbol * kernel_sym, KLT::API::host_t * host_api_, SgScopeStatement * scope)  {
  host_t * host_api = dynamic_cast<host_t *>(host_api_);
  assert(host_api != NULL);

  ::DLX::TileK::language_t::kernel_construct_t * kernel_construct = ::DLX::TileK::language_t::isKernelConstruct(directive->construct);
  assert(kernel_construct != NULL);

  SgExpression * num_threads = NULL;
  std::vector< ::DLX::TileK::language_t::clause_t *>::const_iterator it_clause;
  for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
    ::DLX::TileK::language_t::num_threads_clause_t * num_threads_clause = ::DLX::TileK::language_t::isNumThreadsClause(*it_clause);
    if (num_threads_clause != NULL) {
      assert(num_threads == NULL);
      num_threads = num_threads_clause->parameters.num_threads;
    }
  }
  if (num_threads != NULL) {
    SageInterface::appendStatement(host_api->buildNumThreadsAssign(kernel_sym, SageInterface::copyExpression(num_threads)), scope);
  }
}

std::string Generator::kernel_file_tag("kernel");
std::string Generator::kernel_file_ext("c");

std::string Generator::static_file_tag("static");
std::string Generator::static_file_ext("c");

} // namespace KLT::TileK

} // namespace KLT

