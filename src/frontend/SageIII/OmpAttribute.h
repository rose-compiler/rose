/*!
 * ROSE persistent attribute to represent OpenMP 3.0 C/C++ directives
 * All things go to the OmpSupport namespace to avoid conflicts 
 */
// Goal: share it by both OpenMP translation and automatic OpenMP insertion
//
// Liao 9/17, 2008

// #include <rose.h>
#include <iostream>
#include <string>
#include <map>
#include <cassert>

namespace OmpSupport{

 // OpenMP construct name list
 // We put all directive and clause types into one enumerate type
 // since some internal data structure(map) have to access 
 // both directives and clauses uniformly
  enum	omp_construct_enum {
     e_unknown = 0, 

  // 16 directives as OpenMP 3.0
     e_parallel,
     e_for,
     e_sections,
     e_section,
     e_single,

     e_master, 
     e_critical,
     e_barrier,
     e_atomic,
     e_flush,

     e_threadprivate,
     e_parallel_for,
     e_parallel_sections,
     e_task,
     e_taskwait, 
     // we have both ordered directive and ordered clause, 
    //so make the name explicit
     e_ordered_directive,

  // 15 clauses for OpenMP 3.0
    // 7 data-sharing attributes clauses
     e_default, // the clause
     e_shared,
     e_private,
     e_firstprivate,
     e_lastprivate,
     e_copyin,
     e_copyprivate,

     //8 misc clauses
     e_if, // used with omp parallel or omp task
     e_num_threads, // for omp parallel only
     e_nowait,
     e_ordered_clause,
     e_reduction,
     e_schedule,
     e_collapse,
     e_untied, 

 // Simple values for some clauses
   
     //4 values for default clause 
     //C/C++ default values
     e_default_none,
     e_default_shared,
     //Fortran default values
     e_default_private,
     e_default_firstprivate,


     // reduction operations
     //8 operand for C/C++
     e_reduction_plus,
     e_reduction_minus,
     e_reduction_mul,
     e_reduction_bitand, // &
     e_reduction_bitor,  // | 

     e_reduction_bitxor,  // ^
     e_reduction_logand,  // &&
     e_reduction_logor,   // ||

     // TODO more reduction intrinsic procedure name for Fortran  
     e_reduction_min, //?
     e_reduction_max,

     //5 schedule policies
     e_schedule_none,
     e_schedule_static,
     e_schedule_dynamic,
     e_schedule_guided,
     e_schedule_auto,
     e_schedule_runtime,

  // not an OpenMP construct
     e_not_omp
  }; //end omp_construct_enum

