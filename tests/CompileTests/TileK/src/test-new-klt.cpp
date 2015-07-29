
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"

#include "MFB/KLT/driver.hpp"

#include "MDCG/Core/model-builder.hpp"

#include "KLT/runtime.hpp"
#include "KLT/looptree.hpp"
#include "KLT/kernel.hpp"
#include "KLT/descriptor.hpp"
#include "KLT/api.hpp"
#include "KLT/data.hpp"

typedef ::DLX::TileK::language_t language_t; // Directives Language
typedef language_t::directive_t directive_t;
typedef language_t::clause_t clause_t;
typedef language_t::kernel_construct_t kernel_construct_t;
typedef language_t::data_clause_t data_clause_t;

typedef ::KLT::Descriptor::data_t data_t;
typedef ::KLT::Descriptor::section_t section_t;
typedef ::KLT::Kernel::kernel_t kernel_t;
typedef ::KLT::LoopTree::node_t node_t;
typedef ::KLT::LoopTree::loop_t loop_t;
typedef SgVariableSymbol vsym_t;

typedef ::MFB::file_id_t file_id_t;

typedef ::MFB::Driver< ::MFB::KLT::KLT> Driver;
typedef Driver::kernel_desc_t kernel_desc_t;

typedef std::vector<clause_t *> clause_list_t;
typedef std::vector<vsym_t *> vsym_list_t;
typedef std::vector<data_t *> data_list_t;
typedef std::map<SgForStatement *, loop_t *> loop_map_t;

data_t * convertData(data_clause_t * data_clause, const DLX::Frontend::data_sections_t & data_section) {
  SgVariableSymbol * data_sym = data_section.first;

  SgType * base_type = data_sym->get_type();
  std::vector<DLX::Frontend::section_t>::const_iterator it_section;
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++) {
         if (isSgPointerType(base_type)) base_type = ((SgPointerType *)base_type)->get_base_type();
    else if (isSgArrayType  (base_type)) base_type = ((SgArrayType   *)base_type)->get_base_type();
    else assert(false);
    assert(base_type != NULL);
  }
  data_t * data = new data_t(data_sym, base_type);
  for (it_section = data_section.second.begin(); it_section != data_section.second.end(); it_section++)
    data->sections.push_back(new section_t(it_section->lower_bound, it_section->size));
  return data;
}

typedef ::KLT::API::host_t host_api_t;
typedef ::KLT::API::kernel_t kernel_api_t;

class Runtime : public ::KLT::Runtime {
  public:
    Runtime(
      MDCG::ModelBuilder & model_builder,
      const std::string & klt_rtl_inc_dir, const std::string & klt_rtl_lib_dir,
      const std::string & usr_rtl_inc_dir, const std::string & usr_rtl_lib_dir,
      kernel_api_t * kernel_api, host_api_t * host_api
    ) :
      ::KLT::Runtime(model_builder, klt_rtl_inc_dir, klt_rtl_lib_dir, usr_rtl_inc_dir, usr_rtl_lib_dir, kernel_api, host_api)
    {}

  protected:
    virtual void loadUserModel() { model_builder.add(tilek_model, "kernel", usr_rtl_inc_dir + "/RTL/Host", "h"); }
    virtual void useUserKernelSymbols(MFB::Driver<MFB::Sage> & driver, size_t file_id) const {}
    virtual void useUserHostSymbols(MFB::Driver<MFB::Sage> & driver, size_t file_id) const {}

  public:
    virtual void applyKernelModifiers(SgFunctionDeclaration * kernel_decl) const {}
    virtual void addRuntimeStaticData(MFB::Driver<MFB::Sage> & driver, const std::string & kernel_file_name, const std::string & static_file_name, size_t static_file_id) const {}
    virtual SgType * buildKernelReturnType(::KLT::Descriptor::kernel_t & kernel) const { return SageBuilder::buildVoidType(); }

    virtual void addKernelArgsForParameter(SgFunctionParameterList * param_list, const std::vector<SgVariableSymbol *> & parameters) const {
      std::vector<SgVariableSymbol *>::const_iterator it;
      for (it = parameters.begin(); it != parameters.end(); it++) {
        // TODO
      }
    }

    virtual void addKernelArgsForData(SgFunctionParameterList * param_list, const std::vector<data_desc_t *> & data) const {
      std::vector<data_desc_t *>::const_iterator it;
      for (it = data.begin(); it != data.end(); it++) {
        // TODO
      }
    }

