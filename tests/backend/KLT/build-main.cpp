
#include "build-main.hpp"
#include "sage3basic.h"

SgVariableSymbol * buildDimArr(
  const std::vector<KLT::Core::Data::section_t> & sections,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body
) {
  SgExprListExp * expr_list = SageBuilder::buildExprListExp();

  std::vector<KLT::Core::Data::section_t>::const_iterator it_section;
  for (it_section = sections.begin(); it_section < sections.end(); it_section++)
    expr_list->append_expression(SageInterface::copyExpression(it_section->second));

  std::vector<SgVarRefExp *> collected_var_refs = SageInterface::querySubTree<SgVarRefExp>(expr_list);
  std::vector<SgVarRefExp *>::iterator it_collected_var_ref;
  for (it_collected_var_ref = collected_var_refs.begin(); it_collected_var_ref < collected_var_refs.end(); it_collected_var_ref++) {
    SgVarRefExp * collected_var_ref = *it_collected_var_ref;
    SgVariableSymbol * collected_var_sym = collected_var_ref->get_symbol();
    assert(collected_var_sym != NULL);
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_gen_sym = gen_symbol_map.find(collected_var_sym);
    if (it_gen_sym != gen_symbol_map.end())
      SageInterface::replaceExpression(collected_var_ref, SageBuilder::buildVarRefExp(it_gen_sym->second));
  }

  std::ostringstream oss_dims;
  oss_dims << "dims_" << sections.size();
  SgVariableSymbol * dims_sym = main_body->lookup_variable_symbol(oss_dims.str());
  SgType * dims_type = SageBuilder::buildArrayType(SageBuilder::buildUnsignedIntType(), SageBuilder::buildIntVal(sections.size()));
  if (dims_sym == NULL) {
    SgVariableDeclaration * dims_decl = SageBuilder::buildVariableDeclaration(oss_dims.str(), dims_type, SageBuilder::buildAggregateInitializer(expr_list, dims_type), main_body);
    SageInterface::appendStatement(dims_decl, main_body);
    dims_sym = main_body->lookup_variable_symbol(oss_dims.str());
  }
  else {
    // assert(dims_sym->get_type() == dims_type); // FIXME Are array types unique ? Probably not because of dims using a SgExpression
    SageInterface::appendStatement(
      SageBuilder::buildAssignStatement(SageBuilder::buildVarRefExp(dims_sym), SageBuilder::buildAggregateInitializer(expr_list, dims_type)),
      main_body
    );
  }
  assert(dims_sym != NULL);

  return dims_sym;
}

struct main_func_desc_t {
  SgFunctionDefinition * defintion;
  SgVariableSymbol * argument_counter;
  SgVariableSymbol * argument_values;
  unsigned long file_id;
};

main_func_desc_t buildMain(MultiFileBuilder::Driver<MultiFileBuilder::Sage> & driver, std::string filename) {
  main_func_desc_t main_desc;

  main_desc.file_id = driver.createStandaloneSourceFile(filename);

  driver.addExternalHeader(main_desc.file_id, "stdlib.h", true);

  SgFunctionParameterList * params = SageBuilder::buildFunctionParameterList(
    SageBuilder::buildInitializedName(
      "argc",
      SageBuilder::buildIntType(),
      NULL
    ),
    SageBuilder::buildInitializedName(
      "argv",
      SageBuilder::buildPointerType(SageBuilder::buildPointerType(SageBuilder::buildCharType())),
      NULL
    )
  );

  MultiFileBuilder::Sage<SgFunctionDeclaration>::object_desc_t desc("main", SageBuilder::buildIntType(), params, NULL, main_desc.file_id);

  MultiFileBuilder::Sage<SgFunctionDeclaration>::build_result_t main_res = driver.build<SgFunctionDeclaration>(desc);

  main_desc.defintion = main_res.definition;

  main_desc.argument_counter = main_desc.defintion->lookup_variable_symbol("argc");
  assert(main_desc.argument_counter != NULL);

  main_desc.argument_values = main_desc.defintion->lookup_variable_symbol("argv");
  assert(main_desc.argument_values != NULL);

  return main_desc;
}

