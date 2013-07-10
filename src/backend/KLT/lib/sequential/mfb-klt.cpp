
#include "KLT/Sequential/mfb-klt.hpp"

#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"

#include "KLT/Sequential/kernel.hpp"

#include "MFB/Sage/function-declaration.hpp"
#include "MFB/Sage/member-function-declaration.hpp"
#include "MFB/Sage/class-declaration.hpp"

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

  SgCtorInitializerList * ctor_init_list = new SgCtorInitializerList();
  SgFunctionParameterList * params = SageBuilder::buildFunctionParameterList();

  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  for (it_var_sym = params_args.begin(); it_var_sym != params_args.end(); it_var_sym++) {
    // TODO create fields
    // TODO fill params
    // TODO fill ctor_init_list : param -> fields
  }
  for (it_var_sym = coefs_args.begin(); it_var_sym != coefs_args.end(); it_var_sym++) {
    // TODO create fields
    // TODO fill params
    // TODO fill ctor_init_list : param -> fields
  }

  std::list< ::KLT::Core::Data *>::const_iterator it_data;
  for (it_data = data_args.begin(); it_data != data_args.end(); it_data++) {
    // TODO create fields
    // TODO fill params
    // TODO fill ctor_init_list : param -> fields
  }

  MultiFileBuilder::Sage<SgMemberFunctionDeclaration>::object_desc_t ctor_arguments_packer_desc(
    arguments_packer_name.str(),
    SageBuilder::buildVoidType(),
    params,
    arguments_packer_result.symbol,
    file_id,
    false,
    false,
    true,
    false,
    true
  );

  MultiFileBuilder::Sage<SgMemberFunctionDeclaration>::build_result_t ctor_arguments_packer_result = 
    ((Driver<Sage> *)this)->build<SgMemberFunctionDeclaration>(ctor_arguments_packer_desc);

  ctor_arguments_packer_result.symbol->get_declaration()->set_CtorInitializerList(ctor_init_list);
    ctor_init_list->set_parent(ctor_arguments_packer_result.symbol->get_declaration());
    ctor_init_list->set_definingDeclaration(ctor_init_list);
    ctor_init_list->set_firstNondefiningDeclaration(ctor_init_list);

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

  {
    SgType * arg_ptr_type = SageBuilder::buildPointerType(arguments_packer_result.symbol->get_type());

    SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(
      "arg",
      arg_ptr_type,
      SageBuilder::buildAssignInitializer(SageBuilder::buildCastExp(
        SageBuilder::buildVarRefExp(kernel_arg_sym),
        arg_ptr_type
      )),
      kernel_body
    );

    // TODO prepare translation map for symbols...

    std::list<SgVariableSymbol *>::const_iterator it_var_sym;
    for (it_var_sym = params_args.begin(); it_var_sym != params_args.end(); it_var_sym++) {
      // TODO create local copy
    }
    for (it_var_sym = coefs_args.begin(); it_var_sym != coefs_args.end(); it_var_sym++) {
      // TODO create local copy
    }

    std::list< ::KLT::Core::Data *>::const_iterator it_data;
    for (it_data = data_args.begin(); it_data != data_args.end(); it_data++) {
      // TODO create local copy
    }

//  SgBasicBlock * body = kernel_body;

    std::list< ::KLT::Core::LoopTrees::loop_t *>::const_iterator it_nested_loop;
    for (it_nested_loop = nested_loops.begin(); it_nested_loop != nested_loops.end(); it_nested_loop++) {
      // TODO generate sequential loop nest (with current symbol)
    }

    std::list< ::KLT::Core::LoopTrees::node_t *>::const_iterator it_body_branch;
    for (it_body_branch = body_branches.begin(); it_body_branch != body_branches.end(); it_body_branch++) {
      // TODO generate inner statements
      // TODO variables (symbol) reassignement
    }
  }

  return result;
}

}
