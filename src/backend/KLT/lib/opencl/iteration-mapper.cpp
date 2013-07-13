
#include "KLT/OpenCL/iteration-mapper.hpp"

namespace KLT {

namespace Core {

std::pair<SgExpression *, SgExpression *> IterationMap<Kernel>::generateIteratorBounds(
  const Kernel::argument_symbol_maps_t & argument_symbol_maps,
  const Kernel::coordinate_symbols_t & coordinate_symbols,
  LoopTrees::loop_t * loop, unsigned int loop_depth
) const {
  assert(!p_have_local_dims); // not supported yet
  assert(p_loop_distribution->loop_nest.size() == p_number_dims); // we will map one loop per dim

  assert(coordinate_symbols.global_ids.size() == p_number_dims);
  assert(coordinate_symbols.locla_ids.size()  == 0);

  assert(loop != NULL);
  assert(loop_depth < p_loop_distribution->loop_nest.size());
  assert(loop->lower_bound != NULL);

  SgVariableSymbol * global_id_sym = coordinate_symbols.global_ids[loop_depth];

  SgExpression * iterator_value = SageBuilder::buildAddOp(
    SageBuilder::buildVarRefExp(global_id_sym),
    SageInterface::copyExpression(loop->lower_bound)
  );

  return std::pair<SgExpression *, SgExpression *>(iterator_value, NULL);
}

IterationMap<Kernel>::IterationMap(unsigned int number_dims, bool have_local_dims, Kernel::loop_distribution_t * loop_distribution) :
  p_number_dims(number_dims),
  p_global_dims(p_number_dims, NULL),
  p_local_dims(have_local_dims ? p_number_dims : 0, NULL),
  p_have_local_dims(have_local_dims),
  p_loop_distribution(loop_distribution)
{}

IterationMap<Kernel>::~IterationMap() {}

unsigned int IterationMap<Kernel>::getNumberDimensions() const { return p_number_dims; }

const std::vector<SgExpression *> & IterationMap<Kernel>::getGlobalDimensions() const { return p_global_dims; }

const std::vector<SgExpression *> & IterationMap<Kernel>::getLocalDimensions() const { return p_local_dims; }

bool IterationMap<Kernel>::haveLocalDimensions() const { return p_have_local_dims; }

SgBasicBlock * IterationMap<Kernel>::generateBodyForMappedIterationDomain(
  const Kernel::argument_symbol_maps_t & argument_symbol_maps,
  SgBasicBlock * kernel_body,
  std::map<SgVariableSymbol *, SgVariableSymbol *> & iter_to_local
) const {

  Kernel::coordinate_symbols_t coordinate_symbols;
  { // FIXME should be outlined
    coordinate_symbols.global_ids.resize(p_number_dims);
    if (p_have__dims)
      coordinate_symbols.local_ids.resize(p_number_dims);

    SgFunctionSymbol * ocl_get_global_id_sym = SageInterface::lookupFunctionSymbolInParentScopes("get_global_id", kernel_body);
    assert(ocl_get_global_id_sym != NULL);

    SgFunctionSymbol * ocl_get_local_id_sym = SageInterface::lookupFunctionSymbolInParentScopes("get_local_id", kernel_body);
    assert(ocl_get_local_id_sym != NULL);

    SgTypedefSymbol * size_t_sym = SageInterface::lookupTypedefSymbolInParentScopes("size_t", kernel_body);
    assert(size_t_sym != NULL);
    SgType * size_type = size_t_sym->get_type();
    assert(size_type != NULL);

    for (unsigned int i = 0; i < p_number_dims; i++) {
      {
        std::ostringstream name_global_id;
        name_global_id << "global_id_" << i;

        SgInitializer * init_global_id = SageBuilder::buildAssignInitializer(
          SageBuilder::buildFunctionCallExp(ocl_get_global_id_sym,
          SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i)))
        );

        SgVariableDeclaration * global_id_decl = SageBuilder::buildVariableDeclaration(
          name_global_id.str(),
          size_type,
          init_global_id,
          kernel_body
        );
        SageInterface::appendStatement(global_id_decl, kernel_body);

        SgVariableSymbol * global_id_sym = result->lookup_variable_symbol(name_global_id.str());
        assert(global_id_sym != NULL);
    
        coordinate_symbols.global_ids.insert(global_id_sym);
      }

      if (p_have_local_dims) {
        std::ostringstream name_local_id;
        name_local_id << "local_id_" << i;

        SgInitializer * init_local_id = SageBuilder::buildAssignInitializer(
          SageBuilder::buildFunctionCallExp(ocl_get_local_id_sym,
          SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i)))
        );