  //! Output omp_construct_enum to a string: 
  // Better using OmpSupport::toString() to avoid ambiguous 
  std::string toString(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a directive
  bool isDirective(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a clause
  bool isClause(omp_construct_enum omp_type);

  //! Check if an OpenMP construct is a reduction operator
  bool isReductionOperator(omp_construct_enum omp_type);

  class OmpAttribute;
  //! Some utility functions to manipulate OmpAttribute
  //
  //! A builder for OmpAttribute
  OmpAttribute* buildOmpAttribute(enum omp_construct_enum directive_type, SgNode* context_node);

  //! Add OmpAttribute to a SgNode
  void addOmpAttribute(OmpAttribute* ompattribute, SgNode* node);

  //! Get OmpAttribute from a SgNode, return NULL if not found
  OmpAttribute* getOmpAttribute(SgNode* node);

  //! Generate a pragma declaration from OmpAttribute attached to a statement
  void generatePragmaFromOmpAttribute(SgNode* sg_node); 
  //TODO this is duplicated from autoParallization project's generatedOpenMPPragmas() 
   // We should remove this duplicate once autopar is moved into rose/src 

//------------------------------------------------------------------
//------------------------------------------------------------------
// By default, the persistent attribute attached to an OpenMP pragma node in SAGE III AST
// Attaching to pragma is easier since a few directives have no obvious 
// associated code blocks, like threadprivate.
//
// The attribute can also be attached by a scope affected by OpenMP. This is used during
// automatic parallelization when the corresponding pragma is not yet generated.
//
// A cure-all approach is used to simplify the handling. 
// OmpAttribute is implemented using a 'flat' data structure encompass all 
// possible directives, clauses
// and their various contents, if any.
//
// different types of pragmas need different information in some cases
// e.g.
//    'omp for' needs scheduling type 
//------------------------------------------------------------------
//------------------------------------------------------------------
class OmpAttribute : public AstAttribute 
{
 public:
   //It is recommended to use OmpSupport::buildOmpAttribute() instead of 
   //using the constructors here
  //!Default constructors
   OmpAttribute()
    {
      mNode = NULL;
      omp_type = e_unknown;
      init();
    }
   //! Constructor for known directive type and originating pragma/scope node
   OmpAttribute(omp_construct_enum omptype, SgNode* mynode):
   mNode(mynode),omp_type(omptype){ 
     /*The initialization order has to match the declaration order, 
      * otherwise get a compilation warning*/
        init();
	assert(isDirective(omptype));
   }
  //!--------------AST connection------------------
  //! Get the associated SgPragmaDeclaration if any
  SgPragmaDeclaration* getPragmaDeclaration();

  //! Get the associated SgNode, can be SgPragmaDeclaration or others( during parallelization)
  SgNode* getNode(){return mNode;};
  void setNode(SgNode* n) { mNode= n;};
   //!------------directive type-------
   void setOmpDirectiveType(omp_construct_enum omptype){ assert (isDirective(omptype)); omp_type = omptype;}
   omp_construct_enum getOmpDirectiveType() {return omp_type;}

   //!-----------clauses----------------
   //!Add a clause into an OpenMP directive, the content of the clause is set by other interface, such as addVariable(), addExpression() , setReductionOperator() etc.
   void addClause(omp_construct_enum clause_type);
   //! Check if a directive has a clause of the specified type 
   bool hasClause(omp_construct_enum clause_type);

   //! Get all existing clauses
   std::vector<omp_construct_enum> getClauses();

   //!--------var list --------------
   //! Add a variable into a variable list of a construct
   void addVariable(omp_construct_enum targetConstruct, const std::string& varString,SgInitializedName* sgvar=NULL);
   //! Check if a variable list is associated with a construct
   bool hasVariableList(omp_construct_enum);
   //! Get the variable list associated with a construct
   std::vector<std::pair<std::string,SgNode* > > 
     getVariableList(omp_construct_enum);

   //! Find the relevant clauses for a variable 
   std::vector<enum omp_construct_enum> get_clauses(const std::string& variable);

   //!--------Expressions -----------------------------
   //! Add an expression to a clause
   void addExpression(omp_construct_enum targetConstruct, const std::string& expString, SgExpression*    sgexp=NULL); 

   //! Get expression of a clause
   std::pair<std::string, SgExpression*>  
   getExpression(omp_construct_enum targetConstruct);

   //!--------values for some clauses ----------
   // Reduction needs special handling 
   // since multiple ones with different operator types can co-exist within one pragma
   // We categories reduction clauses by their operator type and store variable lists for each of the reduction operator type, not with the reduction clause
   // Add a new reduction clauses with the specified operator
   void setReductionOperator(omp_construct_enum operatorx);
   //! Get reduction clauses for each operations,  reduction(op:kind)
   std::vector<omp_construct_enum> getReductionOperators();
   
   // default () value
   void setDefaultValue(omp_construct_enum valuex);
   omp_construct_enum getDefaultValue();

   // Schedule kind
   omp_construct_enum getScheduleKind(); 
   void setScheduleKind(omp_construct_enum kindx);

   //! Check if a variable is inside a variable list of a clause/directive.
   bool isInConstruct(const std::string & variable, enum omp_construct_enum);

   //! Set name for named critical section
   void setCriticalName(const std::string & name);
   std::string  getCriticalName() {return name;};
   bool isNamedCritical(){return hasName;};

   //!Pretty print the OmpAttribute
   void print(); 
   //! Convert OmpAttribute to a legal OpenMP pragma string, 
   //not named toString() to void ambiguous with OmpAttribute::toString()
   std::string toOpenMPString();
//------------------hide the implementation details, could be changed anytime!!
private:  
   //! The associated SgNode for this attribute, could be SgPragmaDeclaration or other nodes
   SgNode*  mNode; 

   //!Directive information: type of OpenMP directive
   enum omp_construct_enum  omp_type; 

   //! Clause information 
   // vector is used to preserve the order of clauses in the directive
   // map is used to fast query if a clause exists or not
   // Some clauses are allowed to appear more than once, merge the content into the first occurrence in our implementation.
   std::vector<omp_construct_enum> clauses;
   std::map<omp_construct_enum,bool> clause_map;

   // Multiple reduction clauses, each has a different operator
   //value for reduction operation: + -, * & | etc
   std::vector<omp_construct_enum> reduction_operators;
   //omp_construct_enum reduction_operator;

   //variable lists------------------- 
   //appeared within some directives and clauses
   //The clauses/directive are: flush, threadprivate, private, firstprivate, 
   //  shared, copyin, reduction, lastprivate, copyprivate
   // We use a pair of (name, SgNode) for each variable 
   // It is highly possible that a variable having more than one OpenMP properties.
   // For example, a variable can be both firstprivate and lastprivate.
   std::map<omp_construct_enum, std::vector<std::pair<std::string,SgNode* > > > variable_lists; 
   // A reverse map from a variable to the clauses the variable appears
   std::map<std::string, std::vector<omp_construct_enum> > var_clauses;

   // expressions ----------------------
   // e.g.: if (exp), num_threads(exp), schedule(,exp), collapse(exp)
   std::map<omp_construct_enum, std::pair<std::string, SgExpression*> > expressions;

   // values for some clauses -------------------------
  // values for default() clause: data scoping information
   // choices are: none,shared, private, firstprivate
   omp_construct_enum default_scope; 

   // value for omp for's schedule policies
   omp_construct_enum schedule_kind;

   // Only used for omp critical to indicate if it is named or not
   // name for the directive, only used for omp critical
   bool hasName; 
   std::string name; 
 
   // Misc fields  --------------------------------
   // help translation and analysis   
   bool isOrphaned; //true if parent omp parallel is not in the static lexical scope

  // Additional information to help translation
   int wrapperCount; // the shared variables from the same scope which needs wrapper

   //optional information
   OmpAttribute * parent; //upper-level OMP pragma's attribute

   //!Member functions --------------------------------- 
    //! Initialize internal data
    void init() ;

   //! Convert entire directives and clauses to string ,
    // invoke OmpSupport::toString() to stringify the enumerate type internally
   std::string toOpenMPString(omp_construct_enum omp_type);

   //! Convert a variable list to x,y,z ,without parenthesis.
   std::string toOpenMPString(std::vector<std::pair<std::string,SgNode* > >);
  }; // end class OmpAttribute


} //end namespace OmpSupport
