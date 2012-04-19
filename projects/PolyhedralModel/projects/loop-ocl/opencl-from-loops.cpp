
#include "rose-kernel/PolyhedralKernel.hpp"
#include "rose/Exception-rose.hpp"
#include "opencl-from-loops.hpp"
#include "rose-utils.hpp"

#include "rose.h"

#include "rose/Parser.hpp"

/**
 *  Helper function synthetizing a loop nest as a list of (iterator, upper-bound).\n
 *  Can be used iteratively adding deeper loops to a loop nest.\n
 *
 * param loop_nest: loops to be added
 * param res: list of (iterator, upper-bound), can contain elements from previous loops
 *
 * I+t currently considers: (and some others cases but none is insured to be correct)\n
 * \t 1 - "for (i = 0; i <= cst; i++)" and insert cst\n
 * \t 2 - "for (i = 0; i <= j op cst; i++)" and insert N op cst where N is 'j' upper bound(imply branch in kernel)\n
 * \t 3 - "for (i = j; i <= cst; i++)" and insert cst (imply branch in kernel)\n
 *
 * TODO Add check to verify that it is one of the three cases
 */
void getIteratorAndBounds(
                           std::vector<SgStatement *> & loop_nest,
                           std::map<SgVariableSymbol *, std::pair<int, int> > & iterators_bounds,
                           std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > & iterators_bounds_expression,
                           std::vector<SgVariableSymbol *> & ordered_iterators
) {

  std::vector<SgStatement *>::iterator it;
  for (it = loop_nest.begin(); it != loop_nest.end(); it++) {
    SgForStatement * for_stmt = isSgForStatement(*it);
    if (for_stmt != NULL) {
      // Get iterator:
      SgVariableSymbol * symbol = NULL;
      SgExpression * init_exp = NULL;
      {
        SgForInitStatement * for_init = for_stmt->get_for_init_stmt();
        std::vector<SgStatement *> for_init_vect = for_init->get_init_stmt();
        ROSE_ASSERT(for_init_vect.size() == 1);
        SgExprStatement * init_stmt = isSgExprStatement(for_init_vect[0]);
        ROSE_ASSERT(init_stmt != NULL);
        SgAssignOp * assign_init = isSgAssignOp(init_stmt->get_expression());
        ROSE_ASSERT(assign_init != NULL);
        SgVarRefExp * iterator = isSgVarRefExp(assign_init->get_lhs_operand_i());
        init_exp = assign_init->get_rhs_operand_i();
        ROSE_ASSERT(iterator != NULL);
        symbol = iterator->get_symbol();
      }
      ROSE_ASSERT(symbol != NULL && init_exp != NULL);
      ordered_iterators.push_back(symbol);

      // Get bounds:
      {
        SgExprStatement * test_stmt = isSgExprStatement(for_stmt->get_test());
        ROSE_ASSERT(test_stmt != NULL);
        SgBinaryOp * bin_op = isSgBinaryOp(test_stmt->get_expression());
        ROSE_ASSERT(bin_op != NULL);
        if (bin_op->variantT() == V_SgLessOrEqualOp) {
          iterators_bounds.insert(std::pair<SgVariableSymbol *, std::pair<int, int> >(symbol, std::pair<int, int>(
                        evalExpressionBounds(init_exp, iterators_bounds).first,
                        evalExpressionBounds(bin_op->get_rhs_operand_i(), iterators_bounds).second
                    )));
          iterators_bounds_expression.insert(std::pair<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> >(
                        symbol, std::pair<SgExpression *, SgExpression *>(
                             init_exp, bin_op->get_rhs_operand_i()
                        )
                    ));
        }
        else if (bin_op->variantT() == V_SgLessThanOp) {
          iterators_bounds.insert(std::pair<SgVariableSymbol *, std::pair<int, int> >(symbol, std::pair<int, int>(
                        evalExpressionBounds(init_exp, iterators_bounds).first,
                        evalExpressionBounds(bin_op->get_rhs_operand_i(), iterators_bounds).second - 1
                    )));
          iterators_bounds_expression.insert(std::pair<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> >(
                        symbol, std::pair<SgExpression *, SgExpression *>(
                             init_exp, SageBuilder::buildSubtractOp(bin_op->get_rhs_operand_i(), SageBuilder::buildIntVal(1))
                        )
                    ));
        }
        else ROSE_ASSERT(false);
      }
    }
    SgIfStmt * if_stmt = isSgIfStmt(*it);
    if (if_stmt != NULL) {
      // TODO
    }
  }
}

void arrayAccessShape(
                       Kernel * kernel,
                       PolyhedricAnnotation::FunctionDomain & domain,
                       PolyhedricAnnotation::FunctionDataAccess & data_access,
                       AnalysisContainer & analysis,
                       std::vector<SgVariableSymbol *> ordered_iterators
) {
  std::map<SgInitializedName *, Array>::iterator it_array;

  std::vector<std::pair<RoseVariable, std::vector<std::vector<std::pair<RoseVariable, int> > > > > * read_accesses = data_access.regenRead();
  std::vector<std::pair<RoseVariable, std::vector<std::vector<std::pair<RoseVariable, int> > > > > * write_accesses = data_access.regenWrite();
  for (it_array = analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
    SgInitializedName * init_name = it_array->first;
    Array array = it_array->second;
    std::vector<std::vector<std::vector<std::pair<RoseVariable, int> > > > reads_;
    std::vector<std::vector<std::vector<std::pair<RoseVariable, int> > > > writes_;
    {
      std::vector<std::pair<RoseVariable, std::vector<std::vector<std::pair<RoseVariable, int> > > > >::iterator it_access;
      for (it_access = read_accesses->begin(); it_access != read_accesses->end(); it_access++)
        if (it_access->first.is(init_name))
          reads_.push_back(it_access->second);
      for (it_access = write_accesses->begin(); it_access != write_accesses->end(); it_access++)
        if (it_access->first.is(init_name))
          writes_.push_back(it_access->second);
    }
    std::vector<std::vector<std::map<SgVariableSymbol *, int> > > reads;
    std::vector<std::vector<std::map<SgVariableSymbol *, int> > > writes;
    {
      std::vector<std::vector<std::vector<std::pair<RoseVariable, int> > > >::iterator it_0;
      std::vector<std::vector<std::pair<RoseVariable, int> > >::iterator it_1;
      std::vector<std::pair<RoseVariable, int> >::iterator it_2;
      for (it_0 = reads_.begin(); it_0 != reads_.end(); it_0++) {
        std::vector<std::map<SgVariableSymbol *, int> > one_read;
        for (it_1 = it_0->begin(); it_1 != it_0->end(); it_1++) {
          std::map<SgVariableSymbol *, int> one_read_dim;
          for (it_2 = it_1->begin(); it_2 != it_1->end(); it_2++) {
            SgVariableSymbol * sym = NULL;
            int coef = it_2->second;
            if (!isConstant(it_2->first)) {
              std::cout << it_2->first.getString() << std::endl;
              for (int i = 0; i < ordered_iterators.size(); i++) {
                if (it_2->first.getString() == ordered_iterators[i]->get_name().getString()) {
                  sym = ordered_iterators[i];
                  break;
                }
              }
              ROSE_ASSERT(sym != NULL);
            }
            std::map<SgVariableSymbol *, int>::iterator it_map = one_read_dim.find(sym);
            if (it_map == one_read_dim.end())
              one_read_dim.insert(std::pair<SgVariableSymbol *, int>(sym, coef));
            else
              it_map->second += coef;
          }
          one_read.push_back(one_read_dim);
        }
        reads.push_back(one_read);
      }

      for (it_0 = writes_.begin(); it_0 != writes_.end(); it_0++) {
        std::vector<std::map<SgVariableSymbol *, int> > one_write;
        for (it_1 = it_0->begin(); it_1 != it_0->end(); it_1++) {
          std::map<SgVariableSymbol *, int> one_write_dim;
          for (it_2 = it_1->begin(); it_2 != it_1->end(); it_2++) {
            SgVariableSymbol * sym = NULL;
            int coef = it_2->second;
            if (!isConstant(it_2->first)) {
              for (int i = 0; i < ordered_iterators.size(); i++) {
                if (it_2->first.is(ordered_iterators[i]->get_declaration())) {
                  sym = ordered_iterators[i];
                  break;
                }
              }
              ROSE_ASSERT(sym != NULL);
            }
            std::map<SgVariableSymbol *, int>::iterator it_map = one_write_dim.find(sym);
            if (it_map == one_write_dim.end())
              one_write_dim.insert(std::pair<SgVariableSymbol *, int>(sym, coef));
            else
              it_map->second += coef;
          }
          one_write.push_back(one_write_dim);
        }
        writes.push_back(one_write);
      }
    }
/*
  std::vector<std::vector<std::map<SgVariableSymbol *, int> > > reads;
  std::vector<std::vector<std::map<SgVariableSymbol *, int> > > writes;
*/
    //

    // TODO compute accessed domain
    // TODO append(<kernel, var, Access(domain)>) to Array
  }
  delete read_accesses;
  delete write_accesses;
}

