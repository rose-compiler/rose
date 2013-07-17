
#include "KLT/OpenCL/mfb-klt.hpp"

#include "KLT/Core/data.hpp"
#include "KLT/Core/loop-trees.hpp"

#include "KLT/OpenCL/kernel.hpp"

#include "KLT/Core/iteration-mapper.hpp"

#include "MFB/Sage/function-declaration.hpp"

#include <cassert>

namespace MultiFileBuilder {

SgBasicBlock * createLocalDeclarations(
  Driver<Sage> & driver,
  SgFunctionDefinition * kernel_defn,
  ::KLT::OpenCL::Kernel::local_symbol_maps_t & local_symbol_maps,
  const std::set<SgVariableSymbol *> & iterators,
  const ::KLT::OpenCL::Kernel::arguments_t & arguments
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

  for (it_var_sym = arguments.parameters.begin(); it_var_sym != arguments.parameters.end(); it_var_sym++) {
    SgVariableSymbol * param_sym = *it_var_sym;
    std::string param_name = param_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("param_" + param_name);
    assert(arg_sym != NULL);

    local_symbol_maps.parameters.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(param_sym, arg_sym));
  }

  // * Lookup coeficient symbols *

  for (it_var_sym = arguments.coefficients.begin(); it_var_sym != arguments.coefficients.end(); it_var_sym++) {
    SgVariableSymbol * coef_sym = *it_var_sym;
    std::string coef_name = coef_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("coef_" + coef_name);
    assert(arg_sym != NULL);

    local_symbol_maps.coefficients.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(coef_sym, arg_sym));
  }

  // * Lookup data symbols *

  for (it_data = arguments.datas.begin(); it_data != arguments.datas.end(); it_data++) {
    ::KLT::Core::Data * data = *it_data;
    SgVariableSymbol * data_sym = data->getVariableSymbol();;
    std::string data_name = data_sym->get_name().getString();

    SgVariableSymbol * arg_sym = kernel_defn->lookup_variable_symbol("data_" + data_name);
    assert(arg_sym != NULL);

    local_symbol_maps.datas.insert(std::pair< ::KLT::Core::Data *, SgVariableSymbol *>(data, arg_sym));
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

  return kernel_body;
}

KLT< ::KLT::OpenCL::Kernel>::object_desc_t::object_desc_t(
  ::KLT::OpenCL::Kernel * kernel_,
  ::KLT::OpenCL::Kernel::loop_mapping_t * loop_mapping_,
  ::KLT::Core::IterationMap< ::KLT::OpenCL::Kernel> * iteration_map_,
  unsigned long file_id_
) :
  kernel(kernel_),
  loop_mapping(loop_mapping_),
  iteration_map(iteration_map_),
  file_id(file_id_)
{}

template <>
KLT< ::KLT::OpenCL::Kernel>::build_result_t Driver<KLT>::build< ::KLT::OpenCL::Kernel>(const KLT< ::KLT::OpenCL::Kernel>::object_desc_t & desc) {
  KLT< ::KLT::OpenCL::Kernel>::build_result_t result = new ::KLT::OpenCL::Kernel::a_kernel();

  std::map<SgVariableSymbol *, SgVariableSymbol *>   param_to_field_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   coef_to_field_map;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> data_to_field_map;

  std::list< ::KLT::Core::LoopTrees::loop_t *>::const_iterator it_nested_loop;
  std::list< ::KLT::Core::LoopTrees::node_t *>::const_iterator it_body_branch;

  // * Build Iterator Set *

  std::set<SgVariableSymbol *> iterators;
    collectIteratorSymbols(desc.kernel->getRoot(), iterators);

  // * Function Declaration *

  std::ostringstream kernel_function_name;
  kernel_function_name << "kernel_function_" << desc.kernel->id << "_" << desc.kernel;
  result->kernel_name = kernel_function_name.str();

  SgFunctionParameterList * kernel_function_params = ::KLT::Core::createParameterList(
    desc.kernel->getArguments().parameters,
    desc.kernel->getArguments().coefficients,
    desc.kernel->getArguments().datas,
    SgTypeModifier::e_ocl_global__, ""
  );

  MultiFileBuilder::Sage<SgFunctionDeclaration>::object_desc_t kernel_function_desc(
    result->kernel_name,
    SageBuilder::buildVoidType(),
    kernel_function_params,
    NULL,
    desc.file_id
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


  ::KLT::OpenCL::Kernel::local_symbol_maps_t local_symbol_maps;

  SgBasicBlock * body = createLocalDeclarations(
    *(Driver<Sage> *)this,
    kernel_result.definition,
    local_symbol_maps,
    iterators,
    desc.kernel->getArguments()
  );

  // * Create code to handle nested loops (mapped on the work sharing grid) *

  std::map<SgVariableSymbol *, SgVariableSymbol *> iter_to_local;

  desc.iteration_map->generateDimensions(local_symbol_maps, result->dimensions);

  body = desc.iteration_map->generateBodyForMappedIterationDomain(local_symbol_maps, body, result->dimensions);

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
