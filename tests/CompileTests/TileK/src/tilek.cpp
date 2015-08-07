
// ROSE

#include "sage3basic.h"

// MFB

#include "MFB/Sage/variable-declaration.hpp"
#include "MFB/Sage/api.hpp"

// KLT

#include "KLT/DLX/compiler.hpp"
#include "KLT/api.hpp"

// TileK

#include "DLX/TileK/language.hpp"

namespace KLT {

namespace TileK {

struct threads_host_t : public ::KLT::API::host_t {
  protected:
    SgVariableSymbol * num_threads_field;

  public:
    virtual void loadUser(const ::MDCG::Model::model_t & model) {
      bool res = true;

      ::MDCG::Model::class_t class_;
      ::MDCG::Model::field_t field_;
      SgClassSymbol * kernel_config_class;

      res = api_t::load(class_   , kernel_config_class , model, "klt_user_config_t" , NULL);   assert(res == true);
        res = api_t::load(field_ , num_threads_field   , model,   "num_threads"     , class_); assert(res == true);  
    }

    SgStatement * buildNumThreadsAssign(SgVariableSymbol * kernel_sym, SgExpression * rhs) const {
      return SageBuilder::buildAssignStatement(
               SageBuilder::buildArrowExp(
                 SageBuilder::buildArrowExp(
                   SageBuilder::buildVarRefExp(kernel_sym),
                   SageBuilder::buildVarRefExp(kernel_config_field)
                 ),
                 SageBuilder::buildVarRefExp(num_threads_field)
               ), rhs
             );
    }
};

struct basic_call_interface_t : public ::KLT::API::call_interface_t {
  public:
    basic_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api) : ::KLT::API::call_interface_t(driver, kernel_api) {}

  protected:
    // adds one arguments for each parameter
    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const {
      param_list->append_arg(SageBuilder::buildInitializedName("param", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
    }

    // adds one arguments for each data
    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<Descriptor::data_t *> & data) const {
      param_list->append_arg(SageBuilder::buildInitializedName("data", SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildVoidType())), NULL));
    }

    // get parameters argument's symbol
    virtual void getSymbolForParameter(SgFunctionDefinition * kernel_defn, const std::vector<SgVariableSymbol *> & parameters, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
      SgVariableSymbol * arg_param_sym = kernel_defn->lookup_variable_symbol("param");
      assert(arg_param_sym != NULL);
      int cnt = 0;

      std::vector<SgVariableSymbol *>::const_iterator it;
      for (it = parameters.begin(); it != parameters.end(); it++) {
        SgVariableSymbol * param_sym = *it;
        std::string param_name = param_sym->get_name().getString();
        SgType * param_type = param_sym->get_type();

        driver.useType(param_type, kernel_defn);

        SgExpression * init = SageBuilder::buildPointerDerefExp(SageBuilder::buildCastExp(
                                  SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_param_sym), SageBuilder::buildIntVal(cnt++)),
                                  SageBuilder::buildPointerType(param_type)
                              ));
        SageInterface::prependStatement(SageBuilder::buildVariableDeclaration(param_name, param_type, SageBuilder::buildAssignInitializer(init), bb), bb);

        SgVariableSymbol * new_sym = bb->lookup_variable_symbol(param_name);
        assert(new_sym != NULL);

        symbol_map.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, new_sym));
      }
    }

    // get data argument's symbol
    virtual void getSymbolForData(SgFunctionDefinition * kernel_defn, const std::vector<Descriptor::data_t *> & data, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) const {
      SgVariableSymbol * arg_data_sym = kernel_defn->lookup_variable_symbol("data");
      assert(arg_data_sym != NULL);
      int cnt = 0;

      std::vector<Descriptor::data_t *>::const_iterator it;
      for (it = data.begin(); it != data.end(); it++) {
        SgVariableSymbol * data_sym = (*it)->symbol;
        std::string data_name = data_sym->get_name().getString();
        SgType * data_type = (*it)->base_type;

        driver.useType(data_type, kernel_defn);

        data_type = SageBuilder::buildPointerType(data_type);

        SgExpression * init = SageBuilder::buildCastExp(
                                SageBuilder::buildPntrArrRefExp(SageBuilder::buildVarRefExp(arg_data_sym), SageBuilder::buildIntVal(cnt++)), data_type
                              );
        SageInterface::prependStatement(SageBuilder::buildVariableDeclaration(data_name, data_type, SageBuilder::buildAssignInitializer(init), bb), bb);

        SgVariableSymbol * new_sym = bb->lookup_variable_symbol(data_name);
        assert(new_sym != NULL);

        symbol_map.data.insert(std::pair<SgVariableSymbol *, Descriptor::data_t *>(data_sym, *it));
        symbol_map.data_trans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(data_sym, new_sym));
        symbol_map.data_rtrans.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(new_sym, data_sym));
      }
    }
};