/**
 * Kernel constructor
 */
Kernel::Kernel(
                SgStatement * stmt,
                unsigned nbr_work_dim_,
                std::map<SgVariableSymbol *, std::pair<int, int> > iterators_bounds_,
                std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > iterators_bounds_expression_,
                std::vector<SgVariableSymbol *> ordered_iterators_,
                AnalysisContainer & analysis_
) :
  id(id_cnt),
  nbr_work_dim(nbr_work_dim_),
  kernel_content(stmt),
  iterators_bounds(iterators_bounds_),
  iterators_bounds_expression(iterators_bounds_expression_),
  ordered_iterators(ordered_iterators_),
  kernel_name(),
  analysis(analysis_)
{
  std::ostringstream str;
  str << "kernel_" << id;
  kernel_name = str.str();
#define ARRAY_ACCESS_ANALYSIS 0
#if ARRAY_ACCESS_ANALYSIS
  std::cout << "In kernel: " << kernel_name << std::endl;

  if (analysis.polyhedral_analysis_success) {
    std::map<SgInitializedName *, Array>::iterator it_array;
    std::vector<SgExprStatement *> expr_stmts = SageInterface::querySubTree<SgExprStatement>(kernel_content);
    std::vector<SgExprStatement *>::iterator it_expr_stmt;
    for (it_expr_stmt = expr_stmts.begin(); it_expr_stmt != expr_stmts.end(); it_expr_stmt++) {
      SgNode * parent = (*it_expr_stmt)->get_parent();
      if (isSgForInitStatement(parent)) continue;
      if (isSgForStatement(parent) && isSgForStatement(parent)->get_test() == *it_expr_stmt) continue;
      try {
        PolyhedricAnnotation::FunctionDomain & domain =
               PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(*it_expr_stmt);
        domain.print(std::cout);
        PolyhedricAnnotation::FunctionDataAccess & data_access =
               PolyhedricAnnotation::getDataAccess<SgFunctionDeclaration, SgExprStatement, RoseVariable>(*it_expr_stmt);
        data_access.print(std::cout);
        
        arrayAccessShape(this, domain, data_access, analysis, ordered_iterators); 
      }
      catch (Exception::RoseAttributeMissing & e) {
        e.print(std::cout);
      }
    }
  }
#endif /* ARRAY_ACCESS_ANALYSIS */
  id_cnt++;
}

/**
 * Generate the associated kernel call
 */
