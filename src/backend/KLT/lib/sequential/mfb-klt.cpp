
#include "KLT/Sequential/mfb-klt.hpp"

#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"

#include "KLT/Sequential/kernel.hpp"

#include "MFB/Sage/function-declaration.hpp"
#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"
#include "MFB/Sage/variable-declaration.hpp"

namespace MultiFileBuilder {

KLT< ::KLT::Sequential::Kernel>::object_desc_t::object_desc_t(::KLT::Sequential::Kernel * kernel_, unsigned long file_id_) :
  kernel(kernel_),
  file_id(file_id_)
{}

template <>
KLT< ::KLT::Sequential::Kernel>::build_result_t Driver<KLT>::build< ::KLT::Sequential::Kernel>(const KLT< ::KLT::Sequential::Kernel>::object_desc_t & desc) {
  KLT< ::KLT::Sequential::Kernel>::build_result_t result;

  ::KLT::Sequential::Kernel * kernel = desc.kernel;
  unsigned long file_id = desc.file_id;

  const std::list<SgVariableSymbol *>   & params_args = kernel->getParametersArguments();
  const std::list<SgVariableSymbol *>   & coefs_args  = kernel->getCoefficientsArguments();
  const std::list< ::KLT::Core::Data *> & data_args   = kernel->getDatasArguments();

  const std::list< ::KLT::Core::LoopTrees::loop_t *>  & nested_loops  = kernel->getPerfectlyNestedLoops();
  const std::list< ::KLT::Core::LoopTrees::node_t *>  & body_branches = kernel->getBodyBranches();

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Core::Data *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>   param_to_field_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   coef_to_field_map;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> data_to_field_map;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_coef_to_field;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *>::const_iterator it_data_to_field;

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

  result.arguments_packer = arguments_packer_result.symbol;

  SgFunctionParameterList * params = SageBuilder::buildFunctionParameterList();

  for (it_var_sym = params_args.begin(); it_var_sym != params_args.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type =  param_sym->get_type();

    // Create fields

    MultiFileBuilder::Sage<SgVariableDeclaration>::object_desc_t param_desc("param_" + param_name, param_type, NULL, result.arguments_packer, file_id);
    MultiFileBuilder::Sage<SgVariableDeclaration>::build_result_t param_result = ((Driver<Sage> *)this)->build<SgVariableDeclaration>(param_desc);

    param_to_field_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, param_result.symbol));

    // Fill Parameter List

