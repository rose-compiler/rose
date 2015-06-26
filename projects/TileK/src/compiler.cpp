
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

#include "MDCG/model-builder.hpp"

#include "rose.h"

#include <cassert>

typedef ::KLT::Language::None Language;
typedef ::KLT::Runtime::TileK Runtime;
typedef ::DLX::KLT_Annotation< ::DLX::TileK::language_t> Annotation;
typedef ::KLT::LoopTrees<Annotation> LoopTrees;
typedef ::KLT::Kernel<Annotation, Language, Runtime> Kernel;

LoopTrees::node_t * buildLoopTree(
  SgStatement * stmt,
  LoopTrees * loop_tree,
  std::vector<SgVariableSymbol *> & iterators,
  std::vector<SgVariableSymbol *> & locals,
  std::vector<SgVariableSymbol *> & others,
  std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map,
  size_t & loop_cnt
);

LoopTrees::block_t * buildLoopTreeBlock(
  SgStatement * stmt,
  LoopTrees * loop_tree,
  std::vector<SgVariableSymbol *> & iterators,
  std::vector<SgVariableSymbol *> & locals,
  std::vector<SgVariableSymbol *> & others,
  std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map,
  size_t & loop_cnt
) {
  LoopTrees::node_t * child = buildLoopTree(stmt, loop_tree, iterators, locals, others, loop_map, loop_cnt);
  assert(child != NULL);
  LoopTrees::block_t * block = dynamic_cast<LoopTrees::block_t *>(child);
  if (block == NULL) {
    block = new LoopTrees::block_t();
    block->children.push_back(child);
  }
  return block;
}

LoopTrees::node_t * buildLoopTree(
  SgStatement * stmt,
  LoopTrees * loop_tree,
  std::vector<SgVariableSymbol *> & iterators,
  std::vector<SgVariableSymbol *> & locals,
  std::vector<SgVariableSymbol *> & others,
  std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map,
  size_t & loop_cnt
) {
  switch (stmt->variantT()) {
    case V_SgBasicBlock:
    {
      SgBasicBlock * bb = (SgBasicBlock *)stmt;

      LoopTrees::block_t * block = new LoopTrees::block_t();

      std::vector<SgStatement *>::const_iterator it_stmt;
      for (it_stmt = bb->get_statements().begin(); it_stmt != bb->get_statements().end(); it_stmt++)
        if (!isSgPragmaDeclaration(*it_stmt))
          block->children.push_back(buildLoopTree(*it_stmt, loop_tree, iterators, locals, others, loop_map, loop_cnt));

      return block;
    }
    case V_SgForStatement:
    {
      SgForStatement * for_stmt = (SgForStatement *)stmt;

      SgVariableSymbol * iterator = NULL;
      SgExpression * lower_bound = NULL;
      SgExpression * upper_bound = NULL;
      SgExpression * stride = NULL;
      assert(SageInterface::getForLoopInformations(for_stmt, iterator, lower_bound, upper_bound, stride));

      iterators.push_back(iterator);
      std::vector<SgVarRefExp *>::const_iterator it_var_ref;
      std::vector<SgVarRefExp *> var_refs;

      var_refs = SageInterface::querySubTree<SgVarRefExp>(for_stmt->get_for_init_stmt());
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (std::find(iterators.begin(), iterators.end(), sym) == iterators.end())
          loop_tree->addParameter(sym); // in a loop : !iterator => parameter
      }

      var_refs = SageInterface::querySubTree<SgVarRefExp>(for_stmt->get_test());
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (std::find(iterators.begin(), iterators.end(), sym) == iterators.end())
          loop_tree->addParameter(sym); // in a loop : !iterator => parameter
      }

      var_refs = SageInterface::querySubTree<SgVarRefExp>(for_stmt->get_increment());
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (std::find(iterators.begin(), iterators.end(), sym) == iterators.end())
          loop_tree->addParameter(sym); // in a loop : !iterator => parameter
      }

      LoopTrees::loop_t * loop = new LoopTrees::loop_t(loop_cnt++, iterator, lower_bound, upper_bound, stride);

      loop_map.insert(std::pair<SgForStatement *, LoopTrees::loop_t *>(for_stmt, loop));

      loop->block = buildLoopTreeBlock(for_stmt->get_loop_body(), loop_tree, iterators, locals, others, loop_map, loop_cnt);

      return loop;
    }
    case V_SgIfStmt:
    {
      SgIfStmt * if_stmt = (SgIfStmt *)stmt;

      SgExprStatement * cond_stmt = isSgExprStatement(if_stmt->get_conditional());
      assert(cond_stmt != NULL);
      SgExpression * cond_expr = cond_stmt->get_expression();
      assert(cond_expr != NULL);

      std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(cond_expr);
      std::vector<SgVarRefExp *>::const_iterator it_var_ref;
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (std::find(iterators.begin(), iterators.end(), sym) == iterators.end() && std::find(locals.begin(), locals.end(), sym) == locals.end())
          others.push_back(sym); 
      }

      LoopTrees::cond_t * cond = new LoopTrees::cond_t(cond_expr);
      
      cond->block_true = buildLoopTreeBlock(if_stmt->get_true_body(), loop_tree, iterators, locals, others, loop_map, loop_cnt);
      cond->block_false = buildLoopTreeBlock(if_stmt->get_false_body(), loop_tree, iterators, locals, others, loop_map, loop_cnt);
      
      return cond;
    }
    case V_SgExprStatement:
    {
      SgExprStatement * expr_stmt = (SgExprStatement *)stmt;
      SgExpression * expr = expr_stmt->get_expression();
      assert(expr != NULL);

      std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(expr);
      std::vector<SgVarRefExp *>::const_iterator it_var_ref;
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (std::find(iterators.begin(), iterators.end(), sym) == iterators.end() && std::find(locals.begin(), locals.end(), sym) == locals.end())
          others.push_back(sym); 
      }

      return new LoopTrees::stmt_t(stmt);
    }
    case V_SgVariableDeclaration:
    {
      SgVariableDeclaration * var_decl = isSgVariableDeclaration(stmt);
      assert(var_decl != NULL);
      SgScopeStatement * scope = var_decl->get_scope();
      assert(scope != NULL);
      const std::vector<SgInitializedName *> & decls = var_decl->get_variables();
      std::vector<SgInitializedName *>::const_iterator it_decl;
      for (it_decl = decls.begin(); it_decl != decls.end(); it_decl++) {
        SgVariableSymbol * var_sym = scope->lookup_variable_symbol((*it_decl)->get_name());
        assert(var_sym != NULL);
        locals.push_back(var_sym);
      }

      std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(stmt);
      std::vector<SgVarRefExp *>::const_iterator it_var_ref;
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (std::find(iterators.begin(), iterators.end(), sym) == iterators.end() && std::find(locals.begin(), locals.end(), sym) == locals.end())
          others.push_back(sym); 
      }

      return new LoopTrees::stmt_t(stmt);
    }
    default:
      std::cerr << "Unsupported statement : " << stmt->class_name() << " ( " << stmt << " )" << std::endl;
      assert(false);
  }

  assert(false);
  return NULL;
}