void buildParameter(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  const std::list<SgVariableSymbol *> & parameters
) {
  std::list<SgVariableSymbol *>::const_iterator it_param;
  for (it_param = parameters.begin(); it_param != parameters.end(); it_param++) {
    SgVariableSymbol * parsed_symbol = *it_param;
    SgType * type = parsed_symbol->get_type();

    SgAssignInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
      "atoi",
      SageBuilder::buildFunctionType(
        SageBuilder::buildIntType(),
        SageBuilder::buildFunctionParameterTypeList(SageBuilder::buildPointerType(SageBuilder::buildCharType()))
      ),
      SageBuilder::buildExprListExp(
        SageBuilder::buildPntrArrRefExp(
          SageBuilder::buildVarRefExp(argument_values),
          SageBuilder::buildUnsignedLongVal(arg_cnt++)
        )
      ),
      main_body
    ));

    std::string var_name = "gen_" + parsed_symbol->get_name().getString();

    SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(var_name, type, init, main_body);
    SageInterface::appendStatement(var_decl, main_body);

    SgVariableSymbol * var_sym = main_body->lookup_variable_symbol(var_name);
    assert(var_sym != NULL);

    gen_symbol_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(parsed_symbol, var_sym));
  }
}

void buildCoefficiant(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  const std::set<SgVariableSymbol *> & coefficients
) {
  assert(coefficients.size() == 0); // FIXME I don't want to bother with coefficients in the tests... It will need to be tested at one point
}

void buildDataIn(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  SgFunctionSymbol * data_manager_load_sym,
  const std::list<KLT::Core::Data *> & datas_in
) {
  std::list<KLT::Core::Data *>::const_iterator it_data_order;
  for (it_data_order = datas_in.begin(); it_data_order != datas_in.end(); it_data_order++) {
    KLT::Core::Data * data = *it_data_order;
    SgVariableSymbol * parsed_symbol = data->getVariableSymbol();
    SgType * base_type = data->getBaseType();
    const std::vector<KLT::Core::Data::section_t> & sections = data->getSections();
    
    SgVariableSymbol * dims_sym = buildDimArr(sections, gen_symbol_map, main_body);

    SgAssignInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
      data_manager_load_sym,
      SageBuilder::buildExprListExp(
        SageBuilder::buildIntVal(sections.size()),
        SageBuilder::buildVarRefExp(dims_sym),
        SageBuilder::buildPntrArrRefExp(
          SageBuilder::buildVarRefExp(argument_values),
          SageBuilder::buildUnsignedLongVal(arg_cnt++)
        )
      )
    ));

    std::string var_name = "gen_" + parsed_symbol->get_name().getString();

    SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(var_name, SageBuilder::buildPointerType(base_type), init, main_body);
    SageInterface::appendStatement(var_decl, main_body);

    SgVariableSymbol * var_sym = main_body->lookup_variable_symbol(var_name);
    assert(var_sym != NULL);

    gen_symbol_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(parsed_symbol, var_sym));
  }
}

void buildDataOut(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  SgFunctionSymbol * data_manager_init_sym,
  const std::list<KLT::Core::Data *> & datas_out
) {
  std::list<KLT::Core::Data *>::const_iterator it_data_order;
  for (it_data_order = datas_out.begin(); it_data_order != datas_out.end(); it_data_order++) {
    KLT::Core::Data * data = *it_data_order;
    SgVariableSymbol * parsed_symbol = data->getVariableSymbol();
    SgType * base_type = data->getBaseType();
    const std::vector<KLT::Core::Data::section_t> & sections = data->getSections();
    
    SgVariableSymbol * dims_sym = buildDimArr(sections, gen_symbol_map, main_body);

    SgAssignInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
      data_manager_init_sym,
      SageBuilder::buildExprListExp(
        SageBuilder::buildIntVal(sections.size()),
        SageBuilder::buildVarRefExp(dims_sym)
      )
    ));

    std::string var_name = "gen_" + parsed_symbol->get_name().getString();

    SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(var_name, SageBuilder::buildPointerType(base_type), init, main_body);
    SageInterface::appendStatement(var_decl, main_body);

    SgVariableSymbol * var_sym = main_body->lookup_variable_symbol(var_name);
    assert(var_sym != NULL);

    gen_symbol_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(parsed_symbol, var_sym));
  }
}