SgStatement * Kernel::genCall() {
  SgScopeStatement * scope = isSgScopeStatement(kernel_content->get_parent());
  ROSE_ASSERT(scope != NULL);
  SgScopeStatement * top_scope = SageBuilder::topScopeStack();

  ROSE_ASSERT(nbr_work_dim != 0);

  SgStatement * res = NULL;
  SgBasicBlock * bb = SageBuilder::buildBasicBlock();
  res = bb;

  SgName size_t_name = "size_t";
  SgTypedefSymbol * size_type_symbol = SageInterface::lookupTypedefSymbolInParentScopes(size_t_name, top_scope);
  ROSE_ASSERT(size_type_symbol != NULL);
  SgType * size_type = size_type_symbol->get_type();
  ROSE_ASSERT(size_type != NULL);

  // Declaration of the 'work_size' variable
  SgVariableDeclaration * work_size_decl = NULL;
  {
    std::vector<SgExpression *> expr_vect;
    for (int i = 0; i < nbr_work_dim; i++) {
      int idx = ordered_iterators.size() - nbr_work_dim + i;
      SgVariableSymbol * sym = ordered_iterators[idx];
      std::pair<int,int> bounds = iterators_bounds[sym];
      std::pair<SgExpression *, SgExpression *> & bounds_expression = iterators_bounds_expression[sym];
      SgExpression * lb = bounds_expression.first;
      SgExpression * ub = bounds_expression.second;
      SgExpression * dim_size = substract(ub, lb, 1);
      if (isSgVarRefExp(dim_size)) {
        SgVariableSymbol * ref_it = isSgVarRefExp(dim_size)->get_symbol();
        for (int j = ordered_iterators.size() - nbr_work_dim; j < idx; j++)
          if (ordered_iterators[j] == ref_it)
            dim_size = SageBuilder::buildIntVal(iterators_bounds[ordered_iterators[j]].second);
      }
      else {
        std::vector<SgVarRefExp *> var_ref = SageInterface::querySubTree<SgVarRefExp>(dim_size);
        std::vector<SgVarRefExp *>::iterator it_var_ref;
        for (it_var_ref = var_ref.begin(); it_var_ref != var_ref.end(); it_var_ref++) {
          SgVariableSymbol * ref_it = (*it_var_ref)->get_symbol();
          for (int j = ordered_iterators.size() - nbr_work_dim; j < idx; j++)
            if (ordered_iterators[j] == ref_it) {
              bool need_lb = false;
              SgMultiplyOp * parent = isSgMultiplyOp((*it_var_ref)->get_parent());
              if (parent != NULL) {
                SgIntVal * coef = isSgIntVal(parent->get_lhs_operand_i());
                ROSE_ASSERT(coef != NULL);
                need_lb = coef->get_value() < 0;
              }
              if (need_lb)
                SageInterface::replaceExpression(*it_var_ref, SageBuilder::buildIntVal(iterators_bounds[ordered_iterators[j]].first), true);
              else
                SageInterface::replaceExpression(*it_var_ref, SageBuilder::buildIntVal(iterators_bounds[ordered_iterators[j]].second), true);
            }
        }
      }

      dim_size = simplify(dim_size);

      expr_vect.push_back(dim_size);
    }

    SgType * type = SageBuilder::buildArrayType(size_type, SageBuilder::buildIntVal(nbr_work_dim));
    SgExprListExp * expr_list = SageBuilder::buildExprListExp(expr_vect);
    SgAggregateInitializer * init = SageBuilder::buildAggregateInitializer(expr_list, type);
    work_size_decl = SageBuilder::buildVariableDeclaration("work_size", type, init, scope);
  }
  bb->append_statement(work_size_decl);

  // Instantiate the kernel
  {
    SgVarRefExp * kernel_obj = SageBuilder::buildVarRefExp(kernel_name, top_scope);
    {
      // Obtains the SgType for "cl_kernel"
      SgName kernel_type_name = "cl_kernel";
      SgTypedefSymbol * sym = top_scope->lookup_typedef_symbol(kernel_type_name);
      ROSE_ASSERT(sym != NULL);
      SgType * kernel_type = sym->get_type();
      ROSE_ASSERT(kernel_type != NULL);

      SgName create_kernel = "createKernel";
      std::vector<SgExpression *> param_list;
      {
        param_list.push_back(SageBuilder::buildVarRefExp("program", top_scope));
        param_list.push_back(SageBuilder::buildStringVal(kernel_name));
      }
      SgExprListExp * params = SageBuilder::buildExprListExp(param_list);
      SgFunctionCallExp * create_kernel_call = SageBuilder::buildFunctionCallExp(create_kernel, kernel_type, params, top_scope);
      SgStatement * kernel_creation = SageBuilder::buildExprStatement(SageBuilder::buildAssignOp(kernel_obj, create_kernel_call));
      bb->append_statement(kernel_creation);
    }

    {
      unsigned arg_cnt = 0;
      {
        SgName set_kernel_arg_value = "setKernelArgToValue";
        SgExpression * size = SageBuilder::buildSizeOfOp(SageBuilder::buildIntType());
        for (int i = 0; i < ordered_iterators.size() - nbr_work_dim; i++) {
          SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                  kernel_obj,
                                                                  SageBuilder::buildIntVal(arg_cnt),
                                                                  SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(ordered_iterators[i])),
                                                                  size
                                                                );
          SgFunctionCallExp * set_kernel_arg_value_call = SageBuilder::buildFunctionCallExp(
                                                                                             set_kernel_arg_value,
                                                                                             SageBuilder::buildVoidType(),
                                                                                             params,
                                                                                             top_scope
                                                                                           );
          bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_value_call));
          arg_cnt++;
        }
        std::map<SgInitializedName *, Scalar>::iterator it_scalar;
        for (it_scalar = analysis.scalars.begin(); it_scalar != analysis.scalars.end(); it_scalar++) {
          SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                  kernel_obj,
                                                                  SageBuilder::buildIntVal(arg_cnt),
                                                                  SageBuilder::buildAddressOfOp(
                                                                    SageBuilder::buildVarRefExp(it_scalar->first,
                                                                    top_scope)
                                                                  ),
                                                                  SageBuilder::buildSizeOfOp(it_scalar->second.type)
                                                                );
          SgFunctionCallExp * set_kernel_arg_value_call = SageBuilder::buildFunctionCallExp(
                                                                                             set_kernel_arg_value,
                                                                                             SageBuilder::buildVoidType(),
                                                                                             params,
                                                                                             top_scope
                                                                                           );
          bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_value_call));
          arg_cnt++;
        }
        SgName set_kernel_arg_buffer = "setKernelArgToBuffer";
        std::map<SgInitializedName *, Array>::iterator it_array;
        for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
          SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                  kernel_obj,
                                                                  SageBuilder::buildIntVal(arg_cnt),
                                                                  SageBuilder::buildVarRefExp(it_array->second.associated_ocl_buffer)
                                                                );
          SgFunctionCallExp * set_kernel_arg_buffer_call = SageBuilder::buildFunctionCallExp(
                                                                                              set_kernel_arg_buffer,
                                                                                              SageBuilder::buildVoidType(),
                                                                                              params,
                                                                                              top_scope
                                                                                            );
          bb->append_statement(SageBuilder::buildExprStatement(set_kernel_arg_buffer_call));
          arg_cnt++;
        }
      }
    }
  }

  // Call to the enqueue function
  SgStatement * call_stmt = NULL;
  {
    SgName nd_range = "enqueueKernel";
    std::vector<SgExpression *> param_list;
    {
      param_list.push_back(SageBuilder::buildVarRefExp("queue", top_scope)); // queue
      param_list.push_back(SageBuilder::buildVarRefExp(kernel_name, top_scope)); // kernel
      param_list.push_back(SageBuilder::buildIntVal(nbr_work_dim)); // work_dim
      param_list.push_back(SageBuilder::buildVarRefExp(work_size_decl)); // global_work_size
      param_list.push_back(SageBuilder::buildIntVal(0)); // local_work_size
    }
    SgExprListExp * params = SageBuilder::buildExprListExp(param_list);
    SgFunctionCallExp * nd_range_call = SageBuilder::buildFunctionCallExp(nd_range, SageBuilder::buildVoidType(), params, top_scope);
    call_stmt = SageBuilder::buildExprStatement(nd_range_call);
  }
  bb->append_statement(call_stmt);

  return res;
}

/**
 * Add a kernel instantiation after 'previous' statement (and update it to point after the instantiation)
 * and generate the kernel function in the OpenCL file.
 */
