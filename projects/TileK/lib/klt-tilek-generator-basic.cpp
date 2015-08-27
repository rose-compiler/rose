
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"
#include "KLT/TileK/generator-basic.hpp"

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

void Generator::insertUserConfig(::DLX::TileK::language_t::directive_t * directive, SgVariableSymbol * kernel_sym, KLT::API::host_t * host_api_, SgScopeStatement * scope) {}

std::string Generator::kernel_file_tag("kernel");
std::string Generator::kernel_file_ext("c");

std::string Generator::static_file_tag("static");
std::string Generator::static_file_ext("c");

} // namespace KLT::TileK

} // namespace KLT