void buildDataLocal(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  SgFunctionSymbol * data_manager_init_sym,
  const std::set<KLT::Core::Data *> & local_datas
) {
  std::set<KLT::Core::Data *>::const_iterator it_data;
  for (it_data = local_datas.begin(); it_data != local_datas.end(); it_data++) {
    KLT::Core::Data * data = *it_data;
    assert(data != NULL);
    SgVariableSymbol * parsed_symbol = data->getVariableSymbol();
    SgType * base_type = data->getBaseType();
    const std::vector<KLT::Core::Data::section_t> & sections = data->getSections();
    
    SgVariableSymbol * dims_sym = buildDimArr(sections, gen_symbol_map, main_body);

    SgAssignInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
      data_manager_init_sym,
      SageBuilder::buildExprListExp(
        SageBuilder::buildIntVal(sections.size()),
        SageBuilder::buildVarRefExp(dims_sym)
      )
    ));

    std::string var_name = "gen_" + parsed_symbol->get_name().getString();

    SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(var_name, SageBuilder::buildPointerType(base_type), init, main_body);
    SageInterface::appendStatement(var_decl, main_body);

    SgVariableSymbol * var_sym = main_body->lookup_variable_symbol(var_name);
    assert(var_sym != NULL);
  
    gen_symbol_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(parsed_symbol, var_sym));
  }
}

SgVariableSymbol * buildTargetData(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  SgFunctionSymbol * data_manager_load_sym,
  SgFunctionSymbol * data_manager_compare_sym,
  const std::list<KLT::Core::Data *> & datas_out
) {

  SgVariableDeclaration * result_var = SageBuilder::buildVariableDeclaration(
    "result",
    SageBuilder::buildBoolType(),
    SageBuilder::buildAssignInitializer(SageBuilder::buildBoolValExp(true)),
    main_body
  );
  SageInterface::appendStatement(result_var, main_body);
  
  SgVariableSymbol * result_sym = main_body->lookup_variable_symbol("result");

  std::list<KLT::Core::Data *>::const_iterator it_data_order;
  for (it_data_order = datas_out.begin(); it_data_order != datas_out.end(); it_data_order++) {
    KLT::Core::Data * data = *it_data_order;
    SgVariableSymbol * parsed_symbol = data->getVariableSymbol();
    SgType * base_type = data->getBaseType();
    const std::vector<KLT::Core::Data::section_t> & sections = data->getSections();
    
    SgVariableSymbol * dims_sym = buildDimArr(sections, gen_symbol_map, main_body);

    SgAssignInitializer * init = SageBuilder::buildAssignInitializer(SageBuilder::buildFunctionCallExp(
      data_manager_load_sym,
      SageBuilder::buildExprListExp(
        SageBuilder::buildIntVal(sections.size()),
        SageBuilder::buildVarRefExp(dims_sym),
        SageBuilder::buildPntrArrRefExp(
          SageBuilder::buildVarRefExp(argument_values),
          SageBuilder::buildUnsignedLongVal(arg_cnt++)
        )
      )
    ));

    std::string var_name = "target_" + parsed_symbol->get_name().getString();

    SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(var_name, SageBuilder::buildPointerType(base_type), init, main_body);
    SageInterface::appendStatement(var_decl, main_body);

    SgVariableSymbol * target_symbol = main_body->lookup_variable_symbol(var_name);
    assert(target_symbol != NULL);

    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_gen_data_sym = gen_symbol_map.find(parsed_symbol);
    assert(it_gen_data_sym != gen_symbol_map.end());
    SgVariableSymbol * gen_symbol = it_gen_data_sym->second;

    SageInterface::appendStatement(
      SageBuilder::buildExprStatement(
        SageBuilder::buildAndAssignOp(
          SageBuilder::buildVarRefExp(result_sym),
          SageBuilder::buildFunctionCallExp(
            data_manager_compare_sym,
            SageBuilder::buildExprListExp(
              SageBuilder::buildIntVal(sections.size()),
              SageBuilder::buildVarRefExp(dims_sym),
              SageBuilder::buildVarRefExp(target_symbol),
              SageBuilder::buildVarRefExp(gen_symbol)
            )
          )
        )
      ),
      main_body
    );
  }

  return result_sym;
}

