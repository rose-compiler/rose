
#include "sage3basic.h"

#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"
#include "DLX/KLT/loop-trees.hpp"

#include "DLX/TileK/language.hpp"

#include "KLT/Core/loop-trees.hpp"
#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-tiler.hpp"
#include "KLT/Core/loop-mapper.hpp"
#include "KLT/Core/data-flow.hpp"
#include "KLT/Core/cg-config.hpp"
#include "KLT/Core/generator.hpp"
#include "KLT/Core/kernel.hpp"
#include "KLT/Core/mfb-klt.hpp"

#include "KLT/TileK/tilek.hpp"

#include "MFB/Sage/driver.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

#include "MDCG/model-builder.hpp"
#include "MDCG/static-initializer.hpp"

#include "MDCG/TileK/model.hpp"

#include "rose.h"

#include <cassert>

typedef ::KLT::Language::None Language;
typedef ::KLT::Runtime::TileK Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::TileK::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

void replaceKernelDirectiveByHostRuntime(MFB::KLT_Driver & driver, SgStatement * kernel_region, Kernel * kernels) {
  // TileK only produces one version of each kernel
  assert(kernels->getKernels().size() == 1);
  Kernel::kernel_desc_t * kernel = kernels->getKernels()[0];
  assert(kernel != NULL);

  // Declare symbols used in host code (create include statement)

    assert(kernel_region->get_scope() != NULL);
    Runtime::useSymbolsHost(driver, driver.getFileID(kernel_region->get_scope()));

  // Replace kernel region by empty basic block

    SgBasicBlock * bb = SageBuilder::buildBasicBlock();
    SageInterface::replaceStatement(kernel_region, bb);

  // Insert: "struct kernel_t * kernel = build_kernel('kernel->id');"

    SgInitializer * init = SageBuilder::buildAssignInitializer(
      SageBuilder::buildFunctionCallExp(
        Runtime::host_api.build_kernel_func,
        SageBuilder::buildExprListExp(
          SageBuilder::buildIntVal(kernel->id)
        )
      )
    );
    SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration(
      "kernel", SageBuilder::buildPointerType(Runtime::host_api.kernel_class->get_type()), init, bb
    );
    SageInterface::appendStatement(kernel_decl, bb);

    SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
    assert(kernel_sym != NULL);

  // Arguments

    const Kernel::arguments_t & arguments = kernels->getArguments();

    // Set kernel's parameters

      std::list<SgVariableSymbol *>::const_iterator it_param;
      size_t param_cnt = 0;
      for (it_param = arguments.parameters.begin(); it_param != arguments.parameters.end(); it_param++) {
        SageInterface::appendStatement(SageBuilder::buildAssignStatement(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::host_api.kernel_param_field)
            ),
            SageBuilder::buildIntVal(param_cnt++)
          ),
          SageBuilder::buildVarRefExp(*it_param)
        ), bb);
      }

    // TODO scalars

    // Set kernel's data

      std::list<KLT::Data<Annotation> *>::const_iterator it_data;
      size_t data_cnt = 0;
      for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
        SgExpression * data_first_elem = SageBuilder::buildVarRefExp((*it_data)->getVariableSymbol());
        for (size_t i = 0; i < (*it_data)->getSections().size(); i++)
          data_first_elem = SageBuilder::buildPntrArrRefExp(data_first_elem, SageBuilder::buildIntVal(0));
        SageInterface::appendStatement(SageBuilder::buildAssignStatement(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::host_api.kernel_data_field)
            ),
            SageBuilder::buildIntVal(data_cnt++)
          ),
          SageBuilder::buildAddressOfOp(data_first_elem)
        ), bb);
      }

  // Set kernel's loop's bounds

    std::vector<Runtime::loop_desc_t *>::const_iterator it_loop;
    size_t loop_cnt = 0;
    for (it_loop = kernel->loops.begin(); it_loop != kernel->loops.end(); it_loop++) {
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildDotExp(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::host_api.kernel_loop_field)
            ),
            SageBuilder::buildIntVal(loop_cnt)
          ),
          SageBuilder::buildVarRefExp(Runtime::host_api.loop_lower_field)
        ),
        SageInterface::copyExpression((*it_loop)->lb)
      ), bb);

      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildDotExp(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::host_api.kernel_loop_field)
            ),
            SageBuilder::buildIntVal(loop_cnt)
          ),
          SageBuilder::buildVarRefExp(Runtime::host_api.loop_upper_field)
        ),
        SageInterface::copyExpression((*it_loop)->ub)
      ), bb);

      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildDotExp(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::host_api.kernel_loop_field)
            ),
            SageBuilder::buildIntVal(loop_cnt)
          ),
          SageBuilder::buildVarRefExp(Runtime::host_api.loop_stride_field)
        ),
        SageInterface::copyExpression((*it_loop)->stride)
      ), bb);

      loop_cnt++;
    }

    // Insert: "execute_kernel(kernel);"

      SageInterface::appendStatement(
        SageBuilder::buildFunctionCallStmt(
          SageBuilder::buildFunctionRefExp(
            Runtime::host_api.execute_kernel_func
          ),
          SageBuilder::buildExprListExp(
            SageBuilder::buildVarRefExp(kernel_sym)
          )
        ), bb
      );
}