struct threads_call_interface_t : public basic_call_interface_t {
  protected:
    SgVariableSymbol * tid_symbol;

  public:
    threads_call_interface_t(::MFB::Driver< ::MFB::Sage> & driver, ::KLT::API::kernel_t * kernel_api) : basic_call_interface_t(driver, kernel_api), tid_symbol(NULL) {}

  protected:
    virtual void prependUserArguments(SgFunctionParameterList * param_list) const {
      param_list->append_arg(SageBuilder::buildInitializedName("tid", SageBuilder::buildIntType(), NULL));
    }

    virtual void getSymbolForUserArguments(SgFunctionDefinition * kernel_defn, Utils::symbol_map_t & symbol_map, SgBasicBlock * bb) {
      tid_symbol = kernel_defn->lookup_variable_symbol("tid");
      assert(tid_symbol != NULL);
    }

    virtual SgExpression * getTileIdx(const Descriptor::tile_t & tile) const {
      assert(tid_symbol != NULL);
      return SageBuilder::buildVarRefExp(tid_symbol);
    }
};

class Generator : public KLT::Generator {
  friend class KLT::Generator;

  public:
#if defined(TILEK_BASIC)
    typedef KLT::API::host_t   host_t;
    typedef KLT::API::kernel_t kernel_t;
    typedef basic_call_interface_t call_interface_t;
#elif defined(TILEK_THREADS)
    typedef threads_host_t host_t;
    typedef KLT::API::kernel_t kernel_t;
    typedef threads_call_interface_t call_interface_t;
#else
#  error
#endif

  protected:
    Generator(MFB::Driver<MFB::KLT::KLT> & driver, ::MDCG::Tools::ModelBuilder & model_builder) :
      KLT::Generator(driver, model_builder)
    {}

    virtual void loadExtraModel(const std::string & usr_inc_dir) {
      model_builder.add(tilek_model, "tilek-rtl", usr_inc_dir + "/RTL/Host", "h");
    }

  public:
    static void insertUserConfig(::DLX::TileK::language_t::directive_t * directive, SgVariableSymbol * kernel_sym, KLT::API::host_t * host_api_, SgScopeStatement * scope) {
      host_t * host_api = dynamic_cast<host_t *>(host_api_);
      assert(host_api != NULL);
#if defined(TILEK_THREADS)
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
      SageInterface::appendStatement(host_api->buildNumThreadsAssign(kernel_sym, SageInterface::copyExpression(num_threads)), scope);
#endif
    }
};

} // namespace KLT::TileK

namespace MDCG {

template <>
SgExpression * VersionSelector< ::DLX::TileK::language_t, ::KLT::TileK::Generator>::createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
) {
  assert(false); // TileK does not support version selection so 'klt_version_selector_t' is an empty structure => this should not be called
  return NULL;
}

template <>
SgExpression * SubkernelConfig< ::KLT::TileK::Generator>::createFieldInitializer(
    MFB::Driver<MFB::Sage> & driver,
    ::MDCG::Model::field_t element,
    size_t field_id,
    const input_t & input,
    size_t file_id
) {
  assert(field_id == 0); // TileK's 'klt_subkernel_config_t' has one field: function pointer or kernel name
#if defined(TILEK_BASIC) || defined(TILEK_THREADS)
  ::MDCG::Model::type_t type = element->node->type;

  assert(type->node->kind == ::MDCG::Model::node_t< ::MDCG::Model::e_model_type>::e_typedef_type);
//driver.useType(type->node->type, file_id);

  MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(input.kernel_name, type->node->type, NULL, NULL, file_id, false, true);
  MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = driver.build<SgVariableDeclaration>(var_decl_desc);

  SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(var_decl_res.symbol->get_declaration()->get_parent());
    decl_stmt->get_declarationModifier().unsetDefault();
    decl_stmt->get_declarationModifier().get_storageModifier().setExtern();

  return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(var_decl_res.symbol));
#elif defined(TILEK_ACCELERATOR)
  assert(false);
  return NULL;
#endif
}

}

} // namespace KLT

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  KLT::DLX::Compiler< ::DLX::TileK::language_t, ::KLT::TileK::Generator> compiler(project, KLT_PATH, TILEK_PATH, basename);

//  MFB::api_t * api = compiler.getDriver().getAPI();
//  dump_api(api);

  compiler.compile(project);

  project->unparse();

  return 0;
}