void Kernel::createKernel(SgStatement ** previous, SgSourceFile * ocl_file) {
  // No kernel if no 'work_dim'
  if (nbr_work_dim == 0) return;

  SgScopeStatement * scope = SageBuilder::topScopeStack();

  SgName size_t_name = "size_t";
  SgTypedefSymbol * size_type_symbol = SageInterface::lookupTypedefSymbolInParentScopes(size_t_name, scope);
  ROSE_ASSERT(size_type_symbol != NULL);
  SgType * size_type = size_type_symbol->get_type();
  ROSE_ASSERT(size_type != NULL);

  // Declaration of the variable for the kernel
  SgVariableDeclaration * kernel_decl = NULL;
  {
    // Obtains the SgType for "cl_kernel"
    SgName kernel_type_name = "cl_kernel";
    SgTypedefSymbol * sym = scope->lookup_typedef_symbol(kernel_type_name);
    ROSE_ASSERT(sym != NULL);
    SgType * kernel_type = sym->get_type();
    ROSE_ASSERT(kernel_type != NULL);

    // DON'T Generate the call to "createKernel" initializing the kernel
    SgInitializer * init = NULL; // It have to be done for each call (at least when their is outer loop iterators implied)

    kernel_decl = SageBuilder::buildVariableDeclaration(kernel_name, kernel_type, init, scope);
  }
  SageInterface::insertStatementAfter(*previous, kernel_decl, false);
  *previous = kernel_decl;

  // Generate the kernel function in the OpenCL file
  {
    SgScopeStatement * ocl_scope = ocl_file->get_globalScope();
    SageBuilder::pushScopeStack(ocl_scope);

    SgFunctionParameterList * param_list = SageBuilder::buildFunctionParameterList();
    for (int i = 0; i < ordered_iterators.size() - nbr_work_dim; i++) {
      SgInitializedName * init_name = SageBuilder::buildInitializedName(ordered_iterators[i]->get_name(), SageBuilder::buildUnsignedIntType());
      param_list->append_arg(init_name);
    }
    std::map<SgInitializedName *, Scalar>::iterator it_scalar;
    for (it_scalar = analysis.scalars.begin(); it_scalar != analysis.scalars.end(); it_scalar++) {
      Scalar & scalar = it_scalar->second;
      SgInitializedName * init_name = SageBuilder::buildInitializedName(scalar.name, scalar.type);
      param_list->append_arg(init_name);
    }
    std::map<SgInitializedName *, Array>::iterator it_array;
    for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
      Array & array = it_array->second;

      int size = 1;
      std::vector<unsigned>::iterator it;
      for (it = array.dimensions.begin(); it != array.dimensions.end(); it++) size *= *it;
      SgType * array_type = SageBuilder::buildArrayType(array.base_type, SageBuilder::buildIntVal(size));
      

      array_type = SageBuilder::buildModifierType(array_type);
      ((SgModifierType *)array_type)->get_typeModifier().setOpenclGlobal();

      SgInitializedName * init_name = SageBuilder::buildInitializedName(array.name, array_type);
      
      param_list->append_arg(init_name);
    }

    SgFunctionDeclaration * kernel_decl = SageBuilder::buildDefiningFunctionDeclaration(
                                                SgName(kernel_name), SageBuilder::buildVoidType(), param_list
                                          );
    kernel_decl->get_functionModifier().setOpenclKernel();
    SgFunctionDefinition * kernel_def = kernel_decl->get_definition();
    ROSE_ASSERT(kernel_def != NULL);
    ocl_scope->append_statement(kernel_decl);
    SageBuilder::pushScopeStack(kernel_def);

    SgBasicBlock * kernel_body = kernel_def->get_body();
    ROSE_ASSERT(kernel_body != NULL);
    SageBuilder::pushScopeStack(kernel_body);

    SgName global_id = "get_global_id";
    for (int i = ordered_iterators.size() - nbr_work_dim; i < ordered_iterators.size(); i++) {
      SgExprListExp * params = SageBuilder::buildExprListExp(SageBuilder::buildIntVal(i + nbr_work_dim - ordered_iterators.size()));
      SgFunctionCallExp * get_global_id_call = SageBuilder::buildFunctionCallExp(global_id, SageBuilder::buildIntType(), params, kernel_def);
      SgExpression * loc_it = SageBuilder::buildAddOp(get_global_id_call, iterators_bounds_expression[ordered_iterators[i]].first);
      SgInitializer * init = SageBuilder::buildAssignInitializer(loc_it, SageBuilder::buildIntType());
      SgVariableDeclaration * var_decl = SageBuilder::buildVariableDeclaration(
                                                                                ordered_iterators[i]->get_name(),
                                                                                SageBuilder::buildIntType(),
                                                                                init,
                                                                                kernel_def
                                                                              );
      kernel_body->append_statement(var_decl);
    }

    SgExpression * bounds_control_exp = NULL;
    std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> >::iterator it;
    for (it = iterators_bounds_expression.begin(); it != iterators_bounds_expression.end(); it++) {
      SgVariableSymbol * sym = it->first;
      int loop_id;
      for (loop_id = 0; loop_id < ordered_iterators.size(); loop_id++)
        if (ordered_iterators[loop_id] == sym)
          break;
      if (loop_id >= ordered_iterators.size() - nbr_work_dim) {
        // FIXME only add the bound only if needed
        SgExpression * lb = SageBuilder::buildGreaterOrEqualOp(
                                                                SageBuilder::buildVarRefExp(sym),
                                                                it->second.first
                                                              );
        SgExpression * ub = SageBuilder::buildLessOrEqualOp(
                                                             SageBuilder::buildVarRefExp(sym),
                                                             it->second.second
                                                           );
        if (bounds_control_exp == NULL)
          bounds_control_exp = SageBuilder::buildAndOp(lb, ub);
        else
          bounds_control_exp = SageBuilder::buildAndOp(SageBuilder::buildAndOp(lb, ub), bounds_control_exp);
      }
    }
    if (bounds_control_exp != NULL) {
      SgIfStmt * if_stmt = SageBuilder::buildIfStmt(bounds_control_exp, kernel_content, NULL);
      kernel_body->append_statement(if_stmt);
    }
    else {
      kernel_body->append_statement(kernel_content);
    }

    std::vector<SgVarRefExp *> var_refs = SageInterface::querySubTree<SgVarRefExp>(kernel_body);
    std::vector<SgVarRefExp *>::iterator it_var_ref;
    for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
      SgVarRefExp * var_ref = *it_var_ref;
      SgVariableSymbol * old_sym = var_ref->get_symbol();
      SgName var_name = old_sym->get_name();
      SgVariableSymbol * new_sym = kernel_def->lookup_variable_symbol(var_name);
      if (new_sym == NULL) {
        kernel_body->prepend_statement(SageBuilder::buildVariableDeclaration(var_name, SageBuilder::buildIntType(), NULL, kernel_def));
        new_sym = kernel_def->lookup_variable_symbol(var_name);
      }
      ROSE_ASSERT(new_sym != NULL);
      var_ref->set_symbol(new_sym);

      std::map<SgInitializedName *, Array>::iterator it_array = analysis.arrays.find(old_sym->get_declaration());
      if (it_array != analysis.arrays.end()) {
        Array & array = it_array->second;
        SgExpression * exp = NULL;
        SgPntrArrRefExp * array_ref = isSgPntrArrRefExp(var_ref->get_parent());
        ROSE_ASSERT(array_ref != NULL);
        std::vector<unsigned>::iterator it;
        for (it = array.dimensions.begin() + 1; it != array.dimensions.end(); it++) {
          SgExpression * tmp = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(*it), array_ref->get_rhs_operand_i());
          if (exp == NULL)
            exp = tmp;
          else
            exp = SageBuilder::buildAddOp(exp, tmp);
          array_ref = isSgPntrArrRefExp(array_ref->get_parent());
          ROSE_ASSERT(array_ref != NULL);
        }
        if (exp == NULL)
          exp = array_ref->get_rhs_operand_i();
        else
          exp = SageBuilder::buildAddOp(exp,  array_ref->get_rhs_operand_i());
        exp = SageBuilder::buildPntrArrRefExp(var_ref, exp);
        SageInterface::replaceExpression(array_ref, exp, true);
      }
    }

    SageBuilder::popScopeStack(); // kernel_body
    SageBuilder::popScopeStack(); // kernel_def
    SageBuilder::popScopeStack(); // ocl_scope
    SageInterface::setSourcePositionForTransformation(ocl_scope);
  }
}

unsigned Kernel::id_cnt = 0;

/**
 * Insert common OpenCL initialization (devices, programs, ...) after 'previous' (and update it to be the last inserted statement)
 */
