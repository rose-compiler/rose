/*!
 * 
 * \file lib/openacc/compiler.cpp
 *
 * \author Tristan Vanderbruggen
 *
 */

#include "DLX/Core/compiler.hpp"
#include "DLX/OpenACC/language.hpp"
#include "DLX/OpenACC/compiler.hpp"

#include "KLT/Core/loop-trees.hpp"
#include "KLT/OpenACC/iteration-mapper.hpp"

namespace DLX {

namespace OpenACC {

compiler_modules_t::compiler_modules_t(
  SgProject * project,
  const std::string & ocl_kernels_file_,
  const std::string & kernels_desc_file_,
  const std::string & versions_db_file_,
  const std::string & libopenacc_inc_dir,
  const std::string & kernels_dir
) :
  driver(project),
  model_builder(driver),
  codegen(driver),
  generator(driver, ocl_kernels_file_),
  cg_config(
    new KLT::LoopMapper<Annotation, Language, Runtime>(),
    new KLT::OpenACC::IterationMapper(),
    new KLT::DataFlow<Annotation, Language, Runtime>()
  ),
  libopenacc_model(0),
  host_data_file_id(0),
  ocl_kernels_file(ocl_kernels_file_),
  versions_db_file(versions_db_file_),
  region_desc_class(NULL),
  comp_data()
{
  host_data_file_id = driver.add(boost::filesystem::path(kernels_desc_file_));
    driver.setUnparsedFile(host_data_file_id);

  libopenacc_model = MDCG::OpenACC::readOpenaccModel(model_builder, libopenacc_inc_dir);
  std::set<MDCG::Model::class_t> classes;
  model_builder.get(libopenacc_model).lookup<MDCG::Model::class_t>("acc_compiler_data_t_", classes);
  assert(classes.size() == 1);
  region_desc_class = *(classes.begin());

  comp_data.runtime_dir = SageBuilder::buildStringVal(libopenacc_inc_dir);
  comp_data.ocl_runtime = SageBuilder::buildStringVal("lib/opencl/libopenacc.cl");
  comp_data.kernels_dir = SageBuilder::buildStringVal(kernels_dir);

  KLT::Runtime::OpenACC::loadAPI(driver, libopenacc_inc_dir);
}

}

namespace Compiler {

void translateDataSections(
  const std::vector<Frontend::data_sections_t> & data_sections,
  Directives::generic_clause_t<OpenACC::language_t> * clause,
  LoopTrees * loop_tree
) {
  std::vector<Frontend::data_sections_t>::const_iterator it_data;
  for (it_data = data_sections.begin(); it_data != data_sections.end(); it_data++) {
    KLT::Data<KLT_Annotation<OpenACC::language_t> > * data = new KLT::Data<KLT_Annotation<OpenACC::language_t> >(it_data->first, it_data->second.size());
    assert(data != NULL);
    loop_tree->addData(data);

    data->annotations.push_back(KLT_Annotation<OpenACC::language_t>(clause));

    std::vector<Frontend::section_t>::const_iterator it_section;
    for (it_section = it_data->second.begin(); it_section != it_data->second.end(); it_section++) {
      KLT::Data<DLX::KLT_Annotation<DLX::OpenACC::language_t> >::section_t section;
      section.lower_bound = it_section->lower_bound;
      section.size = it_section->size;
      section.stride = it_section->stride;
      data->addSection(section);

      std::vector<SgVarRefExp *>::const_iterator it_var_ref;
      std::vector<SgVarRefExp *> var_refs;

      if (section.lower_bound != NULL) {
        var_refs = SageInterface::querySubTree<SgVarRefExp>(section.lower_bound);
        for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
          loop_tree->addParameter((*it_var_ref)->get_symbol());
      }
      if (section.size != NULL) {
        var_refs = SageInterface::querySubTree<SgVarRefExp>(section.size);
        for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
          loop_tree->addParameter((*it_var_ref)->get_symbol());
      }
      if (section.stride != NULL) {
        var_refs = SageInterface::querySubTree<SgVarRefExp>(section.stride);
        for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++)
          loop_tree->addParameter((*it_var_ref)->get_symbol());
      }
    }
  }
}

void interpretClauses(
  const std::vector<Directives::generic_clause_t<OpenACC::language_t> *> & clauses,
  LoopTrees * loop_tree
) {
  std::vector<Directives::generic_clause_t<OpenACC::language_t> *>::const_iterator it_clause;
  for (it_clause = clauses.begin(); it_clause != clauses.end(); it_clause++) {
    switch ((*it_clause)->kind) {
      case OpenACC::language_t::e_acc_clause_if:
      {
        assert(false); /// \todo
        break;
      }
      case OpenACC::language_t::e_acc_clause_async:
      {
        assert(false); /// \todo
        break;
      }
      case OpenACC::language_t::e_acc_clause_num_gangs:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_gangs> *)(*it_clause);
        loop_tree->setNumGangs(clause->parameters.lvl, clause->parameters.exp);
        break;
      }
      case OpenACC::language_t::e_acc_clause_num_workers:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_num_workers> *)(*it_clause);
        loop_tree->setNumWorkers(clause->parameters.lvl, clause->parameters.exp);
        break;
      }
      case OpenACC::language_t::e_acc_clause_vector_length:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_vector_length> *)(*it_clause);
        loop_tree->setVectorLength(clause->parameters.exp);
        break;
      }
      case OpenACC::language_t::e_acc_clause_reduction:
      {
        assert(false); /// \todo
        break;
      }
      case OpenACC::language_t::e_acc_clause_copy:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copy> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copy> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_copyin:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyin> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyin> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_copyout:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyout> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_copyout> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_create:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_create> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_create> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_present:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_present_or_copy:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copy> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copy> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_present_or_copyin:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyin> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyin> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_present_or_copyout:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyout> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_copyout> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_present_or_create:
      {
        Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_create> * clause =
                 (Directives::clause_t<OpenACC::language_t, OpenACC::language_t::e_acc_clause_present_or_create> *)(*it_clause);
        translateDataSections(clause->parameters.data_sections, clause, loop_tree);
        break;
      }
      case OpenACC::language_t::e_acc_clause_deviceptr:
      {
        assert(false); /// \todo
        break;
      }
      case OpenACC::language_t::e_acc_clause_private:
      {
        assert(false); /// \todo
        break;
      }
      case OpenACC::language_t::e_acc_clause_firstprivate:
      {
        assert(false); /// \todo
        break;
      }
      default:
        assert(false);
    }
  }
}

