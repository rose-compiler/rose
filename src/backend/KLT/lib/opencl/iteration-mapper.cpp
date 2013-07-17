
#include "KLT/Core/iteration-mapper.hpp"

#include "KLT/OpenCL/kernel.hpp"

#include <cassert>

#include "sage3basic.h"

namespace KLT {

namespace Core {

template <>
std::pair<SgExpression *, SgExpression *> IterationMap<OpenCL::Kernel>::generateIteratorBounds(
  const OpenCL::Kernel::local_symbol_maps_t & local_symbol_maps,
  const OpenCL::Kernel::coordinate_symbols_t & coordinate_symbols,
  LoopTrees::loop_t * loop, unsigned int loop_depth
) const {
  std::pair<SgExpression *, SgExpression *> result(NULL, NULL);

  SgExpression * lower_bound = SageInterface::copyExpression(loop->lower_bound);
  std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(lower_bound);
  std::vector<SgVarRefExp *>::const_iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgVarRefExp * var_ref = *it_var_ref;
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_parameter = local_symbol_maps.parameters.find(var_ref->get_symbol());
    std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_iterator  = local_symbol_maps.iterators.find(var_ref->get_symbol());
    if (it_parameter != local_symbol_maps.parameters.end())
      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(it_parameter->second));
    else if (it_iterator != local_symbol_maps.iterators.end())
      SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(it_iterator->second));
    else assert(false);
  }

  result.first = SageBuilder::buildAddOp(SageBuilder::buildVarRefExp(coordinate_symbols.global_ids[loop_depth]), lower_bound);

  return result;
}

template <>
void IterationMap<OpenCL::Kernel>::generateDimensions(
  const OpenCL::Kernel::local_symbol_maps_t & local_symbol_maps,
  OpenCL::Kernel::dimensions_t & dimensions
) const {
  dimensions.have_local_work_size = false;

  dimensions.number_dims = 0;
  std::list<LoopTrees::loop_t *>::const_iterator it_loop;
  for (it_loop = p_loop_mapping->loop_nest.begin(); it_loop != p_loop_mapping->loop_nest.end(); it_loop++) {
    dimensions.global_work_size.push_back(SageBuilder::buildSubtractOp(
      SageInterface::copyExpression((*it_loop)->upper_bound),
      SageInterface::copyExpression((*it_loop)->lower_bound)
    ));
 // dimensions.local_work_size.insert(...);
    dimensions.number_dims++;
  }
}

template <>
void IterationMap<OpenCL::Kernel>::generateCoordinates(
  OpenCL::Kernel::coordinate_symbols_t & coordinate_symbols,
  SgBasicBlock * kernel_body,
  const OpenCL::Kernel::dimensions_t & dimensions
) const {
  coordinate_symbols.global_ids.resize(dimensions.number_dims);
  if (dimensions.have_local_work_size)
    coordinate_symbols.local_ids.resize(dimensions.number_dims);

  SgFunctionSymbol * ocl_get_global_id_sym = SageInterface::lookupFunctionSymbolInParentScopes("get_global_id", kernel_body);
  assert(ocl_get_global_id_sym != NULL);

  SgFunctionSymbol * ocl_get_local_id_sym = SageInterface::lookupFunctionSymbolInParentScopes("get_local_id", kernel_body);
  assert(ocl_get_local_id_sym != NULL);

  SgTypedefSymbol * size_t_sym = SageInterface::lookupTypedefSymbolInParentScopes("size_t", kernel_body);
  assert(size_t_sym != NULL);
  SgType * size_type = size_t_sym->get_type();
  assert(size_type != NULL); 

  for (unsigned int i = 0; i < dimensions.number_dims; i++) {
    {
      std::ostringstream name_global_size;
      name_global_size << "global_id_" << i;

      SgInitializer * init_global_size = SageBuilder::buildAssignInitializer(
        SageBuilder::buildFunctionCallExp(ocl_get_global_id_sym,
        SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i)))
      );

      SgVariableDeclaration * global_size_decl = SageBuilder::buildVariableDeclaration(name_global_size.str(), size_type, init_global_size, kernel_body);
      SageInterface::appendStatement(global_size_decl, kernel_body);

      SgVariableSymbol * global_size_sym = kernel_body->lookup_variable_symbol(name_global_size.str());
      assert(global_size_sym != NULL);

      coordinate_symbols.global_ids[i] = global_size_sym;
    }

    if (dimensions.have_local_work_size) {
      std::ostringstream name_local_size;
      name_local_size << "local_id_" << i;

      SgInitializer * init_local_size = SageBuilder::buildAssignInitializer(
        SageBuilder::buildFunctionCallExp(ocl_get_local_id_sym,
        SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i)))
      );

      SgVariableDeclaration * local_size_decl = SageBuilder::buildVariableDeclaration(name_local_size.str(), size_type, init_local_size, kernel_body);
      SageInterface::appendStatement(local_size_decl, kernel_body);

      SgVariableSymbol * local_size_sym = kernel_body->lookup_variable_symbol(name_local_size.str());
      assert(local_size_sym != NULL);

      coordinate_symbols.local_ids[i] = local_size_sym;
    }
  }
}

}

}