void createOclInit(SgStatement ** previous, std::string ocl_file_name, AnalysisContainer & analysis) {
  SgScopeStatement * scope = SageBuilder::topScopeStack();

  SageInterface::attachComment(*previous, "Declaration and instantiation of basic OpenCL objects", PreprocessingInfo::after);

  SgVariableDeclaration * context_decl = NULL;
  {
    SgTypedefDeclaration * context_type_decl = SageBuilder::buildTypedefDeclaration("cl_context", SageBuilder::buildIntType(), scope);
    SgType * context_type = SgTypedefType::createType(context_type_decl);
    context_decl = SageBuilder::buildVariableDeclaration("context", context_type, NULL, scope);
    SageInterface::insertStatementAfter(*previous, context_decl, false);
    *previous = context_decl;
  }

  SgVariableDeclaration * program_decl = NULL;
  {
    SgTypedefDeclaration * program_type_decl = SageBuilder::buildTypedefDeclaration("cl_program", SageBuilder::buildIntType(), scope);
    SgType * program_type = SgTypedefType::createType(program_type_decl);
    program_decl = SageBuilder::buildVariableDeclaration("program", program_type, NULL, scope);
    SageInterface::insertStatementAfter(*previous, program_decl, false);
    *previous = program_decl;
  }

  SgVariableDeclaration * queue_decl = NULL;
  {
    SgTypedefDeclaration * queue_type_decl = SageBuilder::buildTypedefDeclaration("cl_command_queue", SageBuilder::buildIntType(), scope);
    SgType * queue_type = SgTypedefType::createType(queue_type_decl);
    queue_decl = SageBuilder::buildVariableDeclaration("queue", queue_type, NULL, scope);
    SageInterface::insertStatementAfter(*previous, queue_decl, false);
    *previous = queue_decl;
  }

  {
    SgName init_opencl = "initOpenCL";
    SgExprListExp * params = SageBuilder::buildExprListExp(
                                                            SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(context_decl)),
                                                            SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(queue_decl)),
                                                            SageBuilder::buildAddressOfOp(SageBuilder::buildVarRefExp(program_decl)),
                                                            SageBuilder::buildStringVal(ocl_file_name)
                                                          );
    SgFunctionCallExp * init_opencl_call = SageBuilder::buildFunctionCallExp(init_opencl, SageBuilder::buildVoidType(), params, scope);
    SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(init_opencl_call);
    SageInterface::insertStatementAfter(*previous, expr_stmt, false);
    *previous = expr_stmt;
  }

  SageInterface::attachComment(*previous, "Creation of the buffers on the device.", PreprocessingInfo::after);

  // Insert the buffers
  {
    SgTypedefDeclaration * buffer_type_decl = SageBuilder::buildTypedefDeclaration("cl_mem", SageBuilder::buildIntType(), scope);
    SgType * buffer_type = SgTypedefType::createType(buffer_type_decl);
    std::map<SgInitializedName *, Array>::iterator it_array;
    for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
      Array & array = it_array->second;

      SgName ro_buf = "createROBuffer";
      SgName rw_buf = "createRWBuffer";
      SgExpression * size = SageBuilder::buildSizeOfOp(array.base_type);
      for (int i = 0; i < array.dimensions.size(); i++)
        size = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(array.dimensions[i]), size);
      SgExprListExp * params = SageBuilder::buildExprListExp(SageBuilder::buildVarRefExp(context_decl), size);
      SgFunctionCallExp * create_buffer_call = NULL;
      if (array.access_type == in)
        create_buffer_call = SageBuilder::buildFunctionCallExp(ro_buf, buffer_type, params, scope);
      else
        create_buffer_call = SageBuilder::buildFunctionCallExp(rw_buf, buffer_type, params, scope);
      SgInitializer * init = SageBuilder::buildAssignInitializer(create_buffer_call);

      SgVariableDeclaration * buffer_decl = SageBuilder::buildVariableDeclaration("d_" + array.name, buffer_type, init, scope);

      array.associated_ocl_buffer = isSgVariableSymbol(buffer_decl->get_variables()[0]->get_symbol_from_symbol_table());
      ROSE_ASSERT(array.associated_ocl_buffer != NULL);

      SageInterface::insertStatementAfter(*previous, buffer_decl, false);
      *previous = buffer_decl;
    }
  }

  SageInterface::attachComment(*previous, "Initial data transfert", PreprocessingInfo::after);

  // Data transfert
  {
    std::map<SgInitializedName *, Array>::iterator it_array;
    for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
      Array & array = it_array->second;
      if (array.access_type == in || array.access_type == inout) {
        SgName write = "writeBuffer";
        SgExpression * size = SageBuilder::buildSizeOfOp(array.base_type);
        for (int i = 0; i < array.dimensions.size(); i++)
          size = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(array.dimensions[i]), size);
        SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                SageBuilder::buildVarRefExp(queue_decl),
                                                                SageBuilder::buildVarRefExp(array.associated_ocl_buffer),
                                                                size,
                                                                SageBuilder::buildVarRefExp(it_array->first, scope)
                                                              );
        SgFunctionCallExp * write_call = SageBuilder::buildFunctionCallExp(write, SageBuilder::buildVoidType(), params, scope);
        SgExprStatement * stmt = SageBuilder::buildExprStatement(write_call);

        SageInterface::insertStatementAfter(*previous, stmt, false);
        *previous = stmt;
      }
    }
  }

  SageInterface::attachComment(*previous, "Declaration of the kernels objects", PreprocessingInfo::after);

  // Build additionnal OpenCL API types
  {
    SgTypedefDeclaration * kernel_type_decl = SageBuilder::buildTypedefDeclaration("cl_kernel", SageBuilder::buildIntType(), scope);
  }
}

/**
 * Insert OpenCL release code after 'previous' (and update it to be the last inserted statement)
 */
void createOclRelease(SgStatement ** previous, AnalysisContainer & analysis) {
  SgScopeStatement * scope = SageBuilder::topScopeStack();

  SageInterface::attachComment(*previous, "Final data transfert", PreprocessingInfo::after);

  // Data transfert
  {
    SgVariableSymbol * queue_sym = scope->lookup_variable_symbol("queue");
    SgVarRefExp * queue_ref = SageBuilder::buildVarRefExp(queue_sym);
    std::map<SgInitializedName *, Array>::iterator it_array;
    for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
      Array & array = it_array->second;
      if (array.access_type == out || array.access_type == inout) {
        SgName read = "readBuffer";
        SgExpression * size = SageBuilder::buildSizeOfOp(array.base_type);
        for (int i = 0; i < array.dimensions.size(); i++)
          size = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(array.dimensions[i]), size);
        SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                queue_ref,
                                                                SageBuilder::buildVarRefExp(array.associated_ocl_buffer),
                                                                size,
                                                                SageBuilder::buildVarRefExp(it_array->first, scope)
                                                              );
        SgFunctionCallExp * read_call = SageBuilder::buildFunctionCallExp(read, SageBuilder::buildVoidType(), params, scope);
        SgExprStatement * stmt = SageBuilder::buildExprStatement(read_call);
      
        SageInterface::insertStatementAfter(*previous, stmt, false);
        *previous = stmt;
      }
    }

    {
      SgName sync = "clFinish";
      SgExprListExp * params = SageBuilder::buildExprListExp(queue_ref);
      SgFunctionCallExp * sync_call = SageBuilder::buildFunctionCallExp(sync, SageBuilder::buildVoidType(), params, scope);
      SgExprStatement * stmt = SageBuilder::buildExprStatement(sync_call);

      SageInterface::insertStatementAfter(*previous, stmt, false);
      *previous = stmt;
    }
  }

  // TODO Release
}

