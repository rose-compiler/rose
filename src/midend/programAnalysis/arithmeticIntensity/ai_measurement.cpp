#include "sage3basic.h"
#include "inliner.h"
#include "ai_measurement.h"
#include "constantFolding.h"

using namespace std;
using namespace Rose;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

// synthesized attribute to evaluate expressions like 7*sizeof(float) + 5 * sizeof(double) 
// 2*sizeof(float) + 5* sizeof(double)
class IntExpressionEvaluationAttribute
{
 public: 
  int newValue; // load store bytes are integer values
  IntExpressionEvaluationAttribute():newValue(0){};
  IntExpressionEvaluationAttribute (const IntExpressionEvaluationAttribute &X):newValue(X.newValue) {};
};

class IntExpressionEvaluationTraversal: public SgBottomUpProcessing<IntExpressionEvaluationAttribute>
{
  public:
    IntExpressionEvaluationAttribute evaluateSynthesizedAttribute (SgNode* n, SubTreeSynthesizedAttributes synthesizedAttributeList);
};

#if 0
// midend/programTransformation/constantFolding/constantFolding.C has a more complete calculate_t()
// we move that one into constantFolding.h
// For T type which is compatible for all binary operators we are interested in.
template<typename T>
T calculate_t (SgBinaryOp* binaryOperator, T lhsValue, T rhsValue)
{
  T foldedValue; // to be converted to result type   
  switch (binaryOperator->variantT())
  {
    // integer-exclusive oprations
    case V_SgAddOp:
      {
        foldedValue = lhsValue + rhsValue;
        break;
      }
    case V_SgMultiplyOp:
      {
        foldedValue = lhsValue * rhsValue;
        break;
      }
    default:
      {
        cerr<<"warning: calculuate - unhandled operator type:"<<binaryOperator->class_name()<<endl;
        ROSE_ASSERT(false); // the expression is very limited form. We try to be restrictive here. //Not every binary operation type can be evaluated
      }
  }// end switch
  return foldedValue; 
} 
// For T type which is compatible for all unary operators we are interested in.
template<typename T>
T calculate_u_t (SgUnaryOp* unaryOperator, T theValue)
{
  T foldedValue; // to be converted to result type
  switch (unaryOperator->variantT())
  {
#if 0
    // integer-exclusive oprations
    case V_SgMinusOp: // this should not appear in our size expressions.
      {
        foldedValue = -theValue;
        break;
      }
#endif
   default:
      {
        cerr<<"warning: calculuate - unhandled operator type:"<<unaryOperator->class_name()<<endl;
        ROSE_ASSERT(false); // the expression is very limited form. We try to be restrictive here. //Not every binary operation type can be evaluated
      }
  }
  return foldedValue;
}

#endif 

// 7*sizeof(float) + 5 * sizeof(double)
IntExpressionEvaluationAttribute
IntExpressionEvaluationTraversal::evaluateSynthesizedAttribute ( SgNode* astNode, SubTreeSynthesizedAttributes synthesizedAttributeList)
{
  IntExpressionEvaluationAttribute returnAttribute;

  SgExpression* exp = isSgExpression(astNode);
  if (exp != NULL)
  {
    if (SgSizeOfOp * sop = isSgSizeOfOp(exp))
    {
       SgType* t = sop->get_operand_type();
       returnAttribute.newValue = ArithmeticIntensityMeasurement::getSizeOf (t);
    }
    else if (SgBinaryOp* bop = isSgBinaryOp(exp) )
    {
      int lhsvalue, rhsvalue;
      lhsvalue =  synthesizedAttributeList[SgBinaryOp_lhs_operand_i].newValue;
      rhsvalue =  synthesizedAttributeList[SgBinaryOp_rhs_operand_i].newValue;
      returnAttribute.newValue = ConstantFolding::calculate_t ( bop, lhsvalue, rhsvalue);
    }
    else if (SgValueExp* vexp = isSgValueExp(exp))
    {
      returnAttribute.newValue = ArithmeticIntensityMeasurement::get_int_value (vexp);
    }
    else // propagate the result for others
    {
      cerr<<"error, unhandled case in IntExpressionEvaluationTraversal::evaluateSynthesizedAttribute() "<<astNode->class_name() <<endl;
    }
  }
  else
  {
      cerr<<"error, non-expression is encountered in IntExpressionEvaluationTraversal::evaluateSynthesizedAttribute() "<<astNode->class_name() <<endl;
  }
  
//  cout<<"debugging IntExpressionEvaluationTraversal::evaluateSynthesizedAttribute() synth value is "<< returnAttribute.newValue << " for" <<astNode->class_name()<<endl;
  return returnAttribute;

}

//----------------------------------------
namespace ArithmeticIntensityMeasurement
{
  running_mode_enum running_mode = e_analysis_and_instrument;

  std::map <SgNode*, bool> FPVisitMAP; // record if a flop operation is counted or not

  //we have to be more specific, if a variable is processed and the variable is within a inner loops
  //then we skip it's counting in outer loop for load/store
  //The best case is we have access to all variable references, in addition to SgInitializedName for Side Effect Analysis
  std::map <SgNode*, std::set<SgInitializedName*> > LoopLoadVariables; // record processed read variables for a loop
  std::map <SgNode*, std::set<SgInitializedName*> > LoopStoreVariables; // record processed write variables for a loop

  // helper array to conver to string
  const char* FPOpKindNameList[] =
  {
    "e_unknown", 
    "e_total", 
    "e_plus", 
    "e_minus", 
    "e_multiply", 
    "e_divide" 
  };

  int get_int_value(SgValueExp * sg_value_exp)
  {
    int rtval;
    ROSE_ASSERT (sg_value_exp!=NULL);
    if (isSgBoolValExp(sg_value_exp))
      rtval = isSgBoolValExp(sg_value_exp)->get_value();
    else if (isSgCharVal(sg_value_exp))
      rtval = isSgCharVal(sg_value_exp)->get_value();
    else if (isSgEnumVal(sg_value_exp))
      rtval = isSgEnumVal(sg_value_exp)->get_value();
    else if (isSgIntVal(sg_value_exp))
      rtval = isSgIntVal(sg_value_exp)->get_value();
    else if (isSgShortVal(sg_value_exp))
      rtval = isSgShortVal(sg_value_exp)->get_value();
    else
    {
      cerr<<"error: wrong value exp type for cf_get_int_value():"<<sg_value_exp->class_name()<<endl;
      ROSE_ASSERT(false);
    }
    return rtval;
  }


  std::string toString (fp_operation_kind_enum op_kind)
  {
    std::string rt; 
    return string(FPOpKindNameList[op_kind]);
  } 
  bool debug;
  bool e_inline;
  int algorithm_version = 1;

  // default file name to store the report
  string report_filename = "ai_tool_report.txt";

  int loop_id = 0;

  bool isAssignmentStmtOf (SgStatement* stmt, SgInitializedName* init_name)
  {

    bool rt = false;

    ROSE_ASSERT (stmt != NULL);
    ROSE_ASSERT ( init_name != NULL);
    if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
    {
      if (SgAssignOp * assign_op = isSgAssignOp (exp_stmt->get_expression()))
      {
        if (SgVarRefExp* var_exp = isSgVarRefExp (assign_op->get_lhs_operand()) )
        {
          if (var_exp->get_symbol()->get_declaration() == init_name)
            rt = true;
        }
      }
    }
    return rt;
  }