    params->append_arg(SageBuilder::buildInitializedName("param_" + param_name + "_", param_type, NULL));
  }
  for (it_var_sym = coefs_args.begin(); it_var_sym != coefs_args.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();
    SgType * coef_type = coef_sym->get_type();

    // Create fields

    MultiFileBuilder::Sage<SgVariableDeclaration>::object_desc_t coef_desc("coef_" + coef_name, coef_type, NULL, result.arguments_packer, file_id);
    MultiFileBuilder::Sage<SgVariableDeclaration>::build_result_t coef_result = ((Driver<Sage> *)this)->build<SgVariableDeclaration>(coef_desc);

    coef_to_field_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(coef_sym, coef_result.symbol));

    // Fill Parameter List

    params->append_arg(SageBuilder::buildInitializedName("coef_" + coef_name + "_", coef_type, NULL));
  }

  for (it_data = data_args.begin(); it_data != data_args.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    // Create fields

    MultiFileBuilder::Sage<SgVariableDeclaration>::object_desc_t data_desc("data_" + data_name, field_type, NULL, result.arguments_packer, file_id);
    MultiFileBuilder::Sage<SgVariableDeclaration>::build_result_t data_result = ((Driver<Sage> *)this)->build<SgVariableDeclaration>(data_desc);

    data_to_field_map.insert(std::pair< ::KLT::Core::Data *, SgVariableSymbol *>(data, data_result.symbol));

    // Fill Parameter List

    params->append_arg(SageBuilder::buildInitializedName("data_" + data_name + "_", field_type, NULL));
  }

  MultiFileBuilder::Sage<SgMemberFunctionDeclaration>::object_desc_t ctor_arguments_packer_desc(
    arguments_packer_name.str(),
    SageBuilder::buildVoidType(),
    params,
    result.arguments_packer,
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

  SgCtorInitializerList * ctor_init_list = ctor_defn_decl->get_CtorInitializerList();
  if (ctor_init_list == NULL) {
    ctor_init_list = new SgCtorInitializerList();

    ctor_defn_decl->set_CtorInitializerList(ctor_init_list);

    ctor_init_list->set_parent(ctor_arguments_packer_result.symbol->get_declaration());
    ctor_init_list->set_definingDeclaration(ctor_init_list);
    ctor_init_list->set_firstNondefiningDeclaration(ctor_init_list);
  }

  for (it_var_sym = params_args.begin(); it_var_sym != params_args.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type =  param_sym->get_type();

    SgVariableSymbol * param_param_sym = ctor_arguments_packer_result.definition->lookup_variable_symbol("param_" + param_name + "_");
    assert(param_param_sym != NULL);

    ctor_init_list->append_ctor_initializer(SageBuilder::buildInitializedName(
      "param_" + param_name, param_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(param_param_sym))
    ));
  }
  for (it_var_sym = coefs_args.begin(); it_var_sym != coefs_args.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();
    SgType * coef_type = coef_sym->get_type();

    SgVariableSymbol * param_coef_sym = ctor_arguments_packer_result.definition->lookup_variable_symbol("coef_" + coef_name + "_");
    assert(param_coef_sym != NULL);

    ctor_init_list->append_ctor_initializer(SageBuilder::buildInitializedName(
      "coef_" + coef_name, coef_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(param_coef_sym))
    ));
  }
  for (it_data = data_args.begin(); it_data != data_args.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();
    std::string data_name = data_sym->get_name();

    SgType * base_type = data->getBaseType();
    SgType * field_type = SageBuilder::buildPointerType(base_type);

    SgVariableSymbol * param_data_sym = ctor_arguments_packer_result.definition->lookup_variable_symbol("data_" + data_name + "_");
    assert(param_data_sym != NULL);

    ctor_init_list->append_ctor_initializer(SageBuilder::buildInitializedName(
      "data_" + data_name, field_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildVarRefExp(param_data_sym))
    ));
  }

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

  result.kernel = kernel_result.symbol;

  SgFunctionDefinition * kernel_defn = kernel_result.definition;
  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);
  SgVariableSymbol * kernel_arg_sym = kernel_defn->lookup_variable_symbol("arg_");
  assert(kernel_arg_sym != NULL);

  SgType * arg_ptr_type = SageBuilder::buildPointerType(arguments_packer_result.symbol->get_type());

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

  std::map<SgVariableSymbol *, SgVariableSymbol *>   param_to_local_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   coef_to_local_map;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> data_to_local_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   iter_to_local_map;

  for (it_var_sym = params_args.begin(); it_var_sym != params_args.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();
    SgType * param_type = param_sym->get_type();

    it_param_to_field = param_to_field_map.find(param_sym);
    assert(it_param_to_field !=  param_to_field_map.end());
    SgVariableSymbol * field_sym = it_param_to_field->second;

    SgVariableDeclaration * param_decl = SageBuilder::buildVariableDeclaration(
      "local_" + param_name,
      param_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(arg_cast_sym),
        SageBuilder::buildVarRefExp(field_sym)
      )),
      kernel_body
    );
    SageInterface::appendStatement(param_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_" + param_name);
    assert(local_sym != NULL);
    param_to_local_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, local_sym));
  }

  for (it_var_sym = coefs_args.begin(); it_var_sym != coefs_args.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();
    SgType * coef_type = coef_sym->get_type();

    it_coef_to_field = coef_to_field_map.find(coef_sym);
    assert(it_coef_to_field != coef_to_field_map.end());
    SgVariableSymbol * field_sym = it_coef_to_field->second;

    SgVariableDeclaration * coef_decl = SageBuilder::buildVariableDeclaration(
      "local_" + coef_name,
      coef_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildDotExp(
        SageBuilder::buildVarRefExp(arg_cast_sym),
        SageBuilder::buildVarRefExp(field_sym)
      )),
      kernel_body
    );
    SageInterface::appendStatement(coef_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_" + coef_name);
    assert(local_sym != NULL);
    coef_to_local_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(coef_sym, local_sym));
  }

  for (it_data = data_args.begin(); it_data != data_args.end(); it_data++) {
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
    data_to_local_map.insert(std::pair< ::KLT::Core::Data *, SgVariableSymbol *>(data, local_sym));
  }

  std::list< ::KLT::Core::LoopTrees::loop_t *>::const_iterator it_nested_loop;
  std::list< ::KLT::Core::LoopTrees::node_t *>::const_iterator it_body_branch;

  std::set<SgVariableSymbol *> iterators;
  if (nested_loops.size() > 0)
    collectIteratorSymbols(nested_loops.front(), iterators);
  else
    for (it_body_branch = body_branches.begin(); it_body_branch != body_branches.end(); it_body_branch++)
      collectIteratorSymbols(*it_body_branch, iterators);

  std::set<SgVariableSymbol *>::const_iterator it_iterator;
  for (it_iterator = iterators.begin(); it_iterator != iterators.end(); it_iterator++) {
    SgVariableSymbol * iterator_sym = *it_iterator;
    std::string iterator_name = iterator_sym->get_name().getString();
    SgType * iterator_type = iterator_sym->get_type();

    assert(isSgTypeUnsignedLong(iterator_type) != NULL); // FIXME artificial constraint for debug

    SgVariableDeclaration * iterator_decl = SageBuilder::buildVariableDeclaration(
      "local_it_" + iterator_name,
      iterator_type,
      NULL,
      kernel_body
    );
    SageInterface::appendStatement(iterator_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("local_it_" + iterator_name);
    assert(local_sym != NULL);
    iter_to_local_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iterator_sym, local_sym));
  }

  SgBasicBlock * body = kernel_body;

  for (it_nested_loop = nested_loops.begin(); it_nested_loop != nested_loops.end(); it_nested_loop++) {
    SgForStatement * for_stmt = isSgForStatement(::KLT::Core::generateStatement(
      *it_nested_loop,
      param_to_local_map,
      coef_to_local_map,
      data_to_local_map,
      iter_to_local_map,
      false, // We only want the given loop to be transform
      false // no in_depth and assume loop => no array flattening. No data dependent loop (Not in the perfectly nested loops)
     ));
    assert(for_stmt != NULL);
    SageInterface::appendStatement(for_stmt, body);
    body = isSgBasicBlock(for_stmt->get_loop_body());
    assert(body != NULL);
  }

  for (it_body_branch = body_branches.begin(); it_body_branch != body_branches.end(); it_body_branch++) {
    SgStatement * stmt = ::KLT::Core::generateStatement(
      *it_body_branch, 
      param_to_local_map,
      coef_to_local_map,
      data_to_local_map,
      iter_to_local_map,
      true, // generate statement for the whole tree not only the top node
      true  // flatten array reference
    );
    SageInterface::appendStatement(stmt, body);
  }

  return result;
}

}