void surroundBySync(SgStatement * first, SgStatement * last, AnalysisContainer & analysis, bool gen_read = true, bool gen_write = true) {
  SgScopeStatement * top_scope = SageBuilder::topScopeStack();

  SgVariableSymbol * queue_sym = top_scope->lookup_variable_symbol("queue");
  SgVarRefExp * queue_ref = SageBuilder::buildVarRefExp(queue_sym);
  std::map<SgInitializedName *, Array>::iterator it_array;

  if (gen_read) {
    for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
      Array & array = it_array->second;
      if (array.access_type == out || array.access_type == inout) {
        SgName read = "readBuffer";
        SgExpression * size = SageBuilder::buildSizeOfOp(array.base_type);
        for (int i = 0; i < array.dimensions.size(); i++)
          size = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(array.dimensions[i]), size);
        SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                queue_ref,
                                                                SageBuilder::buildVarRefExp(array.associated_ocl_buffer),
                                                                size,
                                                                SageBuilder::buildVarRefExp(it_array->first, top_scope)
                                                              );
        SgFunctionCallExp * read_call = SageBuilder::buildFunctionCallExp(read, SageBuilder::buildVoidType(), params, top_scope);
        SageInterface::insertStatementBefore(first, SageBuilder::buildExprStatement(read_call));
      }
    }

    {
      SgName sync = "clFinish";
      SgExprListExp * params = SageBuilder::buildExprListExp(queue_ref);
      SgFunctionCallExp * sync_call = SageBuilder::buildFunctionCallExp(sync, SageBuilder::buildVoidType(), params, top_scope);
      SageInterface::insertStatementBefore(first, SageBuilder::buildExprStatement(sync_call));
    }
  }

  if (gen_write) {
    for (it_array =  analysis.arrays.begin(); it_array != analysis.arrays.end(); it_array++) {
      Array & array = it_array->second;
      if (array.access_type == out || array.access_type == inout) {
        SgName write = "writeBuffer";
        SgExpression * size = SageBuilder::buildSizeOfOp(array.base_type);
        for (int i = 0; i < array.dimensions.size(); i++)
          size = SageBuilder::buildMultiplyOp(SageBuilder::buildIntVal(array.dimensions[i]), size);
        SgExprListExp * params = SageBuilder::buildExprListExp(
                                                                queue_ref,
                                                                SageBuilder::buildVarRefExp(array.associated_ocl_buffer),
                                                                size,
                                                                SageBuilder::buildVarRefExp(it_array->first, top_scope)
                                                              );
        SgFunctionCallExp * write_call = SageBuilder::buildFunctionCallExp(write, SageBuilder::buildVoidType(), params, top_scope);
        SageInterface::insertStatementAfter(last, SageBuilder::buildExprStatement(write_call));
      }
    }
  }
}

void GenerateKernel(
                     SgStatement * first,
                     SgStatement * last, 
                     unsigned dim_out, 
                     unsigned dim_used, 
                     std::vector<Kernel *> & generated_kernel, 
                     AnalysisContainer & analysis
) {
  SgStatement * first_stmt_for_sync = NULL;
  SgStatement * last_stmt_for_sync = NULL;
  std::vector<std::pair<SgStatement *, SgStatement *> > to_be_sync;
  SgStatement * current = first;
  while (current != NULL) {
    SgStatement * next;
    if (current != last)
      next = SageInterface::getNextStatement(current);
    else
      next = NULL;
    unsigned number_of_kernel = generated_kernel.size();
    SageInterface::replaceStatement(
                                     current,
                                     traverseLeftOutLoop(current, dim_out, dim_used, std::vector<SgForStatement *>(), generated_kernel, analysis)
                                   );
    if (number_of_kernel == generated_kernel.size()) {
      if (first_stmt_for_sync == NULL) last_stmt_for_sync = first_stmt_for_sync = current;
      else last_stmt_for_sync = current;
    }
    else if (first_stmt_for_sync != NULL) {
      to_be_sync.push_back(std::pair<SgStatement *, SgStatement *>(first_stmt_for_sync, last_stmt_for_sync));
      first_stmt_for_sync = NULL;
      last_stmt_for_sync = NULL;
    }
    current = next;
  }
  if (first_stmt_for_sync != NULL && first_stmt_for_sync != first)
    to_be_sync.push_back(std::pair<SgStatement *, SgStatement *>(first_stmt_for_sync, last_stmt_for_sync));
  std::vector<std::pair<SgStatement *, SgStatement *> >::iterator it_;
  for (it_ = to_be_sync.begin(); it_ != to_be_sync.end(); it_++)
    surroundBySync(it_->first, it_->second, analysis, it_->first != first, it_->second != last);
}

/**
 * Traverse loop nest to form the 'work_dim' of a kernel, then instantiate a kernel object with the remaining statements
 */
