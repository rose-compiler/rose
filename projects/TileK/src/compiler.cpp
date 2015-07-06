
#include "sage3basic.h"

#include "DLX/Core/parser.hpp"
#include "DLX/Core/compiler.hpp"

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

void extractLoopTrees(
  const std::vector<DLX::Directives::directive_t<DLX::TileK::language_t> *> & directives,
  std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *> & loop_trees,
  std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map
) {
  std::vector<DLX::Directives::directive_t<DLX::TileK::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {

    DLX::Directives::directive_t<DLX::TileK::language_t> * directive = *it_directive;
    if (directive->construct->kind != DLX::TileK::language_t::e_construct_kernel) continue; // (language dependant)

    DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> * construct =
                 (DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> *)(directive->construct);

    // Create empty loop-tree (generic)
    LoopTrees * loop_tree = new LoopTrees();
    loop_trees.insert(std::pair<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *>(directive, loop_tree));

    // Add data to loop-tree (language dependant)
    std::vector<DLX::Directives::generic_clause_t<DLX::TileK::language_t> *>::const_iterator it_clause;
    for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) {
      DLX::Directives::generic_clause_t<DLX::TileK::language_t> * clause = *it_clause;
      if (clause->kind != DLX::TileK::language_t::e_clause_data) continue;
      DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_data> * data_clause = 
                   (DLX::Directives::clause_t<DLX::TileK::language_t, DLX::TileK::language_t::e_clause_data> *)clause;

      std::vector<DLX::Frontend::data_sections_t>::const_iterator it_data_sections;
      for (it_data_sections = data_clause->parameters.data_sections.begin(); it_data_sections != data_clause->parameters.data_sections.end(); it_data_sections++) {
        SgVariableSymbol * data_sym = it_data_sections->first;

        SgType * base_type = data_sym->get_type();
        std::vector<DLX::Frontend::section_t>::const_iterator it_section;
        for (it_section = it_data_sections->second.begin(); it_section != it_data_sections->second.end(); it_section++) {
          SgPointerType * ptr_type = isSgPointerType(base_type);
          SgArrayType * arr_type = isSgArrayType(base_type);
          if (ptr_type != NULL)
            base_type = ptr_type->get_base_type();
          else if (arr_type != NULL)
            base_type = arr_type->get_base_type();
          else assert(false);
          assert(base_type != NULL);
        }
        KLT::Data<Annotation> * data = new KLT::Data<Annotation>(data_sym, base_type);
        for (it_section = it_data_sections->second.begin(); it_section != it_data_sections->second.end(); it_section++) {
          KLT::Data<Annotation>::section_t section;
            section.lower_bound = it_section->lower_bound;
            section.size = it_section->size;
            section.stride = it_section->stride;
          data->addSection(section);
        }
        loop_tree->addData(data);
      }
    }

    std::vector<SgVariableSymbol *> iterators, locals, others;

    // Parse the loop nest (generic)
    SgStatement * region_base = construct->assoc_nodes.kernel_region;
    SgBasicBlock * region_bb = isSgBasicBlock(region_base);
    size_t loop_cnt = 0;
    if (region_bb != NULL) {
      std::vector<SgStatement *>::const_iterator it_stmt;
      for (it_stmt = region_bb->get_statements().begin(); it_stmt != region_bb->get_statements().end(); it_stmt++)
        if (!isSgPragmaDeclaration(*it_stmt)) {
          loop_tree->addTree(LoopTrees::build(*it_stmt, loop_tree, iterators, locals, others, loop_map, loop_cnt));
        }
    }
    else {
      loop_tree->addTree(LoopTrees::build(region_base, loop_tree, iterators, locals, others, loop_map, loop_cnt));
    }

    // Detect scalar used in loop-tree
    const std::vector<SgVariableSymbol *> & params = loop_tree->getParameters();
    const std::vector<KLT::Data<Annotation> *> & datas_ = loop_tree->getDatas();
    std::vector<SgVariableSymbol *> datas;
    std::vector<KLT::Data<Annotation> *>::const_iterator it_data;
    for (it_data = datas_.begin(); it_data != datas_.end(); it_data++)
      datas.push_back((*it_data)->getVariableSymbol());
    std::vector<SgVariableSymbol *>::const_iterator it_other;
    for (it_other = others.begin(); it_other != others.end(); it_other++)
      if (std::find(params.begin(), params.end(), *it_other) == params.end() && std::find(datas.begin(), datas.end(), *it_other) == datas.end())
        loop_tree->addScalar(*it_other); // Neither iterators or parameters or data
  }

  // Apply loop construct to loops in loop-tree
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    DLX::Directives::directive_t<DLX::TileK::language_t> * directive = *it_directive;

    if (directive->construct->kind != DLX::TileK::language_t::e_construct_loop) continue;

    DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_loop> * construct =
                 (DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_loop> *)(directive->construct);
    assert(construct->assoc_nodes.for_loop != NULL);

    std::map<SgForStatement *, LoopTrees::loop_t *>::const_iterator it_loop = loop_map.find(construct->assoc_nodes.for_loop);
    assert(it_loop != loop_map.end());

    LoopTrees::loop_t * loop = it_loop->second;

    const std::vector<DLX::Directives::generic_clause_t<DLX::TileK::language_t> *> & clauses = directive->clause_list;
    std::vector<DLX::Directives::generic_clause_t<DLX::TileK::language_t> *>::const_iterator it_clause;
    for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
      loop->annotations.push_back(DLX::KLT_Annotation<DLX::TileK::language_t>(*it_clause));
    }
  }
}
  