  string FPCounters::toString(std::string comment)
  {
    stringstream ss; 
    ss<<"/////////////////////////////////////////////"<<endl;
  //  ss<<"Floating Point Operation Counts:"<<endl;
    if (comment.size()!=0) 
      ss<<comment<<endl;
    if (node != NULL)
    {
      ss<<node->class_name() <<"@";
      //debugging
     // if (!isSgForStatement(node))
     //   ROSE_ASSERT (false); // we cannot assert this. we pass loop body to the counting function. The body can be any types of statements

      ss<< node->get_file_info()->get_filename()<<":"<<node->get_file_info()->get_line()<<":"<<node->get_file_info()->get_col() <<endl;
    }
    else
      ss<< "NULL node"<<endl;
   // if non-zero, send out error code   

//    if (error_code!=0)
    ss<<"Error Code:"<<error_code<<endl;

    ss<<"\tFP_plus:"<< plus_count<<endl;
    ss<<"\tFP_minus:"<< minus_count<<endl;
    ss<<"\tFP_multiply:"<< multiply_count<<endl;
    ss<<"\tFP_divide:"<< divide_count<<endl;
    ss<<"\tFP_total:"<< getTotalCount()<<endl;

//    ss<<"Memory Operation Counts:"<<endl;
    if (load_bytes== NULL)
      ss<<"\tMem Load Byte Count Formula: NULL "<<endl;
    else
      ss<<"\tMem Load Byte Count Formula:"<< load_bytes->unparseToString()<<endl;
    ss<<"\tMem Loads Byte Count Value: "<<load_bytes_int<<endl;

    if (store_bytes== NULL)
      ss<<"\tMem Stores Byte Count Formula: NULL "<<endl;
    else
      ss<<"\tMem Stores Byte Count Formula:"<< store_bytes->unparseToString()<<endl;
    ss<<"\tMem Store Byte Count Value: "<<store_bytes_int<<endl;
    ss<<"Arithmetic Intensity="<<intensity<<endl;

    return ss.str();
  }

  void FPCounters::printInfo(std::string comment/* ="" */)
  {
    cout<<toString(comment);
  }

  // a transformation to instrument loops to obtain loop iteration counts at runtime

  bool FPCounters::consistentWithReference(FPCounters* refCounters)
  {
    //We should not use getTotalCount() since it calcultes the total count on the fly!
    bool rt = true;
    if (refCounters->getRawTotalCount() != 0 )
      if (total_count != refCounters->getRawTotalCount()) rt = false;

    if (refCounters->getPlusCount() != 0 )
      if (plus_count != refCounters->getPlusCount()) rt = false;

    if (refCounters->getMinusCount() != 0 )
      if (minus_count != refCounters->getMinusCount()) rt = false;

    if (refCounters->getMultiplyCount() != 0 )
      if (multiply_count != refCounters->getMultiplyCount()) rt = false;

    if (refCounters->getDivideCount() != 0 )
      if (divide_count != refCounters->getDivideCount()) rt = false;

    return rt; 
  }

  void FPCounters::addCount (fp_operation_kind_enum c_type, int i )
  {
    switch (c_type)
    {
      case e_total: 
        cerr<<"FPCounters::addCount(): adding to total FP count is not allowed. Must add to counters of specific operation (+, -, *, or /)!"<<endl;
        assert (false);
        break;
      case e_plus:
        addPlusCount(i);
        break;
      case e_minus:
        addMinusCount(i);
        break;
      case e_multiply:
        addMultiplyCount(i);
        break;
      case e_divide:
        addDivideCount(i);
        break;
      default:
        {
          //TODO : we should ignore some unrecognized op kind
          //Another case list to ignore them one by one
          cerr<< ArithmeticIntensityMeasurement::toString(c_type) <<endl; 
          assert (false);  
          break;
        }
    }
  }

  // used for storing parsed counter numbers from pragmas
  void FPCounters::setCount (fp_operation_kind_enum c_type, int i )
  {
    switch (c_type)
    {
      case e_total: 
        if (total_count == 0)
        {
          total_count = i;
        }
        else
        {
          cerr<<"FPCounters::setCount(): adding total count to a none zero existing value, possibly overwritting it!"<<endl;
          assert (false);
        }
        break;
      case e_plus:
        if (plus_count == 0)
        {
          plus_count = i;
        }
        else
        {
          cerr<<"FPCounters::setCount(): adding plus count to a none zero existing value, possibly overwritting it!"<<endl;
          assert (false);
        }
        break;
      case e_minus:
        if (minus_count == 0)
        {
          minus_count = i;
        }
        else
        {
          cerr<<"FPCounters::setCount(): adding minus count to a none zero existing value, possibly overwritting it!"<<endl;
          assert (false);
        }
        break;
      case e_multiply:
        if (multiply_count == 0)
        {
          multiply_count = i;
        }
        else
        {
          cerr<<"FPCounters::setCount(): adding multiply count to a none zero existing value, possibly overwritting it!"<<endl;
          assert (false);
        }
        break;
      case e_divide:
        if (divide_count == 0)
        {
          divide_count = i;
        }
        else
        {
          cerr<<"FPCounters::setCount(): adding divide count to a none zero existing value, possibly overwritting it!"<<endl;
          assert (false);
        }
        break;
      default:
        assert (false);  
    }
  }

  float FPCounters::getIntensity()
  {
    //if already calculated, return it directly
    if (intensity != -1.0)
      return intensity;

    //Otherwise do the evaluation: operations/bytes
    IntExpressionEvaluationTraversal trav1, trav2;
    IntExpressionEvaluationAttribute a1, a2;
    a1 = trav1.traverse (load_bytes);
    a2 = trav2.traverse (store_bytes);
    load_bytes_int = a1.newValue;
    store_bytes_int = a2.newValue;
    int total_bytes = load_bytes_int + store_bytes_int;

    if (total_bytes !=0)
       intensity = (float)total_count/total_bytes;
    else if (total_count ==0 )
       intensity = 0.0 ;  
    else  //  non-zero_FP operations divided by zero mem access bytes
       intensity = 99999.9;  //max value
    return intensity;
  }


  int FPCounters::getCount (fp_operation_kind_enum c_type = e_total)
  {
    switch (c_type)
    {
      case e_total: 
        return getTotalCount();
        break;
      case e_plus:
        return getPlusCount();
        break;
      case e_minus:
        return getMinusCount();
        break;
      case e_multiply:
        return getMultiplyCount();
        break;
      case e_divide:
        return getDivideCount();
        break;
      default:
        assert (false);  
    }
    assert (false);  
    return 0;
  }

  FPCounters * getFPCounters (SgLocatedNode* n)
  {
    assert (n!= NULL);
    FPCounters * fp_counters = NULL; 
    if (n->attributeExists("FPCounters")) 
    {
      AstAttribute* attr = n->getAttribute("FPCounters");
      fp_counters = dynamic_cast<FPCounters* > (attr);
    }
    else
    {
      fp_counters = new FPCounters (n);
      assert (fp_counters != NULL);
      n->setAttribute("FPCounters", fp_counters);
    }

    assert(n->attributeExists("FPCounters")); 
    assert (fp_counters != NULL);

    return fp_counters;
  }

  // Helper function to manipulate the attribute
  // Get a FP operation count from node n.
  int getFPCount (SgLocatedNode* n, fp_operation_kind_enum c_type)
  {
    assert (n != NULL);
    FPCounters * fp_counters = getFPCounters (n);
    return fp_counters ->getCount(c_type);
  }

