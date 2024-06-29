#ifndef AI_MEASUREMENT_H
#define AI_MEASUREMENT_H
// Other .C files must include  "sage3basic.h" before including this file!!
// Program analysis to estimate arithemetic intensity of one loop iteration
//#include <rosePublicConfig.h>
#include <fstream>
#include <iostream>

namespace ArithmeticIntensityMeasurement
{
  extern bool debug;
  extern bool e_inline; // enable inlining or not
  // the version of the counting algorithm: 
  // value 1: flow-insensitive (default choice now), treating all statements as straightline statements  
  //       2: flow-sensitive (under development), considering if branches, loop iteration counts, etc. 
  extern int algorithm_version;

  //file name to store the report
  extern std::string report_filename;

  extern int loop_id; // roughly assign a unique id for each loop, at least within the context of a single function

  // we support different execution modes
  //
  enum running_mode_enum {
    e_analysis_and_instrument,  // recognize special chiterations = .., do the counting based instrumentation
    e_static_counting // recognize #pragma aitool and do the counting, writing to a report file
  };
  // Floating point operation types
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

  //! Estimate the byte size of types
  int getSizeOf(SgType* t);

  //Data structure to store the information for an associated node, mostly a loop.
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
      void setCount(fp_operation_kind_enum c_type, int i =1);
      void updateTotal() { // if (total_count == 0 ) 
        total_count = plus_count + minus_count + multiply_count + divide_count ; }

      void setErrorCode(int i) {error_code = i; };

      void printInfo(std::string comment="");

      // convert the counter information to a string
      std::string toString(std::string comment);

      // Override member function to remove compiler warning
      std::string toString() override {
         return toString(std::string{});
      }

      SgLocatedNode* getNode () {return node; }
      void  setNode (SgLocatedNode* n) {node=n; }

      // compare the values of this object with a reference FPCounters object(often obtained from Pragma)
      // Consistent is defined as if a non-zero value is provided by the reference object, this object's corresponding value must be the same.
      bool consistentWithReference(FPCounters* refCounters);

      // access load/store counters;
      void setLoadBytes(SgExpression* n) { load_bytes = n; }
      void setStoreBytes(SgExpression* n) { store_bytes = n; }

      SgExpression* getLoadBytes() {return load_bytes; }
      SgExpression* getStoreBytes() {return store_bytes; }

      float getIntensity(); // obtain the estimated intensity

      // default constructor required
      FPCounters(SgLocatedNode *n=NULL): node(n)
    {
      error_code = 0;
      plus_count = 0;
      minus_count = 0;
      multiply_count = 0;
      divide_count = 0;
      total_count = 0;

      load_bytes = NULL;
      store_bytes = NULL;

      load_bytes_int = 0;
      store_bytes_int = 0;

      intensity = -1.0; 
    }

      FPCounters& operator=(const FPCounters &) = default; // otherwise warning regarding copy constructor

      // copy constructor is required for synthesized attributes
      FPCounters (const FPCounters &x)
      {
        node = x.node; 
        plus_count = x.plus_count;
        minus_count = x.minus_count;
        multiply_count = x.multiply_count;
        divide_count = x.divide_count;
        total_count = x.total_count;

        load_bytes = x.load_bytes;
        store_bytes = x.store_bytes;

        load_bytes_int = x.load_bytes_int;
        store_bytes_int  = x.store_bytes_int;

        intensity = x.intensity; 
      }

      // used to synthesize counters from children nodes
      FPCounters operator+( const FPCounters & right) const; 

    private:
      SgLocatedNode* node ; //associated AST node

      //0 means no error, 1 means side effect analysis failed, mostly due to unrecognized function calls
      int error_code;  // It is possible our analysis fails to estimate the numbers

      // Floating point operation counters
      int plus_count;
      int minus_count;
      int multiply_count;
      int divide_count;
      int total_count;

      // Store load and store bytes
      // Type sizes are machine dependent, we first generate expressions with sizeof() terms.
      SgExpression* load_bytes;
      SgExpression* store_bytes; 

      // Using a typical 64-bit Intel machine, evaluate the load/store byte expressions to obtain estimated sizes
      // Evaluated integer result of the load_bytes expression
      int load_bytes_int; 
      // Evaluated integer result of the store_bytes expression
      int store_bytes_int; 

      float intensity; // the final intensity

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

  // Version 2 counting using a bottomup traversal and synthesized attributes
  /*
   *
SgBasicBlock: sum of  count(stmt)
straight line expression statement, atomic case, scan expression for operations, store into attribute, count (exp)
function call expression: summarize the counter using some function annotation or lookup table!!
travese expression tree, recursively, accumulate the counter, where to store the results : attributes vs. lookup table (this is easier)
loops:  loop iteration * count[loop_body]
branches: using branch prediction ratios, if no available, pick the max or minimum branch, based on a flag. upper bound and lower bound FLOPS
really need constant folding or symbolic evaluation to simplify things here!!
   *
   */
  //class OperationCountingTraversal: public AstBottomUpProcessing <FPCounters> 
  class OperationCountingTraversal: public SgBottomUpProcessing <FPCounters> 
  {
    public: 
      FPCounters evaluateSynthesizedAttribute (SgNode* n, SubTreeSynthesizedAttributes synthesizedAttributeList );
  }; 

  // Top level interface: estimate AI for an input stmt (it could be a block of stmts)
  FPCounters* calculateArithmeticIntensity(SgLocatedNode* stmt, bool includeScalars = false, bool includeIntType = false); 

  // interface functions to manipulate FPCounters
  void CountFPOperations(SgLocatedNode* input);
  void printFPCount (SgLocatedNode* n);

  //! Obtain the kind of FP operation from a binary operation
  fp_operation_kind_enum getFPOpKind (SgBinaryOp* bop);

  //! Count memory load/store operations, store into attribute FPCounters
  void CountMemOperations(SgLocatedNode* input, bool includeScalars = false, bool includeIntType = false);

  FPCounters * getFPCounters (SgLocatedNode* n);
  int getFPCount (SgLocatedNode* n, fp_operation_kind_enum c_type);
  void addFPCount (SgLocatedNode* n, fp_operation_kind_enum c_type, int i =1);

  // functions to handle pragmas
  bool parse_fp_counter_clause(fp_operation_kind_enum* fp_op_kind, int *op_count);
  FPCounters* parse_aitool_pragma (SgPragmaDeclaration* pragmaDecl);

  // Count the load and store bytes of the code rooted at input node. If fails , return false. 
  // Otherwise return true and the two expressions calculating save load/store bytes.
  bool CountLoadStoreBytes (SgLocatedNode* input,  std::pair <SgExpression*, SgExpression*> & result, 
      bool includeScalars = false, bool includeIntType = false); 

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

  //! Process a source file, add instrumentation to collect loop iteration counts, return the number of 
  int instrumentLoopIterationCount(SgSourceFile* sfile);

  //! Check If a statement is an assignment statement to a variable
  bool isAssignmentStmtOf (SgStatement* stmt, SgInitializedName* init_name);

  int get_int_value(SgValueExp * sg_value_exp);

} // end namespace

#endif