void buildKernelLaunch(
  unsigned int & arg_cnt,
  SgVariableSymbol * argument_counter,
  SgVariableSymbol * argument_values,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & gen_symbol_map,
  SgBasicBlock * main_body,
  const std::list<KLT::Sequential::Kernel *> & kernel_list
) {
  std::list<KLT::Core::Data *>::const_iterator it_data_order;
  std::list<SgVariableSymbol *>::const_iterator it_var_sym_order;

  std::set<SgVariableSymbol *>::const_iterator it_var_sym;

  std::list<KLT::Sequential::Kernel *>::const_iterator it_kernel;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_gen_sym;

  unsigned long cnt_kernel = 0;

  for (it_kernel = kernel_list.begin(); it_kernel != kernel_list.end(); it_kernel++) {
    KLT::Sequential::Kernel * kernel_ = dynamic_cast<KLT::Sequential::Kernel *>(*it_kernel);
    assert(kernel_ != NULL);

    const KLT::Sequential::Kernel::dataflow_t & data_flow = kernel_->getDataflow();

    const  KLT::Sequential::Kernel::arguments_t & arguments_order = kernel_->getArguments();

    const std::set<KLT::Core::Kernel::loop_mapping_t *> & loop_mappings = kernel_->getLoopMappings();
    assert(loop_mappings.size() == 1); // FIXME sould be true for some time (versionning due to loop selection in the Loop-Mapper)
    KLT::Core::Kernel::loop_mapping_t * loop_mapping = *(loop_mappings.begin());

    const std::set<KLT::Core::IterationMap<KLT::Sequential::Kernel> *> & iter_mappings = kernel_->getIterationMaps(loop_mapping);
    assert(iter_mappings.size() == 1); // FIXME sould be true for some time (versionning due to the Iteration-Mapper)
    KLT::Core::IterationMap<KLT::Sequential::Kernel> * iter_mapping = *(iter_mappings.begin());

    KLT::Sequential::Kernel::a_kernel * kernel = kernel_->getKernel(loop_mapping, iter_mapping);
    assert(kernel != NULL);

    assert(data_flow.local.size() == 0); // FIXME extra field, currently unused... 

    SgExprListExp * packer_ctor_args = SageBuilder::buildExprListExp();
    for (it_var_sym_order = arguments_order.parameters.begin(); it_var_sym_order != arguments_order.parameters.end(); it_var_sym_order++) {
      it_gen_sym = gen_symbol_map.find(*it_var_sym_order);
      assert(it_gen_sym != gen_symbol_map.end());
      SgVariableSymbol * gen_sym = it_gen_sym->second;

      packer_ctor_args->append_expression(SageBuilder::buildVarRefExp(gen_sym));
    }
    for (it_var_sym_order = arguments_order.coefficients.begin(); it_var_sym_order != arguments_order.coefficients.end(); it_var_sym_order++) {
      it_gen_sym = gen_symbol_map.find(*it_var_sym_order);
      assert(it_gen_sym != gen_symbol_map.end());
      SgVariableSymbol * gen_sym = it_gen_sym->second;

      packer_ctor_args->append_expression(SageBuilder::buildVarRefExp(gen_sym));
    }
    for (it_data_order = arguments_order.datas.begin(); it_data_order != arguments_order.datas.end(); it_data_order++) {
      it_gen_sym = gen_symbol_map.find((*it_data_order)->getVariableSymbol());
      assert(it_gen_sym != gen_symbol_map.end());
      SgVariableSymbol * gen_sym = it_gen_sym->second;

      packer_ctor_args->append_expression(SageBuilder::buildVarRefExp(gen_sym));
    }

    SgType * packer_ctor_type = kernel->arguments_packer->get_type();

    SgConstructorInitializer * packer_ctor_init = SageBuilder::buildConstructorInitializer(NULL, packer_ctor_args, packer_ctor_type, false, false, true, false);
    SgNewExp * new_packer_ctor = SageBuilder::buildNewExp(packer_ctor_type, NULL, packer_ctor_init, NULL, 0, NULL);

    SageInterface::appendStatement(
      SageBuilder::buildFunctionCallStmt(
        SageBuilder::buildFunctionRefExp(kernel->kernel),
        SageBuilder::buildExprListExp(new_packer_ctor)
      ),
      main_body
    );

    cnt_kernel++;
  }
}