  void printFPCount (SgLocatedNode* n)
  {
    assert (n != NULL);
    FPCounters * fp_counters = getFPCounters (n);
    fp_counters ->printInfo();

  }
  void addFPCount (SgLocatedNode* n, fp_operation_kind_enum c_type, int i/* =1 */)
  {
    assert (n != NULL);
    FPCounters * fp_counters = dynamic_cast<FPCounters* > (n->getAttribute("FPCounters"));
    if (fp_counters == NULL ) 
    {
      fp_counters = new FPCounters (n);
      assert (fp_counters != NULL);
      n->setAttribute("FPCounters", fp_counters);
    }

    return fp_counters ->addCount(c_type, i);
  }

  // ! A helper function to check scalar vs. array types
  static string scalar_or_array(SgType* t)
  {
    assert (t!= NULL);
    string scalar_or_array;
    if (isScalarType(t))
      scalar_or_array = "Scalar";
    else if (isSgArrayType(t))
      scalar_or_array = "Array";
    else
    {
      cerr<<"Error. scalar_or_array(SgType*) encounters a type which is neither a scalar nor array type!" << t->class_name()<<endl;
    }
    return scalar_or_array;
  }

  //! Estimate the size of some types, workaround of sizeof
  //Assuming 64-bit Linux machine
  // http://docs.oracle.com/cd/E19957-01/805-4939/z40007365fe9/index.html 
  int getSizeOf(SgType* t)
  {
    int rt =0; 
    assert (t!=NULL);

    // strip off typedefs, reference, modifiers
    t = t->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_RVALUE_REFERENCE_TYPE|SgType::STRIP_TYPEDEF_TYPE);