        SgVariableDeclaration * local_id_decl = SageBuilder::buildVariableDeclaration(
          name_local_id.str(),
          size_type,
          init_local_id,
          kernel_body
        );
        SageInterface::appendStatement(local_id_decl, kernel_body);

        SgVariableSymbol * local_id_sym = result->lookup_variable_symbol(name_local_id.str());
        assert(local_id_sym != NULL);

        coordinate_symbols.local_ids.insert(local_id_sym);
      }
    }
  }

  SgBasicBlock * result = kernel_body;

  const std::list<LoopTrees::loop_t *> & loop_nest = p_loop_distribution->loop_nest;
  std::list<LoopTrees::loop_t *>::const_iterator it_loop;
  unsigned int loop_depth = 0;
  for (it_loop = loop_nest.begin(); it_loop != loop_nest.end(); it_loop++) {
    LoopTrees::loop_t * loop = *it_loop;
    std::pair<SgExpression *, SgExpression *> bounds = generateIteratorBounds(argument_symbol_maps, coordinate_symbols, loop, loop_depth++);
    assert(bounds.first != NULL);

    { // TODO outline
      std::map<SgVariableSymbol *, SgVariableSymbol *>::const_iterator it_sym_to_local;
      std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(bounds.first);
      if (bounds.second != NULL) {
        std::vector<SgVarRefExp *> var_refs_ = SageInterface::querySubTree<SgVarRefExp>(bounds.second);
        var_refs.insert(var_refs.end(), var_refs_.begin(), var_refs_.end());
      }
      for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
        SgVarRefExp * var_ref = *it_var_ref;
        SgVariableSymbol * var_sym = var_ref->get_symbol();

        if (std::find(coordinate_symbols.global_ids.begin(), coordinate_symbols.global_ids.end(), var_sym) != coordinate_symbols.global_ids.end()) continue;
        if (std::find(coordinate_symbols.local_ids.begin(),  coordinate_symbols.local_ids.end(),  var_sym) != coordinate_symbols.local_ids.end())  continue;

        SgVariableSymbol * local_sym = NULL;

        it_sym_to_local = argument_symbol_maps.param_to_args.find(var_sym);
        if (it_sym_to_local != argument_symbol_maps.param_to_args.end())
          local_sym = it_sym_to_local->second;

        it_sym_to_local = iter_to_local.find(var_sym);
        if (it_sym_to_local != iter_to_local.end()) {
          assert(local_sym == NULL);

          local_sym = it_sym_to_local->second;
        }

        assert(local_sym != NULL); // implies VarRef to an unknown variable symbol

        SageInterface::replaceExpression(var_ref, SageBuilder::buildVarRefExp(local_sym));
      }
    }

    SgVariableSymbol * local_iterator = NULL;
    { // TODO outline ?
      std::string iterator_name = loop->iterator->get_name().getString();
      SgType * iterator_type = loop->iterator->get_type();

      assert(isSgTypeUnsignedLong(iterator_type) != NULL); // FIXME artificial constraint for debug

      SgVariableDeclaration * iterator_decl = SageBuilder::buildVariableDeclaration(
        "it_" + iterator_name,
        iterator_type,
        NULL,
        result
      );
      SageInterface::appendStatement(iterator_decl, result);

      local_iterator = result->lookup_variable_symbol("it_" + iterator_name);
      assert(local_iterator != NULL);
      iter_to_local.insert(std::pair<SgVariableSymbol *, SgVariableSymbol *>(loop->iterator, local_iterator));
    }
    if (bounds.second == NULL) {
      SgExprStatement * expr_stmt = SageBuilder::buildAssignStatement(
        SageBuilder::buildVarRefExp(local_iterator),
        bounds.first
      );
      SageInterface::appendStatement(expr_stmt, result);
    }
    else {
      assert(false); // TODO build loop. Is bounds.second the upper_bound or the size? (size seems better, problem: loops are from lower bound to upper bound)
    }
  }

  return result;
}

void IterationMapper<Kernel>::generateShapes(
  Kernel::loop_distribution_t * loop_distribution,
  std::set<IterationMap<Kernel> *> & shapes
) const {
  shapes.insert(new IterationMap<Kernel>(nested_loops.size(), false, loop_distribution));
}

}

}