LoopTrees::node_t * buildLoopTree(SgStatement * stmt, LoopTrees * loop_tree, std::set<SgVariableSymbol *> & iterators, std::set<SgVariableSymbol *> & others, std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map);

LoopTrees::block_t * buildLoopTreeBlock(SgStatement * stmt, LoopTrees * loop_tree, std::set<SgVariableSymbol *> & iterators, std::set<SgVariableSymbol *> & others, std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map) {
  LoopTrees::node_t * child = buildLoopTree(stmt, loop_tree, iterators, others, loop_map);
  assert(child != NULL);
  LoopTrees::block_t * block = dynamic_cast<LoopTrees::block_t *>(child);
  if (block == NULL) {
    block = new LoopTrees::block_t();
    block->children.push_back(child);
  }
  return block;
}

LoopTrees::node_t * buildLoopTree(SgStatement * stmt, LoopTrees * loop_tree, std::set<SgVariableSymbol *> & iterators, std::set<SgVariableSymbol *> & others, std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map) {
  switch (stmt->variantT()) {
    case V_SgBasicBlock:
    {
      SgBasicBlock * bb = (SgBasicBlock *)stmt;

      LoopTrees::block_t * block = new LoopTrees::block_t();

      std::vector<SgStatement *>::const_iterator it_stmt;
      for (it_stmt = bb->get_statements().begin(); it_stmt != bb->get_statements().end(); it_stmt++)
        if (!isSgPragmaDeclaration(*it_stmt))
          block->children.push_back(buildLoopTree(*it_stmt, loop_tree, iterators, others, loop_map));

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

      iterators.insert(iterator);
      std::vector<SgVarRefExp *>::const_iterator it_var_ref;
      std::vector<SgVarRefExp *> var_refs;

      var_refs = SageInterface::querySubTree<SgVarRefExp>(for_stmt->get_for_init_stmt());
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (iterators.find(sym) == iterators.end())
          loop_tree->addParameter(sym); // in a loop : !iterator => parameter
      }

      var_refs = SageInterface::querySubTree<SgVarRefExp>(for_stmt->get_test());
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (iterators.find(sym) == iterators.end())
          loop_tree->addParameter(sym); // in a loop : !iterator => parameter
      }

      var_refs = SageInterface::querySubTree<SgVarRefExp>(for_stmt->get_increment());
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVariableSymbol * sym = (*it_var_ref)->get_symbol();
        if (iterators.find(sym) == iterators.end())
          loop_tree->addParameter(sym); // in a loop : !iterator => parameter
      }

      LoopTrees::loop_t * loop = new LoopTrees::loop_t(iterator, lower_bound, upper_bound, stride);

      loop_map.insert(std::pair<SgForStatement *, LoopTrees::loop_t *>(for_stmt, loop));

      loop->block = buildLoopTreeBlock(for_stmt->get_loop_body(), loop_tree, iterators, others, loop_map);

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
        if (iterators.find(sym) == iterators.end())
          others.insert(sym); 
      }

      LoopTrees::cond_t * cond = new LoopTrees::cond_t(cond_expr);
      
      cond->block_true = buildLoopTreeBlock(if_stmt->get_true_body(), loop_tree, iterators, others, loop_map);
      cond->block_false = buildLoopTreeBlock(if_stmt->get_false_body(), loop_tree, iterators, others, loop_map);
      
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
        if (iterators.find(sym) == iterators.end())
          others.insert(sym); 
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

