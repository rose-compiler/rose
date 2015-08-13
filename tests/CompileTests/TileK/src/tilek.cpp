
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"

#if defined(TILEK_BASIC)
#  include "KLT/TileK/api-basic.hpp"
#elif defined(TILEK_THREADS)
#  include "KLT/TileK/api-threads.hpp"
#elif defined(TILEK_ACCELERATOR) && defined(TILEK_TARGET_OPENCL)
#  include "KLT/TileK/api-opencl.hpp"
#elif defined(TILEK_ACCELERATOR) && defined(TILEK_TARGET_CUDA)
#  include "KLT/TileK/api-cuda.hpp"
#elif defined(TILEK_ACCELERATOR)
#  error "Asked for TileK::Accelerator but target language not defined (OpenCL or CUDA)."
#else
#  error "Need to choose between TileK::Basic, TileK::Threads, and TileK::Accelerator."
#endif

#include "KLT/DLX/compiler.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/variable-declaration.hpp"
#include "MFB/Sage/api.hpp"

#ifndef ROSE_INCLUDE_DIR
#  error "Need to provide ROSE's include directory."
#endif

namespace KLT {

namespace TileK {

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
#elif defined(TILEK_ACCELERATOR)
    typedef accelerator_host_t host_t;
#  if defined(TILEK_TARGET_OPENCL)
    typedef opencl_kernel_t kernel_t;
#  elif defined(TILEK_TARGET_CUDA)
    typedef cuda_kernel_t kernel_t;
#  else
#    error
#  endif
    typedef accelerator_call_interface_t call_interface_t;
#else
#  error
#endif

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
     ) {
#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
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
#  elif defined(TILEK_TARGET_CUDA)
      driver.build<SgVariableDeclaration>(
        MFB::Sage<SgVariableDeclaration>::object_desc_t(
          "cuda_kernel_file",
          SageBuilder::buildPointerType(SageBuilder::buildCharType()),
          SageBuilder::buildAssignInitializer(SageBuilder::buildStringVal(kernel_file_name)),
          NULL, static_file_id, false, true
        )
      );
#  else
#    error
#  endif
#endif
     }

  protected:
    Generator(MFB::Driver<MFB::KLT::KLT> & driver, ::MDCG::Tools::ModelBuilder & model_builder) :
      KLT::Generator(driver, model_builder)
    {}

    virtual void loadExtraModel(const std::string & usr_inc_dir) {
      model_builder.add(tilek_model, "tilek-rtl", usr_inc_dir + "/RTL/Host", "h");
#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
      model_builder.add(tilek_model, "preinclude-opencl", std::string(ROSE_INCLUDE_DIR) + "/opencl_HEADERS/", "h");
#  elif defined(TILEK_TARGET_CUDA)
      model_builder.add(tilek_model, "preinclude-cuda", std::string(ROSE_INCLUDE_DIR) + "/cuda_HEADERS/", "h");
#  else
#    error
#  endif
#endif
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
      if (num_threads != NULL) {
        SageInterface::appendStatement(host_api->buildNumThreadsAssign(kernel_sym, SageInterface::copyExpression(num_threads)), scope);
      }
#elif defined(TILEK_ACCELERATOR)
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
#endif
    }
};

std::string Generator::kernel_file_tag("kernel");
#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
std::string Generator::kernel_file_ext("cl");
#  elif defined(TILEK_TARGET_CUDA)
std::string Generator::kernel_file_ext("cu");
#  else
#    error
#  endif
#else
std::string Generator::kernel_file_ext("c");
#endif

std::string Generator::static_file_tag("static");
std::string Generator::static_file_ext("c");

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

  MFB::Sage<SgVariableDeclaration>::object_desc_t var_decl_desc(input.kernel_name, type->node->type, NULL, NULL, file_id, false, true);
  MFB::Sage<SgVariableDeclaration>::build_result_t var_decl_res = driver.build<SgVariableDeclaration>(var_decl_desc);

  SgDeclarationStatement * decl_stmt = isSgDeclarationStatement(var_decl_res.symbol->get_declaration()->get_parent());
    decl_stmt->get_declarationModifier().unsetDefault();
    decl_stmt->get_declarationModifier().get_storageModifier().setExtern();

  return SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(var_decl_res.symbol));
#elif defined(TILEK_ACCELERATOR)
  return SageBuilder::buildStringVal(input.kernel_name);
#endif
}

}

} // namespace KLT

int main(int argc, char ** argv) {
  std::vector<std::string> args(argv, argv + argc);

#if defined(TILEK_ACCELERATOR)
#  if defined(TILEK_TARGET_OPENCL)
  args.push_back("-DSKIP_OPENCL_SPECIFIC_DEFINITION");
#  endif
#endif

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

