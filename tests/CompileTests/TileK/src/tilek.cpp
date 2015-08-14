
#include "sage3basic.h"

#include "DLX/TileK/language.hpp"

#if defined(TILEK_BASIC)
#  include "KLT/TileK/generator-basic.hpp"
#elif defined(TILEK_THREADS)
#  include "KLT/TileK/generator-threads.hpp"
#elif defined(TILEK_ACCELERATOR) && defined(TILEK_TARGET_OPENCL)
#  include "KLT/TileK/generator-opencl.hpp"
#elif defined(TILEK_ACCELERATOR) && defined(TILEK_TARGET_CUDA)
#  include "KLT/TileK/generator-cuda.hpp"
#elif defined(TILEK_ACCELERATOR)
#  error "Asked for TileK::Accelerator but target language not defined (OpenCL or CUDA)."
#else
#  error "Need to choose between TileK::Basic, TileK::Threads, and TileK::Accelerator."
#endif

#include "KLT/DLX/compiler.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/variable-declaration.hpp"

namespace KLT {

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

} // namespace KLT::MDCG

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