    virtual void addKernelArgsForContext(SgFunctionParameterList * param_list) const {
      // TODO
    }

    virtual SgVariableSymbol * getSymbolForParameter(SgVariableSymbol * parameter, SgBasicBlock * bb) const {
      SgVariableSymbol * symbol = NULL;

      // TODO

      return symbol;
    }

    virtual SgVariableSymbol * getSymbolForData(data_desc_t * data, SgBasicBlock * bb) const {
      SgVariableSymbol * symbol = NULL;

      // TODO

      return symbol;
    }

    virtual SgVariableSymbol * createLoopIterator(const loop_desc_t & loop, SgBasicBlock * bb) const {
      SgVariableSymbol * symbol = NULL;

      // TODO

      return symbol;
    }

    virtual SgVariableSymbol * createTileIterator(const tile_desc_t & tile, SgBasicBlock * bb) const {
      SgVariableSymbol * symbol = NULL;

      // TODO

      return symbol;
    }
};

void compile(SgProject * project, const std::string & KLT_RTL, const std::string & USER_RTL, const std::string & kernel_file, const std::string & static_file) {
  ::DLX::Frontend::Frontend<language_t> frontend;
  ::MFB::Driver< ::MFB::KLT::KLT> driver(project);
  ::MDCG::ModelBuilder model_builder(driver);

  language_t::init();

  kernel_api_t * kernel_api = NULL;
  host_api_t   * host_api = NULL;

  std::string klt_rtl_inc_dir( KLT_RTL + "/include");
  std::string klt_rtl_lib_dir( KLT_RTL + "/lib");
  std::string usr_rtl_inc_dir(USER_RTL + "/include");
  std::string usr_rtl_lib_dir(USER_RTL + "/lib");

  Runtime * runtime = new Runtime(model_builder, klt_rtl_inc_dir, klt_rtl_lib_dir, usr_rtl_inc_dir, usr_rtl_lib_dir, kernel_api, host_api);
    runtime->loadModel();

  file_id_t kernel_file_id = driver.create(boost::filesystem::path(kernel_file));
      driver.setUnparsedFile(kernel_file_id);
      driver.setCompiledFile(kernel_file_id);
  runtime->useKernelSymbols(driver, kernel_file_id);

  file_id_t static_file_id = driver.create(boost::filesystem::path(static_file));
      driver.setUnparsedFile(static_file_id);
      driver.setCompiledFile(static_file_id);
  runtime->useHostSymbols(driver, static_file_id);

  if (!frontend.parseDirectives(project)) {
    std::cerr << "Error in FrontEnd !!!" << std::endl;
    exit(1);
  }

  std::vector<directive_t *>::const_iterator it_directive;
  for (it_directive = frontend.directives.begin(); it_directive != frontend.directives.end(); it_directive++) {
    directive_t * directive = *it_directive;

    kernel_construct_t * kernel_construct = language_t::isKernelConstruct(directive->construct);
    if (kernel_construct == NULL) continue;

    SgStatement * region_base = language_t::getKernelRegion(kernel_construct);

    data_list_t data;
    clause_list_t::const_iterator it_clause;
    for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
      data_clause_t * data_clause = language_t::isDataClause(*it_clause);
      if (data_clause != NULL) {
        const std::vector<DLX::Frontend::data_sections_t> & data_sections = language_t::getDataSections(data_clause);
        std::vector<DLX::Frontend::data_sections_t>::const_iterator it_data_sections;
        for (it_data_sections = data_sections.begin(); it_data_sections != data_sections.end(); it_data_sections++)
          data.push_back(convertData(data_clause, *it_data_sections));
      }
      else assert(false);
    }

    kernel_t kernel(region_base, data);

    node_t * root = kernel.root;

    // TODO Apply loop construct to transform 'root' using 'std::map<SgForStatement *, loop_t *> kernel_t::loop_map'

    kernel_desc_t kernel_desc(root, kernel.parameters, kernel.data, runtime, kernel_file_id);
    
    ::KLT::Descriptor::kernel_t kernel_out = driver.build<kernel_t>(kernel_desc);
  }


  // TODO
}

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

  SgProject * project = new SgProject(args);
  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();
  std::string basename = filename.substr(0, filename.find_last_of('.'));

  std::string kernel_filename(basename + "-kernel.c");
  std::string static_filename(basename + "-data.c");

  compile(project, std::string(KLT_RTL_INC_PATH), std::string(TILEK_RTL_INC_PATH), kernel_filename, static_filename);

  project->unparse();

  return 0;
}

