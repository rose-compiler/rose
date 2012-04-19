
#include "opencl-from-loops.hpp"
#include "rose-utils.hpp"

void GenerateSPMD(
                     SgStatement * first,
                     SgStatement * last, 
                     AnalysisContainer & analysis,
                     SPMD_Driver & driver,
                     SPMD_Generator & generator
) {
  ScopTree * root = new ScopRoot(analysis.base_func);
  try {
    int cnt = 0;
    SgStatement * current = first;
    while (current != NULL) {
      cnt += Traverse<SgStatement>(current, root, cnt, analysis.base_func);
      current = current != last ? SageInterface::getNextStatement(current) : NULL;
    }

    root->Traverse();

    PolyhedricAnnotation::PolyhedralProgram<SgStatement, SgExprStatement, RoseVariable> & polyhedral_program =
      PolyhedricAnnotation::getPolyhedralProgram<SgStatement, SgExprStatement, RoseVariable>(analysis.base_func);

    polyhedral_program.finalize();

    const std::vector<SgExprStatement *> & exps = polyhedral_program.getExpressions();
    std::vector<SgExprStatement *>::const_iterator it;
    for (it = exps.begin(); it != exps.end(); it++) {
      PolyhedricAnnotation::DataAccess<SgStatement, SgExprStatement, RoseVariable> & data_access =
        PolyhedricAnnotation::getDataAccess<SgStatement, SgExprStatement, RoseVariable>(*it);
      PolyhedricAnnotation::makeAccessAnnotation<SgStatement, SgExprStatement, RoseVariable>(*it, data_access);
    }
  }
  catch (Exception::ExceptionBase & e) {
    e.print(std::cerr);
    analysis.polyhedral_analysis_success = false;
  }

  SPMD_Tree * res
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

