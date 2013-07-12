
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
  ::KLT::OpenCL::WorkSizeShape * work_size_shape,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & param_to_args_map,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & coef_to_args_map,
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> & data_to_args_map,
  std::map<SgVariableSymbol *, SgVariableSymbol *>   & iter_to_local_map,
  std::vector<SgVariableSymbol *> & global_size_coords,
  std::vector<SgVariableSymbol *> & local_size_coords,
  const std::list<SgVariableSymbol *>   & params,
  const std::list<SgVariableSymbol *>   & coefs,
  const std::list< ::KLT::Core::Data *> & datas,
  const std::set<SgVariableSymbol *>   & iterators
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

  // * Build local declaration for coordinate in the work sharing grid *

  global_size_coords.resize(work_size_shape->getNumberDimensions());
  if (work_size_shape->haveLocalDimensions())
    local_size_coords.resize(work_size_shape->getNumberDimensions());

  SgFunctionSymbol * ocl_get_global_id_sym = SageInterface::lookupFunctionSymbolInParentScopes("get_global_id", kernel_body);
  assert(ocl_get_global_id_sym != NULL);

  SgFunctionSymbol * ocl_get_local_id_sym = SageInterface::lookupFunctionSymbolInParentScopes("get_local_id", kernel_body);
  assert(ocl_get_local_id_sym != NULL);

  SgTypedefSymbol * size_t_sym = SageInterface::lookupTypedefSymbolInParentScopes("size_t", kernel_body);
  assert(size_t_sym != NULL);
  SgType * size_type = size_t_sym->get_type();
  assert(size_type != NULL);
  
  for (unsigned int i = 0; i < work_size_shape->getNumberDimensions(); i++) {
    {
      std::ostringstream name_global_size;
      name_global_size << "global_id_" << i;

      SgInitializer * init_global_size = SageBuilder::buildAssignInitializer(
        SageBuilder::buildFunctionCallExp(ocl_get_global_id_sym,
        SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i)))
      );

      SgVariableDeclaration * global_size_decl = SageBuilder::buildVariableDeclaration(
        name_global_size.str(),
        size_type,
        init_global_size,
        kernel_body
      );
      SageInterface::appendStatement(global_size_decl, kernel_body);

      SgVariableSymbol * global_size_sym = kernel_body->lookup_variable_symbol(name_global_size.str());
      assert(global_size_sym != NULL);

      global_size_coords[i] = global_size_sym;
    }

    if (work_size_shape->haveLocalDimensions()) {
      std::ostringstream name_local_size;
      name_local_size << "local_id_" << i;

      SgInitializer * init_local_size = SageBuilder::buildAssignInitializer(
        SageBuilder::buildFunctionCallExp(ocl_get_local_id_sym,
        SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i)))
      );

      SgVariableDeclaration * local_size_decl = SageBuilder::buildVariableDeclaration(
        name_local_size.str(),
        size_type,
        init_local_size,
        kernel_body
      );
      SageInterface::appendStatement(local_size_decl, kernel_body);

      SgVariableSymbol * local_size_sym = kernel_body->lookup_variable_symbol(name_local_size.str());
      assert(local_size_sym != NULL);
    }
  }

  // * Build local iterator *

  std::set<SgVariableSymbol *>::const_iterator it_iterator;
  for (it_iterator = iterators.begin(); it_iterator != iterators.end(); it_iterator++) {
    SgVariableSymbol * iterator_sym = *it_iterator;
    std::string iterator_name = iterator_sym->get_name().getString();
    SgType * iterator_type = iterator_sym->get_type();

    assert(isSgTypeUnsignedLong(iterator_type) != NULL); // FIXME artificial constraint for debug

    SgVariableDeclaration * iterator_decl = SageBuilder::buildVariableDeclaration(
      "it_" + iterator_name,
      iterator_type,
      NULL,
      kernel_body
    );
    SageInterface::appendStatement(iterator_decl, kernel_body);

    SgVariableSymbol * local_sym = kernel_body->lookup_variable_symbol("it_" + iterator_name);
    assert(local_sym != NULL);
    iter_to_local_map.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(iterator_sym, local_sym));
  }

  // ***************

  return kernel_body;
}