void genKernel(
                SgStatement * stmt,
                unsigned dim_used,
                std::vector<SgStatement *> stmt_list,
                std::vector<Kernel *> & generated_kernel,
                std::map<SgVariableSymbol *, std::pair<int, int> > iterators_bounds,
                std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > iterators_bounds_expression,
                std::vector<SgVariableSymbol *> ordered_iterator,
                AnalysisContainer & analysis
) {
  // We have reach the limit of dimension for 'work_dim' so we generate a kernel
  if (dim_used == 0) {
    getIteratorAndBounds(stmt_list, iterators_bounds, iterators_bounds_expression, ordered_iterator);
    unsigned nbr_for = 0;
    for (std::vector<SgStatement *>::iterator it = stmt_list.begin(); it != stmt_list.end(); it++) if (isSgForStatement(*it)) nbr_for++;
    generated_kernel.push_back(new Kernel(stmt, nbr_for, iterators_bounds, iterators_bounds_expression, ordered_iterator, analysis));
  }
  else {
    switch (stmt->variantT()) {
      case V_SgBasicBlock:
      {
        SgBasicBlock * bb = isSgBasicBlock(stmt);
        std::vector<SgStatement *>::iterator it;
        SgStatement * first_stmt_for_sync = NULL;
        SgStatement * last_stmt_for_sync = NULL;
        std::vector<std::pair<SgStatement *, SgStatement *> > to_be_sync;
        for (it = bb->get_statements().begin(); it != bb->get_statements().end(); it++) {
          unsigned number_of_kernel = generated_kernel.size();
          genKernel(*it, dim_used, stmt_list, generated_kernel, iterators_bounds, iterators_bounds_expression, ordered_iterator, analysis);
          if (number_of_kernel == generated_kernel.size()) {
            if (first_stmt_for_sync == NULL) last_stmt_for_sync = first_stmt_for_sync = *it;
            else last_stmt_for_sync = *it;
          }
          else if (first_stmt_for_sync != NULL) {
            to_be_sync.push_back(std::pair<SgStatement *, SgStatement *>(first_stmt_for_sync, last_stmt_for_sync));
            first_stmt_for_sync = NULL;
            last_stmt_for_sync = NULL;
          }
        }
        if (first_stmt_for_sync != NULL && first_stmt_for_sync != *(bb->get_statements().begin()))
          to_be_sync.push_back(std::pair<SgStatement *, SgStatement *>(first_stmt_for_sync, last_stmt_for_sync));
        std::vector<std::pair<SgStatement *, SgStatement *> >::iterator it_;
        for (it_ = to_be_sync.begin(); it_ != to_be_sync.end(); it_++)
          surroundBySync(it_->first, it_->second, analysis);
        break;
      }
      case V_SgForStatement:
      {
        SgForStatement * for_stmt = isSgForStatement(stmt);
        bool no_dep = true;
        if (analysis.polyhedral_analysis_success) {
          PolyhedricAnnotation::FunctionPolyhedralProgram & polyhedral_program = 
              PolyhedricAnnotation::getPolyhedralProgram<SgFunctionDeclaration, SgExprStatement, RoseVariable>(analysis.base_func);
          std::map<unsigned, std::pair<unsigned, unsigned> > stmt_id_it;
          {
            std::vector<SgExprStatement *> expr_stmts = SageInterface::querySubTree<SgExprStatement>(for_stmt->get_loop_body());
            std::vector<SgExprStatement *>::iterator it_expr_stmt;
            for (it_expr_stmt = expr_stmts.begin(); it_expr_stmt != expr_stmts.end(); it_expr_stmt++) {
              SgNode * parent = (*it_expr_stmt)->get_parent();
              if (isSgForInitStatement(parent)) continue;
              if (isSgForStatement(parent) && isSgForStatement(parent)->get_test() == *it_expr_stmt) continue;
              try {
                stmt_id_it.insert(std::pair<unsigned, std::pair<unsigned, unsigned> >(
                  polyhedral_program.getStatementID(*it_expr_stmt),
                  std::pair<unsigned, unsigned>(
                    polyhedral_program.getIteratorID(*it_expr_stmt, PolyhedricAnnotation::getIterator(for_stmt)),
                    PolyhedricAnnotation::getDomain<SgFunctionDeclaration, SgExprStatement, RoseVariable>(*it_expr_stmt).getNumberOfIterators()
                  )
                ));
              }
              catch (Exception::RoseAttributeMissing & e) {
                e.print(std::cerr);
                break;
              }
            }
          }
          std::vector<PolyhedricDependency::FunctionDependency *>::iterator it;
          for (it = analysis.dependencies->begin(); it != analysis.dependencies->end(); it++) {
             std::map<unsigned, std::pair<unsigned, unsigned> >::iterator it1 = stmt_id_it.find((*it)->getFrom());
             std::map<unsigned, std::pair<unsigned, unsigned> >::iterator it2 = stmt_id_it.find((*it)->getTo());
             if (it1 == stmt_id_it.end() || it2 == stmt_id_it.end()) continue;
             unsigned it_to = it2->second.first;
             unsigned it_from = it1->second.first + it2->second.second;
             const ConstraintSystem & cs = (*it)->getPolyhedron().constraints();
             ConstraintSystem::const_iterator it_cs;
             // FIXME verify the math
             for (it_cs = cs.begin(); it_cs != cs.end(); it_cs++) {
               if (it_cs->is_equality()) { // it is an equality
                 if (it_cs->coefficient(VariableID(it_to)) == 0 && it_cs->coefficient(VariableID(it_from)) == 0) // Not implied in the constraint
                   continue;
                 else if (it_cs->coefficient(VariableID(it_to)) == 0 || it_cs->coefficient(VariableID(it_from)) == 0) { // Only one implied
                   no_dep = false;
                   break;
                 }
                 else {
                   for (int i = 0; i < it1->second.second + it2->second.second; i++) {
                     no_dep = (i == it_to) || (i == it_from) || (it_cs->coefficient(VariableID(i)) == 0);
                     if (no_dep == false) break;
                   }
                   no_dep = it_cs->inhomogeneous_term().get_si() == 0;
                 }
               }
             }
             if (no_dep == false) break;
          }
        }
        if (!no_dep && stmt_list.size() > 0) {
          getIteratorAndBounds(stmt_list, iterators_bounds, iterators_bounds_expression, ordered_iterator);
          unsigned nbr_for = 0;
          for (std::vector<SgStatement *>::iterator it = stmt_list.begin(); it != stmt_list.end(); it++) if (isSgForStatement(*it)) nbr_for++;
          generated_kernel.push_back(new Kernel(stmt, nbr_for, iterators_bounds, iterators_bounds_expression, ordered_iterator, analysis));
        }
        else {
          if (no_dep) stmt_list.push_back(for_stmt);
          genKernel(
                     for_stmt->get_loop_body(),
                     no_dep ? dim_used-1 : dim_used,
                     stmt_list,
                     generated_kernel,
                     iterators_bounds,
                     iterators_bounds_expression,
                     ordered_iterator,
                     analysis
                   );
        }
        break;
      }
      case V_SgExprStatement:
      {
        if (stmt_list.size() > 0) {
          getIteratorAndBounds(stmt, iterators_bounds, iterators_bounds_expression, ordered_iterator);
          unsigned nbr_for = 0;
          for (std::vector<SgStatement *>::iterator it = stmt_list.begin(); it != stmt_list.end(); it++) if (isSgForStatement(*it)) nbr_for++;
          generated_kernel.push_back(new Kernel(stmt, nbr_for, iterators_bounds, iterators_bounds_expression, ordered_iterator, analysis));
        }
        break;
      }
      default:
        std::cout << stmt->class_name() << std::endl;
        ROSE_ASSERT(false);
    }
  }
}

/**
 * Traverse loop nest until a certain depth then generated kernels (via 'genKernel') and instantiate the kernel calls
 */
