
#include "KLT/Sequential/mfb-klt.hpp"

#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"

#include "KLT/Sequential/kernel.hpp"

#include "KLT/Core/iteration-mapper.hpp"

#include "MFB/Sage/function-declaration.hpp"
#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

namespace MultiFileBuilder {

void createFields(
  Driver<Sage> & driver,
  SgClassSymbol * arguments_packer,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & param_to_field_map,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & coef_to_field_map,
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> & data_to_field_map,
  const ::KLT::Core::Kernel::arguments_t & arguments,
  unsigned long file_id
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Core::Data *>::const_iterator it_data;

  // ******************

  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type =  param_sym->get_type();

    MultiFileBuilder::Sage<SgVariableDeclaration>::object_desc_t param_desc("param_" + param_name, param_type, NULL, arguments_packer, file_id);
    MultiFileBuilder::Sage<SgVariableDeclaration>::build_result_t param_result = driver.build<SgVariableDeclaration>(param_desc);

    param_to_field_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, param_result.symbol));
  }

  // ******************

  for (it_var_sym = arguments.coefficients.begin(); it_var_sym != arguments.coefficients.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();
    SgType * coef_type = coef_sym->get_type();

    // Create fields

    MultiFileBuilder::Sage<SgVariableDeclaration>::object_desc_t coef_desc("coef_" + coef_name, coef_type, NULL, arguments_packer, file_id);
    MultiFileBuilder::Sage<SgVariableDeclaration>::build_result_t coef_result = driver.build<SgVariableDeclaration>(coef_desc);

    coef_to_field_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(coef_sym, coef_result.symbol));
  }

  // ******************

  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    // Create fields

    MultiFileBuilder::Sage<SgVariableDeclaration>::object_desc_t data_desc("data_" + data_name, field_type, NULL, arguments_packer, file_id);
    MultiFileBuilder::Sage<SgVariableDeclaration>::build_result_t data_result = driver.build<SgVariableDeclaration>(data_desc);

    data_to_field_map.insert(std::pair< ::KLT::Core::Data *, SgVariableSymbol *>(data, data_result.symbol));
  }
}

void createConstructorInitializerList(
  Driver<Sage> & driver,
  SgMemberFunctionDeclaration * ctor_decl,
  const ::KLT::Core::Kernel::arguments_t & arguments
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Core::Data *>::const_iterator it_data;

  SgCtorInitializerList * ctor_init_list = ctor_decl->get_CtorInitializerList();
  assert(ctor_init_list != NULL);
  SgFunctionDefinition * ctor_defn = ctor_decl->get_definition();
  assert(ctor_defn != NULL);

  // ******************

  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type =  param_sym->get_type();

    SgVariableSymbol * param_param_sym = ctor_defn->lookup_variable_symbol("param_" + param_name + "_");
    assert(param_param_sym != NULL);

    ctor_init_list->append_ctor_initializer(SageBuilder::buildInitializedName(
      "param_" + param_name, param_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(param_param_sym))
    ));
  }

  // ******************
  
  for (it_var_sym = arguments.coefficients.begin(); it_var_sym != arguments.coefficients.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();
    SgType * coef_type = coef_sym->get_type();

    SgVariableSymbol * param_coef_sym = ctor_defn->lookup_variable_symbol("coef_" + coef_name + "_");
    assert(param_coef_sym != NULL);

    ctor_init_list->append_ctor_initializer(SageBuilder::buildInitializedName(
      "coef_" + coef_name, coef_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(param_coef_sym))
    ));
  }

  // ******************
  
  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    SgVariableSymbol * param_data_sym = ctor_defn->lookup_variable_symbol("data_" + data_name + "_");
    assert(param_data_sym != NULL);

    ctor_init_list->append_ctor_initializer(SageBuilder::buildInitializedName(
      "data_" + data_name, field_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(param_data_sym))
    ));
  }
}