void loadDataManager(
  MultiFileBuilder::Driver<MultiFileBuilder::Sage> & driver,
  SgBasicBlock * main_body,
  SgFunctionSymbol * & data_manager_init_sym,
  SgFunctionSymbol * & data_manager_load_sym,
  SgFunctionSymbol * & data_manager_compare_sym
) {
  unsigned long data_manager_file_id = driver.loadPairOfFiles("data-manager", SRCDIR, SRCDIR);
  
  // Get the global scope accross files
  SgGlobal * global_scope_across_files = driver.project->get_globalScopeAcrossFiles();
  assert(global_scope_across_files != NULL);

  // Lookup namespace DataManager
  SgNamespaceSymbol * data_manager_sym = SageInterface::lookupNamespaceSymbolInParentScopes("DataManager", global_scope_across_files);
  assert(data_manager_sym != NULL);
  SgNamespaceDeclarationStatement * data_manager_decl = data_manager_sym->get_declaration();
  assert(data_manager_decl != NULL);
  SgNamespaceDefinitionStatement * data_manager_defn = data_manager_decl->get_definition();
  assert(data_manager_defn != NULL);

  // Lookup function float * DataManager::init(unsigned long size)
  data_manager_init_sym = SageInterface::lookupFunctionSymbolInParentScopes("init", data_manager_defn);
  assert(data_manager_init_sym != NULL);
  data_manager_init_sym = driver.useSymbol<SgFunctionDeclaration>(data_manager_init_sym, main_body);

  // Lookup function float * DataManager::load(char * filename)
  data_manager_load_sym = SageInterface::lookupFunctionSymbolInParentScopes("load", data_manager_defn);
  assert(data_manager_load_sym != NULL);
  data_manager_load_sym = driver.useSymbol<SgFunctionDeclaration>(data_manager_load_sym, main_body);

  // Lookup function float DataManager::compare(float * a, float * b, unsigned long size)
  data_manager_compare_sym = SageInterface::lookupFunctionSymbolInParentScopes("compare", data_manager_defn);
  assert(data_manager_compare_sym != NULL);
  data_manager_compare_sym = driver.useSymbol<SgFunctionDeclaration>(data_manager_compare_sym, main_body);
}

void createMain(
  MultiFileBuilder::Driver<MultiFileBuilder::Sage> & driver,
  std::string filename,
  const KLT::Core::LoopTrees & loop_trees,
  const std::list<KLT::Sequential::Kernel *> & kernel_list,
  const std::list<SgVariableSymbol *> parameter_order,
  const std::pair<std::list<KLT::Core::Data *>, std::list<KLT::Core::Data *> > & inout_data_order
) {
  main_func_desc_t main_desc = buildMain(driver, filename);

  SgBasicBlock * main_body = main_desc.defintion->get_body();
  SgVariableSymbol * argument_counter = main_desc.argument_counter;
  SgVariableSymbol * argument_values  = main_desc.argument_values;

  SgFunctionSymbol * data_manager_init_sym = NULL;
  SgFunctionSymbol * data_manager_load_sym = NULL;
  SgFunctionSymbol * data_manager_compare_sym = NULL;
  
  loadDataManager(driver, main_body, data_manager_init_sym, data_manager_load_sym, data_manager_compare_sym);

  std::map<SgVariableSymbol *, SgVariableSymbol *> gen_symbol_map;
  unsigned int arg_cnt = 1;
  
  buildParameter(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, parameter_order);

  buildCoefficiant(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, loop_trees.getCoefficients());

  buildDataIn(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, data_manager_load_sym, inout_data_order.first);

  buildDataOut(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, data_manager_init_sym, inout_data_order.second);

  buildDataLocal(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, data_manager_init_sym, loop_trees.getDatasLocal());

  buildKernelLaunch(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, kernel_list);

  SgVariableSymbol * result_sym = buildTargetData(arg_cnt, argument_counter, argument_values, gen_symbol_map, main_body, data_manager_load_sym, data_manager_compare_sym, inout_data_order.second);

  SageInterface::appendStatement(
    SageBuilder::buildReturnStmt(
      SageBuilder::buildVarRefExp(result_sym)
    ),
    main_body
  );
}
