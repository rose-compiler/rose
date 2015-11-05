#include "ai_measurement.h"

using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace AstFromString;

namespace ArithemeticIntensityMeasurement
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

  std::string toString (fp_operation_kind_enum op_kind)
  {
    std::string rt; 
    return string(FPOpKindNameList[op_kind]);
  } 
  bool debug;
  // default file name to store the report
  string report_filename = "ai_tool_report.txt";
  string report_option ="-report-file";

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
   ss<<"----------Floating Point Operation Counts---------------------"<<endl;
   ss<<comment<<endl;
   //cout<<"Floating point operations found for node "<<node->class_name() <<"@" <<endl;
   ss<<node->class_name() <<"@" <<endl;
   ss<< node->get_file_info()->get_filename()<<":"<<node->get_file_info()->get_line() <<endl;
   ss<<"\tfp_plus:"<< plus_count<<endl;
   ss<<"\tfp_minus:"<< minus_count<<endl;
   ss<<"\tfp_multiply:"<< multiply_count<<endl;
   ss<<"\tfp_divide:"<< divide_count<<endl;
   ss<<"\tfp_total:"<< getTotalCount()<<endl;

   return ss.str();
 }
 void FPCounters::printInfo(std::string comment/* ="" */)
 {
#if 0   
   cout<<"----------Floating Point Operation Counts---------------------"<<endl;
   cout<<comment<<endl;
   //cout<<"Floating point operations found for node "<<node->class_name() <<"@" <<endl;
   cout<<node->class_name() <<"@" <<endl;
   cout<< node->get_file_info()->get_filename()<<":"<<node->get_file_info()->get_line() <<endl;
   cout<<"\tfp_plus:"<< plus_count<<endl;
   cout<<"\tfp_minus:"<< minus_count<<endl;
   cout<<"\tfp_multiply:"<< multiply_count<<endl;
   cout<<"\tfp_divide:"<< divide_count<<endl;
   cout<<"\tfp_total:"<< getTotalCount()<<endl;
#else
  cout<<toString(comment);
#endif
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
       assert (false);  
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
 void getVariablesProcessedByInnerLoops (SgScopeStatement* current_loop_body, bool isRead, std::set<SgInitializedName*>& var_set)
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
  SgExpression* calculateBytes (std::set<SgInitializedName*>& name_set, SgScopeStatement* scope, bool isRead)
  {
    SgExpression* result = NULL; 
    if (name_set.size()==0) return result;

   // the input is essentially the loop body, a scope statement
   ROSE_ASSERT (scope != NULL);
    // We need to record the associated loop info.
    SgStatement* loop= NULL;
    SgForStatement* forloop = isSgForStatement(scope->get_scope());
    SgFortranDo* doloop = isSgFortranDo(scope->get_scope());

    if (forloop)
      loop = forloop;
    else if (doloop)
      loop = doloop;
    else
    {
      cerr<<"Error in CountLoadStoreBytes (): input is not loop body type:"<< scope->class_name()<<endl;
      assert(false);
    }   

    std::map<SgType* , int> type_based_counters; 

   // get all processed variables by inner loops
    std::set<SgInitializedName*> processed_var_set; 
    getVariablesProcessedByInnerLoops (scope, isRead, processed_var_set);
    
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
      ROSE_ASSERT (isSgArrayType (t) == false); 
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


 // Only keep desired types
 std::set<SgInitializedName* > filterVariables(const std::set<SgInitializedName* > & input)
 {
   std::set<SgInitializedName* > result; 
   std::set<SgInitializedName*>::iterator it;

   for (it=input.begin(); it!=input.end(); it++)
   {
     SgInitializedName* iname = (*it);
     if (isSgArrayType (iname->get_type()))
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
  std::pair <SgExpression*, SgExpression*> CountLoadStoreBytes (SgLocatedNode* input, bool includeScalars /* = true */, bool includeIntType /* = true */)
  {
    std::pair <SgExpression*, SgExpression*> result; 
    assert (input != NULL);
   // the input is essentially the loop body, a scope statement
    SgScopeStatement* scope = isSgScopeStatement(input);

    // We need to record the associated loop info.
    //SgStatement* loop= NULL;
    SgForStatement* forloop = isSgForStatement(scope->get_scope());
    SgFortranDo* doloop = isSgFortranDo(scope->get_scope());

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

    //Plan A: use and extend Qing's side effect analysis
    std::set<SgInitializedName*> readVars;
    std::set<SgInitializedName*> writeVars;

    bool success = SageInterface::collectReadWriteVariables (isSgStatement(input), readVars, writeVars);
    if (success!= true)
    {
       cout<<"Warning: CountLoadStoreBytes(): failed to collect load/store, mostly due to existence of function calls inside of loop body @ "<<input->get_file_info()->get_line()<<endl;
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
      readVars =  filterVariables (readVars);
    if (debug)
      cout<<"debug: found write variables (SgInitializedName) count = "<<writeVars.size()<<endl;
    for (it=writeVars.begin(); it!=writeVars.end(); it++)
    {
      SgInitializedName* iname = (*it);
      if (debug)
        cout<<scalar_or_array(iname->get_type()) <<" "<<iname->get_name()<<"@"<<iname->get_file_info()->get_line()<<endl;
    }
    if (!includeScalars )
      writeVars =  filterVariables (writeVars);
    result.first =  calculateBytes (readVars, scope, true);
    result.second =  calculateBytes (writeVars, scope, false);
    return result;
  }

  //! Count floating point operations seen in a subtree
  void CountFPOperations(SgLocatedNode* input)
  {
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
        // we assume the traverse is inside out, the inner loop will be processed first!
        // An operation is counted once when its innermost enclosing loop is processed. 
        // Using a map to avoid double counting an operation when it is enclosed in multiple loops
        if (!FPVisitMAP[bop]) 
        {
	  addFPCount (input, op_kind);
          FPVisitMAP[bop] = true;
        }
      }	
    }  // end for
    //Must update the total counter here
    FPCounters* fp_counters = getFPCounters (input); 
    fp_counters->updateTotal ();
    // write results to a report file
    if (running_mode == e_static_counting)
    {
      ofstream reportFile(report_filename.c_str(), ios::app);
      cout<<"Writing counter results to "<< report_filename <<endl;
      reportFile<< fp_counters->toString();
    }
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

    c_sgnode = getNextStatement(pragmaDecl);
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
    SgStatement* prev_stmt = getPreviousStatement(loop,false);

    // backwards search, skipping pragma declaration etc.
    while (prev_stmt!=NULL && !isAssignmentStmtOf (prev_stmt, chiterations_sym->get_declaration()))
      prev_stmt = getPreviousStatement(prev_stmt,false);
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
    //CountLoadStoreBytes (SgLocatedNode* input, bool includeScalars = true, bool includeIntType = true);
    std::pair <SgExpression*, SgExpression*> load_store_count_pair = CountLoadStoreBytes (loop_body, false, true);
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


} // end of namespace