SgBasicBlock * createLocalDeclarations(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  SgClassSymbol * arguments_packer,
  ::KLT::Sequential::Kernel::local_symbol_maps_t & local_symbol_maps,
  const std::map<SgVariableSymbol *, SgVariableSymbol *>   & param_to_field_map,
  const std::map<SgVariableSymbol *, SgVariableSymbol *>   & coef_to_field_map,
  const std::map< ::KLT::Core::Data *, SgVariableSymbol *> & data_to_field_map,
  const ::KLT::Core::Kernel::arguments_t & arguments,
  const std::set<SgVariableSymbol *> & iterators
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Core::Data *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_coef_to_field;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *>::const_iterator it_data_to_field;
  
  // * Definition *

  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);
  SgVariableSymbol * kernel_arg_sym = kernel_defn->lookup_variable_symbol("arg_");
  assert(kernel_arg_sym != NULL);

  // * Argument Cast *

  SgType * arg_ptr_type = SageBuilder::buildPointerType(arguments_packer->get_type());

  SgVariableDeclaration * arg_cast_decl = SageBuilder::buildVariableDeclaration(
    "arg",
    arg_ptr_type,
    SageBuilder::buildAssignInitializer(SageBuilder::buildCastExp(
      SageBuilder::buildVarRefExp(kernel_arg_sym),
      arg_ptr_type
    )),
    kernel_body
  );
  SageInterface::appendStatement(arg_cast_decl, kernel_body);

  SgVariableSymbol * arg_cast_sym = kernel_body->lookup_variable_symbol("arg");
  assert(arg_cast_sym != NULL);

  // ***************

  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type = param_sym->get_type();

    it_param_to_field = param_to_field_map.find(param_sym);
    assert(it_param_to_field !=  param_to_field_map.end());
    SgVariableSymbol * field_sym = it_param_to_field->second;

    SgVariableDeclaration * param_decl = SageBuilder::buildVariableDeclaration(
      "local_param_" + param_name,
      param_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(arg_cast_sym),
        SageBuilder::buildVarRefExp(field_sym)
      )),
      kernel_body
    );
    SageInterface::appendStatement(param_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_param_" + param_name);
    assert(local_sym != NULL);
    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, local_sym));
  }

  // ***************

  for (it_var_sym = arguments.coefficients.begin(); it_var_sym != arguments.coefficients.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();
    SgType * coef_type = coef_sym->get_type();

    it_coef_to_field = coef_to_field_map.find(coef_sym);
    assert(it_coef_to_field != coef_to_field_map.end());
    SgVariableSymbol * field_sym = it_coef_to_field->second;

    SgVariableDeclaration * coef_decl = SageBuilder::buildVariableDeclaration(
      "local_coef_" + coef_name,
      coef_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(arg_cast_sym),
        SageBuilder::buildVarRefExp(field_sym)
      )),
      kernel_body
    );
    SageInterface::appendStatement(coef_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_coef_" + coef_name);
    assert(local_sym != NULL);
    local_symbol_maps.coefficients.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(coef_sym, local_sym));
  }

  // ***************

  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();;
    std::string data_name = data_sym->get_name().getString();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    it_data_to_field = data_to_field_map.find(data);
    assert(it_data_to_field != data_to_field_map.end());
    SgVariableSymbol * field_sym = it_data_to_field->second;

    SgVariableDeclaration * data_decl = SageBuilder::buildVariableDeclaration(
      "local_data_" + data_name,
      field_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(arg_cast_sym),
        SageBuilder::buildVarRefExp(field_sym)
      )),
      kernel_body
    );
    SageInterface::appendStatement(data_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_data_" + data_name);
    assert(local_sym != NULL);
    local_symbol_maps.datas.insert(std::pair< ::KLT::Core::Data *, SgVariableSymbol *>(data, local_sym));
  }

  // ***************

  std::set<SgVariableSymbol *>::const_iterator it_iter_sym;
  for (it_iter_sym = iterators.begin(); it_iter_sym != iterators.end(); it_iter_sym++) {
    SgVariableSymbol * iter_sym = *it_iter_sym;
    std::string iter_name = iter_sym->get_name().getString();
    SgType * iter_type = iter_sym->get_type();

    SgVariableDeclaration * iter_decl = SageBuilder::buildVariableDeclaration(
      "local_it_" + iter_name,
      iter_type,
      NULL,
      kernel_body
    );
    SageInterface::appendStatement(iter_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_it_" + iter_name);
    assert(local_sym != NULL);
    local_symbol_maps.iterators.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iter_sym, local_sym));
  }

  // ***************

  return kernel_body;
}

KLT< ::KLT::Sequential::Kernel>::object_desc_t::object_desc_t(
  ::KLT::Sequential::Kernel * kernel_,
  ::KLT::Sequential::Kernel::loop_mapping_t * loop_mapping_,
  ::KLT::Core::IterationMap< ::KLT::Sequential::Kernel> * iteration_map_,
  unsigned long file_id_
) :
  kernel(kernel_),
  loop_mapping(loop_mapping_),
  iteration_map(iteration_map_),
  file_id(file_id_)
{}

