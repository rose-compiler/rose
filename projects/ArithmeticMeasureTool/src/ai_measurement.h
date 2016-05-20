#ifndef AI_MEASUREMENT_H
#define AI_MEASUREMENT_H

#include <rose.h>
#include <fstream>
#include <iostream>

namespace ArithemeticIntensityMeasurement
{
  extern bool debug;
  extern std::string report_option;
  extern std::string report_filename;
  extern int loop_id; // roughly assign a unique id for each loop, at least within the context of a single function

  // we support different execution modes
  //
  enum running_mode_enum {
    e_analysis_and_instrument,  // recognize special chiterations = .., do the counting based instrumentation
    e_static_counting // recognize #pragma aitool and do the counting, writing to a report file
  };
  enum fp_operation_kind_enum {
    e_unknown = 0,
    e_total =1 ,
    e_plus,
    e_minus,
    e_multiply,
    e_divide
  };

  extern running_mode_enum running_mode; // = e_analysis_and_instrument; 

  std::string toString (fp_operation_kind_enum op_kind);

  //Data structure to store the information for an associated node, mostly a loop 
  // The counters are for a single iteration, not considering iteration count.
  //Using AstAttribute to enable propagation
  class FPCounters: public AstAttribute
  {
    public:
      // Retrieve and add counters
      int getTotalCount () { assert (total_count == plus_count + minus_count + multiply_count + divide_count);  return total_count; }
      int getCount (fp_operation_kind_enum c_type);
      void  addCount(fp_operation_kind_enum c_type, int i =1);
      // Directly set counters. Use this carefully. Designed to support storing info. parsed from pragmas
      void  setCount(fp_operation_kind_enum c_type, int i =1);
      void updateTotal() {  if (total_count == 0 ) total_count = plus_count + minus_count + multiply_count + divide_count ; }
      void printInfo(std::string comment="");
      // convert the counter information to a string
      std::string toString(std::string comment="");

      SgLocatedNode* getNode () {return node; }
      // compare the values of this object with a reference FPCounters object(often obtained from Pragma)
      // Consistent is defined as if a non-zero value is provided by the reference object, this object's corresponding value must be the same.
      bool consistentWithReference(FPCounters* refCounters);

      // access load/store counters;
      void setLoadBytes(SgExpression* n) { assert (n!=NULL); load_bytes = n; }
      void setStoreBytes(SgExpression* n) {assert (n!=NULL); store_bytes = n; }

      SgExpression* getLoadBytes() {return load_bytes; }
      SgExpression* getStoreBytes() {return store_bytes; }

      FPCounters(SgLocatedNode *n): node(n)
    {
      plus_count = 0;
      minus_count = 0;
      multiply_count = 0;
      divide_count = 0;
      total_count = 0;

      load_bytes = NULL;
      store_bytes = NULL;
    }

    private:
      SgLocatedNode* node ; //associated AST node
      int plus_count;
      int minus_count;
      int multiply_count;
      int divide_count;
      int total_count;

      // extensions to store load and store bytes
      //type sizes are machine dependent, we only generate expressions with sizeof() terms.
      SgExpression* load_bytes;
      SgExpression* store_bytes; 

      // without assertion
      int getRawTotalCount () { return total_count; }
      int getPlusCount() {return plus_count;}
      int getMinusCount() {return minus_count ;}
      int getMultiplyCount() {return multiply_count;}
      int getDivideCount() {return divide_count;}

      void addPlusCount(int i = 1)     {assert (i>=1); plus_count += i;};
      void addMinusCount(int i = 1)    {assert (i>=1); minus_count += i;}
      void addMultiplyCount(int i = 1) {assert (i>=1); multiply_count += i;}
      void addDivideCount(int i = 1)   {assert (i>=1); divide_count += i;}
  }; // end class FPCounters

 // interface functions to manipulate FPCounters
  void CountFPOperations(SgLocatedNode* input);
  void printFPCount (SgLocatedNode* n);

  FPCounters * getFPCounters (SgLocatedNode* n);
  int getFPCount (SgLocatedNode* n, fp_operation_kind_enum c_type);
  void addFPCount (SgLocatedNode* n, fp_operation_kind_enum c_type, int i =1);

  // functions to handle pragmas
  bool parse_fp_counter_clause(fp_operation_kind_enum* fp_op_kind, int *op_count);
  FPCounters* parse_aitool_pragma (SgPragmaDeclaration* pragmaDecl);

  std::pair <SgExpression*, SgExpression*> 
     CountLoadStoreBytes (SgLocatedNode* input, bool includeScalars = true, bool includeIntType = true);
  // Create load/store = loads + iteration * load/store_count_per_iteration
  // lhs_exp = lhs_exp + iter_count_exp * per_iter_bytecount_exp
  SgExprStatement* buildByteCalculationStmt(SgVariableSymbol * lhs_sym, SgVariableSymbol* iter_count_sym, SgExpression* per_iter_bytecount_exp);

  // Build counter accumulation statement like chloads = chloads + chiterations * (2 * 8)
  // chloads is the counter name, chiterations is iteration_count_name, 2*8 is the per-iteration count expression.
  SgExprStatement* buildCounterAccumulationStmt (std::string counter_name, std::string iteration_count_name, SgExpression* count_exp_per_iteration, SgScopeStatement* scope);

  //A generic function to check if a loop has a tag statement prepended to it, asking for instrumentation
  //If so, the loop will be instrumented and the tag statement will be returned.
  // This function supports both C/C++ for loops and Fortran Do loops
  SgStatement* instrumentLoopForCounting(SgStatement* loop);
  
  //process a source file, add instrumentation to collect loop iteration counts, return the number of 
  int instrumentLoopIterationCount(SgSourceFile* sfile);

 //! if a statement is an assignment statement to a variable
  bool isAssignmentStmtOf (SgStatement* stmt, SgInitializedName* init_name);

} // end namespace

#endif