int main(int argc, char ** argv) {
  SgProject * project = new SgProject::SgProject(argc, argv);

  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();

  std::string basename = filename.substr(0, filename.find_last_of('.'));

  DLX::TileK::language_t::init();

  // Run DLX frontend
  DLX::Frontend::Frontend<DLX::TileK::language_t> frontend;
  bool res = frontend.parseDirectives(project);
  assert(res);

  // Extract LoopTree
  std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *> loop_trees;
  std::map<SgForStatement *, LoopTrees::loop_t *> loop_map;
  extractLoopTrees(frontend.directives, loop_trees, loop_map);

  MFB::KLT_Driver driver(project);
  MDCG::ModelBuilder model_builder(driver);

  // Load model of TileK
  unsigned tilek_model = model_builder.create();
    model_builder.add(tilek_model, "tile",   std::string(TILEK_INC_PATH) + "/TileK", "h");
    model_builder.add(tilek_model, "loop",   std::string(TILEK_INC_PATH) + "/TileK", "h");
    model_builder.add(tilek_model, "kernel", std::string(TILEK_INC_PATH) + "/TileK", "h");

  Runtime::loadAPI(model_builder.get(tilek_model));

  KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator(driver, basename + "-kernel.c");
  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
    new KLT::LoopMapper<Annotation, Language, Runtime>(),
    new KLT::LoopTiler<Annotation, Language, Runtime>(),
    new KLT::DataFlow<Annotation, Language, Runtime>()
  );

  std::vector<Kernel *> all_kernels;
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

    all_kernels.push_back(kernels);

    // TileK only produces one version of each kernel
    Kernel::kernel_desc_t * kernel = kernels->getKernels()[0];
    assert(kernel != NULL);

    // Cast kernel construct associated with the loop-tree
    DLX::Directives::directive_t<DLX::TileK::language_t> * directive = it_loop_tree->first;
    assert(directive->construct->kind == DLX::TileK::language_t::e_construct_kernel);
    DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> * construct =
                 (DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> *)(directive->construct);

    // Declare symbols used in host code (create include statement)
    assert(construct->assoc_nodes.kernel_region->get_scope() != NULL);
    Runtime::useSymbolsHost(driver, driver.getFileID(construct->assoc_nodes.kernel_region->get_scope()));

    // Replace kernel region by empty basic block
    SgBasicBlock * bb = SageBuilder::buildBasicBlock();
    SageInterface::replaceStatement(construct->assoc_nodes.kernel_region, bb);

    // Insert: "struct kernel_t * kernel = build_kernel(0);"
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

  // Stores static description of the generated kernels
  {
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
  }

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