template <>
KLT< ::KLT::Sequential::Kernel>::build_result_t Driver<KLT>::build< ::KLT::Sequential::Kernel>(const KLT< ::KLT::Sequential::Kernel>::object_desc_t & desc) {
  KLT< ::KLT::Sequential::Kernel>::build_result_t result = new ::KLT::Sequential::Kernel::a_kernel();

  ::KLT::Sequential::Kernel * kernel = desc.kernel;
  unsigned long file_id = desc.file_id;
  
  const ::KLT::Core::Kernel::arguments_t & arguments = kernel->getArguments();

  std::map<SgVariableSymbol *, SgVariableSymbol *>   param_to_field_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   coef_to_field_map;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> data_to_field_map;

  std::list< ::KLT::Core::LoopTrees::loop_t *>::const_iterator it_nested_loop;
  std::list< ::KLT::Core::LoopTrees::node_t *>::const_iterator it_body_branch;

  // * Build Iterator Set *

  std::set<SgVariableSymbol *> iterators;
    collectIteratorSymbols(desc.kernel->getRoot(), iterators);

  // * Argument Packer Declaration *

  std::ostringstream arguments_packer_name;
  arguments_packer_name << "arguments_packer_" << kernel->id << "_" << kernel;

  MultiFileBuilder::Sage<SgClassDeclaration>::object_desc_t arguments_packer_desc(
    arguments_packer_name.str(),
    (unsigned long)SgClassDeclaration::e_struct,
    NULL,
    file_id,
    true
  );

  Sage<SgClassDeclaration>::build_result_t arguments_packer_result = ((Driver<Sage> *)this)->build<SgClassDeclaration>(arguments_packer_desc);

  result->arguments_packer = arguments_packer_result.symbol;

  // * Argument Packer Fields *
  
  createFields(
    *(Driver<Sage> *)this,
    result->arguments_packer,
    param_to_field_map, coef_to_field_map, data_to_field_map,
    kernel->getArguments(),
    file_id
  );

  // * Argument Packer Ctor. Declaration *

  SgFunctionParameterList * ctor_param_list = ::KLT::Core::createParameterList(
    kernel->getArguments().parameters,
    kernel->getArguments().coefficients,
    kernel->getArguments().datas,
    SgTypeModifier::e_default, "_"
  );

  MultiFileBuilder::Sage<SgMemberFunctionDeclaration>::object_desc_t ctor_arguments_packer_desc(
    arguments_packer_name.str(),
    SageBuilder::buildVoidType(),
    ctor_param_list,
    result->arguments_packer,
    file_id,
    false,
    false,
    true,
    false,
    true
  );

  MultiFileBuilder::Sage<SgMemberFunctionDeclaration>::build_result_t ctor_arguments_packer_result = 
    ((Driver<Sage> *)this)->build<SgMemberFunctionDeclaration>(ctor_arguments_packer_desc);

  SgMemberFunctionDeclaration * ctor_defn_decl = ctor_arguments_packer_result.symbol->get_declaration();
  assert(ctor_defn_decl != NULL);
  ctor_defn_decl = isSgMemberFunctionDeclaration(ctor_defn_decl->get_definingDeclaration());
  assert(ctor_defn_decl != NULL);

  // * Constructor Init. List *

  createConstructorInitializerList(*(Driver<Sage> *)this, ctor_defn_decl, kernel->getArguments());

  // * Function Declaration *

  std::ostringstream kernel_function_name;
  kernel_function_name << "kernel_function_" << kernel->id << "_" << kernel;

  SgFunctionParameterList * kernel_function_params = SageBuilder::buildFunctionParameterList(
    SageBuilder::buildInitializedName("arg_", SageBuilder::buildPointerType(SageBuilder::buildVoidType()))
  );

  MultiFileBuilder::Sage<SgFunctionDeclaration>::object_desc_t kernel_function_desc(
    kernel_function_name.str(),
    SageBuilder::buildVoidType(),
    kernel_function_params,
    NULL,
    file_id
  );

  MultiFileBuilder::Sage<SgFunctionDeclaration>::build_result_t kernel_result = 
    ((Driver<Sage> *)this)->build<SgFunctionDeclaration>(kernel_function_desc);

  result->kernel = kernel_result.symbol;

  // * Local Declarations *

  ::KLT::Sequential::Kernel::local_symbol_maps_t local_symbol_maps;

  SgBasicBlock * body = createLocalDeclarations(
    *(Driver<Sage> *)this,
    kernel_result.definition,
    result->arguments_packer,
    local_symbol_maps,
    param_to_field_map, coef_to_field_map, data_to_field_map,
    kernel->getArguments(), iterators
  );

  desc.iteration_map->generateDimensions(local_symbol_maps, result->dimensions);

  body = desc.iteration_map->generateBodyForMappedIterationDomain(local_symbol_maps, body,  result->dimensions);

  // * Create the content of the loop nest *

  for (it_body_branch = desc.loop_mapping->body.begin(); it_body_branch != desc.loop_mapping->body.end(); it_body_branch++) {
    SgStatement * stmt = ::KLT::Core::generateStatement(
      *it_body_branch, 
      local_symbol_maps.parameters,
      local_symbol_maps.coefficients,
      local_symbol_maps.datas,
      local_symbol_maps.iterators,
      true, // generate statement for the whole tree not only the top node
      true  // flatten array reference
    );
    SageInterface::appendStatement(stmt, body);
  }

  return result;
}

}
