
#ifndef __KLT_TILEK_GENERATOR_BASIC_HPP__
#define __KLT_TILEK_GENERATOR_BASIC_HPP__

#include "KLT/Core/generator.hpp"

#include "KLT/TileK/api-threads.hpp"

#include <cstddef>

namespace KLT {

namespace TileK {

class Generator : public KLT::Generator {
  friend class KLT::Generator;

  public:
    typedef threads_host_t host_t;
    typedef KLT::API::kernel_t kernel_t;
    typedef threads_call_interface_t call_interface_t;

  public:
    static std::string kernel_file_tag;
    static std::string kernel_file_ext;
    static std::string static_file_tag;
    static std::string static_file_ext;

  public:
     static void addUserStaticData(
       MFB::Driver<MFB::Sage> & driver,
       const std::string & klt_rtl_path, const std::string & user_rtl_path,
       const std::string & static_file_name, MFB::file_id_t static_file_id,
       const std::string & kernel_file_name, MFB::file_id_t kernel_file_id
     );

  protected:
    Generator(MFB::Driver<MFB::KLT::KLT> & driver, ::MDCG::Tools::ModelBuilder & model_builder);

    virtual void loadExtraModel(const std::string & usr_inc_dir);

  public:
    static void insertUserConfig(::DLX::TileK::language_t::directive_t * directive, SgVariableSymbol * kernel_sym, KLT::API::host_t * host_api_, SgScopeStatement * scope);
};

} // namespace KLT::TileK

} // namespace KLT

#endif /* __KLT_TILEK_GENERATOR_BASIC_HPP__ */