KLT< ::KLT::OpenCL::Kernel>::object_desc_t::object_desc_t(::KLT::OpenCL::Kernel * kernel_, ::KLT::OpenCL::WorkSizeShape * work_size_shape_, unsigned long file_id_) :
  kernel(kernel_),
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

  std::map<SgVariableSymbol *, SgVariableSymbol *>   param_to_args_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   coef_to_args_map;
  std::map< ::KLT::Core::Data *, SgVariableSymbol *> data_to_args_map;
  std::map<SgVariableSymbol *, SgVariableSymbol *>   iter_to_local_map;

  std::vector<SgVariableSymbol *> global_size_coords;
  std::vector<SgVariableSymbol *> local_size_coords;

  result->number_dims = desc.work_size_shape->getNumberDimensions();
  result->global_work_size = desc.work_size_shape->getGlobalDimensions();
  result->local_work_size = desc.work_size_shape->getLocalDimensions();
  result->have_local_work_size = desc.work_size_shape->haveLocalDimensions();

  SgBasicBlock * body = createLocalDeclarations(
    *(Driver<Sage> *)this,
    kernel_result.definition,
    desc.work_size_shape,
    param_to_args_map, coef_to_args_map, data_to_args_map, iter_to_local_map,
    global_size_coords, local_size_coords,
    params, coefs, datas, iterators
  );

  // * Create code to handle nested loops (mapped on the work sharing grid) *

  std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;

  for (it_nested_loop = nested_loops.begin(); it_nested_loop != nested_loops.end(); it_nested_loop++) {
    std::pair<SgExpression *, SgExpression *> iterator_bounds = desc.work_size_shape->generateIteratorBounds(
      global_size_coords, local_size_coords, (*it_nested_loop)->iterator
    );

    assert(iterator_bounds.first != NULL);

    std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(iterator_bounds.first);
    if (iterator_bounds.second != NULL) {
      std::vector<SgVarRefExp *> var_refs_ = SageInterface::querySubTree<SgVarRefExp>(iterator_bounds.second);
      var_refs.insert(var_refs.end(), var_refs_.begin(), var_refs_.end());
    }
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
      SgVarRefExp * var_ref = *it_var_ref;
      SgVariableSymbol * var_sym = var_ref->get_symbol();

      if (std::find(global_size_coords.begin(), global_size_coords.end(), var_sym) != global_size_coords.end()) continue;
      if (std::find(local_size_coords.begin(),  local_size_coords.end(),  var_sym) != local_size_coords.end())  continue;

      SgVariableSymbol * local_sym = NULL;

      it_sym_to_local = param_to_args_map.find(var_sym);
      if (it_sym_to_local != param_to_args_map.end())
        local_sym = it_sym_to_local->second;

      it_sym_to_local = iter_to_local_map.find(var_sym);
      if (it_sym_to_local != iter_to_local_map.end()) {
        assert(local_sym == NULL);

        local_sym = it_sym_to_local->second;
      }

      assert(local_sym != NULL); // implies VarRef to an unknown variable symbol

      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym));
    }

    if (iterator_bounds.second == NULL) {
      it_sym_to_local = iter_to_local_map.find((*it_nested_loop)->iterator);
      assert(it_sym_to_local != iter_to_local_map.end());
      SgVariableSymbol * local_iterator = it_sym_to_local->second;
      
      SgExprStatement * expr_stmt = SageBuilder::buildAssignStatement(
        SageBuilder::buildVarRefExp(local_iterator),
        iterator_bounds.first
      );
      SageInterface::appendStatement(expr_stmt, body);
    }
    else {
      assert(false); // TODO case of an interval 
    }
  }

  // * Create the content of the loop nest *

  for (it_body_branch = body_branches.begin(); it_body_branch != body_branches.end(); it_body_branch++) {
    SgStatement * stmt = ::KLT::Core::generateStatement(
      *it_body_branch, 
      param_to_args_map,
      coef_to_args_map,
      data_to_args_map,
      iter_to_local_map,
      true, // generate statement for the whole tree not only the top node
      true  // flatten array reference
    );
    SageInterface::appendStatement(stmt, body);
  }

  return result;
}

}
