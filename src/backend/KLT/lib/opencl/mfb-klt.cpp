
#include "KLT/OpenCL/mfb-klt.hpp"

#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"

#include "KLT/OpenCL/kernel.hpp"

#include "KLT/OpenCL/work-size-shaper.hpp"

#include "MFB/Sage/function-declaration.hpp"

#include <cassert>

namespace MultiFileBuilder {

SgBasicBlock * createLocalDeclarations(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & param_to_args_map,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & coef_to_args_map,
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> & data_to_args_map,
  const std::list<SgVariableSymbol *>   & params,
  const std::list<SgVariableSymbol *>   & coefs,
  const std::list< ::KLT::Core::Data *> & datas
) {
  std::list<SgVariableSymbol *>::const_iterator it_var_sym;
  std::list< ::KLT::Core::Data *>::const_iterator it_data;

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_param_to_field;
  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator   it_coef_to_field;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *>::const_iterator it_data_to_field;
  
  // * Definition *

  SgBasicBlock * kernel_body = kernel_defn->get_body();
  assert(kernel_body != NULL);

  // * Lookup parameter symbols *

  for (it_var_sym = params.begin(); it_var_sym != params.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("param_" + param_name);
    assert(arg_sym != NULL);

    param_to_args_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, arg_sym));
  }

  // * Lookup coeficient symbols *

  for (it_var_sym = coefs.begin(); it_var_sym != coefs.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("coef_" + coef_name);
    assert(arg_sym != NULL);

    coef_to_args_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(coef_sym, arg_sym));
  }

  // * Lookup data symbols *

  for (it_data = datas.begin(); it_data != datas.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();;
    std::string data_name = data_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("data_" + data_name);
    assert(arg_sym != NULL);

    data_to_args_map.insert(std::pair< ::KLT::Core::Data *, SgVariableSymbol *>(data, arg_sym));
  }

  return kernel_body;
}

KLT< ::KLT::OpenCL::Kernel>::object_desc_t::object_desc_t(
  ::KLT::OpenCL::Kernel * kernel_,
  ::KLT::OpenCL::Kernel::loop_distribution_t * loop_distribution_,
  ::KLT::Core::IterationMap< ::KLT::OpenCL::Kernel> * iteration_map_,
  unsigned long file_id_
) :
  kernel(kernel_),
  loop_distribution(loop_distribution_),
  work_size_shape(work_size_shape_),
  file_id(file_id_)
{}

template <>
KLT< ::KLT::OpenCL::Kernel>::build_result_t Driver<KLT>::build< ::KLT::OpenCL::Kernel>(const KLT< ::KLT::OpenCL::Kernel>::object_desc_t & desc) {
  KLT< ::KLT::OpenCL::Kernel>::build_result_t result = new ::KLT::OpenCL::Kernel::a_kernel();

  ::KLT::OpenCL::Kernel * kernel = desc.kernel;
  unsigned long file_id = desc.file_id;

  const std::list<SgVariableSymbol *>   & params = kernel->getParametersArguments();
  const std::list<SgVariableSymbol *>   & coefs  = kernel->getCoefficientsArguments();
  const std::list< ::KLT::Core::Data *> & datas   = kernel->getDatasArguments();

  const std::list< ::KLT::Core::LoopTrees::loop_t *>  & nested_loops  = kernel->getPerfectlyNestedLoops();
  const std::list< ::KLT::Core::LoopTrees::node_t *>  & body_branches = kernel->getBodyBranches();

  std::map<SgVariableSymbol *, SgVariableSymbol *>   param_to_field_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   coef_to_field_map;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> data_to_field_map;

  std::list< ::KLT::Core::LoopTrees::loop_t *>::const_iterator it_nested_loop;
  std::list< ::KLT::Core::LoopTrees::node_t *>::const_iterator it_body_branch;

  // * Build Iterator Set *

  std::set<SgVariableSymbol *> iterators;
  if (nested_loops.size() > 0)
    collectIteratorSymbols(nested_loops.front(), iterators);
  else
    for (it_body_branch = body_branches.begin(); it_body_branch != body_branches.end(); it_body_branch++)
      collectIteratorSymbols(*it_body_branch, iterators);

  // * Function Declaration *

  std::ostringstream kernel_function_name;
  kernel_function_name << "kernel_function_" << kernel->id << "_" << kernel;
  result->kernel_name = kernel_function_name.str();

  SgFunctionParameterList * kernel_function_params = ::KLT::Core::createParameterList(params, coefs, datas, SgTypeModifier::e_ocl_global__, "");

  MultiFileBuilder::Sage<SgFunctionDeclaration>::object_desc_t kernel_function_desc(
    result->kernel_name,
    SageBuilder::buildVoidType(),
    kernel_function_params,
    NULL,
    file_id
  );

  MultiFileBuilder::Sage<SgFunctionDeclaration>::build_result_t kernel_result = 
    ((Driver<Sage> *)this)->build<SgFunctionDeclaration>(kernel_function_desc);

  {
    SgFunctionDeclaration * kernel_decl = kernel_result.symbol->get_declaration();
    assert(kernel_decl != NULL);

    SgFunctionDeclaration * first_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_firstNondefiningDeclaration());
    assert(first_kernel_decl != NULL);
    first_kernel_decl->get_functionModifier().setOpenclKernel();

    SgFunctionDeclaration * defn_kernel_decl = isSgFunctionDeclaration(kernel_decl->get_definingDeclaration());
    assert(defn_kernel_decl != NULL);
    defn_kernel_decl->get_functionModifier().setOpenclKernel();
  }
  // * Local Declarations *


  Kernel::argument_symbol_maps_t argument_symbol_maps;

  SgBasicBlock * body = createLocalDeclarations(
    *(Driver<Sage> *)this,
    kernel_result.definition,
    argument_symbol_maps.param_to_args, argument_symbol_maps.coef_to_args, argument_symbol_maps.data_to_args,
    params, coefs, datas
  );

  // * Create code to handle nested loops (mapped on the work sharing grid) *

  std::map<SgVariableSymbol *, SgVariableSymbol *> iter_to_local;

  result->number_dims = desc.work_size_shape->getNumberDimensions();
  result->global_work_size = desc.work_size_shape->getGlobalDimensions();
  result->local_work_size = desc.work_size_shape->getLocalDimensions();
  result->have_local_work_size = desc.work_size_shape->haveLocalDimensions();

  body = desc.work_size_shape->generateBodyForMappedIterationDomain(argument_symbol_maps, body, iter_to_local);

  // * Create the content of the loop nest *

  for (it_body_branch = body_branches.begin(); it_body_branch != body_branches.end(); it_body_branch++) {
    SgStatement * stmt = ::KLT::Core::generateStatement(
      *it_body_branch, 
      argument_symbol_maps.param_to_args,
      argument_symbol_maps.coef_to_args,
      argument_symbol_maps.data_to_args,
      iter_to_local,
      true, // generate statement for the whole tree not only the top node
      true  // flatten array reference
    );
    SageInterface::appendStatement(stmt, body);
  }

  return result;
}

}