int main(int argc, char ** argv) {

  // Process input source file with ROSE

    SgProject * project = new SgProject::SgProject(argc, argv);
    assert(project->numberOfFiles() == 1);

    SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
    assert(source_file != NULL);

    std::string filename = source_file->get_sourceFileNameWithoutPath();
    std::string basename = filename.substr(0, filename.find_last_of('.'));

  // Declarations

    DLX::Frontend::Frontend<DLX::TileK::language_t> frontend;

    MFB::KLT_Driver driver(project);

    MDCG::ModelBuilder model_builder(driver);

    unsigned tilek_model = Runtime::loadAPI(model_builder, std::string(TILEK_INC_PATH) + "/TileK");

    KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator(driver, basename + "-kernel.c");
    KLT::CG_Config<Annotation, Language, Runtime> cg_config;

  // Initialize TileK's language descritption

    DLX::TileK::language_t::init();

  // Run TileK's DLX frontend

    if (!frontend.parseDirectives(project)) {
      std::cerr << "Error in FrontEnd !!!" << std::endl;
      exit(1);
    }

  // Extract LoopTrees

    std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *> loop_trees;
    std::map<SgForStatement *, LoopTrees::loop_t *> loop_map;
    DLX::extractLoopTrees(frontend.directives, loop_trees, loop_map);

  // Generate Kernels

    std::vector<Kernel *> all_kernels;
    std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, Kernel *> kernel_map;

    std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *>::const_iterator it_loop_tree;
    for (it_loop_tree = loop_trees.begin(); it_loop_tree != loop_trees.end(); it_loop_tree++) {
      // Generate Kernels
      std::set<std::list<Kernel *> > kernel_lists;
      generator.generate(*(it_loop_tree->second), kernel_lists, cg_config);

      // TileK only produces one implementation of the kernel made of one kernel
      assert(kernel_lists.size() == 1);
      assert(kernel_lists.begin()->size() == 1);
      Kernel * kernels = *kernel_lists.begin()->begin();
      assert(kernels->getKernels().size() == 1);

      kernel_map.insert(std::pair<DLX::Directives::directive_t<DLX::TileK::language_t> *, Kernel *>(it_loop_tree->first, kernels));
      all_kernels.push_back(kernels);
    }

  // Replace annotated code by host runtime code

  std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, Kernel *>::const_iterator it_kernel;
  for (it_kernel = kernel_map.begin(); it_kernel != kernel_map.end(); it_kernel++) {
    DLX::TileK::language_t::kernel_construct_t * kernel_construct = DLX::TileK::language_t::isKernelConstruct(it_kernel->first->construct);
    assert(kernel_construct != NULL);
    replaceKernelDirectiveByHostRuntime(driver, kernel_construct->assoc_nodes.kernel_region, it_kernel->second);
  }

  // Stores static description of the generated kernels

    unsigned host_data_file_id = driver.create(boost::filesystem::path(basename + "-data.c"));
      driver.setUnparsedFile(host_data_file_id);
      driver.setCompiledFile(host_data_file_id);
    Runtime::useSymbolsHost(driver, host_data_file_id);

    MDCG::StaticInitializer static_initializer(driver);

    std::ostringstream decl_name;
      decl_name << "kernel_desc";

    std::set<MDCG::Model::class_t> classes;
    model_builder.get(tilek_model).lookup<MDCG::Model::class_t>("kernel_desc_t", classes);
    assert(classes.size() == 1);

    static_initializer.createArrayPointer<MDCG::TileK::KernelDesc>(
      *(classes.begin()),
      all_kernels.size(),
      all_kernels.begin(),
      all_kernels.end(),
      host_data_file_id,
      decl_name.str()
    );

  // Removes all TileK pragma

    std::vector<SgPragmaDeclaration * > pragma_decls = SageInterface::querySubTree<SgPragmaDeclaration>(project);
    std::vector<SgPragmaDeclaration * >::iterator it_pragma_decl;
    for (it_pragma_decl = pragma_decls.begin(); it_pragma_decl != pragma_decls.end(); it_pragma_decl++) {
      std::string directive_string = (*it_pragma_decl)->get_pragma()->get_pragma();
      if (::DLX::Frontend::consume_label(directive_string, ::DLX::TileK::language_t::language_label))
        SageInterface::removeStatement(*it_pragma_decl);
    }

  project->unparse();

  return 0;
}