int main(int argc, char ** argv) {
  SgProject * project = new SgProject::SgProject(argc, argv);

  assert(project->numberOfFiles() == 1);

  SgSourceFile * source_file = isSgSourceFile(project->get_fileList()[0]);
  assert(source_file != NULL);

  std::string filename = source_file->get_sourceFileNameWithoutPath();

  std::string basename = filename.substr(0, filename.find_last_of('.'));

  DLX::TileK::language_t::init();

  DLX::Frontend::Frontend<DLX::TileK::language_t> frontend;
  bool res = frontend.parseDirectives(project);
  assert(res);

  // TODO generate DLX::VIZ

  std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *> loop_trees;
  std::map<SgForStatement *, LoopTrees::loop_t *> loop_map;

  std::vector<DLX::Directives::directive_t<DLX::TileK::language_t> *>::const_iterator it_directive;
  for (it_directive = frontend.graph_entry.begin(); it_directive != frontend.graph_entry.end(); it_directive++) {
    DLX::Directives::directive_t<DLX::TileK::language_t> * directive = *it_directive;
    assert(directive->construct->kind == DLX::TileK::language_t::e_construct_kernel);
    DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> * construct =
                 (DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> *)(directive->construct);

    LoopTrees * loop_tree = new LoopTrees();
      loop_trees.insert(std::pair<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *>(directive, loop_tree));
    std::vector<SgVariableSymbol *> iterators;
    std::vector<SgVariableSymbol *> locals;
    std::vector<SgVariableSymbol *> others;

    SgStatement * region_base = construct->assoc_nodes.kernel_region;

    SgBasicBlock * region_bb = isSgBasicBlock(region_base);
    size_t loop_cnt = 0;
    if (region_bb != NULL) {
      std::vector<SgStatement *>::const_iterator it_stmt;
      for (it_stmt = region_bb->get_statements().begin(); it_stmt != region_bb->get_statements().end(); it_stmt++)
        if (!isSgPragmaDeclaration(*it_stmt)) {
          loop_tree->addTree(buildLoopTree(*it_stmt, loop_tree, iterators, locals, others, loop_map, loop_cnt));
        }
    }
    else {
      loop_tree->addTree(buildLoopTree(region_base, loop_tree, iterators, locals, others, loop_map, loop_cnt));
    }

    const std::vector<SgVariableSymbol *> & params = loop_tree->getParameters();
    const std::vector<KLT::Data<Annotation> *> & datas_ = loop_tree->getDatas();

    std::vector<SgVariableSymbol *> datas;
    std::vector<KLT::Data<Annotation> *>::const_iterator it_data;
    for (it_data = datas_.begin(); it_data != datas_.end(); it_data++)
      datas.push_back((*it_data)->getVariableSymbol());

    std::vector<SgVariableSymbol *>::const_iterator it_other;
    for (it_other = others.begin(); it_other != others.end(); it_other++)
      if (std::find(params.begin(), params.end(), *it_other) == params.end() && std::find(datas.begin(), datas.end(), *it_other) == datas.end()) {
        SgType * data_type = (*it_other)->get_type();
        std::vector<KLT::Data<Annotation>::section_t> sections;
        while (isSgPointerType(data_type)) {
          sections.push_back(KLT::Data<Annotation>::section_t{0,0,0});
          data_type = ((SgPointerType *)data_type)->get_base_type();
        }
        KLT::Data<Annotation> * data = new KLT::Data<Annotation>(*it_other, data_type);
        std::vector<KLT::Data<Annotation>::section_t>::const_iterator it_section;
        for (it_section = sections.begin(); it_section != sections.end(); it_section++)
          data->addSection(*it_section);
        loop_tree->addData(data);
        
//      loop_tree->addScalar(*it_other); // Neither iterators or parameters or data
      }

//  loop_tree->toText(std::cout);
  }

  for (it_directive = frontend.directives.begin(); it_directive != frontend.directives.end(); it_directive++) {
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

  MFB::KLT_Driver driver(project);
  MDCG::ModelBuilder model_builder(driver);

  unsigned tilek_model = model_builder.create();

  model_builder.add(tilek_model, "tile",   std::string(TILEK_INC_PATH) + "/TileK", "h");
  model_builder.add(tilek_model, "loop",   std::string(TILEK_INC_PATH) + "/TileK", "h");
  model_builder.add(tilek_model, "kernel", std::string(TILEK_INC_PATH) + "/TileK", "h");

  Runtime::TileK::loadAPI(model_builder.get(tilek_model));

// TODO MDCG::StaticInitializer static_initializer(driver);

  KLT::Generator<Annotation, Language, Runtime, MFB::KLT_Driver> generator(driver, basename + "-kernel.c");
  KLT::CG_Config<Annotation, Language, Runtime> cg_config(
    new KLT::LoopMapper<Annotation, Language, Runtime>(),
    new KLT::LoopTiler<Annotation, Language, Runtime>(),
    new KLT::DataFlow<Annotation, Language, Runtime>()
  );

  std::vector<Kernel::kernel_desc_t *> all_kernels;
  std::map<DLX::Directives::directive_t<DLX::TileK::language_t> *, LoopTrees *>::const_iterator it_loop_tree;
  for (it_loop_tree = loop_trees.begin(); it_loop_tree != loop_trees.end(); it_loop_tree++) {
//  it_loop_tree->second->toText(std::cout);

    // TODO KLT::Viz LoopTrees

    std::set<std::list<Kernel *> > kernel_lists;
    generator.generate(*(it_loop_tree->second), kernel_lists, cg_config);

    assert(kernel_lists.size() == 1);
    assert(kernel_lists.begin()->size() == 1);
    Kernel * kernels = *kernel_lists.begin()->begin();
    assert(kernels->getKernels().size() == 1);

    Kernel::kernel_desc_t * kernel = kernels->getKernels()[0];
    assert(kernel != NULL);
    all_kernels.push_back(kernel);

    DLX::Directives::directive_t<DLX::TileK::language_t> * directive = it_loop_tree->first;
    assert(directive->construct->kind == DLX::TileK::language_t::e_construct_kernel);
    DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> * construct =
                 (DLX::Directives::construct_t<DLX::TileK::language_t, DLX::TileK::language_t::e_construct_kernel> *)(directive->construct);

    // Replace kernel region by empty basic block
    SgBasicBlock * bb = SageBuilder::buildBasicBlock();
    SageInterface::replaceStatement(construct->assoc_nodes.kernel_region, bb);

    // Insert: "struct kernel_t * kernel = build_kernel(0);"
    SgInitializer * init = SageBuilder::buildAssignInitializer(
      SageBuilder::buildFunctionCallExp(
        Runtime::TileK::tilek_host_api.build_kernel_func,
        SageBuilder::buildExprListExp(
          SageBuilder::buildIntVal(kernel->id)
        )
      )
    );
    SgVariableDeclaration * kernel_decl = SageBuilder::buildVariableDeclaration(
      "kernel", SageBuilder::buildPointerType(Runtime::TileK::tilek_host_api.kernel_class->get_type()), init, bb
    );
    SageInterface::appendStatement(kernel_decl, bb);

    SgVariableSymbol * kernel_sym = SageInterface::getFirstVarSym(kernel_decl);
    assert(kernel_sym != NULL);

    const Kernel::arguments_t & arguments = kernels->getArguments();

    // Param

    std::list<SgVariableSymbol *>::const_iterator it_param;
    size_t param_cnt = 0;
    for (it_param = arguments.parameters.begin(); it_param != arguments.parameters.end(); it_param++) {
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildPntrArrRefExp(
          SageBuilder::buildArrowExp(
            SageBuilder::buildVarRefExp(kernel_sym),
            SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.kernel_param_field)
          ),
          SageBuilder::buildIntVal(param_cnt++)
        ),
        SageBuilder::buildVarRefExp(*it_param)
      ), bb);
    }

    // Data

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
            SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.kernel_data_field)
          ),
          SageBuilder::buildIntVal(data_cnt++)
        ),
        SageBuilder::buildAddressOfOp(data_first_elem)
      ), bb);
    }

    // Loop

    std::vector<Runtime::TileK::loop_desc_t *>::const_iterator it_loop;
    size_t loop_cnt = 0;
    for (it_loop = kernel->loops.begin(); it_loop != kernel->loops.end(); it_loop++) {
      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildDotExp(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.kernel_loop_field)
            ),
            SageBuilder::buildIntVal(loop_cnt)
          ),
          SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.loop_lower_field)
        ),
        SageInterface::copyExpression((*it_loop)->lb)
      ), bb);

      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildDotExp(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.kernel_loop_field)
            ),
            SageBuilder::buildIntVal(loop_cnt)
          ),
          SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.loop_upper_field)
        ),
        SageInterface::copyExpression((*it_loop)->ub)
      ), bb);

      SageInterface::appendStatement(SageBuilder::buildAssignStatement(
        SageBuilder::buildDotExp(
          SageBuilder::buildPntrArrRefExp(
            SageBuilder::buildArrowExp(
              SageBuilder::buildVarRefExp(kernel_sym),
              SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.kernel_loop_field)
            ),
            SageBuilder::buildIntVal(loop_cnt)
          ),
          SageBuilder::buildVarRefExp(Runtime::TileK::tilek_host_api.loop_stride_field)
        ),
        SageInterface::copyExpression((*it_loop)->stride)
      ), bb);
      loop_cnt++;
    }

    // Insert: "execute_kernel(kernel);"

    SageInterface::appendStatement(
      SageBuilder::buildFunctionCallStmt(
        SageBuilder::buildFunctionRefExp(
          Runtime::TileK::tilek_host_api.execute_kernel_func
        ),
        SageBuilder::buildExprListExp(
          SageBuilder::buildVarRefExp(kernel_sym)
        )
      ), bb
    );

/*
  execute_kernel(kernel);
*/
  }

  // TODO save static data : 'all_kernels'
/*
extern kernel_func_ptr kernel_0;

struct tile_desc_t tiles_loop_1[2] = {
  { 0, e_tile_static,   2 },
  { 1, e_tile_dynamic,  0 }
};

struct tile_desc_t tiles_loop_2[2] = {
  { 2, e_tile_static,   2 },
  { 3, e_tile_dynamic,  0 }
};

struct loop_desc_t loop_desc[2] = {
  { 0, 2, tiles_loop_1 },
  { 1, 2, tiles_loop_2 }
};

struct kernel_desc_t kernel_desc[1] = {
  { 2, 2, 2, 4, loop_desc, &kernel_0 }
};
*/

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