void extractLoopTrees(
  const std::vector<Directives::directive_t<OpenACC::language_t> *> & directives,
  std::map<Directives::directive_t<OpenACC::language_t> *, LoopTrees *> & regions,
  std::map<SgForStatement *, LoopTrees::loop_t *> & loop_map
) {
  std::vector<Directives::directive_t<OpenACC::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    Directives::directive_t<OpenACC::language_t> * directive = *it_directive;
    switch (directive->construct->kind) {
      case OpenACC::language_t::e_acc_construct_parallel:
      {
        Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> * construct =
                 (Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_parallel> *)(directive->construct);

        LoopTrees * loop_tree = new LoopTrees();
        regions.insert(std::pair<Directives::directive_t<OpenACC::language_t> *, LoopTrees *>(directive, loop_tree));

        interpretClauses(directive->clause_list, loop_tree);

        SgStatement * region_base = construct->assoc_nodes.parallel_region;
        if (isSgPragmaDeclaration(region_base)) {
          assert(directive->successor_list.size() == 1);
          Directives::directive_t<OpenACC::language_t> * child = directive->successor_list.begin()->second;
          assert(child->construct->kind == OpenACC::language_t::e_acc_construct_loop);
          region_base = ((Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> *)directive->construct)->assoc_nodes.for_loop;
        }

       std::set<SgVariableSymbol *> iterators;
       std::set<SgVariableSymbol *> others;

        SgBasicBlock * region_bb = isSgBasicBlock(region_base);
        if (region_bb != NULL) {
           std::vector<SgStatement *>::const_iterator it_stmt;
           for (it_stmt = region_bb->get_statements().begin(); it_stmt != region_bb->get_statements().end(); it_stmt++)
             if (!isSgPragmaDeclaration(*it_stmt))
               loop_tree->addTree(buildLoopTree(*it_stmt, loop_tree, iterators, others, loop_map));
        }
        else loop_tree->addTree(buildLoopTree(region_base, loop_tree, iterators, others, loop_map));

        const std::set<SgVariableSymbol *> & params = loop_tree->getParameters();
        const std::set<KLT::Data<Annotation> *> & datas_ = loop_tree->getDatas();

        std::set<SgVariableSymbol *> datas;
        std::set<KLT::Data<Annotation> *>::const_iterator it_data;
        for (it_data = datas_.begin(); it_data != datas_.end(); it_data++)
          datas.insert((*it_data)->getVariableSymbol());

        std::set<SgVariableSymbol *>::const_iterator it_other;
        for (it_other = others.begin(); it_other != others.end(); it_other++)
          if (params.find(*it_other) == params.end() && datas.find(*it_other) == datas.end())
            loop_tree->addScalar(*it_other); // Nor iterators or parameters or data

        

        loop_tree->toText(std::cout);

        break;
      }
      case OpenACC::language_t::e_acc_construct_kernel:
      {
        assert(false); /// \todo generate LoopTrees from kernel regions
        break;
      }
      case OpenACC::language_t::e_acc_construct_data:
      case OpenACC::language_t::e_acc_construct_loop:
      case OpenACC::language_t::e_acc_construct_host_data:
      case OpenACC::language_t::e_acc_construct_declare:
      case OpenACC::language_t::e_acc_construct_cache:
      case OpenACC::language_t::e_acc_construct_update:
      case OpenACC::language_t::e_acc_construct_blank:
      default:
        break;
    }
  }
}