    // Fortran allow type_kind for types, read this first
    if (SgExpression* kind_exp =t->get_type_kind() )
    {
      SgIntVal* int_val = isSgIntVal (kind_exp);
      if (int_val != NULL)
        rt = int_val->get_value();
      else
      {
        cerr<<"Error in getSizeOf(), only SgIntVal type_kind is handled. Unhandled type_kind "<< kind_exp->class_name()<<endl;
        assert(false);
      }  
    } 
    else
    {
      switch (t->variantT())
      {
        case V_SgTypeDouble:
          rt = 8;
          break;
        case V_SgTypeInt:
          {
            rt = 4;
            break;
          }
        case V_SgTypeFloat:
          rt = 4;
          break;
        default:
          {
            cerr<<"Error in getSizeOf() of ai_measurement.cpp . Unhandled type: "<<t->class_name()<<endl;
            assert (false);
          }
      }
    }
    return rt; 

  }


  // obtain read or write variables processed by all nested loops, if any
  void getVariablesProcessedByInnerLoops (SgStatement* current_loop_body, bool isRead, std::set<SgInitializedName*>& var_set)
  {
    // AST query to find all loops
    // add all read/write variables into the var_set
    VariantVector vv;
    vv.push_back(V_SgForStatement);  
    vv.push_back(V_SgFortranDo);  
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(current_loop_body, vv); 
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      SgStatement* loop = isSgStatement(*i);
      if (debug)
        cout<< "Found nested loop at line:"<< loop->get_file_info()->get_line()<<endl;
      std::set<SgInitializedName*> src_var_set ;
      if (isRead)
        src_var_set = LoopLoadVariables[loop];
      else
        src_var_set = LoopStoreVariables[loop];
      std::set<SgInitializedName*>::iterator j; 
      if (debug)
        cout<< "\t Insert processed variable:"<<endl;
      for (j= src_var_set.begin(); j!= src_var_set.end(); j++)
      {
        var_set.insert(*j);
        if (debug)
          cout<< "\t \t "<<(*j)->get_name()<<endl;
      }
    }
  }
  //! Return an expression like 8*sizeof(int)+ 3*sizeof(float) + 5*sizeof(double) for a list of variables accessed (either read or write)
  // For array variable, we should only count a single element access, not the entire array size
  // Algorithm:  
  //   Iterate on each variable in the set
  //     group them into buckets based on types, using a map<SgType*, int> to store this
  //   Iterate the list of buckets to generate count*sizeof(type) + .. expression 
  SgExpression* calculateBytes (std::set<SgInitializedName*>& name_set, SgStatement* lbody, bool isRead)
  {
    SgExpression* result = NULL; 
    if (name_set.size()==0) return result;

    // the input is essentially the loop body, a scope statement
    ROSE_ASSERT (lbody!= NULL);
    // We need to record the associated loop info.
    SgStatement* loop= NULL;
    SgForStatement* forloop = isSgForStatement(lbody->get_scope());
    SgFortranDo* doloop = isSgFortranDo(lbody->get_scope());

    if (forloop)
      loop = forloop;
    else if (doloop)
      loop = doloop;
    else
    {
      //support RAJA::forall loop, which may be a function definition only
      loop = lbody->get_scope();
//      cerr<<"Error in CountLoadStoreBytes (): input is not loop body type:"<< lbody->class_name()<<endl;
//      assert(false);
    }   

    std::map<SgType* , int> type_based_counters; 

    // get all processed variables by inner loops
    std::set<SgInitializedName*> processed_var_set; 
    // We only exclude inner loop's variables in analysis&instrumentation mode
    // The problem of redundant accumulation during execution happens only in this mode.
    // The static analysis only mode does not have this concern.
    // We want to be able to statically estimate
    // if (running_mode == e_analysis_and_instrument)
    getVariablesProcessedByInnerLoops (lbody, isRead, processed_var_set);

    // fill in the type-based counters
    std::set<SgInitializedName*>::iterator set_iter; 
    for (set_iter = name_set.begin(); set_iter != name_set.end(); set_iter++)
    {
      SgInitializedName* init_name = *set_iter; 
      // skip visited variable when processing inner loops
      // some global variables may be visited by another function
      // But we should count it when processing the current function!
      //
      // We group all references to a same variable into one reference for now
      // if a variable is considered when processing inner loops, the variable
      // will be skipped when processing outer loops.
      if (isRead)
      {
        // if inner loops already processed it, skip it
        if (processed_var_set.find(init_name) != processed_var_set.end())
          continue; 
        else
          LoopLoadVariables[loop].insert(init_name); 
      }
      else
      {
        if (processed_var_set.find(init_name) != processed_var_set.end())
          continue; 
        else
          LoopStoreVariables[loop].insert(init_name); 
      }
      // It is tricky here, TODO consider pointer, typedefs, reference, modifier types
      SgType* stripped_type = (*set_iter)->get_type()->stripTypedefsAndModifiers();
      SgType* base_type = NULL; 
      if (isScalarType(stripped_type))
        base_type = stripped_type;
      else if (isSgArrayType(stripped_type))
      {  // we may have multi-dimensional arrays like int a[][][];
        base_type = stripped_type; 
        do {
          base_type = isSgArrayType(base_type)->get_base_type(); 
        } while (isSgArrayType (base_type));
      }
      else if (isSgPointerType (stripped_type)) // we now allow pointer types used as arrays like double *a..,;  a[i]= ..
      {
        base_type = stripped_type;
        do {
          base_type = isSgPointerType(base_type)->get_base_type(); 
        } while (isSgPointerType (base_type));
      }
      else
      {
        cerr<<"Error in calculateBytes(). Unhandled stripped type:"<<stripped_type->class_name()<<endl;
        assert (false);
      }

      type_based_counters[base_type] ++; 
    } // end for

    // use the type-based counters for byte calculation
    std::map<SgType* , int>::iterator citer; 
    //It is possible now to have zero after filtering out redundant variables
    //assert (type_based_counters.size()>0);
    for (citer = type_based_counters.begin(); citer !=type_based_counters.end(); citer ++)
    {
      SgType* t = (*citer).first;
      // at this point, we should not have array types any more
      ROSE_ASSERT (isSgArrayType (t) == NULL); 
      int count = (*citer).second; 
      assert (t != NULL);
      assert (count>0);
      SgExpression* sizeof_exp = NULL; 
      if (is_Fortran_language())
      {
#if 0  // this does not work. cannot find func symbol for sizeof()       
        // In Fortran sizeof() is a function call, not  SgSizeOfOp.
        // type name is a variable in the AST, 
        // Too much trouble to build 
        assert (scope !=NULL);
        // This does not work
        //SgFunctionSymbol* func_sym = lookupFunctionSymbolInParentScopes(SgName("sizeof"), scope);
        SgGlobal* gscope = getGlobalScope (scope);
        assert (gscope !=NULL);
        SgFunctionSymbol* func_sym = gscope->lookup_function_symbol(SgName("sizeof"));
        assert (func_sym!=NULL);
        SgVarRefExp* type_var = buildVarRefExp( t->unparseToString(), scope );
        assert (type_var !=NULL);
        sizeof_exp = buildFunctionCallExp (func_sym, buildExprListExp(type_var));
#else
        // sizeof is not an operator in Fortran, there is no unparsing support for this
        // sizeof_exp = buildSizeOfOp(t);
        // Directly obtain an integer size value
        sizeof_exp = buildIntVal(getSizeOf(t));
#endif         
      }
      else if (is_C_language() || is_C99_language() || is_Cxx_language())
      {
        sizeof_exp = buildSizeOfOp(t);
      }
      else
      {
        cerr<<"Error in calculateBytes(). Unsupported programming language other than C/Cxx and Fortran. "<<endl;
        assert (false);
      }
      SgExpression* mop = buildMultiplyOp(buildIntVal(count), sizeof_exp);
      if (result == NULL)
        result = mop; 
      else 
        result = buildAddOp(result, mop);
    }
    return result; 
  }  


  //! check if a pointer is ever used as an array
  // Check if there is a SgVarRefExp of this init. And the SgVarRefExp shows up as part of lhs operator of SgPntrArrRefExp. 
  // We cannot just compare lhs, since sometimes there is this->a  involved. 
  // TODO: put into SageInterface
  // We pass a scope parameter to narrow down the search scope. Otherwise we have to search all member function declarations within a class definition
  bool pointerUsedAsArray (SgInitializedName* iname, SgScopeStatement* scope)
  {
    bool rt = false;
//    SgScopeStatement* scope = iname->get_scope();
    ROSE_ASSERT (scope != NULL);
    // find all SgPntrArrRefExp, find all lhs operators
    // This cannot penetrate nested member function declarations within a scope of type SgClassDefinition
    vector<SgPntrArrRefExp* > array_vec = querySubTree<SgPntrArrRefExp> (scope);

    // filter out SgPntrArrRefExp which is not the bottom SgPntrArrRefExp of a multi dimensional array
    vector<SgPntrArrRefExp*> bottom_arrays; 
    for (vector<SgPntrArrRefExp* >::iterator iter = array_vec.begin(); iter != array_vec.end(); iter++)
    {
      SgPntrArrRefExp* current = *iter;
      // lhs is another array, skip
      if (isSgPntrArrRefExp (current->get_lhs_operand_i()))
        continue;
      else
        bottom_arrays.push_back(current);
    }

    // Now for each bottom arrayRefExp, search its lhs for SgVarRefExp which points to the iname
    for (vector<SgPntrArrRefExp* >::iterator iter = bottom_arrays.begin(); iter != bottom_arrays.end(); iter++)
    {
      SgPntrArrRefExp* current = *iter;
      vector<SgVarRefExp* > refs = querySubTree<SgVarRefExp> (current->get_lhs_operand_i());
      for (vector<SgVarRefExp* >::iterator iter2 = refs.begin(); iter2!= refs.end(); iter2 ++)
      {
        SgVarRefExp* cref = *iter2; 
        if (cref->get_symbol()->get_declaration()== iname)
        {
          return true;
        }
      }
    }

    return rt; 
  }


  // Only keep desired types
  // Sometimes pointer typed variables are used as array, such as double* a = new ..., later a[i] = xx, etc
  // provide the search scope, otherwise we have to penetrate member function declarations within a class definition, which is the scope of iname
  std::set<SgInitializedName* > filterVariables(const std::set<SgInitializedName* > & input, SgScopeStatement* scope)
  {
    std::set<SgInitializedName* > result; 
    std::set<SgInitializedName*>::iterator it;
    ROSE_ASSERT (scope != NULL);

    for (it=input.begin(); it!=input.end(); it++)
    {
      SgInitializedName* iname = (*it);
      if (iname ==NULL) continue; // this pointer of a class has NO SgInitializedName associated !!
      if (isSgArrayType (iname->get_type()) || pointerUsedAsArray (iname, scope) )
        result.insert(iname);
      //    cout<<scalar_or_array (iname->get_type()) <<" "<<iname->get_name()<<"@"<<iname->get_file_info()->get_line()<<endl;
    }
    return result; 
  }

  // Count the load and store bytes for the 
  // I think we can only return expressions to calculate the value, not the actual values,
  // since sizeof(type) is machine dependent
  //   Consider both scalar and  array accesses by default. Consider both floating point and integer types by default.
  // return a pair of expressions:  
  //       load_byte_exp, and 
  //       store_byte_exp
  // Algorithm: 
  //    1.  Call side effect analysis to find read/write variables, some reference may trigger both read and write accesses
  //        Accesses to the same array/scalar variable are grouped into one read (or write) access
  //         e.g. array[i][j],  array[i][j+1],  array[i][j-1], etc are counted a single access
  //    2.  Group accesses based on the types (same type?  increment the same counter to shorten expression length)
  //    4.  Iterate on the results to generate expression like  2*sizeof(float) + 5* sizeof(double)
  // As an approximate, we use simple analysis here assuming no function calls.
  //
  // Return false if side effect analysis fails.
  bool CountLoadStoreBytes (SgLocatedNode* input, 
      std::pair <SgExpression*, SgExpression*>& result,
    bool includeScalars /* = false */, bool includeIntType /* = false */)
  {
    assert (input != NULL);

    // the input is essentially the loop body, a statement
    SgStatement* lbody = isSgStatement(input);

#if 0 //RAJA::forall loops don't have classic loop nodes
    // We need to record the associated loop info.
    //SgStatement* loop= NULL;
    SgForStatement* forloop = isSgForStatement(lbody->get_scope());
    SgFortranDo* doloop = isSgFortranDo(lbody->get_scope());
    if (forloop)
    {
      //loop = forloop;
    }
    else if (doloop)
    {  
      //loop = doloop;
    }
    else
    {
      cerr<<"Error in CountLoadStoreBytes (): input is not loop body type:"<< input->class_name()<<endl;
      assert(false);
    }
#endif
    //Plan A: use and extend Qing's side effect analysis
    std::set<SgInitializedName*> readVars;
    std::set<SgInitializedName*> writeVars;

    bool success = SageInterface::collectReadWriteVariables (isSgStatement(input), readVars, writeVars);
    SgScopeStatement* scope = getEnclosingFunctionDefinition(input);
    ROSE_ASSERT (scope!= NULL);
    if (success!= true)
    {
      if (debug)
        cout<<"Warning: CountLoadStoreBytes(): failed to collect load/store, mostly due to existence of function calls inside of loop body @ "<<input->get_file_info()->get_line()<<endl;
      return false;
    }

    std::set<SgInitializedName*>::iterator it;
    if (debug)
      cout<<"debug: found read variables (SgInitializedName) count = "<<readVars.size()<<endl;
    for (it=readVars.begin(); it!=readVars.end(); it++)
    {
      SgInitializedName* iname = (*it);
      if (debug)
        cout<<scalar_or_array (iname->get_type()) <<" "<<iname->get_name()<<"@"<<iname->get_file_info()->get_line()<<endl;
    }

    if (!includeScalars )
      readVars =  filterVariables (readVars, scope);
    if (debug)
      cout<<"debug: found write variables (SgInitializedName) count = "<<writeVars.size()<<endl;
    for (it=writeVars.begin(); it!=writeVars.end(); it++)
    {
      SgInitializedName* iname = (*it);
      if (debug)
        cout<<scalar_or_array(iname->get_type()) <<" "<<iname->get_name()<<"@"<<iname->get_file_info()->get_line()<<endl;
    }
    if (!includeScalars )
      writeVars =  filterVariables (writeVars, scope);
    result.first =  calculateBytes (readVars, lbody, true);
    result.second =  calculateBytes (writeVars, lbody, false);

    return true;
  }

  // count memory load/store operations, store into attribute FPCounters
  void CountMemOperations(SgLocatedNode* input, bool includeScalars /*= false*/, bool includeIntType /*= false */)
  {
    ROSE_ASSERT (input != NULL);
    std::pair <SgExpression*, SgExpression*> load_store_count_pair; 
    bool success = CountLoadStoreBytes (input, load_store_count_pair, includeScalars, includeIntType);

    // retrieve the attribute attached to input, creating one if it does not exist
    FPCounters* mycounters = getFPCounters (input); 
    if (!success)
      mycounters->setErrorCode(1);
    else
    {
      mycounters->setLoadBytes (load_store_count_pair.first);
      mycounters->setStoreBytes (load_store_count_pair.second);
    }
  }
 
  FPCounters*  calculateArithmeticIntensity(SgLocatedNode* body, bool includeScalars /*= false */, bool includeIntType /*= false */)
  {
    ROSE_ASSERT (body != NULL);

    // on-demand inlining of function calls inside the code block rooted at body
    if (e_inline)
    {
      // inlining function calls inside the body
      Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(body, V_SgFunctionCallExp);
      for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
      {
        SgFunctionCallExp *call  = isSgFunctionCallExp(*i);
        doInline (call);
      }
    }

    CountFPOperations (body);
    CountMemOperations (body , includeScalars, includeIntType); 
    FPCounters* fp_counters = getFPCounters (body);
    fp_counters->getIntensity(); // calculate intensity by trying to get it
    return fp_counters;
  }


  //! Count floating point operations seen in a subtree
  void CountFPOperations(SgLocatedNode* input)
  {
    // find all binary operations
    Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(input, V_SgBinaryOp);
    for (Rose_STL_Container<SgNode *>::iterator i = nodeList.begin(); i != nodeList.end(); i++)
    {
      fp_operation_kind_enum op_kind = e_unknown; 
      //      bool isFPType = false;
      // check operation type
      SgBinaryOp* bop= isSgBinaryOp(*i);
      switch (bop->variantT())
      {
        case V_SgAddOp:
        case V_SgPlusAssignOp:
          op_kind = e_plus; 
          break;
        case V_SgSubtractOp:
        case V_SgMinusAssignOp:  
          op_kind = e_minus;
          break;        
        case V_SgMultiplyOp:
        case V_SgMultAssignOp:  
          op_kind = e_multiply;
          break;        
        case V_SgDivideOp:
        case V_SgDivAssignOp:  
          op_kind = e_divide;
          break;        
        default:
          break;  
      } //end switch

      // skip this expression if unknown operation kind 
      if (op_kind == e_unknown) continue; 

      // Check if the operation is on float point data type
      if (bop->get_type()->isFloatType())
      {
        // For instrumentation mode, we need to avoid double count FLOPs in inner loops
        // we assume the traverse is inside out, the inner loop will be processed first!
        // An operation is counted once when its innermost enclosing loop is processed. 
        // Using a map to avoid double counting an operation when it is enclosed in multiple loops
        //
        // For static counting only mode, this is a less concern.
        if (running_mode == e_static_counting)
        {
          addFPCount (input, op_kind);
        }
        else if (!FPVisitMAP[bop]) // dynamic accumulation mode, must avoid duplicated counting
        {
          addFPCount (input, op_kind);
          FPVisitMAP[bop] = true;
        }
      } 
    }  // end for

    //Must update the total counter here
    FPCounters* fp_counters = getFPCounters (input); 
    fp_counters->updateTotal ();
#if 0
    // write results to a report file
    if (running_mode == e_static_counting)
    {
      ofstream reportFile(report_filename.c_str(), ios::app);
      cout<<"Writing counter results to "<< report_filename <<endl;
      reportFile<< fp_counters->toString();
    }
#endif
    // debugging info
    if (debug)
      printFPCount (input);
  }


  // Parse a clause like fp_plus(10) , fp_multiply(1), etc.
  // return true and the parsed op kind and value if the attempt is successful. Otherwise return false.
  bool parse_fp_counter_clause(fp_operation_kind_enum* fp_op_kind, int *op_count)
  {
    // parse operation kind
    if (afs_match_substr("fp_plus"))
    {
      *fp_op_kind = e_plus; 
    }
    else if (afs_match_substr("fp_minus"))
    {
      *fp_op_kind = e_minus; 
    }
    else if (afs_match_substr("fp_multiply"))
    {
      *fp_op_kind = e_multiply; 
    }
    else if (afs_match_substr("fp_divide"))
    {
      *fp_op_kind = e_divide; 
    }
    else if (afs_match_substr("fp_total"))
    {
      *fp_op_kind = e_total; 
    }
    else
    {
      return false;
    }   

    // Now there is no turning point. Must succeed or assert failure. 
    //parse operation count value
    if (!afs_match_char('('))
    {
      cerr<<"Error in parse_aitool_pragma(): expected ( is not found! "<<endl;
      assert (false);
    }
    if (!afs_match_integer_const(op_count))
    {
      cerr<<"Error in parse_aitool_pragma(): expected integer value is not found! "<<endl;
      assert (false);
    }
    if (!afs_match_char(')'))
    {
      cerr<<"Error in parse_aitool_pragma(): expected ) is not found! "<<endl;
      assert (false);
    }

    if (debug)
      cout<<"parse_fp_counter_clause() found "<< toString(*fp_op_kind) <<" with value "<< *op_count<<endl;
    return true;
  }
  // define pragmas to indicated expected results in the code
  // The grammar of pragmas is
  //
  // arithmetic_intensity_pragma = '#pragma' 'aitool' | fp_counter_clause
  // fp_counter_clause = 'fp_plus' '(' INTEGER ')'  | 'fp_minus' '(' INTEGER ')' | 'fp_multiply' '(' INTEGER ')' |
  //                    'fp_divide' '(' INTEGER ')'   | 'fp_total' '(' INTEGER ')'
  //  This pragma indicate the number of FP operations for the followed statement (a loop mostly), without considering repetition.
  //  e.g. #pragma aitool fp_plus(3) fp_minus(3) fp_multiple (6) fp_total (12)   
  FPCounters* parse_aitool_pragma (SgPragmaDeclaration* pragmaDecl)
  {
    FPCounters* result = NULL;
    assert (pragmaDecl != NULL);
    assert (pragmaDecl->get_pragma() != NULL);
    string pragmaString = pragmaDecl->get_pragma()->get_pragma();
    // make sure it is side effect free
    const char* old_char = c_char;
    SgNode* old_node = c_sgnode;

    c_sgnode = SageInterface::getNextStatement(pragmaDecl);
    assert (c_sgnode != NULL);

    c_char = pragmaString.c_str();

    if (afs_match_substr("aitool"))
    { 
      result = new FPCounters (pragmaDecl);
      fp_operation_kind_enum fp_op_kind = e_unknown; 
      int op_count = 0; 
      while (parse_fp_counter_clause (& fp_op_kind, & op_count))
      {
        if (debug)
          cout<<"parse_aitool_pragma() set "<< toString(fp_op_kind) <<" with value "<<op_count<<endl;
        result->setCount (fp_op_kind, op_count); 
      }
    } 
    // may have incomplete info in the pragma
    if (result != NULL)
      result->updateTotal(); 
    // undo side effects
    c_char = old_char;
    c_sgnode = old_node;

    if (debug)
    { 
      if (result != NULL)
        result->printInfo();
    }

    return result;
  } // end parse_aitool_pragma

  // Create load/store = loads + iteration * load/store_count_per_iteration
  // lhs_exp = lhs_exp + iter_count_exp * per_iter_bytecount_exp
  SgExprStatement* buildByteCalculationStmt(SgVariableSymbol * lhs_sym, SgVariableSymbol* iter_count_sym, SgExpression* per_iter_bytecount_exp)
  {
    assert (lhs_sym != NULL);
    assert (iter_count_sym != NULL);
    assert (per_iter_bytecount_exp != NULL);
    SgExpression* lhs_exp = buildVarRefExp(lhs_sym);
    SgExpression* rhs_exp = buildAddOp( buildVarRefExp(lhs_sym),
        buildMultiplyOp (buildVarRefExp(iter_count_sym), per_iter_bytecount_exp) );
    return buildAssignStatement(lhs_exp, rhs_exp);
  }

  // Build counter accumulation statement like chloads = chloads + chiterations * (2 * 8)
  // chloads is the counter name, chiterations is iteration_count_name, 2*8 is the per-iteration count expression.
  SgExprStatement* buildCounterAccumulationStmt (std::string counter_name, std::string iteration_count_name, SgExpression* count_exp_per_iteration, SgScopeStatement* scope)
  {
    assert (scope!= NULL);
    assert (count_exp_per_iteration != NULL);
    assert (counter_name.size()!=0);
    assert (iteration_count_name.size()!=0);

    SgVariableSymbol * chiterations_sym = lookupVariableSymbolInParentScopes(SgName(iteration_count_name), scope);
    assert (chiterations_sym!=NULL);

    SgVariableSymbol * counter_sym = lookupVariableSymbolInParentScopes(SgName(counter_name), scope);
    assert (counter_sym!=NULL);
    SgExprStatement* counter_acc_stmt = buildByteCalculationStmt (counter_sym, chiterations_sym, count_exp_per_iteration);

    return counter_acc_stmt; 
  }

  //A generic function to check if a loop has a tag statement prepended to it, asking for instrumentation
  //If so, the loop will be instrumented and the tag statement will be returned.
  // This function supports both C/C++ for loops and Fortran Do loops
  SgStatement* instrumentLoopForCounting(SgStatement* loop)
  {
    //get scope of the loop
    assert (loop != NULL);
    SgForStatement* forloop = isSgForStatement(loop); 
    SgFortranDo* doloop = isSgFortranDo(loop);  
    SgScopeStatement* scope = NULL; 

    if (forloop)
      scope = forloop->get_scope();
    else if (doloop)
      scope = doloop->get_scope(); 
    else
    {
      cerr<<"Error in instrumentLoopForCounting(): Unrecognized loop type:"<< loop->class_name()<<endl;
      assert(false);
    }
    ROSE_ASSERT(scope != NULL);

    // Only for a do-loop which immediately follows  chiterations =  ..
    SgVariableSymbol * chiterations_sym = lookupVariableSymbolInParentScopes(SgName("chiterations"), isSgScopeStatement(loop));
    if (chiterations_sym==NULL) return NULL;
    SgStatement* prev_stmt = SageInterface::getPreviousStatement(loop,false);

    // backwards search, skipping pragma declaration etc.
    while (prev_stmt!=NULL && !isAssignmentStmtOf (prev_stmt, chiterations_sym->get_declaration()))
      prev_stmt = SageInterface::getPreviousStatement(prev_stmt,false);
    if (prev_stmt == NULL) return NULL;

    // To support nested loops, we need to use unique chiterations variable for each loop
    // otherwise the value stored in inner loop will overwrite the iteration count for the outerloop.
    loop_id ++; // increment loop ID
    // insert size_t chiterations_id ; 
    // Find the enclosing function declaration, including its derived instances like 
    //isSgProcedureHeaderStatement, isSgProgramHeaderStatement, and isSgMemberFunctionDeclaration. 
    SgFunctionDeclaration* func_decl = getEnclosingFunctionDeclaration   (loop); 
    ROSE_ASSERT (func_decl !=NULL);
    SgFunctionDefinition* func_def = func_decl->get_definition();
    ROSE_ASSERT (func_def !=NULL);
    SgBasicBlock* func_body = func_def->get_body();
    // insert a new variable declaration
    std::string new_iter_var_name = std::string("chiterations_") + StringUtility::numberToString(loop_id);
    SgVariableDeclaration* new_iter_var_decl = buildVariableDeclaration(new_iter_var_name, chiterations_sym->get_type(), NULL, func_body); 
    SgStatement* last_decl = findLastDeclarationStatement(func_body);
    if (last_decl!=NULL)
      insertStatementAfter (last_decl, new_iter_var_decl, false);
    else
      prependStatement(new_iter_var_decl, func_body);

    // rewrite the assignment stmt's left hand variable to be the new symbol
    SgExpression* lhs = NULL; 
    bool rt = isAssignmentStatement (prev_stmt, &lhs); 
    ROSE_ASSERT (rt == true);
    ROSE_ASSERT (lhs != NULL);
    SgVarRefExp* var_ref = isSgVarRefExp (lhs);
    ROSE_ASSERT (var_ref != NULL);
    var_ref->set_symbol(getFirstVarSym (new_iter_var_decl));

    SgStatement* loop_body = NULL; 
    if (forloop)
      loop_body = forloop->get_loop_body();
    else if (doloop)
      loop_body = doloop->get_body();
    assert (loop_body != NULL);     

    // count FP operations for each loop
    CountFPOperations (loop_body);
    //chflops=chflops+chiterations*n
    FPCounters* current_result = getFPCounters (loop_body);
    if (current_result->getTotalCount() >0)
    {
      SgExprStatement* stmt = buildCounterAccumulationStmt("chflops", new_iter_var_name , buildIntVal(current_result->getTotalCount()),scope);
      insertStatementAfter (loop, stmt);
      attachComment(stmt,"      aitool generated FLOPS counting statement ...");
    }

    // Obtain per-iteration load/store bytes calculation expressions
    // excluding scalar types to match the manual version
    //CountLoadStoreBytes (SgLocatedNode* input, bool includeScalars = false, bool includeIntType = false);
    std::pair <SgExpression*, SgExpression*> load_store_count_pair ; 

    CountLoadStoreBytes (loop_body, load_store_count_pair, false, true);

    // chstores=chstores+chiterations*8
    if (load_store_count_pair.second!= NULL)
    {
      SgExprStatement* store_byte_stmt = buildCounterAccumulationStmt("chstores", new_iter_var_name, load_store_count_pair.second, scope);
      insertStatementAfter (loop, store_byte_stmt);
      attachComment(store_byte_stmt,"      aitool generated Stores counting statement ...");
    }

    // handle loads stmt 2nd so it can be inserted as the first after the loop
    // build  chloads=chloads+chiterations*2*8
    if (load_store_count_pair.first != NULL)
    {
      SgExprStatement* load_byte_stmt = buildCounterAccumulationStmt("chloads", new_iter_var_name, load_store_count_pair.first, scope);
      insertStatementAfter (loop, load_byte_stmt);
      attachComment(load_byte_stmt,"      aitool generated Loads counting statement ...");
    }

    return prev_stmt; 
  } // end instrumentLoopForCounting()

  // Obtain the kind of FP operation from a binary operation
  fp_operation_kind_enum getFPOpKind (SgBinaryOp* bop)
  {
    fp_operation_kind_enum op_kind = e_unknown; 
    ROSE_ASSERT (bop != NULL);
    switch (bop->variantT())
    {
      case V_SgAddOp:
      case V_SgPlusAssignOp:
        op_kind = e_plus; 
        break;
      case V_SgSubtractOp:
      case V_SgMinusAssignOp:  
        op_kind = e_minus;
        break;      
      case V_SgMultiplyOp:
      case V_SgMultAssignOp:  
        op_kind = e_multiply;
        break;      
      case V_SgDivideOp:
      case V_SgDivAssignOp:  
        op_kind = e_divide;
        break;      
        //skip a set of binary ops which do not involve FP operation at all  
      case V_SgAssignOp:
      case V_SgPntrArrRefExp: // this is integer operation for array address calculation
      case V_SgLessThanOp: //TODO how to convert this if compare two FP operands ??
      case V_SgDotExp:
      case V_SgArrowExp:
      case V_SgCommaOpExp:
        break;
      default:
        {
          cerr<<"getFPOpKind () unrecognized binary op kind: "<< bop->class_name() <<endl;
          ROSE_ASSERT (false);
        }
    } //end switch    

    return op_kind;
  }
  // Bottomup evaluate attribute
  FPCounters OperationCountingTraversal::evaluateSynthesizedAttribute (SgNode* n, SubTreeSynthesizedAttributes synthesizedAttributeList )
  {
    FPCounters returnAttribute; 
    bool hasHandled = false; 
    // Skip compiler generated code
    if (isSgLocatedNode(n) && isSgLocatedNode(n)->get_file_info()->isCompilerGenerated() 
        && !isSgAssignInitializer(n)  // SgAssignInitializer misses file info. right now, workaround it.
        && !isSgBasicBlock(n)  // switch-case statement: basic blocks are generated for case or default option statements
        && !isSgCastExp(n))  // compiler generated cast expressions
    {
      hasHandled = true;  
    }
    else  if (SgExpression* expr = isSgExpression (n))
    {
      if (SgBinaryOp* bop = isSgBinaryOp(expr))
      {
        // sum up lhs and rhs operation counts
        FPCounters lhs_counters = synthesizedAttributeList[SgBinaryOp_lhs_operand_i];
        FPCounters rhs_counters = synthesizedAttributeList[SgBinaryOp_rhs_operand_i];
        returnAttribute = lhs_counters + rhs_counters;

        // only consider FP operation for now TODO: make this configurable
        if (bop->get_type()->isFloatType())
        {
          // based on current bop kind, increment an additional counter 
          fp_operation_kind_enum op_kind = getFPOpKind (bop);    
          if (op_kind !=e_unknown )
          {
            returnAttribute.addCount (op_kind, 1);
            returnAttribute.updateTotal();
          }
        }
        hasHandled = true; 
      }// unary operations
      // Other expressions
      else if (SgFunctionCallExp* func_call = isSgFunctionCallExp (n))
      {
        // Function call handling:   foo(a,b, c+d)
        // The FPLOPs should come from parameter list and function body(synthesized separately )
        SgExpression* func_ref = func_call->get_function();
        SgExprListExp* args = func_call->get_args();
        FPCounters* child_counter1 = getFPCounters (func_ref);
        FPCounters* child_counter2 = getFPCounters (args);
        returnAttribute = *child_counter1 + *child_counter2;
        hasHandled = true; 
      }
      else if (SgFunctionRefExp* func_ref = isSgFunctionRefExp (n))
      {
        // TODO: interprocedural synthesize analysis, until reaching a fixed point 
        if (debug)
          cout<<"Warning: encountering a function call "<< func_ref->get_symbol()->get_name()<<"(), assuming 0 FLOPS for now."  <<endl;
        hasHandled = true; 
      }
      else if (SgConditionalExp * conditional_exp = isSgConditionalExp(n))
      {
        SgExpression*  cond_exp = conditional_exp->get_conditional_exp();
        SgExpression*  true_exp = conditional_exp->get_true_exp();
        SgExpression* false_exp = conditional_exp->get_false_exp();

        FPCounters* cond_counters = new FPCounters();
        FPCounters* true_counters = new FPCounters();
        FPCounters* false_counters= new FPCounters();
        FPCounters* larger_counters=new FPCounters();

        if (cond_exp)
          *cond_counters = *(getFPCounters (cond_exp));
        if (true_exp)
          *true_counters = *(getFPCounters (true_exp));
        if (false_exp)
          *false_counters = *(getFPCounters (false_exp));

        // We estimate the upper bound of FLOPS for now
        // TODO: using runtime profiling information to pick the right branch
        if (true_counters->getTotalCount() > false_counters->getTotalCount())
          larger_counters = true_counters; 
        else
          larger_counters = false_counters; 

        returnAttribute = *cond_counters + *larger_counters;
        hasHandled = true; 
      }
      else if (isSgValueExp(n))
      {

        // accessing a value does not involving FLOP operations.
        hasHandled = true; 
      }
      else if (isSgVarRefExp(n))
      {

        // accessing a variable does not involving FLOP operations.
        hasHandled = true; 
      }
      else if ( isSgVarArgStartOp(n) // other expressions without FLOPs 
          )
      {
        hasHandled = true; 
      }

    }
    else if (SgStatement* stmt = isSgStatement(n))
    {
      if (SgExprStatement* exp_stmt = isSgExprStatement(stmt))
      {
        // Ditch this synthesizedAttributeList thing, hard to use
        // I just directly grab child node's attribute I saved previously.
        // Directly synthesize from child expression
        FPCounters* child_counters = getFPCounters (exp_stmt->get_expression());
        returnAttribute = *child_counters;
        hasHandled = true; 
      }
      else if (SgFunctionDeclaration* func_decl = isSgFunctionDeclaration(n))
      {
        SgFunctionParameterList * plist = func_decl->get_parameterList();
        SgFunctionDefinition* def = func_decl->get_definition();
        if (def != NULL)
        {
          FPCounters* child_counter1 = getFPCounters (plist);
          FPCounters* child_counter2 = getFPCounters (def);
          returnAttribute = *child_counter1 + *child_counter2;
        }
        hasHandled = true; 
      }
      else if (SgVariableDeclaration* decl = isSgVariableDeclaration(n))
      {
        //e.g. float a = b+c;   the initialization may involve some operations
        SgInitializedNamePtrList name_list = decl->get_variables();
        for (size_t i =0; i< name_list.size(); i++)
        {
          SgInitializedName* name = name_list[i];
          FPCounters* child_counters = getFPCounters (name);
          returnAttribute = returnAttribute + *child_counters;
        }
        hasHandled = true; 
      }
      else if (isSgFunctionParameterList(n))
      {
        // only a set of arguments declared, no FP operations are involved. 
        hasHandled = true; 
      }
      else if (SgIfStmt * if_stmt = isSgIfStmt(n))
      {
        SgStatement* cond_stmt = if_stmt->get_conditional();
        SgStatement* true_stmt = if_stmt->get_true_body();
        SgStatement* false_stmt = if_stmt->get_false_body();

        FPCounters* cond_counters = new FPCounters();
        FPCounters* true_counters = new FPCounters();
        FPCounters* false_counters= new FPCounters();
        FPCounters* larger_counters=new FPCounters();

        if (cond_stmt)
          *cond_counters = *(getFPCounters (cond_stmt));
        if (true_stmt)
          *true_counters = *(getFPCounters (true_stmt));
        if (false_stmt)
          *false_counters = *(getFPCounters (false_stmt));

        // We estimate the upper bound of FLOPS for now
        // TODO: using runtime profiling information to pick the right branch
        if (true_counters->getTotalCount() > false_counters->getTotalCount())
          larger_counters = true_counters; 
        else
          larger_counters = false_counters; 

        returnAttribute = *cond_counters + *larger_counters;
        hasHandled = true; 
      }

      else if (SgBasicBlock* block = isSgBasicBlock(n))
      {
        SgStatementPtrList stmt_list = block->get_statements();
        for (size_t i =0; i< stmt_list.size(); i++)
        {
          SgStatement* stmt_in_block = stmt_list[i];
          ROSE_ASSERT (stmt_in_block != NULL);
          SgCaseOptionStmt* case_option = isSgCaseOptionStmt (stmt_in_block);
          SgDefaultOptionStmt * default_option = isSgDefaultOptionStmt (stmt_in_block);
          FPCounters* fpcounters = getFPCounters (stmt_in_block);

          // regular sequence of statement
          if (case_option == NULL && default_option == NULL)
          {
            returnAttribute = returnAttribute + *fpcounters; 
          }
          else // either case option or default option statement, we only keep the max value so far for upper bound estimation
            //TODO using profiling results to improve accuracy
          {
            if (fpcounters->getTotalCount() > returnAttribute.getTotalCount())
              returnAttribute =  *fpcounters;
          }  
        }
        hasHandled = true; 
      }
      else if (SgClassDefinition* block= isSgClassDefinition(n))
      {
        SgDeclarationStatementPtrList stmt_list = block->get_members();
        for (size_t i =0; i< stmt_list.size(); i++)
        {
          SgStatement* stmt_in_block = stmt_list[i];
          ROSE_ASSERT (stmt_in_block != NULL);
          FPCounters* fpcounters = getFPCounters (stmt_in_block);
          returnAttribute = returnAttribute + *fpcounters; 
        }
        hasHandled = true; 
      }
      else if (SgClassDeclaration* decl_stmt = isSgClassDeclaration(n))
      {
        SgClassDefinition* body = decl_stmt->get_definition();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        hasHandled = true; 
      }
      else if (SgForStatement* for_stmt = isSgForStatement(n))
      {
        SgStatement* body = for_stmt->get_loop_body();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        hasHandled = true; 
        // TODO: no multiplication of loop iteration count for now
      }
      else if (SgWhileStmt * w_stmt = isSgWhileStmt(n))
      {
        SgStatement* body = w_stmt->get_body();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        hasHandled = true; 
        // TODO: no multiplication of loop iteration count for now
      }
      else if (SgDoWhileStmt * w_stmt = isSgDoWhileStmt(n))
      {
        SgStatement* body = w_stmt->get_body();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        hasHandled = true; 
        // TODO: no multiplication of loop iteration count for now
      }
      // go through all statements following until reaching break;
      else if (SgCaseOptionStmt * w_stmt = isSgCaseOptionStmt(n))
      {
        SgStatement* body = w_stmt->get_body();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        hasHandled = true; 
      }
      else if (SgDefaultOptionStmt * w_stmt = isSgDefaultOptionStmt(n))
      {
        SgStatement* body = w_stmt->get_body();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        hasHandled = true; 
      }

      else if (SgSwitchStatement* w_stmt = isSgSwitchStatement(n))
      {
        // the body block should be in charge of select the max of case branch
        SgStatement* body = w_stmt->get_body();
        if (body != NULL)
        {
          FPCounters* fpcounters = getFPCounters (body);
          returnAttribute = *fpcounters; 
        }
        SgStatement* selector = w_stmt->get_item_selector();
        if (selector)
        {
          FPCounters* fpcounters = getFPCounters (selector);
          returnAttribute = returnAttribute + *fpcounters; 
        }
        hasHandled = true; 
      }

      else if ( isSgContinueStmt(n) // no OPs for continue;
          ||isSgBreakStmt(n)
          ||isSgLabelStatement(n)
          ||isSgGotoStatement(n)
          ||isSgNullStatement(n)
          ||isSgEnumDeclaration(n)  
          )
      {
        hasHandled = true; 
      }

    } // end if statement

    //    if (isSgAssignInitializer(n))
    //      printf ("find assign initializer %p \n", n);
    //default case: upward propagate counters if only a single child or error messages for multiple children
    if (!hasHandled)
    {
      // only one child, directly synthesize/copy it 
      if (synthesizedAttributeList.size()==1)
      {
        returnAttribute = synthesizedAttributeList[0];
      }
      else
      {
        // ignore some known types of nodes
        if (!isSgPragma(n) && 
            !isSgGlobal(n) && 
            !isSgSourceFile(n))
        {
          cerr<<"Error in OperationCountingTraversal::evaluateSynthesizedAttribute(): unhandled node with multiple children for "<< n->class_name()<<endl;
          if (SgLocatedNode* lnode = isSgLocatedNode(n))
          {
            lnode->get_file_info()->display();
          }
          ROSE_ASSERT (false);
        }
      }  
    }

    // Extra step: copy values to the attribute of the node
    // Patch up the node information here.  operator+ used before may corrupt the node info.
    returnAttribute.setNode (isSgLocatedNode(n));
    SgLocatedNode* lnode = isSgLocatedNode (n);
    if (lnode != NULL )
    {
      FPCounters* fpcounters = getFPCounters (lnode); // create attribute on fly if not existing
      *fpcounters = returnAttribute;
      //debugging here
      if (debug) 
        fpcounters->printInfo (n->class_name()); 

      // write to the report file here, bottom up order though, not a good idea
      ofstream reportFile(report_filename.c_str(), ios::app);
      //      cout<<"Writing counter results to "<< report_filename <<endl;
      reportFile<< fpcounters->toString();
    }


    // Verify the correctness using optionally pragmas 
    // verify the counting results are consistent with reference results from pragmas   
    if (SgStatement* stmt = isSgStatement(n))
    {
      if (SgStatement* prev_stmt = SageInterface::getPreviousStatement(stmt))
      {
        if (SgPragmaDeclaration* p_decl = isSgPragmaDeclaration(prev_stmt))
        {
          FPCounters* ref_result = parse_aitool_pragma(p_decl);                                                          
          FPCounters* current_result = getFPCounters (lnode);
          if (ref_result != NULL)
          {
            if (!current_result->consistentWithReference (ref_result))
            {
              cerr<<"Error. Calculated FP operation counts differ from reference counts parsed from pragma!"<<endl;
              ref_result->printInfo("Reference counts are ....");
              current_result->printInfo("Calculated counts are ....");
              assert (false);
            }
          }
        } // end if pragma
      } // end if prev stmt
    } // end if stmt


    return returnAttribute;
  }

  FPCounters FPCounters::operator+ (const FPCounters & right) const
  {
    FPCounters retVal;

    retVal.plus_count = plus_count + right.plus_count;
    retVal.minus_count = minus_count + right.minus_count;
    retVal.multiply_count = multiply_count + right.multiply_count;
    retVal.divide_count = divide_count + right.divide_count;
    retVal.total_count = total_count + right.total_count;

    //TODO: add load/store expressions
    return retVal; 
  }

} // end of namespace