SgStatement * traverseLeftOutLoop(
                             SgStatement * stmt,
                             unsigned dim_out,
                             unsigned dim_used,
                             std::vector<SgStatement *> stmt_list,
                             std::vector<Kernel *> & generated_kernel_,
                             AnalysisContainer & analysis
) {
  if (dim_out == 0) {
    // We reach the depth of loop when we start generating kernels
    std::vector<Kernel *> generated_kernel;
    {
      std::map<SgVariableSymbol *, std::pair<int, int> > iterators_bounds;
      std::map<SgVariableSymbol *, std::pair<SgExpression *, SgExpression *> > iterators_bounds_expression;
      std::vector<SgVariableSymbol *> ordered_iterator;
      getIteratorAndBounds(stmt_list, iterators_bounds, iterators_bounds_expression, ordered_iterator);
      genKernel(stmt, dim_used, std::vector<SgStatement *>(), generated_kernel, iterators_bounds, iterators_bounds_expression, ordered_iterator, analysis);
    }

    if (generated_kernel.size() > 0) {
      // We generate the code for the kernel calls
      SgStatement * res = NULL;
      {
        if (generated_kernel.size() == 1) {
          res = generated_kernel[0]->genCall();
        }
        else {
          SgBasicBlock * bb = SageBuilder::buildBasicBlock();
          std::vector<Kernel *>::iterator it;
          for (it = generated_kernel.begin(); it != generated_kernel.end(); it++) {
            bb->append_statement((*it)->genCall());
          }
          res = bb;
        }
        // And add the new kernel to the list of all generated kernels
        generated_kernel_.insert(generated_kernel_.end(), generated_kernel.begin(), generated_kernel.end());
      }

      return res;
    }
    else
      return stmt;
  }

  switch (stmt->variantT()) {
    case V_SgBasicBlock:
    {
      SgBasicBlock * bb = isSgBasicBlock(stmt);
      std::vector<SgStatement *>::iterator it;
      SgStatement * first_stmt_for_sync = NULL;
      SgStatement * last_stmt_for_sync = NULL;
      std::vector<std::pair<SgStatement *, SgStatement *> > to_be_sync;
      for (it = bb->get_statements().begin(); it != bb->get_statements().end(); it++) {
        unsigned number_of_kernel = generated_kernel_.size();
        *it = traverseLeftOutLoop(*it, dim_out, dim_used, stmt_list, generated_kernel_, analysis);
        if (number_of_kernel == generated_kernel_.size()) {
          if (first_stmt_for_sync == NULL) last_stmt_for_sync = first_stmt_for_sync = *it;
          else last_stmt_for_sync = *it;
        }
        else if (first_stmt_for_sync != NULL) {
          to_be_sync.push_back(std::pair<SgStatement *, SgStatement *>(first_stmt_for_sync, last_stmt_for_sync));
          first_stmt_for_sync = NULL;
          last_stmt_for_sync = NULL;
        }
      }
      if (first_stmt_for_sync != NULL && first_stmt_for_sync != *(bb->get_statements().begin()))
        to_be_sync.push_back(std::pair<SgStatement *, SgStatement *>(first_stmt_for_sync, last_stmt_for_sync));
      std::vector<std::pair<SgStatement *, SgStatement *> >::iterator it_;
      for (it_ = to_be_sync.begin(); it_ != to_be_sync.end(); it_++)
        surroundBySync(it_->first, it_->second, analysis);
      return stmt;
    }
    case V_SgForStatement:
    {
      SgForStatement * for_stmt = isSgForStatement(stmt);
      stmt_list.push_back(for_stmt);
      for_stmt->set_loop_body(traverseLeftOutLoop(for_stmt->get_loop_body(), dim_out-1, dim_used, stmt_list, generated_kernel_, analysis));
      return for_stmt;
    }
    case V_SgExprStatement:
    {
      return stmt;
    }
    // I reject all other statement than for, basic block and expression statement
    default:
      std::cerr << "Found a " << stmt->class_name() << " in traverseLeftOutLoop()" << std::endl;
      ROSE_ASSERT(false);
  }
}

/**
 * Build an OpenCL file and add it to the project to be unparse
 */
SgSourceFile * buildOpenCLfile(std::string file_name, SgProject * project) {
  SgSourceFile * ocl_file = new SgSourceFile();
  {
    ocl_file->set_unparse_output_filename(file_name);
    ocl_file->set_file_info(new Sg_File_Info(file_name));

    project->get_fileList_ptr()->get_listOfFiles().push_back(ocl_file);

    SgGlobal * gs = new SgGlobal();
    gs->set_file_info(new Sg_File_Info(file_name));
    ocl_file->set_globalScope(gs);

    ocl_file->set_OpenCL_only(true);
    ocl_file->set_outputLanguage(SgFile::e_C_output_language); // FIXME could be remove with older version of rose
  }
  ROSE_ASSERT(ocl_file != NULL);
  return ocl_file;
}

AnalysisContainer::AnalysisContainer() :
  polyhedral_analysis_success(false),
  arrays(),
  scalars(),
  base_func(),
  dependencies(NULL)
{}

Scalar::Scalar(std::string name_) :
  name(name_),
  type(NULL),
  access_type(unknown)
{}

Array::Array(std::string name_) :
  name(name_),
  base_type(NULL),
  dimensions(),
  access_type(unknown),
  associated_ocl_buffer(NULL)
{}

/**
 * Analyze one of the kernel parameter to determine: type (scalar/array and float/int) size (memory footprint)
 */
void findVariableType(SgInitializedName * init_name, AnalysisContainer & analysis) {
  SgType * type = init_name->get_typeptr();
  if (isSgArrayType(type)) {
    std::map<SgInitializedName *, Array>::iterator it_array =
                       analysis.arrays.insert(std::pair<SgInitializedName *, Array>(init_name, Array(init_name->get_name().getString()))).first;
    Array & array = it_array->second;

    while (isSgArrayType(type)) {
      SgArrayType * array_type = isSgArrayType(type);

      SgExpression * index = array_type->get_index();
      ROSE_ASSERT(index != NULL);
      
      SgUnsignedLongVal * dim_size = isSgUnsignedLongVal(index);
      ROSE_ASSERT(dim_size != NULL); //TODO different int types

      type = array_type->get_base_type();
      ROSE_ASSERT(type != NULL);

      array.dimensions.insert(array.dimensions.begin(), dim_size->get_value());
    }
    array.base_type = type;
  }
  else {
    std::map<SgInitializedName *, Scalar>::iterator it_scalar =
                       analysis.scalars.insert(std::pair<SgInitializedName *, Scalar>(init_name, Scalar(init_name->get_name().getString()))).first;
    Scalar & scalar = it_scalar->second;
    scalar.type = type;
  }
}

AccessType compose(AccessType a1, AccessType & a2) {
  if (a1 == a2) return a1;
  if (a1 == inout  || a2 == inout) return inout;
  if (a1 == unknown) return a2;
  if (a2 == unknown) return a1;
  return inout;
}

void findVariableAccessType(std::vector<SgVarRefExp *> & var_refs, AnalysisContainer & analysis) {
  std::vector<SgVarRefExp *>::iterator it_var_ref;
  for (it_var_ref = var_refs.begin(); it_var_ref != var_refs.end(); it_var_ref++) {
    SgInitializedName * init_name = (*it_var_ref)->get_symbol()->get_declaration();
    std::map<SgInitializedName *, Scalar>::iterator it_scalar = analysis.scalars.find(init_name);
    std::map<SgInitializedName *, Array>::iterator it_array = analysis.arrays.find(init_name);

    if (it_scalar == analysis.scalars.end() && it_array == analysis.arrays.end()) continue;

    SgExpression * parent = isSgExpression((*it_var_ref)->get_parent());
    SgExpression * exp = *it_var_ref;
    while (isSgPntrArrRefExp(parent) != NULL) {
      exp = parent;
      parent = isSgExpression(parent->get_parent());
    }
    ROSE_ASSERT(parent != NULL); 

    AccessType a = unknown;
    switch (parent->variantT()) {
      case V_SgAssignOp:
      {
        SgAssignOp * assign = isSgAssignOp(parent);
        if (assign->get_lhs_operand_i() == exp) a = out;
        else a = in;
        break;
      }
      case V_SgPlusPlusOp:
      case V_SgMinusMinusOp:
        a = inout;
        break;
      default:
        a = in;
    }

    if (it_scalar != analysis.scalars.end()) {
      it_scalar->second.access_type = compose(it_scalar->second.access_type, a);
      ROSE_ASSERT(it_scalar->second.access_type == in); // a scalar parameter should never be write
    }
    else if (it_array != analysis.arrays.end()) {
      it_array->second.access_type = compose(it_array->second.access_type, a);
    }
  }
}