/*!
 * \addtogroup grp_dlx_openacc_compiler
 * @{
 */

template <>
bool Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::compile(
  const Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::directives_ptr_set_t & directives,
  const Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::directives_ptr_set_t & graph_entry,
  const Compiler<DLX::OpenACC::language_t, DLX::OpenACC::compiler_modules_t>::directives_ptr_set_t & graph_final
) {
  /// \todo verify that it is correct OpenACC.....

  std::map<Directives::directive_t<OpenACC::language_t> *, LoopTrees *> regions;
  std::map<SgForStatement *, LoopTrees::loop_t *> loop_map;
  extractLoopTrees(directives, regions, loop_map);

  std::vector<Directives::directive_t<OpenACC::language_t> *>::const_iterator it_directive;
  for (it_directive = directives.begin(); it_directive != directives.end(); it_directive++) {
    Directives::directive_t<OpenACC::language_t> * directive = *it_directive;
    if (directive->construct->kind == OpenACC::language_t::e_acc_construct_loop) {
      Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> * loop_construct =
                 (Directives::construct_t<OpenACC::language_t, OpenACC::language_t::e_acc_construct_loop> *)(directive->construct);
      std::map<SgForStatement *, LoopTrees::loop_t *>::const_iterator it_loop = loop_map.find(loop_construct->assoc_nodes.for_loop);
      assert(it_loop != loop_map.end());
      std::vector<Directives::generic_clause_t<OpenACC::language_t> *>::const_iterator it_clause;
      for (it_clause = directive->clause_list.begin(); it_clause != directive->clause_list.end(); it_clause++) 
        it_loop->second->annotations.push_back(KLT_Annotation<OpenACC::language_t>(*it_clause));
    }
  }

  size_t region_cnt = 0;
  std::map<Directives::directive_t<OpenACC::language_t> *, LoopTrees *>::const_iterator it_region;
  for (it_region = regions.begin(); it_region != regions.end(); it_region++) {
    MDCG::OpenACC::RegionDesc::input_t input_region;
      input_region.id = region_cnt++;
      input_region.file = compiler_modules.ocl_kernels_file;

    compiler_modules.generator.generate(*(it_region->second), input_region.kernel_lists, compiler_modules.cg_config);

    compiler_modules.comp_data.regions.push_back(input_region);
  }

  compiler_modules.codegen.addDeclaration<MDCG::OpenACC::CompilerData>(compiler_modules.region_desc_class, compiler_modules.comp_data, compiler_modules.host_data_file_id, "compiler_data");

  MDCG::OpenACC::CompilerData::storeToDB(compiler_modules.versions_db_file, compiler_modules.comp_data);

  return true;
}

/** @} */
}

}

