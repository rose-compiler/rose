/***********************************************************************
Goal:

OpenMP translator
* input: OpenMP source code
* output: multi-threaded code with calls to Omni's runtime library

Major classes:
* ASTtools:    generic AST manipulate toolkit

* OmpAttributeOld: persistent attribute for OpenMP pragma node
                   We have a new OmpAttribute to support OpenMP 3.0,
                   so we add This one with Old suffix to avoid conflict.
                  4/22/2009, Liao

* OmpFrontend: recognize/process pragma strings, make variables and their scope explicit
               top down traversal to build up OmpAttributeOld

* OmpMidend:   a set of translators for different OpenMP constructs
               bottom up traversal and tree restructure using OmpAttributeOld

* OmpProcessor: the driver(not in use)

-------------------------------------
Implemented OpenMP constructs:
  omp parallel 
  omp for, 
  schedule(static/dynamic/guided/runtime, [chunksize]),
  omp sections,
  private, firstprivate, lastprivate,
  reduction(+:), 
  copyin,
  threadprivate(file-scope, simple type), 
  nowait,
  omp single, 
  omp master, 
  omp critical(no name), 
  omp barrier
  omp flush

Not yet handled constructs:
  ordered, 
  num_threads,
  copyprivate
-------------------------------------
Limitations:
* the frontend of Rose does not really parse OpenMP pragmas yet. 
  OmpFrontend has to use string match to recognize pragma type and 
  associated semantic meanings, therefore the chunksize of schedule 
  clause is limited to single variable or constant only for this reason
* Only consider the default shared case for variable scope attributes
* Nested parallelism is not supported yet
* Some preprocessingInfo get lost after transformations
-------------------------------------
Acknowledgement: 
* This code borrows some good ideas from other OpenMP compilers such as Omni and OpenUH.

[Omni] Mitsuhisa Sato, Shigehisa Satoh, Kazuhiro Kusano, and Yoshio Tanaka. Design of OpenMP compiler for an SMP cluster. In Proc. of the 1st European Workshop on OpenMP, September 1999. 32-39

[OpenUH]Chunhua Liao, Oscar Hernandez, Barbara Chapman, Wenguang Chen and Weimin Zheng, "OpenUH: An Optimizing, Portable OpenMP Compiler", 12th Workshop on Compilers for Parallel Computers, A Coruna, Spain, Jan. 9-11, 2006

By Chunhua(Leo) Liao,  (University of Houston) , @ LLNL
   contact info: liaoch@cs.uh.edu

Date: June 15, 2006
Last modified: Apr, 2008
*************************************************************************/
#include "rose.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
using namespace std;
using namespace SageInterface; 
using namespace SageBuilder;

//------------------------------------------------------------------
// generic AST manipulation operations 
//------------------------------------------------------------------
class ASTtools
{
public:
  static SgNode* get_scope(SgNode* astNode); //get the closest scope statement
  static int get_scope_level(SgNode* astNode);
  static int deepCopy(SgStatement* srcstmt, SgBasicBlock * targetbb);
  // loop handling
  static SgExpression* getForLoopTripleValues(int valuetype,SgForStatement* forstmt );
  static int setForLoopTripleValues(int valuetype,SgForStatement* forstmt, SgExpression* exp);
  static SgInitializedName * getLoopIndexVar(SgForStatement* forstmt);
  static bool isLoopIndexVarRef(SgForStatement* forstmt, SgVarRefExp *varref);
  // Check if node1 is an ancester of node2 (parent of parent of.. node 2)
  // a reliable way to compare two scopes: 
  static bool isAncestor(SgNode* node1, SgNode* node2); 
};

//--------------------------------------------------------------
// OpenMP construct name list
enum    omp_construct_enum {
// major directives
   e_parallel=0,
   e_for,
   e_sections,
   e_section,
   e_single,
   e_master=5,
   e_critical,
   e_barrier,
   e_atomic,
   e_flush,
   e_ordered=10,
   e_threadprivate,
   e_parallel_for,
   e_parallel_sections,

//data scope attributes(clauses)
   e_default_none,
   e_default_shared=15,
   e_default_private,
   e_private=17,
   e_shared=18,
   e_firstprivate,
   e_lastprivate,
   e_copyin,

//other clauses
   e_nowait,

// reduction operations
   e_reduction_plus,
   e_reduction_minus,
   e_reduction_mul,
   e_reduction_bitand,
   e_reduction_bitor,
   e_reduction_bitxor,
   e_reduction_logand,
   e_reduction_logor,
   e_reduction_min,
   e_reduction_max,
   
//schedule policies
   e_sched_none = 33,
   e_sched_static = 34,
   e_sched_dynamic = 35,
   e_sched_guided,
   e_sched_runtime,
// not an OpenMP construct
   e_not_omp
};

// reduction data type, be consistent to Omni runtime library
#define OMPC_REDUCTION_CHAR 2
#define OMPC_REDUCTION_UNSIGNED_CHAR 3
#define OMPC_REDUCTION_SHORT 4
#define OMPC_REDUCTION_UNSIGNED_SHORT 5
#define OMPC_REDUCTION_SIGNED 6
#define OMPC_REDUCTION_INT 7
#define OMPC_REDUCTION_UNSIGNED_INT 8
#define OMPC_REDUCTION_LONG 9
#define OMPC_REDUCTION_UNSIGNED_LONG 10
#define OMPC_REDUCTION_LONGLONG 11
#define OMPC_REDUCTION_UNSIGNED_LONGLONG 12
#define OMPC_REDUCTION_FLOAT 13
#define OMPC_REDUCTION_DOUBLE  14
#define OMPC_REDUCTION_LONG_DOUBLE 15
#define OMPC_REDUCTION_MAX_TYPES 16
// reduction operator type
#define OMPC_REDUCTION_PLUS 6
#define OMPC_REDUCTION_MINUS 7
#define OMPC_REDUCTION_MUL 8
#define OMPC_REDUCTION_BITAND 9
#define OMPC_REDUCTION_BITOR 10
#define OMPC_REDUCTION_BITXOR  11
#define OMPC_REDUCTION_LOGAND 12
#define OMPC_REDUCTION_LOGOR 13
#define OMPC_REDUCTION_MIN 14
#define OMPC_REDUCTION_MAX 15
#define OMPC_REDUCTION_MAX_OPS 16

//------------------------------------------------------------------
//------------------------------------------------------------------
// The information about a variable appearing in OpenMP pragmas
// Mostly a pair of (SgInitializedName, omp_construct_enum)
// It is highly possible that a variable having more than one OpenMP properties.
// For example, a variable can be both firstprivate and lastprivate.
//------------------------------------------------------------------
//------------------------------------------------------------------
class OmpSymbol
{
public:
  //  OmpSymbol *next;
  SgNode* origVar; //SgInitializedName here
  omp_construct_enum ompType;  //e_shared, e_private,e_reduction_* 

  //constructor
  OmpSymbol(SgNode* orig=NULL, enum omp_construct_enum omptype=e_not_omp)\
        :origVar(orig),ompType(omptype){}
  void print(){
    SgInitializedName* initName= isSgInitializedName(origVar);
    cout<<"OmpSymbol:"<<(initName->get_name()).getString()<<" of type"<<ompType<<endl;
  }
};


//------------------------------------------------------------------
//------------------------------------------------------------------
// the persistent attribute attached to an OpenMP pragma node in SAGE III AST
// different types of pragmas need different information in some cases
// e.g.
//    'omp for' needs scheduling type , chunk size etc. 
//------------------------------------------------------------------
//------------------------------------------------------------------
class OmpAttributeOld : public AstAttribute 
{
private:
  void init() 
    {   
        parent = NULL;
        parentPragma = NULL;
        hasLastprivate=false;
        hasFirstprivate= false;
        hasReduction = false;
        nowait = false;
        ordered = false;
        isOrphaned = false;
        sched_type = e_sched_none;
        chunk_size = NULL;
        wrapperCount=0;
    }

 public:
   OmpAttributeOld * parent; //upper-level OMP pragma's attribute
   SgPragmaDeclaration *pragma;
   SgPragmaDeclaration *parentPragma;
   omp_construct_enum  omp_type; //pragma type
   Rose_STL_Container<OmpSymbol*> var_list; // all used variables with OpenMP scope info. etc
           // two OmpSymbols could have the same variable but different clause type

   int wrapperCount; // the shared variables from the same scope which needs wrapper
   bool hasLastprivate;
   bool hasFirstprivate;
   bool hasReduction;

   // omp parallel
   omp_construct_enum default_scope; 
   bool nowait;
   // omp for
   omp_construct_enum sched_type;
   SgNode*  chunk_size; //could be an expression
   bool ordered;
   bool isOrphaned; //true if parent omp parallel is not in the static lexical scope
  //constructor
   OmpAttributeOld()
    {
        init();
    }
   OmpAttributeOld(enum omp_construct_enum omptye):omp_type(omptye){
        init();
   }

   enum omp_construct_enum get_clause(SgInitializedName *);
   bool isInClause(SgInitializedName*, enum omp_construct_enum);
   void print() {//TODO print all
     cout<<"----------------------"<<endl;
     cout<<"pragma:"<<pragma->get_pragma()->get_pragma()<<endl;
     cout<<"Parent pragma is:";
     if(parentPragma != NULL) cout<<parentPragma->get_pragma()->get_pragma()<<endl;
     else cout<<"none"<<endl;
     cout<<"OmpAttributeOld: pragma type: "<<omp_type<<endl;
     cout<<"isOrphaned: "<<isOrphaned<<endl;
     cout<<"Number of shared variables needing wrapping:"<<wrapperCount<<endl;
     cout<<"Variable list :"<<endl;
     for(Rose_STL_Container<OmpSymbol*>::iterator i= var_list.begin();i!=var_list.end();i++)
        (*i)->print();
    if (omp_type == e_parallel_for || omp_type == e_for)
    {
       cout<<"sched_type is:"<<sched_type<<endl;
       cout<<"chunk_size is:";
       if (chunk_size == NULL)
        cout<<"not set"<<endl;
       else if(isSgIntVal(chunk_size) != NULL) 
            cout<<isSgIntVal(chunk_size)->get_value()<<endl;
       else if(isSgInitializedName(chunk_size))
            cout<<(isSgInitializedName(chunk_size)->get_name()).getString()<<endl;
     }
  }//end print
};
//------------------------------------------------------------------
//------------------------------------------------------------------
// This class will process the pragma fully and generate OpenMP information
// It uses some (ugly) workaround since there is no OpenMP frontend in EDG
//------------------------------------------------------------------
//------------------------------------------------------------------
class OmpFrontend : public AstSimpleProcessing
{
public:
  static SgPragmaDeclaration* parentPragma;
  Rose_STL_Container<string> threadprivatelist;// TODO delete this, use threadprivateinitnamelist
  Rose_STL_Container<SgInitializedName *> threadprivateinitnamelist;

  OmpFrontend() {parentPragma = NULL; }
 // other utilities, some of them should be moved to ASTtools (TODO)
  static Rose_STL_Container<string>* parsePragmaString(enum omp_construct_enum,const char* pragmaChars);
  static void formalizePragma(SgPragmaDeclaration* decl)
    {
      //TODO one time processing to eliminate unnecessary spaces, unify lower case/uper case
        // then OpenMP pragma recognization is easier
        // skipped with the assumption that all test code is well written.
    };
  static char *trim_leading_trailing_spaces(char*);
  static enum omp_construct_enum recognizePragma(SgPragmaDeclaration*);
  static void removePreprocessingInfo(SgProject * project); 
  int createOmpAttributeOld(SgNode *);
  static int removeIfdefOmp(SgProject* project);  
//TODO (low priority) remove #ifdef _OPENMP #endif ,
//current workaround is to pass -D_OPENMP to backend compilers
  static bool isInStringList(Rose_STL_Container<string>* slist,string* input );
  void print();
protected:
  void virtual visit (SgNode*);
private:

} ;

SgPragmaDeclaration * OmpFrontend::parentPragma= NULL;

//----------------------------------------------------------------------
//----------------------------------------------------------------------
// main class to do all OpenMP related transformations
//   contains a set of public translators for each construct, 
//     along with some private utility functions
//----------------------------------------------------------------------
//----------------------------------------------------------------------
class OmpMidend
{
public:
  //TODO  a centralized list of the internal runtime functions' names, for possible changes
  //      like:  string rtlDoParallel("_ompc_do_parallel"); etc
  static int counter; //unique number for outlined functions 
  static int labelCounter; //for labels 

  static int insertHeaders(SgProject*); // put it into OmpMidend is better now 
  static int bottomupProcessing(SgProject *);
  static int transParallelRegion(SgPragmaDeclaration * decl);
  static int transOmpFor(SgPragmaDeclaration *decl);
  static int transParallelFor(SgPragmaDeclaration *decl);
  static int transParallelSections(SgPragmaDeclaration *decl);
  static int transSections(SgPragmaDeclaration * decl);
  static int transCritical(SgPragmaDeclaration *decl);
  static int transAtomic(SgPragmaDeclaration *decl);
  static int transSingle(SgPragmaDeclaration * decl);
  static int transMaster(SgPragmaDeclaration * decl);
  static int transBarrier(SgPragmaDeclaration * decl);
  static int transFlush(SgPragmaDeclaration * decl);
  static int insertRTLinitAndCleanCode(SgProject*, OmpFrontend *);
private:
  // variable handling routines
  static int addGlobalOmpDeclarations(OmpFrontend*, SgGlobal*, bool hasMain);
  static int addThreadprivateDeclarations(SgPragmaDeclaration *,OmpAttributeOld *, SgBasicBlock *);
  static int addReductionCalls(SgPragmaDeclaration *, OmpAttributeOld *, SgBasicBlock *);
  static int addLastprivateStmts(SgPragmaDeclaration *, OmpAttributeOld *, SgBasicBlock *);
  static int addPrivateVarDeclarations(SgPragmaDeclaration *, OmpAttributeOld *, SgBasicBlock *);
  static int addSharedVarDeclarations(SgPragmaDeclaration *, SgFunctionDeclaration*, \
                OmpAttributeOld *, SgBasicBlock *);
  static int variableSubstituting(SgPragmaDeclaration *,OmpAttributeOld*, SgNode*);

  static int splitCombinedParallelForSections(SgPragmaDeclaration* decl);
  static SgFunctionDeclaration* generateOutlinedFunction(SgPragmaDeclaration* decl);
  static void generateOutlinedFunctionDefinition(SgPragmaDeclaration* decl,\
                                SgFunctionDeclaration * func, OmpAttributeOld *ompattribute);
  static void insertOutlinedFunction(SgPragmaDeclaration* pragDecl, \
                                SgFunctionDeclaration *outlinedFunc);
  static void replacePragmaBlock(SgPragmaDeclaration* pragDecl, SgBasicBlock *bb1);

  static SgBasicBlock* generateParallelRTLcall(SgPragmaDeclaration* pragDecl, SgFunctionDeclaration * outlinedFunc, OmpAttributeOld *ompattribute);
  static int convertTypeId(SgType* sgtype);
  static int getSectionCount(SgPragmaDeclaration *decl);
  // C++ outlined member function's wrapper generation
  static int generateWrapperFunction(SgClassDefinition*, SgMemberFunctionDeclaration*);

};

int OmpMidend::counter=0;
int OmpMidend::labelCounter=0;

//-------------------
// find the OpenMP clause type for a variable
//Be careful when use this because a variable can have multiple types
// and this function only returns the first one
// only used to check if a name is in the OmpSymobl list or not currently!
//-------------------
 enum omp_construct_enum OmpAttributeOld::get_clause(SgInitializedName *initname)
{
  enum omp_construct_enum rttype=e_not_omp;
  for (Rose_STL_Container<OmpSymbol*>::iterator i= var_list.begin(); i!= var_list.end();i++)
  {
    if (isSgInitializedName((*i)->origVar) == initname)
     {
         rttype=(*i)->ompType; 
         break;
     }
  }
  return rttype;
}

//--------------------------
//Judge if a name is in a clause's variable list: like private(), copyin(), etc.
bool OmpAttributeOld::isInClause(SgInitializedName* initname, enum omp_construct_enum omptype)
{
  bool rt=false;
  ROSE_ASSERT(initname != NULL);
  for (Rose_STL_Container<OmpSymbol*>::iterator i= var_list.begin(); i!= var_list.end();i++)
  {
    if ((isSgInitializedName((*i)->origVar) == initname)&& (omptype==(*i)->ompType))
      {
        rt= true;
        break;
      }
  }
  return rt;
}

//--------------------------
inline void OmpFrontend::print()
{
  if (threadprivatelist.size()>0)
  {
    cout<<"Threadprivate string variables:-------"<<endl;
     for (Rose_STL_Container<string>::iterator i=threadprivatelist.begin();\
        i!=threadprivatelist.end();i++)
     cout<<*i<<endl;
  }

  if( threadprivateinitnamelist.size()>0)
  {
    cout<<"Threadprivate initializedName variables:-------"<<endl;
    for (Rose_STL_Container<SgInitializedName *>::iterator i=threadprivateinitnamelist.begin();\
        i!=threadprivateinitnamelist.end();i++)
    {
      string varname = (*i)->get_name().getString();
      cout<<varname<<endl;
    }
  }//end if threadprivate
}
//--------------------------
inline bool OmpFrontend::isInStringList(Rose_STL_Container<string>* slist,string* input )
 {  bool rt=false;
        for (Rose_STL_Container<string>::iterator i=slist->begin();i!=slist->end();i++)
                if ((*i)==(*input)) { rt=true; break; }
      return rt;
   }

//--------------------------
char * OmpFrontend::trim_leading_trailing_spaces(char* input)
 {
  char * output =input;
  char * tail;
   ROSE_ASSERT(output != NULL);
  while (*output==' ')
   output++;

  tail = output + strlen(output)-1;

  while ((*tail==' ')&&(tail>output))
     *tail--='\0';
  return output;

 }
//----------------------------
//Given the clause type,  get arguments or options from the OpenMP pragma string
// for example  parsePragma(e_private, "#pragma omp parallel private(x,y,z)") will
//              return a pointer to a list of strings: "x","y","z".
// for schedule clause, return the kind and chunk_size; 
//      limit chunk_size to one variable or a constant here, 
// for reduction, Only handle the most frequently used 'reduction plus' here. 
//
// TODO: remove this when the frontend(FE) is ready
Rose_STL_Container<string>* OmpFrontend::parsePragmaString ( enum omp_construct_enum omptype, const char* pragmaChars )
   {
     Rose_STL_Container<string> * rtstrings = new Rose_STL_Container<string>;
     ROSE_ASSERT(rtstrings != NULL);

  // printf ("Inside of OmpFrontend::parsePragmaString(): omptype = %d pragmaChars = %s \n",omptype,pragmaChars);

  // DQ (1/3/2007): This is meaningless code!
     rtstrings->empty();

     const char *offset;
     const char *input = pragmaChars;
     char matchchars[100] = "";
     char argchars[100] = "";
     char s[100] = "";
     int counter = 0;

  // printf ("In OmpFrontend::parsePragmaString(): before setting matchchars: input = %s matchchars = %s \n",input,matchchars); 

     if (omptype==e_private) strcpy(matchchars," private"); // a space avoid confusing firstprivate
     else if (omptype==e_firstprivate) strcpy(matchchars,"firstprivate");
     else if (omptype==e_lastprivate) strcpy(matchchars,"lastprivate");
     else if (omptype==e_reduction_plus) strcpy(matchchars,"reduction");
     else if (omptype==e_copyin) strcpy(matchchars,"copyin");
     else if (omptype==e_sched_none) strcpy(matchchars,"schedule");
     else if (omptype==e_threadprivate) strcpy(matchchars,"threadprivate");
     else if (omptype==e_flush) strcpy(matchchars,"flush");
     else
        {
          cout<<"OmpFrontend::parsePragmaString reaches unknown clause:"<<omptype<<endl;
          ROSE_ASSERT(false);
        }

//   printf ("In OmpFrontend::parsePragmaString(): input = %s matchchars = %s \n",input,matchchars); 

     offset = strstr(input,matchchars); 
     if (offset == NULL)
        {
       // printf ("Inside of OmpFrontend::parsePragmaString (premature return (returning an empty list)) \n");

       // DQ (1/3/2007): Added assertion that list is non-empty before defererence of first element.
       // ROSE_ASSERT(rtstrings->empty() == false);

          return rtstrings;
        }


     while ((*offset!='(')&&(*offset!='\0'))
          offset ++;

     if (*offset == '\0')
        {
       // printf ("Inside of OmpFrontend::parsePragmaString (premature return) \n");

       // DQ (1/3/2007): Added assertion that list is non-empty before defererence of first element.
        // flush 's variable list is optional, Liao 4/24/2008
         if (omptype != e_flush)
            ROSE_ASSERT(rtstrings->empty() == false);
       //   printf("Debug: L617: found omp flush without a variable list.\n");
          return rtstrings;
        }

     if (omptype == e_reduction_plus)
          while (*offset!=':') 
               offset ++;


     do {
          offset++;
          counter=0;
          while ((*offset!=',')&&(*offset!=')'))
             {  
               argchars[counter]=*offset;
               offset ++;
               counter++;
             }
          argchars[counter]='\0';
          strcpy(s,trim_leading_trailing_spaces(argchars));
       // printf("debug: found argument:|%s|%s|\n","full",s);
          rtstrings->push_back(s);
          counter=0;
        }
     while((*offset!=')') &&(*offset!='\0'));

  // DQ (1/3/2007): Added assertion that list is non-empty before refrerence of first element.
  // flush 's variable list is optional, Liao 4/24/2008
   if (omptype != e_flush)
      ROSE_ASSERT(rtstrings->empty() == false);

  // printf ("Returning a valid list! (rtstrings->empty() == false) \n");

     return rtstrings;
   }

//----------------------------
// build OpenMP attribute for different types of OpenMP constructs
//  e.g.  the list of OpenMP symbols with data scope attribute
//         scheduling type, chunk size of 'omp for'
// For  parallel region etc : clause is one of the following:
//       private(list), firstprivate(list),default(shared|none), shared(list),copyin(list)
//       if (scalar-expression), num_threads(integer-expression)
//Major task: make data scope attribute explicit and store them into OmpAttributeOld
//For all used variable in the parallel region, only those with the same (or higher) scope
//as( or than) the pragma matter in order to build up private and shared variable lists
// e.g. Assume the default(shared) case, all variables of interest which are not in 
//      private, reduction, firstprivate, ...lists should be treated as shared
// We do not need interprocedural analysis to find out all referenced variables in a parallel region
//  since we really only care the variables in the same scope 

// TODO consider default(private)
// TODO split into several smaller functions
// TODO handle other constructs with attributes: omp critical,etc
int OmpFrontend::createOmpAttributeOld(SgNode* node)
   {
     SgPragmaDeclaration* pragDecl = isSgPragmaDeclaration(node);
     ROSE_ASSERT(pragDecl != NULL);
     AstAttribute * ompattribute = NULL;

  // ----------------------------------------------
     if ((recognizePragma(pragDecl)==e_parallel) || (recognizePragma(pragDecl)==e_parallel_for)||
         (recognizePragma(pragDecl)==e_parallel_sections)|| (recognizePragma(pragDecl)==e_sections)||
         (recognizePragma(pragDecl)==e_for) )
        {
          ompattribute = new OmpAttributeOld(recognizePragma(pragDecl));
          dynamic_cast<OmpAttributeOld*> (ompattribute)->pragma = pragDecl;
       // cout<<"L553, .... test constructor.."<<endl;
       // dynamic_cast<OmpAttributeOld*> (ompattribute)->print();

      // 0. assume top down processing, using the static parentPragma to store the most recent
      //    'omp parallel', which should be the parent parallel region for 'omp for' etc.
      //    TODO a more robust way to set parent pragma
          if ((recognizePragma(pragDecl)==e_parallel)|| (recognizePragma(pragDecl)==e_parallel_for))
             {
               dynamic_cast<OmpAttributeOld*> (ompattribute)->parentPragma = NULL;
               parentPragma =  pragDecl;
             }
            else  
               dynamic_cast<OmpAttributeOld*> (ompattribute)->parentPragma = parentPragma;

       // set isOrphaned for "omp for" and others?
          if (recognizePragma(pragDecl)==e_for)
             {
               if (parentPragma == NULL) 
                    dynamic_cast<OmpAttributeOld*>(ompattribute)->isOrphaned = true;// no parentPragma, must be orphaned
                 else
                    if ((getEnclosingFunctionDeclaration(parentPragma) == getEnclosingFunctionDeclaration(node))&& (recognizePragma(parentPragma)==e_parallel))
                         dynamic_cast<OmpAttributeOld*>(ompattribute)->isOrphaned = false; // has parent omp parallel in the same scope
                      else 
                         dynamic_cast<OmpAttributeOld*>(ompattribute)->isOrphaned = true; // must be orphaned otherwise
             }

       // set nowait
          string pragmaString = pragDecl->get_pragma()->get_pragma();
          if (pragmaString.find("nowait")!=string::npos) // TODO change this to formal one
               dynamic_cast<OmpAttributeOld*> (ompattribute)->nowait = true;
          ROSE_ASSERT(ompattribute !=NULL );

       // 1. Find all reference variables of interest from outer or same scope
          Rose_STL_Container<SgNode*> all_var_list; //unique declaration list for referenced variables
          Rose_STL_Container<SgNode*> used_variables = NodeQuery::querySubTree(getNextStatement(pragDecl),V_SgVarRefExp);
       // cout<<"debug: used variable size is: "<<used_variables.size()<<endl;

          SgScopeStatement* currentscope= isSgFunctionDeclaration( getEnclosingFunctionDeclaration(node))->get_definition()->get_body();
          for (Rose_STL_Container<SgNode*>::iterator i=used_variables.begin();i!=used_variables.end();i++)
             {
            // only care the used variables from the same scope and outer scope
            // interestingly, outer scope is 'smaller' currentscope >= orig_scope
               SgInitializedName* initname= isSgVarRefExp(*i)->get_symbol()->get_declaration();
               SgScopeStatement* orig_scope=isSgScopeStatement(ASTtools::get_scope(initname));
               if (( currentscope == orig_scope)||ASTtools::isAncestor(orig_scope,currentscope) )
                    all_var_list.push_back(initname);
             } //end for

       // 2. Get all variables from private(x,y,z..), reduction(), firstprivate(), lastprivate()
          Rose_STL_Container<string>* private_name_listptr;
          Rose_STL_Container<string>* firstprivate_name_listptr;
          Rose_STL_Container<string>* lastprivate_name_listptr;
          Rose_STL_Container<string>* copyin_name_listptr;// only valid for 'omp parallel *'
          Rose_STL_Container<string>* reduction_plus_name_listptr; //TODO all reduction operators

          const char *pragmachars     = pragDecl->get_pragma()->get_pragma().c_str();

          private_name_listptr        = parsePragmaString(e_private, pragmachars);
          copyin_name_listptr         = parsePragmaString(e_copyin, pragmachars);
          firstprivate_name_listptr   = parsePragmaString(e_firstprivate, pragmachars);
          lastprivate_name_listptr    = parsePragmaString(e_lastprivate, pragmachars);
          reduction_plus_name_listptr = parsePragmaString(e_reduction_plus, pragmachars);

      // 3. Get scheduling kind and chunksize
          if ((dynamic_cast<OmpAttributeOld*> (ompattribute)->omp_type == e_parallel_for)|| (dynamic_cast<OmpAttributeOld*> (ompattribute)->omp_type == e_for) )
             {
            // printf ("Calling parsePragmaString which appear to be a problem! e_sched_none = %d pragmachars = %s \n",e_sched_none,pragmachars);

               Rose_STL_Container<string>* sched_info_listptr = parsePragmaString(e_sched_none,pragmachars);

            // DQ (1/3/2007): Added assertion that list is non-empty before defererence of first element.
            // ROSE_ASSERT(sched_info_listptr->empty() == false);

            // 3.1 handle scheduling kind
               if ( sched_info_listptr->size() == 0)
                  {
                    dynamic_cast<OmpAttributeOld*> (ompattribute)->sched_type = e_sched_none;
                    dynamic_cast<OmpAttributeOld*> (ompattribute)->chunk_size = NULL;
                  }

            // DQ (1/3/2007): Added assertion that list is non-empty before defererence of first element.
            // ROSE_ASSERT(sched_info_listptr->empty() == false);

            // DQ (1/3/2007): Moved this to introduce conditional for where list size is greater than 0
               int list_size = sched_info_listptr->size();
               ROSE_ASSERT(list_size <= 2);

            // DQ (1/3/2007): Added code to handle case where list is returned empty (which seems to be appropriate for the parsePragmaString() member function)
               if (list_size > 0)
                  {
                    if ( *(sched_info_listptr->begin())==*(new string("static")))
                       {
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->sched_type = e_sched_static;
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->chunk_size = NULL;
                       }

                    if ( *(sched_info_listptr->begin())==*(new string("dynamic")))
                       {
                      // default chunk size is 1
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->sched_type = e_sched_dynamic;
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->chunk_size = buildIntVal(1);
                       }

                    if ( *(sched_info_listptr->begin())==*(new string("guided")))
                       {
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->sched_type = e_sched_guided;
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->chunk_size = buildIntVal(1);
                       }
                    if ( *(sched_info_listptr->begin())==*(new string("runtime")))
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->sched_type = e_sched_runtime;
                  }

            // int list_size = sched_info_listptr->size();
            // ROSE_ASSERT(list_size <= 2);// schedule(kind, chunksize)

            // 3.2 handle chunksize
               if (list_size == 2)
                  {
                    string chunksize = *(++(sched_info_listptr->begin()));
     
                 // cout<<"632, parsed chunksize is:"<<chunksize<<endl;
                 // only handle two cases here: integer or single variable
                    std::istringstream iss(chunksize);
                    int ichunksize;
                    if ((iss>>std::dec>>ichunksize).fail())
                       {
                      // single variable
                         SgVariableSymbol *symbol = lookupVariableSymbolInParentScopes(
                          chunksize,pragDecl->get_scope()); 
                         ROSE_ASSERT(symbol!= NULL);
                         SgInitializedName* initname = symbol->get_declaration();
                         ROSE_ASSERT(initname != NULL);
                      // fatal error if not found or it is an expression
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->chunk_size = initname;

                      // special handling of chunksize variable's scope
                      // propagate the variable used in chunksize to parent 'omp for' pragma since
                      // it may not shown in original loop body and will add into runtime call
                      // during the transformation, so the parallel region has treat it as a new
                      // shared variable
                      // TODO a more elegant solution when FE is ready
                         SgScopeStatement* varscope = isSgScopeStatement(ASTtools::get_scope(initname));
                      // chunksize variable must be shared
                      // two cases: omp_parallel_for , add into var_list
                      //            omp_for,  add into parent's ompsymbol list
                         if (dynamic_cast<OmpAttributeOld*> (ompattribute)->omp_type == e_parallel_for)
                              all_var_list.push_back(initname);
                           else
                            {
                           // orphaned omp for, add into parent pragma's ompattribute
                              SgPragmaDeclaration *parentpragma = dynamic_cast<OmpAttributeOld*> (ompattribute)->parentPragma;
                              ROSE_ASSERT(parentpragma != NULL);
                              AstAttribute* astattribute2 = parentpragma->getAttribute("OmpAttributeOld");
                              OmpAttributeOld *  ompattribute2= dynamic_cast<OmpAttributeOld* > (astattribute2);
                           // insert it only if not exist
                              if (ompattribute2->get_clause(initname)==e_not_omp)
                                 {
                                   if (currentscope==varscope || isSgFunctionParameterList(isSgInitializedName(initname)->get_parent()))
                                        ompattribute2->wrapperCount++;
                                   OmpSymbol *ompsymbol = new OmpSymbol(initname,e_shared);
                                   ompattribute2->var_list.push_back(ompsymbol);
                                 } //end if not exist
                            } // end if omp_parallel_for
                       } // end variable case of chunksize
                      else 
                         dynamic_cast<OmpAttributeOld*> (ompattribute)->chunk_size = buildIntVal(ichunksize);
                  } // end if list_size==2
             } // end of 3. schedule clause handling

    //4.  Insert variable list into OmpAttributeOld with the correct scope info.
        // also set the counter for shared variables needing wrapping. 
        // TODO consider all possible combination cases: 

   // DQ (9/26/2007): Not supported by std::vector
   // all_var_list.sort(); // only store unique items
      std::sort(all_var_list.begin(),all_var_list.end());
   // all_var_list.unique(); // all variables of interest, shared by default
      Rose_STL_Container<SgNode*>::iterator new_end;
      new_end = std::unique (all_var_list.begin(), all_var_list.end());
      all_var_list.erase(new_end, all_var_list.end());

    //  printf ("Commented out sort() and unique() member function calls because they don't exist for std::vector \n");

      dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount=0;

      for (Rose_STL_Container<SgNode*>::iterator i=all_var_list.begin();i!=all_var_list.end();i++)
        { 
         OmpSymbol * ompsymbol;
          string varName=string(isSgInitializedName(*i)->get_name().str());
      SgScopeStatement* varscope = isSgScopeStatement(ASTtools::get_scope(*i));
           //insert copyin variables
          if ( isInStringList(copyin_name_listptr, &varName) ) {
            ompsymbol=new OmpSymbol((*i),e_copyin);
           dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
           }  
            //insert threadprivate variables
          if ( isInStringList(&threadprivatelist, &varName) ) {
            ompsymbol=new OmpSymbol((*i),e_threadprivate);
           dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
           } else 
            // insert private variables
          if ( isInStringList(private_name_listptr, &varName) ) {
            ompsymbol=new OmpSymbol((*i),e_private);
           dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
           }
          else if ( isInStringList(firstprivate_name_listptr, &varName)||\
                  isInStringList(lastprivate_name_listptr, &varName) )
           {    
                // a variable could be both first and last private
// both firstprivate and lastprivate need to access shared copy, which demands
// a wapper
             // firstprivate
            if ( isInStringList(firstprivate_name_listptr, &varName) ){
              if (currentscope==isSgScopeStatement(ASTtools::get_scope(*i)))
                 dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount++;
              ompsymbol=new OmpSymbol((*i),e_firstprivate);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->hasFirstprivate = true;
            }
            if ( isInStringList(lastprivate_name_listptr, &varName) )
            { // lastprivate
              if (currentscope==isSgScopeStatement(ASTtools::get_scope(*i)))
                 dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount++;
              ompsymbol=new OmpSymbol((*i),e_lastprivate);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->hasLastprivate = true;
            }
           }
         else if ( isInStringList(reduction_plus_name_listptr, &varName) )
            {// for reduction variables from the scope, also need wrapper
            if (currentscope==isSgScopeStatement(ASTtools::get_scope(*i)))
              dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount++;
              ompsymbol=new OmpSymbol((*i),e_reduction_plus);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->hasReduction = true;
            }
           else { // TODO: consider default(private) case
           // store the rest as the shared variables,
           //count the ones with the same scope as shared to be wrapped
        // special handling for function parameters
        // the scope of a function parameter has some problem(bug?)
            if (currentscope==varscope ||
         isSgFunctionParameterList(isSgInitializedName(*i)->get_parent()))
               dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount++;
            ompsymbol=new OmpSymbol((*i),e_shared);
           dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
            }

       }//end for
        node->addNewAttribute("OmpAttributeOld",ompattribute);
        //cout<<"L827,debug "<<endl;
        //dynamic_cast<OmpAttributeOld*>(ompattribute)->print();
   } // end if 
  else
//---------------------------------------------------------
// parse private(), firstprivate, copyprivate and nowait for 'omp single'
// TODO consider orphaned omp single
  if (recognizePragma(pragDecl)==e_single)
  {
     ompattribute = new OmpAttributeOld(recognizePragma(pragDecl));
     dynamic_cast<OmpAttributeOld*> (ompattribute)->pragma = pragDecl;

    dynamic_cast<OmpAttributeOld*> (ompattribute)->parentPragma = parentPragma;
     string pragmaString = pragDecl->get_pragma()->get_pragma();
     if (pragmaString.find("nowait")!=string::npos) // TODO change this to formal one
     dynamic_cast<OmpAttributeOld*> (ompattribute)->nowait = true;
      ROSE_ASSERT(ompattribute !=NULL );
     //1. find all reference variables of interest from outer or same scope
      Rose_STL_Container<SgNode*> all_var_list; //unique declaration list for referenced variables
      Rose_STL_Container<SgNode*> used_variables =\
            NodeQuery::querySubTree(getNextStatement(pragDecl),V_SgVarRefExp);
      SgScopeStatement* currentscope= isSgFunctionDeclaration(\
                   getEnclosingFunctionDeclaration(node))\
                   ->get_definition()->get_body();
      for (Rose_STL_Container<SgNode*>::iterator i=used_variables.begin();i!=used_variables.end();i++)
      {
        // only care the used variables from the same scope and outer scope
        // interestingly, outer scope is 'smaller'
         SgInitializedName* initname= isSgVarRefExp(*i)->get_symbol()->get_declaration();
         //if ( currentscope >= isSgScopeStatement(ASTtools::get_scope(initname)) )
         SgScopeStatement* orig_scope = isSgScopeStatement(ASTtools::get_scope(initname));
         if (( currentscope==orig_scope) || ASTtools::isAncestor(orig_scope,currentscope))
            all_var_list.push_back(initname);
       } //end for

   // DQ (9/26/2007): Not supported by std::vector
   //   printf ("Commented out sort() and unique() member function calls because they don't exist for std::vector \n");
   // all_var_list.sort(); // only store unique items
      std::sort(all_var_list.begin(),all_var_list.end());
   // all_var_list.unique(); // all variables of interest, shared by default
      Rose_STL_Container<SgNode*>::iterator new_end;
      new_end = std::unique (all_var_list.begin(), all_var_list.end());
      all_var_list.erase(new_end, all_var_list.end());

       //2. Get all variables from private(x,y,z..), etc
       Rose_STL_Container<string>* private_name_listptr;
       Rose_STL_Container<string>* firstprivate_name_listptr;
       // TODO std::list<string>* copyprivate_name_listptr;

       const char *pragmachars=pragDecl->get_pragma()->get_pragma().c_str();
       private_name_listptr= parsePragmaString(e_private, pragmachars);
       firstprivate_name_listptr= parsePragmaString(e_firstprivate, pragmachars);
        // insert variable list into OmpAttributeOld with the correct scope info.
       for (Rose_STL_Container<SgNode*>::iterator i=all_var_list.begin();i!=all_var_list.end();i++)
        {
         OmpSymbol * ompsymbol;
          string varName=string(isSgInitializedName(*i)->get_name().str());
           // insert private variables
          if ( isInStringList(private_name_listptr, &varName) ) {
            ompsymbol=new OmpSymbol((*i),e_private);
           dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
           }
          else if ( isInStringList(firstprivate_name_listptr, &varName))
           {
              if (currentscope==isSgScopeStatement(ASTtools::get_scope(*i)))
                 dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount++;
              ompsymbol=new OmpSymbol((*i),e_firstprivate);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
              dynamic_cast<OmpAttributeOld*> (ompattribute)->hasFirstprivate = true;
          }
       }//end for
        node->addNewAttribute("OmpAttributeOld",ompattribute);
        
   } // end if single
   
   // Handle omp flush(x,y,z,...)
  if (recognizePragma(pragDecl)==e_flush)
  {
    ompattribute = new OmpAttributeOld(recognizePragma(pragDecl));
    ROSE_ASSERT(ompattribute !=NULL );

    dynamic_cast<OmpAttributeOld*> (ompattribute)->pragma = pragDecl;
    dynamic_cast<OmpAttributeOld*> (ompattribute)->parentPragma = parentPragma;
    dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount = 0;

    //parse the optional variable list associated with omp flush
    const char *pragmachars=pragDecl->get_pragma()->get_pragma().c_str();
    Rose_STL_Container<string>* flush_name_listptr;
    flush_name_listptr= parsePragmaString(e_flush, pragmachars);

     // insert variable list into OmpAttributeOld
    for (Rose_STL_Container<string>::iterator i=flush_name_listptr->begin();
                      i!=flush_name_listptr->end();i++)
     {
       SgName name1(*i);
       SgVariableSymbol* symbol = isSgVariableSymbol(SageInterface::lookupSymbolInParentScopes
                      (name1,pragDecl->get_scope()));
       ROSE_ASSERT(symbol);
       SgInitializedName* initName= symbol->get_declaration();

        // insert flush variables
       OmpSymbol* ompsymbol=new OmpSymbol(initName,e_flush);
       dynamic_cast<OmpAttributeOld*> (ompattribute)->var_list.push_back(ompsymbol);
    }//end for
    node->addNewAttribute("OmpAttributeOld",ompattribute);
  }

//  cout<<"---------L1029 debug "<<endl;
//  if (ompattribute != NULL ) dynamic_cast<OmpAttributeOld*>(ompattribute)->print();
  return 0;
}

//----------------------------
// tasks:
//     build OmpAttributeOld for OMP pragma node
void OmpFrontend::visit(SgNode * node)
{
  if (isSgPragmaDeclaration(node) != NULL)
  {
    SgPragmaDeclaration * pragDecl = isSgPragmaDeclaration(node);
    // handle omp parallel, omp for, omp single, ..etc.
    createOmpAttributeOld(node);

    // handle threadprivate()
    //TODO consider threadprivate variables not from file scope in the transformation
    //     attach threadprivate info to some scope statements  
    //     for each scope, check if there are visible threadprivate variables
   if (recognizePragma(pragDecl) == e_threadprivate) {
     Rose_STL_Container<string> resultlist = *parsePragmaString(e_threadprivate,\
                pragDecl->get_pragma()->get_pragma().c_str()); 
    
     for (Rose_STL_Container<string>::iterator i= resultlist.begin(); i!=resultlist.end();i++) 
     { 
#if 0       
     // find reached definitions and store them 
       reachDefinitionFinder * finder = new reachDefinitionFinder (node,&(*i));
       finder->traverse(SageInterface::getGlobalScope(node),preorder);
       ROSE_ASSERT(finder->result != NULL);
       threadprivateinitnamelist.push_back(finder->result);
#else
      SgVariableSymbol * symbol=lookupVariableSymbolInParentScopes(SgName(*i),
             pragDecl->get_scope());
      ROSE_ASSERT(symbol);
      SgInitializedName * init_name = symbol->get_declaration();
      threadprivateinitnamelist.push_back(init_name);
#endif       
     } // end for

      threadprivatelist.insert(threadprivatelist.end(),resultlist.begin(),resultlist.end());// merge operation will empty resultlist!!

// TODO handle duplicated declarations in frontend's semantic check
//    threadprivatelist.sort();
//    threadprivatelist.unique();
    }
  } 

}// end OmpFrontend::visit

//----------------------------
// ugly workaround for lack of the OpenMP pragma parser
// the order of if stmt matters!
// TODO replace it when the frontend is ready 
omp_construct_enum OmpFrontend::recognizePragma(SgPragmaDeclaration* pragmaDecl)

{
    formalizePragma(pragmaDecl);
    string pragmaString = pragmaDecl->get_pragma()->get_pragma();

    if (pragmaString.find("omp parallel for")!=string::npos)
        return e_parallel_for;
    if (pragmaString.find("omp parallel sections")!=string::npos)
        return e_parallel_sections;
    if (pragmaString.find("omp sections")!=string::npos)
        return e_sections;
    if (pragmaString.find("omp section")!=string::npos) 
        return e_section;
    if (pragmaString.find("omp parallel")!=string::npos)
        return e_parallel;
    if (pragmaString.find("omp critical")!=string::npos)
        return e_critical;
    if (pragmaString.find("omp atomic")!=string::npos)
        return e_atomic;
    if (pragmaString.find("omp master")!=string::npos)
        return e_master;
    if (pragmaString.find("omp single")!=string::npos)
        return e_single;
    if (pragmaString.find("omp barrier")!=string::npos)
        return e_barrier;
    if (pragmaString.find("omp for")!=string::npos)
        return e_for;
    if (pragmaString.find("omp threadprivate")!=string::npos)
        return e_threadprivate;
    if (pragmaString.find("omp flush")!=string::npos)
        return e_flush;
     return e_not_omp; 
}

//----------------------------
// deep copy srcstmt and insert it into target basic block
// if srcstmt is a basic block, deep copy its stmts and insert them
int ASTtools::deepCopy(SgStatement* srcstmt, SgBasicBlock * targetbb)
{
  ROSE_ASSERT(srcstmt !=NULL);
  ROSE_ASSERT(targetbb !=NULL);

   if (isSgBasicBlock(srcstmt)==NULL) {
      // copy a statement
     SgStatement* myStatement= isSgStatement(SageInterface::deepCopy(srcstmt));
     appendStatement(myStatement,targetbb);
   }
   else
   {  // deep copy every statement from the basic block
     SgStatementPtrList srcStmtList = isSgBasicBlock(srcstmt)->get_statements();
    for (Rose_STL_Container<SgStatement*>::iterator i=srcStmtList.begin();i!=srcStmtList.end();i++){
        SgStatement* mystmt= isSgStatement(SageInterface::deepCopy(*i));
        appendStatement(mystmt,targetbb);
     }
   }
  return 0;
}
//----------------------------
// get the loop index variable from a for loop
// 2 cases: for (i=X,...) or for (int i=X,...)
// only consider canonical form because that is the only legal form for OpenMP loop
// TODO: replace with SageInterface::isCanonicalForLoop(SgNode* loop,SgInitializedName** ivar/*=NULL*/, SgExpression** lb/*=NULL*/, SgExpression** ub/*=NULL*/, SgExpression** step/*=NULL*/, SgStatement** body/*=NULL*/)
SgInitializedName * ASTtools::getLoopIndexVar(SgForStatement* forstmt)
{
   SgVarRefExp *loopvar = NULL;
   ROSE_ASSERT(forstmt != NULL);

   Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree(\
                *((forstmt->get_init_stmt()).begin()), V_SgAssignOp);

  if (testList.size()>0){// for (i=x,..)
      ROSE_ASSERT(testList.size()==1);
      loopvar =isSgVarRefExp(isSgAssignOp((*testList.begin()))->get_lhs_operand());
    //  cout<<"debug:"<<"case for (i=x)"<<endl;
      ROSE_ASSERT(loopvar !=NULL);
      return loopvar->get_symbol()->get_declaration();

    } else { // for (int i=x,...)
      Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree(\
                *((forstmt->get_init_stmt()).begin()),  V_SgVariableDeclaration);
      if (testList.size()>0){
        ROSE_ASSERT(testList.size()==1);// only handle the case of 1 declaration statement
        SgInitializedNamePtrList &namelist = isSgVariableDeclaration((*testList.begin()))\
                        ->get_variables();
        ROSE_ASSERT(namelist.size()==1); // declaration for only 1 variable
        return isSgInitializedName(*namelist.begin());
      //cout<<"debug:"<<"case for (int i=x)"<<endl;
      }
    }

  ROSE_ASSERT(false);
  return NULL;
}

//----------------------------
// if a variable reference expression is referring to a loop index variable
// compare both name and scope to make sure
bool ASTtools::isLoopIndexVarRef(SgForStatement* forstmt, SgVarRefExp *varref)
{
  ROSE_ASSERT(forstmt != NULL);
  ROSE_ASSERT(varref != NULL);
  SgInitializedName *loopvar = getLoopIndexVar(forstmt);
  SgInitializedName * refvar = varref->get_symbol()->get_declaration();

  return ( (loopvar->get_name() == refvar->get_name())
           &&(loopvar->get_scope() == refvar->get_scope()) );

}
//----------------------------
// modify the loop control variables to new ones from the loop scheduler
// value type: 0- lower, 1-upper, 2-stride
int
ASTtools::setForLoopTripleValues(int valuetype,SgForStatement* forstmt, SgExpression* exp)
   {
     ROSE_ASSERT((valuetype==0)||(valuetype==1)||(valuetype==2));
     ROSE_ASSERT(forstmt!=NULL);
     ROSE_ASSERT(exp!=NULL);
//     SgType *inttype =  SgTypeInt::createType();
     if (valuetype==0)//set lower bound expression
        {
       // two cases: init_stmt is 
       //       SgVariableDeclaration or 
       //       SgExprStatement (assignment) like i=0;
          Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree( *((forstmt->get_init_stmt()).begin()), V_SgAssignOp);

          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size()==1);// only handle the case of 1 statement, canonical form
               SgAssignOp * assignop = isSgAssignOp((*testList.begin()));
               ROSE_ASSERT(assignop);
               if( assignop->get_rhs_operand()->get_lvalue())
                  exp->set_lvalue(true);
               assignop->set_rhs_operand(exp);

             }
            else
             {
            // SgVariableDeclaration, change it to int _p_loop_index= _p_loop_lower 
               Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree( *((forstmt->get_init_stmt()).begin()),  V_SgAssignInitializer );
               ROSE_ASSERT(testList.size()==1);// only handle the case of 1 statement, canonical form
               isSgAssignInitializer((*testList.begin()))->set_operand(exp);
            // set lhs name to _p_loop_index
            // TODO should not work in current AST with symbol table, Liao, 11/16/2007 Bug 120
            // should set the whole SgInitializedName 
               SgVariableDeclaration * vardecl = isSgVariableDeclaration( (*(forstmt->get_init_stmt()).begin()) );
               ROSE_ASSERT(vardecl!=NULL);
               SgInitializedName *initname = isSgInitializedName(*((vardecl->get_variables()).begin()));
               ROSE_ASSERT(initname!=NULL);
               initname->set_name(SgName("_p_loop_index"));
             }
        } // end if value 0

     if (valuetype ==1)
        {
      // set upper bound expression
         SgBinaryOp * binop= isSgBinaryOp(isSgExprStatement(forstmt->get_test())->get_expression());
         ROSE_ASSERT(binop != NULL);
         binop->set_rhs_operand(exp);
      // normalize <= to < TODO add it into the loop normalization phase
         if (isSgLessOrEqualOp(binop)!=NULL)
            {
              //SgLessThanOp * lessthanop = buildLessThanOp(binop->get_lhs_operand(), binop->get_rhs_operand());
               SgExpression* lhs = isSgExpression(SageInterface::deepCopy(binop->get_lhs_operand()));
                SgExpression* rhs = isSgExpression(SageInterface::deepCopy(binop->get_rhs_operand()));
                SgLessThanOp * lessthanop = buildLessThanOp(lhs,rhs);
               // TODO deleteSubtree(binop)

           // DQ (11/7/2006): modified to reflect SgExpressionRoot no longer being used.
              isSgExprStatement(forstmt->get_test())->set_expression(lessthanop);
            }

       // normalize >= to > TODO add it into the loop normalization phase
          if (isSgGreaterOrEqualOp(binop)!=NULL)
             {
#if 0
           // should not reuse operand expression!!
               SgGreaterThanOp * greaterthanop = buildGreaterThanOp(binop->get_lhs_operand(),binop->get_rhs_operand());
               //TODO release previous >= node and its right hand operand 
#else
                SgExpression* lhs = isSgExpression(SageInterface::deepCopy(binop->get_lhs_operand()));
                SgExpression* rhs = isSgExpression(SageInterface::deepCopy(binop->get_rhs_operand()));
                SgGreaterThanOp * greaterthanop = buildGreaterThanOp(lhs,rhs);
               
               // delete (binop); // should delete whole tree, otherwise dangling children
#endif 
               isSgExprStatement(forstmt->get_test())->set_expression(greaterthanop);
             }

        }

     if (valuetype == 2)
        {
       // set stride expression
       // case 1: i++ change to i+=stride
          Rose_STL_Container<SgNode*> testList = NodeQuery::querySubTree( forstmt->get_increment(), V_SgPlusPlusOp);
          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size() == 1); // should have only one
               SgVarRefExp *loopvarexp = isSgVarRefExp(SageInterface::deepCopy
                    (isSgPlusPlusOp( *testList.begin())->get_operand()));
               SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, exp);
               forstmt->set_increment(plusassignop);
             }

       // DQ (1/3/2007): I think this is a meaningless statement.
          testList.empty();

       // case 1.5: i-- also changed to i+=stride
          testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgMinusMinusOp);
          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size()==1);// should have only one
               SgVarRefExp *loopvarexp =isSgVarRefExp(SageInterface::deepCopy
                              (isSgMinusMinusOp(*testList.begin())->get_operand()));

            // DQ (9/6/2006): skip explicit specification of type (which is computed dynamically).
               SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, exp);
               forstmt->set_increment(plusassignop);
              }

       // DQ (1/3/2007): I think this is a meaningless statement.
          testList.empty();

       // case 2: i+=X
          testList = NodeQuery::querySubTree( forstmt->get_increment(), V_SgPlusAssignOp);
          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size()==1);// should have only one
               SgPlusAssignOp * assignop = isSgPlusAssignOp(*(testList.begin()));
               ROSE_ASSERT(assignop!=NULL);
               assignop->set_rhs_operand(exp);
             }

       // DQ (1/3/2007): I think this is a meaningless statement.
          testList.empty();

       // case 2.5: i-=X changed to i+=stride
          testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgMinusAssignOp);
          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size()==1);// should have only one
               SgVarRefExp *loopvarexp =isSgVarRefExp(SageInterface::deepCopy
                      (isSgMinusAssignOp(*testList.begin())->get_lhs_operand()));
                //Liao (10/12/2007), SgExpressionRoot is deprecated!
              // SgExpressionRoot *exproot = isSgExpressionRoot((*testList.begin())->get_parent());
               SgExprStatement* exprstmt = isSgExprStatement((*testList.begin())->get_parent());        
               ROSE_ASSERT(exprstmt !=NULL);
            // DQ (9/6/2006): skip explicit specification of type (which is computed dynamically).
               SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, exp);
               exprstmt->set_expression(plusassignop);
             }

       // DQ (1/3/2007): I think this is a meaningless statement.
          testList.empty();

       // case 3: i=i + X or i =X +i  i
       // TODO; what if users use i*=,etc ??
       //      send out a warning: OpenMP error: not cannonical FOR/DO loop
       //      or do this in the real frontend. MUST conform to canonical form
          testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgAddOp);
          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size()==1);// should have only one ??
            // consider only the top first one
               SgAddOp * addop = isSgAddOp(*(testList.begin()));
               ROSE_ASSERT(addop!=NULL);
               string loopvar= (isSgVarRefExp(isSgAssignOp(addop->get_parent())->get_lhs_operand())->get_symbol()->get_name()).getString();
               if (isSgVarRefExp(addop->get_rhs_operand())!=NULL)
                  {
                    if ((isSgVarRefExp(addop->get_rhs_operand())->get_symbol()->get_name()).getString() ==loopvar)
                         addop->set_lhs_operand(exp);
                      else
                         addop->set_rhs_operand(exp);
                  }
                 else
                    addop->set_rhs_operand(exp);
             }

       // case 3.5: i=i - X 
          testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgSubtractOp);
          if (testList.size()>0)
             {
               ROSE_ASSERT(testList.size()==1);// should have only one ??
            // consider only the top first one
               SgSubtractOp * subtractop = isSgSubtractOp(*(testList.begin()));
               ROSE_ASSERT(subtractop!=NULL);
               SgVarRefExp *loopvarexp =isSgVarRefExp(SageInterface::deepCopy
                     (isSgSubtractOp(*testList.begin())->get_lhs_operand()));
               SgAssignOp *assignop = isSgAssignOp((*testList.begin())->get_parent());
               ROSE_ASSERT(assignop !=NULL);
            // DQ (9/6/2006): skip explicit specification of type (which is computed dynamically).
               SgPlusAssignOp *plusassignop = buildPlusAssignOp(loopvarexp, exp);
               assignop->set_rhs_operand(plusassignop);
             }
        } // end if value 2

     return 0;
   }
//----------------------------
// get lower, upper and stride expressions for a given 'for' loop statement
// value type: 0- lower, 1-upper, 2-stride
SgExpression* ASTtools::getForLoopTripleValues(int valuetype,SgForStatement* forstmt )
{
  ROSE_ASSERT((valuetype==0)||(valuetype==1)||(valuetype==2));
  ROSE_ASSERT(forstmt!=NULL);
  if (valuetype==0)// get lower bound expression
  { 
    // two cases: init_stmt is SgVariableDeclaration or SgExprStatement (assignment)
    Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree(\
                *((forstmt->get_init_stmt()).begin()), V_SgAssignOp);

    if (testList.size()>0){ 
      ROSE_ASSERT(testList.size()==1);// only handle the case of 1 statement
      return isSgAssignOp((*testList.begin()))->get_rhs_operand();
    } else { // SgVariableDeclaration
      Rose_STL_Container<SgNode* > testList = NodeQuery::querySubTree(\
                *((forstmt->get_init_stmt()).begin()),  V_SgAssignInitializer);
      if (testList.size()>0){
        ROSE_ASSERT(testList.size()==1);// only handle the case of 1 statement 
        return isSgAssignInitializer((*testList.begin()))->get_operand();
      }
    }
  }//end if value 0
  
  if(valuetype ==1){ // get upper bound expression
 // SgBinaryOp * binop= isSgBinaryOp(isSgExpressionRoot(isSgExprStatement(forstmt->get_test())->get_expression_root())->get_operand());
    SgBinaryOp * binop= isSgBinaryOp(isSgExprStatement(forstmt->get_test())->get_expression());
    ROSE_ASSERT(binop != NULL);
    return binop->get_rhs_operand();
  }
 
  if(valuetype == 2){ //get stride expression
        // case 1: i++  
     Rose_STL_Container<SgNode*> testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgPlusPlusOp);
     if (testList.size()>0) {
       ROSE_ASSERT(testList.size()==1);// should have only one
       return buildIntVal(1); 
     }  
     testList.empty();

        // case 1.5: i--
     testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgMinusMinusOp);
     if (testList.size()>0) {
       ROSE_ASSERT(testList.size()==1);// should have only one
       return buildIntVal( -1);
     }
     testList.empty();

        // case 2: i+=X
     testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgPlusAssignOp);
     if (testList.size()>0) {
       ROSE_ASSERT(testList.size()==1);// should have only one
       SgPlusAssignOp * assignop = isSgPlusAssignOp(*(testList.begin()));
       ROSE_ASSERT(assignop!=NULL);
       return assignop->get_rhs_operand(); 
     }
     testList.empty();
        // case 2.5: i-=X
     testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgMinusAssignOp);
     if (testList.size()>0) {
       ROSE_ASSERT(testList.size()==1);// should have only one
       SgMinusAssignOp * assignop = isSgMinusAssignOp(*(testList.begin()));
       ROSE_ASSERT(assignop!=NULL);
       SgMinusOp* minusop = buildMinusOp(assignop->get_rhs_operand());
       return minusop; // get -x from x  ??
     }
     testList.empty();

        // case 3: i=i + X or i =X +i  i
        // TODO; what if users use i*=, i-=, etc ?? 
        //      send out a warning: OpenMP error: not cannonical FOR/DO loop
        //      or do this in the real frontend. MUST conform to canonical form
     testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgAddOp);
     if (testList.size()>0) {
       ROSE_ASSERT(testList.size()==1);// should have only one ??
                //TODO or consider only the first one
       SgAddOp * addop = isSgAddOp(*(testList.begin()));
       ROSE_ASSERT(addop!=NULL);
       string loopvar= (isSgVarRefExp(isSgAssignOp(addop->get_parent())->\
                        get_lhs_operand())->get_symbol()->get_name()).getString();
       if(isSgVarRefExp(addop->get_rhs_operand())!=NULL){
           if ((isSgVarRefExp(addop->get_rhs_operand())->\
                get_symbol()->get_name()).getString() ==loopvar)
              return addop->get_lhs_operand(); //get non-loopvar
           else return addop->get_rhs_operand(); 
        }
        else return addop->get_rhs_operand(); // constant variable

     }
      testList.empty();

        // case 3.5: i=i - X,  
     testList = NodeQuery::querySubTree(forstmt->get_increment(), V_SgSubtractOp);
     if (testList.size()>0) {
       ROSE_ASSERT(testList.size()==1);// should have only one ??
                //TODO or consider only the first one
       SgSubtractOp * assignop = isSgSubtractOp(*(testList.begin()));
       ROSE_ASSERT(assignop!=NULL);

       //what to do with
       SgMinusOp* minusop = buildMinusOp(isSgExpression(SageInterface::deepCopy(assignop->get_rhs_operand())));
       //SgMinusOp* minusop = buildMinusOp(assignop->get_rhs_operand());
       return minusop;
     }
      
  } //end if value 2
  return NULL;

}

//-------------
//check if node1 is an ancestor of node 2
bool ASTtools::isAncestor(SgNode* node1, SgNode* node2)
{
  ROSE_ASSERT(node1&&node2);
  SgNode* curnode= node2;
  if (node1==node2)
    return false;
  do {
      curnode= curnode->get_parent();
  } while( (curnode!=NULL)&&(curnode!=node1));

  if (curnode==node1)
   return true;
  else 
    return false;
}

// what if the current node is already the outmost scope??
SgNode* ASTtools::get_scope(SgNode* astNode)
  {
    SgNode * astnode=astNode;
    ROSE_ASSERT(astNode!=NULL);
//    if(isSgScopeStatement(astnode)!=NULL) return astnode;
    do
      {
        astnode=astnode->get_parent();
      }
    while((astnode!=NULL)&&( isSgScopeStatement(astnode)==NULL));
    return astnode;
  }


//-----------------------
int ASTtools::get_scope_level(SgNode* astNode)
  {
    int level=0;
    SgNode * astnode=astNode;
    ROSE_ASSERT(astNode!=NULL);
    do
      {
        astnode=astnode->get_parent();
        if (isSgScopeStatement(astnode)!=NULL)
          ++level;
      }
    while((astnode!=NULL)&&( isSgScopeStatement(astnode)!=NULL));
    return level;
  }

//------------------------------------
// Add include "ompcLib.h" into source files, right before the first statement from users
// Lazy approach: assume all files will contain OpenMP runtime library calls
// TODO: (low priority) a better way is to only insert Headers when OpenMP is used.
// 2/1/2008, try to use MiddleLevelRewrite to parse the content of the header, which
//  should generate function symbols used for runtime function calls 
//  But it is not stable!
int OmpMidend::insertHeaders(SgProject* project)
{
  Rose_STL_Container<SgNode*> globalScopeList = NodeQuery::querySubTree (project,V_SgGlobal);
  for (Rose_STL_Container<SgNode*>::iterator i = globalScopeList.begin(); i != globalScopeList.end(); i++)
  {
    SgGlobal* globalscope = isSgGlobal(*i);
    ROSE_ASSERT (globalscope != NULL);
    SageInterface::insertHeader("ompcLib.h",PreprocessingInfo::after,false,globalscope);
   }
  return 0; //assume always successful currently
}
                                                                                         
//----------------------------
// C++ outlined member function's wrapper generation
// Reason: Pthreads API expects a c function pointer, 
//         not non-static member function pointer
//
// 2 parameters: the class and the member function declaration
//
// e.g. 
//  void * _globalobject; //we insert this in addGlobalOmpDeclaration()
// We need to generate and insert both the forward declaration and defining declaration
//
//  void __ompc_func_4_wrapper(void**);
//  void __ompc_func_4_wrapper(void **__ompc_args)
// {
//   Hello * hello;  
//   hello= reinterpret_cast<Hello*>(_globalobject);
//   hello->__ompc_func_4(__ompc_args);
// }
int OmpMidend::generateWrapperFunction(SgClassDefinition* classdef, \
                                        SgMemberFunctionDeclaration* srcfunc)
{
  ROSE_ASSERT(classdef!=NULL);
  ROSE_ASSERT(srcfunc!=NULL);

  SgGlobal * myscope = SageInterface::getGlobalScope(classdef);
  ROSE_ASSERT(myscope!=NULL);

//1.generate and insert forward declaration
  //SgType* func_return_type = new SgTypeVoid() ;
  SgType* func_return_type = srcfunc->get_type()->get_return_type() ;
  SgFunctionDeclaration * func = new SgFunctionDeclaration(TRANS_FILE,\
        SgName((srcfunc->get_name()).getString()+ "_wrapper"),\
        new SgFunctionType (func_return_type, false), NULL);
        // must set static here to avoid name collision
  ((func->get_declarationModifier()).get_storageModifier()).setStatic();
     // parameter list  void **__ompc_args
  SgFunctionParameterList * parameterList = buildFunctionParameterList();
  ROSE_ASSERT(parameterList != NULL);
  func->set_parameterList(parameterList);
  parameterList->set_parent(func);
  SgName var1_name = "__ompc_args";
  SgPointerType* pType1= new SgPointerType(func_return_type); //share void type with function type
  SgPointerType* pType2 = new SgPointerType(pType1);
                                                                                         
  SgInitializer * var1_initializer = NULL;
  SgInitializedName * var1_init_name = new SgInitializedName(var1_name, pType2,var1_initializer,NULL);
  var1_init_name->set_file_info(TRANS_FILE);
//bug 346
  var1_init_name->set_scope(isSgScopeStatement(func->get_parameterList()));

  ROSE_ASSERT(func->get_parameterList()!=NULL);
  func->get_parameterList()->append_arg(var1_init_name);
  func->set_scope(myscope);
  func->setForward();// the only trick for non-defining function definition
  myscope->prepend_declaration(func);
                                                                                         
//2. the defining function declaration
  SgFunctionDeclaration * func2 = new SgFunctionDeclaration(TRANS_FILE,\
        SgName((srcfunc->get_name()).getString()+ "_wrapper"),\
        new SgFunctionType (func_return_type, false), NULL);
        // must set static here to avoid name collision 
  ((func2->get_declarationModifier()).get_storageModifier()).setStatic();
  SgFunctionParameterList * parameterList2 = buildFunctionParameterList();
  func2->set_parameterList(parameterList2);
  parameterList2->set_parent(func2);
  SgName var2_name = "__ompc_args";
  SgInitializedName * var2_init_name = new SgInitializedName(var2_name, pType2,var1_initializer,NULL);
  var2_init_name->set_file_info(TRANS_FILE);
//bug 346
  var2_init_name->set_scope(isSgScopeStatement(func2->get_parameterList()));
  ROSE_ASSERT(func2->get_parameterList()!=NULL);
  func2->get_parameterList()->append_arg(var2_init_name);
  func2->set_scope(myscope);
  // add definition
  SgFunctionDefinition * func_def =new SgFunctionDefinition(TRANS_FILE,func2);
  SgBasicBlock *func_body = buildBasicBlock();

  func_def->set_body(func_body);

 // DQ (1/4/2007): This should be required!  Only the constructors will set the parents of input IR nodes,
 // the semantics of the access functions to set internal pointers forbids side-effects.  So since
 // the body is set using the set_body() member function the parent of the body must be explicitly 
 // set.
  func_body->set_parent(func_def);

  func_def->set_parent(func2); //necessary or not?
  // add function body
        // Hello * currentobject;
  //SgPointerType *pointertype = new SgPointerType(classdef->get_declaration()->get_type());
  // Must use the nondefiningdeclaration here, otherwise unparser will complain
  SgPointerType *pointertype = new SgPointerType(isSgClassDeclaration(classdef->get_declaration()->get_firstNondefiningDeclaration())->get_type());
  SgVariableDeclaration* vardecl;
  vardecl = new SgVariableDeclaration(TRANS_FILE, SgName("currentobject"),pointertype, NULL);
  SgInitializedName * initname = new SgInitializedName(SgName("currentobject"),pointertype,NULL,vardecl,NULL);
  vardecl->set_firstNondefiningDeclaration(vardecl);
  initname->set_parent(vardecl);
  initname->set_file_info(TRANS_FILE);
  appendStatement(vardecl,func_body);
        //---------------
        // currentobject = reinterpret_cast<Hello *> (_pp_globalobject);
  //SgPointerType * vartype= new SgPointerType( SgTypeVoid::createType());
  SgVarRefExp *lhs = buildVarRefExp(initname,func_body );

    // Liao, 10/16/2007, now we have symbol table!
 // SgInitializedName * rhsinitname = new SgInitializedName (SgName("_pp_globalobject"),vartype,NULL,NULL,NULL);
  ///rhsinitname->set_file_info(TRANS_FILE);
//bug 346
  //rhsinitname->set_scope(isSgScopeStatement(ASTtools::get_globalscope(classdef)));

  SgVarRefExp *rhsvar = buildVarRefExp(SgName("_pp_globalobject"),func_body);
  SgCastExp * rhs = buildCastExp(rhsvar,\
                pointertype, SgCastExp::e_reinterpret_cast);

    SgExprStatement * expstmti= buildAssignStatement(lhs,rhs); 

  appendStatement(expstmti,func_body);
        //---------------
        // currentobject->__ompc_func_4(__ompc_args);
  SgFunctionType *myFuncType= new SgFunctionType(func_return_type,false);
  SgMemberFunctionDeclaration *myfuncdecl = new SgMemberFunctionDeclaration(TRANS_FILE,(srcfunc->get_name()).getString(),myFuncType,srcfunc->get_definition());
  ROSE_ASSERT(myfuncdecl != NULL);
  ROSE_ASSERT(isSgMemberFunctionDeclaration(srcfunc) != NULL);

  //myfuncdecl->set_scope(srcfunc->get_class_scope());
  myfuncdecl->set_scope(classdef);
  myfuncdecl->set_parent(classdef);

  SgMemberFunctionSymbol * func_symbol= new SgMemberFunctionSymbol(srcfunc);
  //SgMemberFunctionSymbol * func_symbol= new SgMemberFunctionSymbol(myfuncdecl);
  ROSE_ASSERT(func_symbol != NULL);
  SgMemberFunctionRefExp * func_ref_exp = new SgMemberFunctionRefExp(TRANS_FILE,func_symbol,0,myFuncType, false);  
  SgExprListExp * parameters = buildExprListExp();
  appendExpression(parameters,buildVarRefExp(var2_init_name, func_body ));

  SgVarRefExp *lhsop = buildVarRefExp(initname,func_body );
  SgArrowExp * arrowexp = new SgArrowExp(TRANS_FILE,lhsop, func_ref_exp,myFuncType);//?
  SgFunctionCallExp * func_call_expr = new SgFunctionCallExp(TRANS_FILE,arrowexp,parameters,myFuncType);       
  SgExprStatement * expstmt2= new SgExprStatement(TRANS_FILE,func_call_expr);
  appendStatement(expstmt2,func_body);

  myscope->append_declaration(func2);
  return 0;
}
                                                                                           

//----------------------------
// return the number of 'omp section' in 'omp sections'
// Also make the 1st implicit pragma explict. 
//TODO move this to OmpFrontend as another OmpAttributeOld's field
inline int OmpMidend::getSectionCount(SgPragmaDeclaration * decl)
{
  int counter=0;

  ROSE_ASSERT(OmpFrontend::recognizePragma(decl)==e_sections);
  SgBasicBlock *bb = isSgBasicBlock(getNextStatement(decl));
  ROSE_ASSERT(bb != NULL); 

  SgStatementPtrList & ptrlist = bb -> get_statements();

  // The very first 'omp section' could be implicit
  SgStatement* firststmt = *ptrlist.begin();
  if (isSgPragmaDeclaration(firststmt)) 
  {
    if (OmpFrontend::recognizePragma(isSgPragmaDeclaration(firststmt)) != e_section)
    { // More semantic check for other pragma types here
      string strompfor("omp section");
      SgPragmaDeclaration *pragmadecl = buildPragmaDeclaration(strompfor);
      prependStatement(pragmadecl,bb);
    }
  }
  else 
  {
    string strompfor("omp section");
    SgPragmaDeclaration *pragmadecl =  buildPragmaDeclaration(strompfor);
    prependStatement(pragmadecl,bb);
  }

  // count all explicit 'omp section' now
  for (Rose_STL_Container<SgStatement*>::iterator i=ptrlist.begin();i!=ptrlist.end();i++)
  {
    if (isSgPragmaDeclaration(*i)) 
      if (OmpFrontend::recognizePragma(isSgPragmaDeclaration(*i)) == e_section)
        counter++;
  } // end for

//cout<<"1377. "<<bb->unparseToString()<<endl;
  return counter;
} 
//----------------------------
// variable substitute for shared, private, and reduction variables
// only care the used variables from the same scope and outer scope
// interestingly, outer scope is 'smaller'
//      search all variable reference site XX,
//      if in private list, use _p_XX,
//      if in reduction list, use _p_XX no matter what scope it belongs to
//      if in shared list and from the scope, use *__pp_XX
// the original variables? keep intact since they might be used in sequential part
//  Ignore variables generated from previous viariable substituting, _p_XX, _pp_XX
// bBlock2 is a BB containing a loop.
int OmpMidend::variableSubstituting(SgPragmaDeclaration * decl, \
        OmpAttributeOld* ompattribute, SgNode* bBlock2)
{
  ROSE_ASSERT(bBlock2 != NULL);
  ROSE_ASSERT(ompattribute != NULL);

//cout<<"------1559, "<<"OmpMidend::variableSubstituting"<<endl; 
//  SgScopeStatement* currentscope=isSgScopeStatement(ASTtools::get_scope(decl));
  SgScopeStatement* currentscope = isSgFunctionDeclaration(\
                getEnclosingFunctionDeclaration(decl))\
                ->get_definition()->get_body();
  ROSE_ASSERT(currentscope != NULL);

  if ((ompattribute->var_list).size()>0)
   {
     Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(bBlock2, V_SgVarRefExp);
     for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
     {
        SgInitializedName* initname= isSgVarRefExp(*i)->get_symbol()->get_declaration();
       // SgScopeStatement* varscope=initname->get_declaration()->get_scope();
        SgScopeStatement* varscope=isSgScopeStatement(ASTtools::get_scope(initname));

        SgName myname=initname->get_name();
        string namestr=myname.getString();
//        SgType *mytype=initname->get_type();
       // skip handled variables during previous substitutions
       //_p_ for private variables, -pp_ for shared variables
       //_ppthdprv_ for threadprivate variables
       if ((namestr.find("_p_",0)==0)|| (namestr.find("_ppthdpriv_",0)==0)|| (namestr.find("_pp_",0)==0)) continue;

        //1. replace variables with _p_XX , only do this once for 'omp parallel'
//Note:the scope condition will change if we also do this on inner pragma like 'omp for'
// not for' omp for' since 'omp for' may not know the scope specified at 'omp parallel'
        // for variables not recognizable, treat them as shared .
        //e.g schedule(static,newvar1), TODO more elegant way when FE is ready
        // e.g. SgPointerDerefExp   j --> (*_pp_j)
       if (((ompattribute->isInClause(initname,e_shared))|| (ompattribute->get_clause(initname)==e_not_omp)) && (  ( (currentscope == varscope)|| isSgFunctionParameterList(initname->get_parent()))) &&(ompattribute->omp_type==e_parallel))
        {// SgPointerDerefExp   j --> *_pp_j
          
          SgName ppname("_pp_"+myname.getString());
#if 0
           SgInitializedName *initname2= new SgInitializedName(SgName("_pp_"+myname.getString()),new SgPointerType(mytype),0,0,0); // lazy method
            initname2->set_file_info(TRANS_FILE);
// bug 346
            initname2->set_scope(varscope);
#endif
            SgPointerDerefExp *derefexp= buildPointerDerefExp(buildVarRefExp(ppname,varscope));
            derefexp->set_need_paren(true);

           replaceExpression(isSgExpression(*i),isSgExpression(derefexp));
        }
        //2. replace variables with _p_XX for private, firstprivate, lastprivate and reduction
         //all private variables need to compare the scope to currentscope
         // except for reduction variables
         // for orphaned 'omp for' pragma, the scope larger than its parent 'omp parallel'
         // i--> _p_i
       if (( ((ompattribute->isInClause(initname,e_private))||
              (ompattribute->isInClause(initname,e_firstprivate))||
              (ompattribute->isInClause(initname,e_lastprivate)))
//              &&(currentscope >= varscope))                         ||
                &&((currentscope==varscope)|| ASTtools::isAncestor(varscope,currentscope)))                         ||
           (ompattribute->isInClause(initname,e_reduction_plus)))
       { 
        //cout<<"debug:private:"<<(initname->get_name()).getString()<<endl;
        SgName name2("_p_"+myname.getString());
        replaceExpression (isSgExpression(*i), buildVarRefExp(name2,varscope));
        // isSgVarRefExp(*i) =  buildgVarRefExp(name2,varscope);
       }
        // 3. Replace threadprivate variables with (type)(*_ppthd_X)
        if(ompattribute->isInClause(initname,e_threadprivate))
        {
          // inner most scope containing the variable reference
          SgScopeStatement* cscope=isSgScopeStatement(ASTtools::get_scope(isSgVarRefExp(*i)));
          ROSE_ASSERT(cscope);
     
          SgName name3("_ppthd_"+myname.getString());
          SgVariableSymbol *symbol3=lookupVariableSymbolInParentScopes(name3,cscope);
          if(symbol3){  // this function is called at several levels in a bottom up order, the symbol
                        // may not yet generated in the innermost level, 
                        // so, we can defer it until a higher level is processed when the symbol has
                        // generated already.
          SgPointerDerefExp *derefexp= buildPointerDerefExp(
                           buildVarRefExp(symbol3->get_declaration(),getScope(bBlock2)));
          replaceExpression(isSgExpression(*i),isSgExpression(derefexp));
        }
        }
     } //end for
   }// end if
  return 0;
}

//----------------------------
//part of variable handling
//append statements like
//      if (_ompc_is_last ())
//        {
//          (*_pp_k_3) = (_p_k_3);// if var scope = currentscope
//            k_4  = p_k_4;       // if var scope < currentscope
//          ......  
//        }

int OmpMidend::addLastprivateStmts(SgPragmaDeclaration *decl, OmpAttributeOld *ompattribute,\
         SgBasicBlock *bBlock1)
{

#if 1
  ROSE_ASSERT(decl != NULL);
  ROSE_ASSERT(ompattribute != NULL);
  ROSE_ASSERT(bBlock1 != NULL);
  SgScopeStatement* currentscope;
  currentscope = isSgFunctionDeclaration(getEnclosingFunctionDeclaration(decl))\
                ->get_definition()->get_body();

  // 1. conditional expression
  SgExprListExp * exp_list_exp1 = buildExprListExp();
                                                                                                
  SgExprStatement * conditionStmt1= buildFunctionCallStmt( 
          SgName("_ompc_is_last"), SgTypeInt::createType(),
         exp_list_exp1, getScope(decl));

  // 2. true body: 
  SgBasicBlock * trueBody1= NULL;
  SgBasicBlock * falseBody1=NULL;

  trueBody1 = buildBasicBlock();
  falseBody1= buildBasicBlock();  // falseBody must not be NULL!

  for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();\
        i!=ompattribute->var_list.end();i++ )
  {
      SgInitializedName *initname = isSgInitializedName((*i)->origVar);
      SgType * mytype= initname->get_type();
      SgName myname = initname->get_name();
      SgScopeStatement* varscope=isSgScopeStatement(\
                ASTtools::get_scope(isSgNode((*i)->origVar)));
      SgVariableDeclaration* privatevar;
//      privatevar = new SgVariableDeclaration(TRANS_FILE,SgName( "_p_"+myname.getString()),
//                mytype,NULL);
     SgName privateName = SgName( "_p_"+myname.getString());
     privatevar = buildVariableDeclaration(privateName, mytype, NULL, varscope);
     
    SgInitializedName * name1 = privatevar->get_decl_item (privateName); 
    ROSE_ASSERT(name1);
  //TODO handle e_firstlastprivate
    if ((*i)->ompType==e_lastprivate)
    {
    // store value back to global copy for firstprivate variables
        //(*_pp_k)=_p_k if k's scope == currentscope or
        //k = _p_k   if k's scope < currentscope
      //  cout<<"entering firstprivate local copy initialization.."<<endl;
       SgName initname2_name("_pp_"+myname.getString());
        SgPointerDerefExp *derefexp= buildPointerDerefExp(
               buildVarRefExp(initname2_name,bBlock1));
                    derefexp->set_need_paren(true);
//      SgAssignOp * assignop = NULL;
      SgExpression * lhs = NULL;
      SgExpression * rhs = NULL;
  
//      if (varscope<currentscope) // I thought higher scope was smaller
      if (ASTtools::isAncestor(varscope,currentscope))
         {
            lhs = buildVarRefExp(initname,bBlock1);
            rhs = buildVarRefExp(name1,bBlock1);
         }
        else
         {
           if(varscope == currentscope)
              {
                lhs = derefexp;
                rhs = buildVarRefExp(name1,bBlock1);
              }
         }
      SgExprStatement *expstmt = buildAssignStatement(lhs,rhs); 
      appendStatement(expstmt,trueBody1);
    } //end if 
  }// end for std::list 

  //3. if statement
  SgIfStmt * ifStmt1=buildIfStmt(isSgStatement(conditionStmt1),\
                                   trueBody1,falseBody1);
  appendStatement(ifStmt1,bBlock1);

#endif
  return 0;
}

//----------------------------
//part of variable handling
//add ompc_reduction() call for reduction variables
// 2 types of arguments: for reduction variable from higher scope or from same scope
// _ompc_reduction(&localcopy,&gi,datatype,operator); // higher scope
//  _ompc_reduction(&localcopy,_pp_gj,datatype,operator); // same scope
//
// One special case is that the orphaned 'omp for' is located in a separated function 
// from its parent 'omp parallel', still use 
// _ompc_reduction(&localcopy,&gi,datatype,operator); //

int OmpMidend::addReductionCalls(SgPragmaDeclaration *decl, OmpAttributeOld *ompattribute,\
                 SgBasicBlock *bBlock1)
{
  ROSE_ASSERT(decl != NULL);
  ROSE_ASSERT(ompattribute != NULL);
  ROSE_ASSERT(bBlock1 != NULL);
  SgScopeStatement* currentscope;

// should compare scope with the parent function, not parent pragma!!
// Since the outlined function is the same scope of the enclosing function!!
 // currentscope is the function body containing the pragma.
 // varscope is the scope of the original variable in the reduction list
  currentscope = isSgFunctionDeclaration(getEnclosingFunctionDeclaration(decl))\
                ->get_definition()->get_body();

  for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();\
        i!=ompattribute->var_list.end();i++ )
  {
    SgInitializedName *initname = isSgInitializedName((*i)->origVar);
    SgType * mytype= initname->get_type();
    SgName myname = initname->get_name();
    SgScopeStatement* varscope=getScope(isSgNode((*i)->origVar));   

    // TODO handle all reduction operators
    if ((*i)->ompType==e_reduction_plus)
    {
          // SgFunctionRefExp: _ompc_reduction()
      SgType* voidtype = SgTypeVoid::createType();

         //SgExprListExp, 4 parameters (&_p_var1, &var1/__pp_var1, data type, op type )
                //arg 1, &_p_gi
      SgExprListExp * exp_list_exp = buildExprListExp();
//bug 346
      SgName varName1("_p_"+myname.getString());
    // bottom up translation, innner most code are generated first, symbols may not be ready
    // what to do?
      SgVarRefExp   * varlocal = buildVarRefExp(varName1,bBlock1);
      SgAddressOfOp * addressop1 = buildAddressOfOp(varlocal);
      appendExpression(exp_list_exp, addressop1);
                //arg 2: _pp_gj or &gi
      if ((ompattribute->isOrphaned==false)&&(currentscope==varscope)) 
     //reduction variables from the same scope: need to refer to the unwrapped copy __pp_gj
        {
         SgName varName2("_pp_"+myname.getString());
         SgVarRefExp *arg2= buildVarRefExp(varName2,bBlock1);
          appendExpression(exp_list_exp,arg2);
        }
 //&gi for reduction variables from higher scope or for all orphaned case
      else if (((ompattribute->isOrphaned==false)&&(ASTtools::isAncestor(varscope,currentscope)))||\
                (ompattribute->isOrphaned==true))
        {
         SgName varName2(myname.getString());
          SgVarRefExp   * varlocal = buildVarRefExp(varName2,bBlock1);
          SgAddressOfOp * addressop2 = buildAddressOfOp(varlocal);
          appendExpression(exp_list_exp, addressop2);
        }
                //arg 3
      appendExpression(exp_list_exp, buildIntVal(OmpMidend::convertTypeId(mytype)));
                //arg 4
      appendExpression(exp_list_exp, buildIntVal(OMPC_REDUCTION_PLUS));
          //build call exp
      SgExprStatement * callstmt= buildFunctionCallStmt(SgName("_ompc_reduction"),
        voidtype, exp_list_exp,getScope(decl)); //bBlock is not attached into AST yet
       appendStatement(callstmt,bBlock1);
    }// end if
  } // end for


  return 0;
}

//----------------------------
// add statements into outlined function definition if the parallel region access
// threadprivate variables (m,mm)
//      auto double *_ppthd_mm;
//      auto int *_ppthd_m;
//      (_ppthd_mm) = (((double *) (_ompc_get_thdprv (&_thdprv_mm, 8, &mm))));
//      (_ppthd_m) = (((int *) (_ompc_get_thdprv (&_thdprv_m, 4, &m))));
// It also handles copyin() clause by adding one more runtime call
//       _ompc_copyin_thdprv (_ppthd_mm, &mm, 4);

int OmpMidend::addThreadprivateDeclarations(SgPragmaDeclaration *decl, \
        OmpAttributeOld *ompattribute, SgBasicBlock *bBlock1)
{
  ROSE_ASSERT(bBlock1 != NULL);
  ROSE_ASSERT(decl != NULL);
  ROSE_ASSERT(ompattribute != NULL);
                                                                                              
//SgScopeStatement* currentscope = isSgFunctionDeclaration(
//              getEnclosingFunctionDeclaration(decl))
//              ->get_definition()->get_body();
                                                                                              
  SgScopeStatement * globalscope = isSgScopeStatement(SageInterface::getGlobalScope(decl));
  for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();\
        i!=ompattribute->var_list.end();i++ )
   {
     SgInitializedName *initname = isSgInitializedName((*i)->origVar);
     SgType * mytype= initname->get_type();
     SgName myname = initname->get_name();
        //used for sizeof(variable) function call expression
//     SgVariableSymbol *varSymbol3=isSgVariableSymbol(initname->get_symbol_from_symbol_table());
//     ROSE_ASSERT(varSymbol3);
     SgVarRefExp* varref3  = buildVarRefExp(initname,bBlock1);
     SgExprListExp * exp_list_exp3 = buildExprListExp();
     appendExpression(exp_list_exp3,varref3);
 
    if ( (*i)->ompType==e_threadprivate) 
     { // add local declaration for threadprivate pointers
        SgVariableDeclaration* sharedvar;
        SgType* pointertype = new SgPointerType(mytype);
        SgName varName("_ppthd_"+myname.getString());
                                                                                              
        sharedvar = buildVariableDeclaration(varName, pointertype,NULL,bBlock1);
        SgInitializedName * name1 = sharedvar->get_decl_item(varName); 
//        SgVariableSymbol * symbol1 = isSgVariableSymbol(name1->get_symbol_from_symbol_table());
//        ROSE_ASSERT(symbol1);
                                                                                              
        prependStatement(sharedvar,bBlock1);
  // (_ppthd_m) = (((int *) (_ompc_get_thdprv (&_thdprv_m, 4, &m))));

        // add dereferencing statements for threadprivate variables
        SgVarRefExp* lhs = buildVarRefExp(name1,bBlock1);
                //arguments for function calls
        SgExprListExp * exp_list_exp1 = buildExprListExp();
                        //arg 1 : &_thdprv_m, previously declared in global scope
        SgName varname("_thdprv_"+myname);
//      SgPointerType * vartype= new SgPointerType( SgTypeVoid::createType());
//bug 346

        SgVariableSymbol *varSymbol1=isSgVariableSymbol(globalscope->lookup_variable_symbol(varname));
        ROSE_ASSERT(varSymbol1);
        SgVarRefExp* varglobal  = buildVarRefExp(varSymbol1->get_declaration(),bBlock1);

        appendExpression(exp_list_exp1,buildAddressOfOp(varglobal));
                        //arg 2: size of mytype
        appendExpression(exp_list_exp1,
                buildFunctionCallExp(SgName("sizeof"), \
                         SgTypeInt::createType(), exp_list_exp3,bBlock1));
                        //arg 3: &m, but m if mytype is array
        SgVarRefExp* arg3  = buildVarRefExp(initname,bBlock1);
        if(mytype->variantT() != V_SgArrayType)
          appendExpression(exp_list_exp1,buildAddressOfOp(arg3));
        else
          appendExpression(exp_list_exp1,arg3);

         // build function call expression
       SgType *return_type = new SgPointerType( SgTypeVoid::createType() );
       SgName funcName("_ompc_get_thdprv");
      
        SgFunctionCallExp * func_call_expr = buildFunctionCallExp
           (funcName,return_type, exp_list_exp1,bBlock1);
       
        SgCastExp *rhs = buildCastExp(func_call_expr, \
                pointertype, SgCastExp::e_C_style_cast); 

        SgExprStatement * expstmti= buildAssignStatement(lhs,rhs);

      //cout<<"debug:in addThreadprivateDeclarations:"<<expstmti->unparseToString()<<endl;
        appendStatement(expstmti,bBlock1);

        // handle copyin case, insert a call like:
        //  _ompc_copyin_thdprv (_ppthd_mm, &mm, 4);
        // reuse the variable reference expressions before
        if (ompattribute->isInClause(initname,e_copyin))
        {
                //arguments for function calls
          SgExprListExp * exp_list_exp2 = buildExprListExp();
         
          appendExpression(exp_list_exp2,lhs);
        if(mytype->variantT() != V_SgArrayType)
          appendExpression(exp_list_exp2,buildAddressOfOp
               (isSgExpression(SageInterface::deepCopy(arg3)))); // no shared expression allowed
        else
          appendExpression(exp_list_exp2,isSgExpression(SageInterface::deepCopy(arg3)));

          appendExpression(exp_list_exp2,
               buildFunctionCallExp(SgName("sizeof"),
                         SgTypeInt::createType(), exp_list_exp3,bBlock1));
          SgExprStatement * expStmt2 = buildFunctionCallStmt( 
                SgName("_ompc_copyin_thdprv"),buildVoidType(), exp_list_exp2, bBlock1);
          appendStatement(expStmt2,bBlock1);
        }// end if copyin

     }// end if threadprivate()
   }// end for

  return 0;
}

//----------------------------
//part of variable handling
// add declaration statements for shared variables into basic block bb
// from pragma declaration decl based on ompattribute
//
// Also handle the declaration reduction variables from the same scope
int OmpMidend::addSharedVarDeclarations(SgPragmaDeclaration *decl, SgFunctionDeclaration* func,\
        OmpAttributeOld *ompattribute, SgBasicBlock *bBlock1)
{
  ROSE_ASSERT(bBlock1 != NULL);
  ROSE_ASSERT(decl != NULL);
  ROSE_ASSERT(ompattribute != NULL);
 
  pushScopeStack(bBlock1);
//   SgScopeStatement* currentscope=isSgScopeStatement(ASTtools::get_scope(decl));
// currentscope is the original scope of the pragma declaration
 SgScopeStatement* currentscope = isSgFunctionDeclaration(\
                getEnclosingFunctionDeclaration(decl))\
                ->get_definition()->get_body();

   int counter=0;

  for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();\
        i!=ompattribute->var_list.end();i++ )
   {
     SgInitializedName *initname = isSgInitializedName((*i)->origVar);
     SgType * mytype= initname->get_type();
     SgName myname = initname->get_name();
    //varscope is the original scope for the shared variables
     SgScopeStatement* varscope=isSgScopeStatement(\
                ASTtools::get_scope(isSgNode((*i)->origVar)));                     // function parameters have strange scope, special handling
    if ( ((currentscope==varscope) ||
          (isSgFunctionParameterList(initname->get_parent())) )&&
         (((*i)->ompType==e_shared)||((*i)->ompType==e_reduction_plus)
           || ((*i)->ompType==e_firstprivate)||((*i)->ompType==e_lastprivate)) )
     { // add local declaration for shared pointers
        // firstprivate, lastprivate also need transferring value between local and master thread 
        //int *_pp_X
      
        SgType* finalType=mytype; 
#if 1        
        //Liao, 4/23/2009. 
        // for an array type variable used a function parameter, must convert its first dimension to a pointer type
        // for example int a[], is converted to int* a; int a[][] -> int *a[],
        // This change will allow all npb 2.3 C benchmarks to pass 
        if (isSgArrayType(mytype))
          //if (isSgGlobal(varscope)) ASTtools::get_scope() will get a global scope for function parameter, which is a bug
          if (isSgFunctionDefinition(initname->get_scope()))
          { 
            finalType = SageBuilder::buildPointerType(isSgArrayType(mytype)->get_base_type());
          }
#endif
        SgType * pointertype = new SgPointerType(finalType);
        //SgType * pointertype = new SgPointerType(mytype);
        SgName varname_1 = SgName( "_pp_"+myname.getString());

       SgVariableDeclaration * sharedvar = buildVariableDeclaration(varname_1,pointertype,NULL);
       appendStatement(sharedvar,bBlock1);

        // add dereferencing statements for shared variables
                // _pp_i = argv[0]
        SgInitializedName * name1 = sharedvar->get_decl_item (varname_1);
        ROSE_ASSERT(name1);
        SgVarRefExp* lhs = buildVarRefExp(name1,currentscope);
        ROSE_ASSERT(lhs);
                //Get the only argument
//       SgName var1_name = "__ompc_args";
//       SgPointerType* pType1= new SgPointerType( SgTypeVoid::createType()); 
//       SgPointerType* pType2 = new SgPointerType(pType1);
       SgInitializedNamePtrList args = func->get_parameterList()->get_args();
       ROSE_ASSERT(args.size()==1);
       SgInitializedName * arg1 = isSgInitializedName(*(args.begin()));
       ROSE_ASSERT(arg1);
   
       SgVarRefExp *var1 = buildVarRefExp(arg1,currentscope );
        SgPntrArrRefExp * rhsvar = buildPntrArrRefExp(var1,buildIntVal(counter)); 
        SgCastExp * rhs = buildCastExp(rhsvar,\
                pointertype,SgCastExp::e_C_style_cast);
 
        SgExprStatement * expstmti= buildAssignStatement(lhs,rhs);
                                                                                                
        //cout<<"debug:"<<expstmti->unparseToString()<<endl;
        appendStatement(expstmti,bBlock1);
        counter ++;
     }
   }// end for

  popScopeStack();
  return 0;
}

//----------------------------
// add global leveldeclaration statements for 
//     threadprivate variables into a namespace
//     lock for critical section
//     object pointer to a class with outlined member function
// for example:
//      void *_thdprv_m;  // global or namespace-scope
//      void * __ompc_lock_critical
//      void * _pp_globalobject
int OmpMidend::addGlobalOmpDeclarations(OmpFrontend* ompfrontend, SgGlobal* myscope, bool hasMain)
{
  ROSE_ASSERT(ompfrontend != NULL);
  ROSE_ASSERT(myscope != NULL);

// printf ("In OmpMidend::addGlobalOmpDeclarations(): hasMain = %s \n",hasMain ? "true" : "false");

  //std::list<string> varlist = ompfrontend->threadprivatelist;
  Rose_STL_Container<SgInitializedName *> varlist = ompfrontend->threadprivateinitnamelist;
  for (Rose_STL_Container<SgInitializedName *>::iterator i= varlist.begin(); i!= varlist.end();i++)
  {
  //generate statements
  SgName varname("_thdprv_"+((*i)->get_name()).getString());
  SgPointerType * vartype=new SgPointerType( new SgPointerType( SgTypeVoid::createType()));

  SgVariableDeclaration* vardecl= buildVariableDeclaration (varname, vartype,0,myscope);

// set storage modifier according to the reach definition initializedName
  if( ((*i)->get_declaration()->get_declarationModifier()).\
        get_storageModifier().isExtern() )
    ((vardecl->get_declarationModifier()).get_storageModifier()).setExtern();
  if( ((*i)->get_declaration()->get_declarationModifier()).\
        get_storageModifier().isStatic() )
    ((vardecl->get_declarationModifier()).get_storageModifier()).setStatic();

  // add into the declaration, TODO better place to insert
   prependStatement(vardecl,myscope);
  }

//      void * __ompc_lock_critical
//TODO insert only needed!
//  extern or not depending on main() function's presence
  SgName varname("__ompc_lock_critical");
  SgPointerType * vartype= new SgPointerType(SgTypeVoid::createType());
  ROSE_ASSERT(vartype);

  SgVariableDeclaration* vardecl= buildVariableDeclaration(varname, vartype,NULL, myscope);
  if (!hasMain)
       ((vardecl->get_declarationModifier()).get_storageModifier()).setExtern();
  //myscope->prepend_declaration(vardecl);
  prependStatement(vardecl,myscope);

//      void * _pp_globalobject
//TODO insert only needed!
//  extern or not depending on main() function's presence
  SgName varname2("_pp_globalobject");
  SgVariableDeclaration* vardecl2 = buildVariableDeclaration(varname2, vartype,NULL,myscope);
  if (!hasMain) ((vardecl2->get_declarationModifier()).get_storageModifier()).setExtern();
 // myscope->prepend_declaration(vardecl2);
   prependStatement(vardecl2,myscope);
  return 0;
}

//----------------------------
//part of variable handling
// add declaration statements for private variables into basic block bb
// from pragma declaration decl based on ompattribute
// declarations are prepended while definition are appended
//
// Also handle the declaration and initialization of reduction variables
int OmpMidend::addPrivateVarDeclarations(SgPragmaDeclaration *decl, \
        OmpAttributeOld *ompattribute, SgBasicBlock *bBlock1)
{
  ROSE_ASSERT(bBlock1 != NULL);
  ROSE_ASSERT(decl != NULL);
  ROSE_ASSERT(ompattribute != NULL);
 
 SgScopeStatement* currentscope = isSgFunctionDeclaration(\
                getEnclosingFunctionDeclaration(decl))\
                ->get_definition()->get_body();
  Rose_STL_Container<SgNode*> decl_list; 
        // record all variables declared, avoid duplicated declaration
        //since two OmpSymbols may have the same variable but different omp attributes

  for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();\
        i!=ompattribute->var_list.end();i++ )
   {
    // TODO consider scope for private variables ??
    //for reduction variables local copies are needed also
    if ( ((*i)->ompType==e_private)||((*i)->ompType==e_reduction_plus)|| \
         ((*i)->ompType==e_firstprivate)||((*i)->ompType==e_lastprivate) )
    {
      //get the original private variable
      SgInitializedName *initname = isSgInitializedName((*i)->origVar);
      SgType * mytype= initname->get_type();
      SgName myname = initname->get_name();
      SgScopeStatement* varscope=isSgScopeStatement(\
                ASTtools::get_scope(isSgNode((*i)->origVar)));   
       
      // internal private copy of private variable.
      SgVariableDeclaration* privatevar=NULL;
      SgInitializedName * name1 = NULL;

     //add intializer expression for firstprivate variables
        // Not using separated assignment statement because copy constructor
        //is required for class type in OpenMP specification
        //mytype _p_k = (*_pp_k) if k's varscope == currentscope or
        //mytype _p_k = k  if k's varscope < currentscope
     SgExpression * rhsexp = NULL;
     SgAssignInitializer * init2=NULL;
#if 1
          // special handling for firstprivate
     if ((*i)->ompType == e_firstprivate) {
        SgName varname1_name(myname.getString()); 
        SgName initname2_name("_pp_"+myname.getString());

//      if (varscope<currentscope)  
      if (ASTtools::isAncestor(varscope,currentscope))  
      {   
        // reusing initname directly will have weird behavior during runtime.
        // so build varname1 from scratch!!
        SgVarRefExp * initExp = buildVarRefExp(varname1_name,bBlock1);
         rhsexp= initExp;
      }
      else if(varscope == currentscope) 
      {
        SgPointerDerefExp *derefexp= buildPointerDerefExp(
               buildVarRefExp(initname2_name,bBlock1));
         derefexp->set_need_paren(true);
         rhsexp= derefexp;
      }                                                                          

     } //end if firstprivate 
#endif
     if(rhsexp!=NULL)
       init2 = buildAssignInitializer (rhsexp);

      SgName privateName( "_p_"+myname.getString());
//cout<<"debugging .........omptranslator: 2775: "<< privateName << endl; //danglling symbol ?
      privatevar = buildVariableDeclaration(privateName, mytype,init2,bBlock1);

     // privatevar->set_firstNondefiningDeclaration(privatevar);
      name1 = privatevar->get_decl_item(privateName);
      ROSE_ASSERT(name1);
   // add declaration for the local copy, only if not declared previously
      if(find(decl_list.begin(),decl_list.end(),(*i)->origVar)==decl_list.end())
    {
        appendStatement(privatevar,bBlock1);
        decl_list.push_back((*i)->origVar);
      }
    // add initialization statements for reduction variables' local copies
        //TODO handle all reduction operators
        // TODO copy constructor needed instead of operator=
    if ((*i)->ompType==e_reduction_plus)
       {
         
         SgExprStatement *expstmt = buildAssignStatement(buildVarRefExp(name1,currentscope), buildIntVal(0));
         appendStatement(expstmt,bBlock1);
       }

     } //end if (..||.. ||.. ||)
  } //end for

  return 0;
}

//----------------------------
//convert the SgType to the Omni RTL data type used in call _ompc_reduction(..,data type,..)
// refer to Omni runtime library for enumerate type, also copied at the top of this file

int OmpMidend::convertTypeId(SgType* sgtype)
{
  int rt=OMPC_REDUCTION_MAX_TYPES;
  switch (sgtype->variantT())
  {
    case  V_SgTypeChar:
    case  V_SgTypeSignedChar:
        {
        rt=OMPC_REDUCTION_CHAR;
        break;
        }
    case  V_SgTypeUnsignedChar:
        {
        rt=OMPC_REDUCTION_UNSIGNED_CHAR;
        break;
        }
    case  V_SgTypeShort:
    case  V_SgTypeSignedShort:
        {
        rt=OMPC_REDUCTION_SHORT;
        break;
        }
    case  V_SgTypeUnsignedShort:
        {
        rt=OMPC_REDUCTION_UNSIGNED_SHORT;
        break;
        }
    case  V_SgTypeInt:
    case  V_SgTypeSignedInt:
        {
        rt=OMPC_REDUCTION_INT;
        break;
        }
    case  V_SgTypeUnsignedInt:
        {
        rt=OMPC_REDUCTION_UNSIGNED_INT;
        break;
        }
    case  V_SgTypeLong:
    case  V_SgTypeSignedLong:
        {
        rt=OMPC_REDUCTION_LONG;
        break;
        }
    case  V_SgTypeUnsignedLong:
        {
        rt=OMPC_REDUCTION_UNSIGNED_LONG;
        break;
        }
    case  V_SgTypeLongLong:
        {
        rt=OMPC_REDUCTION_LONGLONG;
        break;
        }
    case  V_SgTypeUnsignedLongLong:
        {
        rt=OMPC_REDUCTION_UNSIGNED_LONGLONG;
        break;
        }
    
    case  V_SgTypeFloat:
        {
        rt=OMPC_REDUCTION_FLOAT;
        break;
        }
    case  V_SgTypeDouble:
        {
        rt=OMPC_REDUCTION_DOUBLE;
        break;
        }
    case  V_SgTypeLongDouble:
        {
        rt=OMPC_REDUCTION_LONG_DOUBLE;
        break;
        }
  default:
   {
    cout<<"Warning: OmpMidend::convertTypeId(): Sorry, omp type conversion not implemented (default reached) for SgType: "<<sgtype->class_name()<<endl;
 // ROSE_ASSERT(false);
   }
  }
  return rt;
}

//----------------------------
// split the combined #pragma omp parallel for into
//          #pragma omp parallel and #pragma omp for
//      all clauses are kept in #pragma omp parallel currently
// TODO: fine-grained split for all possible clauses, could also happen in OmpFrontend
// similar work as to combined #pragma omp parallel sections        
int OmpMidend::splitCombinedParallelForSections(SgPragmaDeclaration* decl)
{
  ROSE_ASSERT(decl != NULL);
  ROSE_ASSERT((OmpFrontend::recognizePragma(decl)==e_parallel_for)||\
        (OmpFrontend::recognizePragma(decl)==e_parallel_sections)) ;

  bool isParallelFor= (OmpFrontend::recognizePragma(decl)==e_parallel_for); 
  int offset;
  // generate new pragma 'omp for',
  string stromp;
  if (isParallelFor)
    stromp="omp for";// not "#pragma omp for" here
   else 
    stromp="omp sections";
  SgPragmaDeclaration *pragmadecl2 =  buildPragmaDeclaration(stromp);
  // generate OmpAttributeOld for 'omp for' 
  AstAttribute * ompattribute = new OmpAttributeOld(e_for);
  ROSE_ASSERT(ompattribute !=NULL );
  dynamic_cast<OmpAttributeOld*>(ompattribute)->pragma = pragmadecl2;
  dynamic_cast<OmpAttributeOld*>(ompattribute)->omp_type = e_for;
  dynamic_cast<OmpAttributeOld*> (ompattribute)->parentPragma=decl;
  dynamic_cast<OmpAttributeOld*> (ompattribute)->wrapperCount=0;

//move lastprivate() and firstprivate() from 'omp parallel for' to 'omp for'
// Don't keep firstprivate on 'omp parallel' 
// since it will declare private copy again there!
//TODO scheduling clause

// TODO: move the clause string also, bug not very necessary since OmpMidend only
// works on OmpAttributeOld ,not on the pragma string
  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  ROSE_ASSERT(astattribute != NULL);// Do we really need this assert?
  OmpAttributeOld *  srcattribute= dynamic_cast<OmpAttributeOld* > (astattribute);
  ROSE_ASSERT(srcattribute != NULL);

  for (Rose_STL_Container<OmpSymbol*>::iterator i=srcattribute->var_list.begin();\
        i!=srcattribute->var_list.end();i++)
  {
    if(( (*i)->ompType == e_lastprivate ) ||((*i)->ompType == e_firstprivate ))
    {
      OmpSymbol * ompsymbol = new OmpSymbol((*i)->origVar, (*i)->ompType);
      dynamic_cast<OmpAttributeOld*>(ompattribute)->var_list.push_back(ompsymbol);
      if( (*i)->ompType == e_firstprivate ) 
         dynamic_cast<OmpAttributeOld*>(ompattribute)->hasFirstprivate = true;
      if( (*i)->ompType == e_lastprivate ) 
        dynamic_cast<OmpAttributeOld*>(ompattribute)->hasLastprivate = true;
      (*i)->ompType = e_shared; // tricky method. lastprivate need wrapping at omp parallel
    }
  }

 // srcattribute->print();
 // dynamic_cast<OmpAttributeOld*>(ompattribute)->print();

// also move the scheduling attributes to 'omp for'
  dynamic_cast<OmpAttributeOld*>(ompattribute)->sched_type = srcattribute->sched_type;
  dynamic_cast<OmpAttributeOld*>(ompattribute)->chunk_size = srcattribute->chunk_size;

  pragmadecl2->addNewAttribute("OmpAttributeOld",ompattribute);

  // modify the orignal one from 'omp parallel for' to 'omp parallel'
  string *strnew;
  srcattribute->omp_type = e_parallel; 
  strnew = new string(decl->get_pragma()->get_pragma()); 
  if (isParallelFor)
  { 
    offset = strnew->find("for");
    strnew->erase(offset,3);
  }
  else
  { 
    offset = strnew->find("sections");
    strnew->erase(offset,8);
  }
  setPragma(decl, new SgPragma((char*)(strnew->c_str()),TRANS_FILE));
  SgBasicBlock * bb = buildBasicBlock();

  SgStatement * nextstmt = getNextStatement(decl);
// cout<<"L2081, before split:"<<nextstmt->unparseToString()<<endl;

  //the next statement of #pragma omp parallel for is the ' for loop'
  // according to the specification
  if (isParallelFor)
  { //directly add for loop
    ROSE_ASSERT(isSgForStatement(nextstmt)!=NULL);
    ASTtools::deepCopy(nextstmt,bb);
  }
  else // need a new scope for omp sections
  {
    ROSE_ASSERT(isSgBasicBlock(nextstmt)!=NULL);
    SgBasicBlock * bb2 = buildBasicBlock();

    ASTtools::deepCopy(nextstmt,bb2);
    appendStatement(bb2,bb);
  }
   
  prependStatement(pragmadecl2,bb);
  // replace the for loop with the new one with omp for pragma
  SgStatement *targetBB=isSgStatement(decl->get_parent());
  ROSE_ASSERT(targetBB !=NULL);
  targetBB-> replace_statement(nextstmt, bb);
// cout<<"After split,debug:"<<targetBB->unparseToString()<<endl;

  return 0;
}


//----------------------------
// Bottom-up processing AST tree to translate all OpenMP constructs
// the major interface of OmpMidend
//----------------------------
int OmpMidend::bottomupProcessing(SgProject *project)
{ 
//Use bottom-up traversal from ASTSimpleProcessing(?) to do the transformation
/*
  OmpProcessor myprocessor;
  myprocessor.traverse(project, postorder); // not working for both pre or post order
          //the outermost parallel region is always translated first in both cases 

For example:

#pragma omp parallel
  {
    #pragma omp parallel
       statement1;
  }

The AST looks like:

  SgBasicBlock1
    /      \
   /        \
SgPragma1  SgBasicBlock2   
             /      \
         SgPragma2  SgStatement1

For preorder, the order of visit would be: 
     (bb1, pragma1, bb2, pragma2, stmt1)
for postorder, it is:
     (pragma1, pragma2, stmt1,bb2, bb1) 
pragma1 is always visited before pragma2, not desired

Alternative way: use the default preorder in AST query and reverse it:
We get what we want then:
        (stmt1, pragma2, bb2,pragma1, bb1).
*/
// a workaround to get bottom up processing we really want
  Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(project, V_SgPragmaDeclaration);
  typedef Rose_STL_Container<SgNode*>::reverse_iterator pragmaListIterator;
  for (pragmaListIterator listElement=pragmaList.rbegin();listElement!=pragmaList.rend();\
                ++listElement)
  {
    SgPragmaDeclaration * decl = isSgPragmaDeclaration(*listElement);
    ROSE_ASSERT(decl != NULL);
    switch (OmpFrontend::recognizePragma(decl))
    {
        case e_parallel_for:
                {
                OmpMidend::transParallelFor(decl);
                break;
                }
        case e_parallel:
                {
                OmpMidend::transParallelRegion(decl);
                break;
                }
        case e_for:
                {
                OmpMidend::transOmpFor(decl);
                break;
                }
        case e_parallel_sections:
                {
                OmpMidend::transParallelSections(decl);
                break;
                }
        case e_sections:
                {
                OmpMidend::transSections(decl);
                break;
                }
        case e_critical:
                {
                OmpMidend::transCritical(decl);
                break;
                }
        case e_atomic:
                {
                OmpMidend::transAtomic(decl);
                break;
                }
        case e_single:
                {
                OmpMidend::transSingle(decl);
                break;
                }
        case e_master:
                {
                OmpMidend::transMaster(decl);
                break;
                }
        case e_barrier:
                {
                OmpMidend::transBarrier(decl);
                break;
                }
        case e_flush:
                {
                OmpMidend::transFlush(decl);
                break;
                }
        case e_threadprivate: // the translation does not happen near the pragma
                { // OmpFrontend already collects the variables
                  // only delete the declaration here
                LowLevelRewrite::remove(decl);
                break;
                }
        case e_section:
               {
                 break; // section is processed with sections, no further action needed.
               }
        default:
           {
             cout<<"Warning: pragma ignored, not implemented or no implementation needed."<<endl;
             cout<<decl->unparseToString()<<" at line:"<< (decl->get_file_info())->get_line()<<endl;
          // ROSE_ASSERT(false);
           }
    } // end of switch
  } // end of for()
  //Liao, 10/25/2007
  // set parent pointers and fixup template names, etc
//  AstPostProcessing(project);

  return 0;
}
//----------------------------
// transform an orphaned 'sections' directive, e.g.
// ------------------------------
//  #pragma omp sections
//      {
//  #pragma omp section
//     printf("hello, 1\n");
//  #pragma omp section
//     printf("hello, 2\n");
//      }
//-- is translated into--:
//     _ompc_section_init (2);
//      // extern int _ompc_section_id();
//    L_0:;
//      switch (_ompc_section_id ())
//        {
//        case 0:
//          {
//            printf ("hello, 1\012");
//          }
//          goto L_0;
//        case 1:
//          {
//            printf ("hello, 2\012");
//          }
//          goto L_0;
//        }
//      _ompc_barrier ();
//    }
// also handling all possible clauses like: private, firstpirvate, 
// lastprivate, reduction and nowait here
int OmpMidend::transSections(SgPragmaDeclaration *decl)
{
  ROSE_ASSERT(decl != NULL);
  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  OmpAttributeOld *  ompattribute= dynamic_cast<OmpAttributeOld* > (astattribute);

  SgBasicBlock *bb1 = buildBasicBlock();

  // private variable handling etc
  addPrivateVarDeclarations(decl,ompattribute,bb1);

  SgScopeStatement * globalscope = isSgScopeStatement(SageInterface::getGlobalScope(decl));
  // extern void _ompc_section_init(int n_sections);
  SgExprListExp * exp_list_exp1 = buildExprListExp();
  appendExpression(exp_list_exp1, buildIntVal(OmpMidend::getSectionCount(decl)));

  SgExprStatement *stmt1= buildFunctionCallStmt(
         SgName("_ompc_section_init"),buildVoidType(),exp_list_exp1, getScope(decl));
  appendStatement(stmt1,bb1);
  // L_0:;  label statement is special since its symbol is stored in functionDef, not nearest scope statement such as basic block
  stringstream ss;
  ss<<labelCounter;
  SgName label_name=SgName("L_"+ss.str());
  labelCounter++;//increase static variable to get unique lable id
  SgFunctionDefinition * label_scope = getEnclosingFunctionDefinition(decl);
  ROSE_ASSERT(label_scope);
  SgLabelStatement * labelstmt = buildLabelStatement(label_name,NULL,label_scope);
  appendStatement(labelstmt,bb1);
   // switch ()
  //int _ompc_section_id();
  SgExprListExp * exp_list_exp2 = buildExprListExp();
  SgExprStatement *item_selector= buildFunctionCallStmt( 
         SgName("_ompc_section_id"),buildIntType(),exp_list_exp2, globalscope);
  // SgCaseOptionStmt in switch stmt body
  SgBasicBlock * switchbody=  buildBasicBlock();
  SgBasicBlock *declbody = isSgBasicBlock(getNextStatement(decl));
  ROSE_ASSERT(declbody != NULL);
  SgStatementPtrList & ptrlist = declbody -> get_statements();
 // find all structured code blocks for 'omp section'
  // use them to construct case options
  // switch -> switchbody-> caseoptionstmt-> copiedbody + gotostmt
  int counter=0;
  for (Rose_STL_Container<SgStatement*>::iterator i=ptrlist.begin();i!=ptrlist.end();i++)
  {
    if (isSgPragmaDeclaration(*i) &&
      (OmpFrontend::recognizePragma(isSgPragmaDeclaration(*i)) == e_section))
    {
      SgBasicBlock *casebody = buildBasicBlock();
      SgBasicBlock* copiedbody = buildBasicBlock();

      // deep copy works with dangling target BB ?
      ASTtools::deepCopy(getNextStatement(isSgPragmaDeclaration(*i)),copiedbody);
      // variable substituting
      variableSubstituting(decl, ompattribute, copiedbody);
      appendStatement(copiedbody,casebody);
      appendStatement(buildGotoStatement(labelstmt),casebody );
    
      SgCaseOptionStmt * optionstmt = buildCaseOptionStmt( \
                buildIntVal(counter),casebody);
      casebody->set_parent(optionstmt);

      appendStatement(optionstmt,switchbody);
      counter++; 
    }
  } // end for
                                                                                          
  // switch stmt
  SgSwitchStatement* switchstmt =  buildSwitchStatement(\
                item_selector,switchbody);
  item_selector->set_parent(switchstmt);
  switchbody->set_parent(switchstmt);
  
  appendStatement(switchstmt,bb1);

  // handle lastprivate right after 'reduction' and before 'barrier'
  if(ompattribute->hasReduction) addReductionCalls(decl,ompattribute,bb1);
  if (ompattribute->hasLastprivate) addLastprivateStmts(decl,ompattribute,bb1);
  // barrier
 if(!ompattribute->nowait)
  {

    SgExprListExp * exp_list_exp3 = buildExprListExp();
    SgExprStatement * expStmt3= buildFunctionCallStmt( 
        SgName("_ompc_barrier"),buildVoidType(), exp_list_exp3,globalscope);
    appendStatement(expStmt3,bb1);
  }

// cout<<"-------DEBUG------2178:in transSections.."<< bb1->unparseToString()<<endl;
  // replace the two new statements with new basic block
  SgStatement *parentBB = isSgStatement(decl->get_parent());
  ROSE_ASSERT(parentBB != NULL);

  insertStatement (decl, bb1, true);
  LowLevelRewrite::remove(decl);
  LowLevelRewrite::remove(declbody);
  return 0;
}

//----------------------------
// transform an OpenMP orphaned 'for' directive
// generate a basic block:
// {
//   // 1. declare local copies for loop control variables
//
//   int _p_loop_index, _p_loop_lower, _p_loop_upper, _p_loop_stride;
//   //2. initialize them
//
//   _p_loop_lower = original lower;
//   _p_loop_upper = original upper (or +1 if <=) ; ???
//   _p_loop_stride = original stride;
//
//   // 3. call scheduling runtime call
//              //  handles dynamic, guided scheduling etc.
//   _ompc_default_sched(&_p_loop_lower,&_p_loop_upper, &_p_loop_stride);
//
//   // 4. deep copy original loop body
//
//   // 5. replace any references to loop control variables   
//        // also replace other private/shared/reduction varialbes
//   for ((_p_loop_index) = (_p_loop_lower); (_p_loop_index) < (_p_loop_upper);
//                                 (_p_loop_index) += (_p_loop_stride))
//     {
//     }
//
//   // 6. add the implicit barrier // handles nowait
//    _ompc_barrier ();
//}
//  replace the pragma block with the generated basic block(new scope here)
int OmpMidend::transOmpFor(SgPragmaDeclaration *decl)
{
  SgBasicBlock *bb1 = buildBasicBlock();
  pushScopeStack(bb1);
  ROSE_ASSERT(bb1 != NULL);

  SgStatement* forstmt = getNextStatement(decl);
  ROSE_ASSERT(isSgForStatement(forstmt) != NULL);
  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  OmpAttributeOld *  ompattribute= dynamic_cast<OmpAttributeOld* > (astattribute);
  //handling shared/private/reduction variables specified with 'omp for'
  if (ompattribute != NULL)
  {
    addPrivateVarDeclarations(decl,ompattribute,bb1);
//   transParallelRegion will handle this, otherwise overlapped declaration and definition!!
//    addSharedVarDeclarations(decl,ompattribute,bb1);
    // for orphaned omp for, has to add declaration here!!
   if(ompattribute->isOrphaned)
       addThreadprivateDeclarations(decl,ompattribute,bb1);
  }
  //int _p_loop_index, _p_loop_lower, _p_loop_upper, _p_loop_stride;
  SgType *inttype =  SgTypeInt::createType();

  SgName indexName("_p_loop_index");
  SgName lowerName("_p_loop_lower");
  SgName upperName("_p_loop_upper");
  SgName strideName("_p_loop_stride");

  SgVariableDeclaration* loopvardecl = buildVariableDeclaration(indexName,inttype, NULL,bb1);
  SgVariableDeclaration* loopvardecl2 = buildVariableDeclaration(lowerName,inttype, NULL,bb1);
  SgVariableDeclaration* loopvardecl3 = buildVariableDeclaration(upperName,inttype, NULL,bb1);
  SgVariableDeclaration* loopvardecl4 = buildVariableDeclaration(strideName,inttype, NULL,bb1);

  SgInitializedName *loopindex = loopvardecl->get_decl_item(indexName);
  SgInitializedName *looplower = loopvardecl2->get_decl_item(lowerName);
  SgInitializedName *loopupper = loopvardecl3->get_decl_item(upperName);
  SgInitializedName *loopstride = loopvardecl4->get_decl_item(strideName);
  ROSE_ASSERT(loopindex);
  ROSE_ASSERT(looplower);
  ROSE_ASSERT(loopupper);
  ROSE_ASSERT(loopstride);

  SgVariableSymbol * loopindexsymbol= isSgVariableSymbol(loopindex->get_symbol_from_symbol_table ());

  ROSE_ASSERT(loopindexsymbol);

  // only declare int _p_loop_index if for (i=X,, ), not for(int i=X,....)
  SgVariableDeclaration * vardecl = isSgVariableDeclaration(\
                (*(isSgForStatement(forstmt)->get_init_stmt()).begin()) );
  if (vardecl==NULL)  appendStatement(loopvardecl,bb1);

  appendStatement(loopvardecl2,bb1);
  appendStatement(loopvardecl3,bb1);
  appendStatement(loopvardecl4,bb1);
        // set init values for local copies of loop control variables
  ROSE_ASSERT(isSgForStatement(forstmt)!=NULL);
  SgExpression* lowervalue=isSgExpression( SageInterface::deepCopy(ASTtools::getForLoopTripleValues \
                (0, isSgForStatement(forstmt))));

  //SgExpression should not be shared at all! So create them each time when needed!!
  SgExpression * rhslower = isSgExpression(SageInterface::deepCopy(lowervalue));

  SgExprStatement * expstmtlower= buildAssignStatement(buildVarRefExp(looplower),rhslower);
  appendStatement(expstmtlower,bb1);
                //set upper bound
  SgExpression* uppervalue=isSgExpression( SageInterface::deepCopy(ASTtools::getForLoopTripleValues \
                (1, isSgForStatement(forstmt))));

//  SgVarRefExp *lhsupper = buildVarRefExp(loopupper);
  SgExpression * rhsupper= isSgExpression(SageInterface::deepCopy(uppervalue));

  SgExpression * testexp=isSgExprStatement(isSgForStatement(forstmt)->get_test())->get_expression();
  ROSE_ASSERT(testexp != NULL);
  if (isSgLessOrEqualOp(testexp)!=NULL) 
        // using upper +1 since <= changed to < after normalization
  { 
    rhsupper = buildAddOp(rhsupper, buildIntVal(1));
  }
  if (isSgGreaterOrEqualOp(testexp)!=NULL) 
        // using upper -1 since >= changed to > after normalization
  { 
    rhsupper = buildAddOp(rhsupper, buildIntVal(-1));
  }

  SgExprStatement * expstmtupper= buildAssignStatement(buildVarRefExp(loopupper),rhsupper);
  appendStatement(expstmtupper,bb1);

  SgExpression* stridevalue=isSgExpression( SageInterface::deepCopy(ASTtools::getForLoopTripleValues \
                (2, isSgForStatement(forstmt))));
                                                                                                
//  SgVarRefExp *lhsstride = buildVarRefExp(loopstride);
  SgExpression * rhsstride = isSgExpression(SageInterface::deepCopy(stridevalue));
  SgExprStatement * expstmtstride= buildAssignStatement(buildVarRefExp(loopstride),rhsstride);
  appendStatement(expstmtstride,bb1);

  //3.   _ompc_default_sched(&_p_loop_lower,&_p_loop_upper, &_p_loop_stride);
  SgExprListExp * parameters=NULL;
  SgExprListExp * parameters2=NULL;
  SgExprListExp * parameters3=NULL;// used for ompc_xx_sched_next(&lower, &upper)
  SgExpression * chunkexp=NULL;
  SgExprStatement * callstmt=NULL;
  SgExprStatement * condcall=NULL;

  if (ompattribute->chunk_size != NULL)
  {
     if(isSgIntVal(ompattribute->chunk_size) != NULL)
        chunkexp = buildIntVal(isSgIntVal(ompattribute->chunk_size)->get_value());
     else if(isSgInitializedName(ompattribute->chunk_size))
        chunkexp = buildVarRefExp(isSgInitializedName(ompattribute->chunk_size),bb1 );
  }
   //default and static without chunk parameters// TODO build only if needed!!
     parameters = buildExprListExp();
     appendExpression(parameters,buildAddressOfOp(buildVarRefExp(looplower)));
     appendExpression(parameters,buildAddressOfOp(buildVarRefExp(loopupper)));
     appendExpression(parameters,buildAddressOfOp(buildVarRefExp(loopstride)));
   // used for non-default scheduling init calls
     parameters2 = buildExprListExp();
     appendExpression(parameters2,buildVarRefExp(looplower));
     appendExpression(parameters2,buildVarRefExp(loopupper));
     appendExpression(parameters2,buildVarRefExp(loopstride));

     parameters3 = buildExprListExp();
     appendExpression(parameters3,buildAddressOfOp(buildVarRefExp(looplower)));
     appendExpression(parameters3,buildAddressOfOp(buildVarRefExp(loopupper)));
 
  if (ompattribute->sched_type == e_sched_none)
  {
     callstmt= buildFunctionCallStmt( 
        SgName("_ompc_default_sched"),buildVoidType(), parameters, bb1);
  }  
    else if (ompattribute->sched_type == e_sched_static && ompattribute->chunk_size==NULL)
  {
     callstmt= buildFunctionCallStmt( 
        SgName("_ompc_static_bsched"),buildVoidType(), parameters, bb1);
  }
   else if (ompattribute->sched_type == e_sched_static && ompattribute->chunk_size!=NULL)
   {
     appendExpression(parameters2,chunkexp);
    callstmt= buildFunctionCallStmt( 
     SgName("_ompc_static_sched_init"),buildVoidType(), parameters2, bb1);

    condcall=buildFunctionCallStmt( 
        SgName("_ompc_static_sched_next"),buildIntType(), parameters3,bb1);
   } 
   else if (ompattribute->sched_type == e_sched_dynamic)
   {
     appendExpression(parameters2,chunkexp);
    callstmt= buildFunctionCallStmt( 
     SgName("_ompc_dynamic_sched_init"),buildVoidType(),parameters2, bb1);

    condcall=buildFunctionCallStmt( 
        SgName("_ompc_dynamic_sched_next"),buildIntType(), parameters3,bb1);
   } 
   else if (ompattribute->sched_type == e_sched_guided)
   {
     appendExpression(parameters2,chunkexp);
    callstmt= buildFunctionCallStmt( 
     SgName("_ompc_guided_sched_init"),buildVoidType(),parameters2, bb1);

    condcall=buildFunctionCallStmt( 
        SgName("_ompc_guided_sched_next"),buildIntType(),parameters3,bb1);
   } 
   else if (ompattribute->sched_type == e_sched_runtime)
   {
    callstmt= buildFunctionCallStmt( 
     SgName("_ompc_runtime_sched_init"),buildVoidType(),parameters2, bb1);

    condcall=buildFunctionCallStmt( 
        SgName("_ompc_runtime_sched_next"),buildIntType(),parameters3,bb1);
   } else 
   {
     cout<<"Fatal error in transOmpFor! unkown sched_type"<<endl;
     ROSE_ASSERT(false);
   }

  appendStatement(callstmt,bb1);
  // generate the loop body: 2 cases: default scheduling(or static without chunksize)
  //          and all others
  
  SgForStatement *newforstmt;
  if  ( (ompattribute->sched_type == e_sched_none) ||\
   (ompattribute->sched_type == e_sched_static && ompattribute->chunk_size==NULL))
  {
    //case 1: directly deep copy the original loop body
    ASTtools::deepCopy(forstmt,bb1);
    SgStatementPtrList& stmtptrlist = bb1->get_statements();
    Rose_STL_Container<SgStatement*>::iterator i=stmtptrlist.end();
    newforstmt = isSgForStatement(*--i);
  } // case 2: wrap the loop body into a while loop
   else if (ompattribute->sched_type == e_sched_dynamic||\
      (ompattribute->sched_type == e_sched_static&& ompattribute->chunk_size!=NULL)||\
            ompattribute->sched_type == e_sched_guided||\
            ompattribute->sched_type == e_sched_runtime)
  {
    SgBasicBlock * wbody = buildBasicBlock();

    ASTtools::deepCopy(forstmt,wbody);
    SgWhileStmt * wstmt = buildWhileStmt(condcall,wbody); 

    SgStatementPtrList& stmtptrlist = wbody->get_statements();
    Rose_STL_Container<SgStatement*>::iterator i=stmtptrlist.end();
    newforstmt = isSgForStatement(*--i);
  
    appendStatement(wstmt,bb1);
   
  } else
   {
     cout<<"Fatal error in transOmpFor!,unknown sched_type"<<endl;
     ROSE_ASSERT(false);
   }


  // 5. variable substitution for loop control variable
  //  on the newly copied 'for stmt' in the basic block

  Rose_STL_Container<SgNode*> reflist = NodeQuery::querySubTree(newforstmt, V_SgVarRefExp);
        // replace all reference sites to the loop index variables

// Liao, 11/1/2007
// How could this work before? The loop index gets changed for the first time, the following
// check will happen against the changed one, not the original one. 
// The result: only the  first appearance of the loop index gets changed!
  SgInitializedName *loopvar = ASTtools::getLoopIndexVar(newforstmt);
  for (Rose_STL_Container<SgNode*>::iterator i=reflist.begin();i!=reflist.end();i++)
  {
//    if (ASTtools::isLoopIndexVarRef(isSgForStatement(newforstmt), isSgVarRefExp(*i)))
      SgInitializedName * refvar = isSgVarRefExp(*i)->get_symbol()->get_declaration();
      if ( (loopvar->get_name() == refvar->get_name())\
           &&(loopvar->get_scope() == refvar->get_scope()) )
        isSgVarRefExp(*i)->set_symbol(loopindexsymbol);
  }
        // replace the original lower,uppper,and stride with the new ones
        // the increment control operator may be also changed from <= to < 
        // TODO  having a loop normalization phase to simplify the process
  ASTtools::setForLoopTripleValues(0,newforstmt, buildVarRefExp(looplower));
  ASTtools::setForLoopTripleValues(1,newforstmt, buildVarRefExp(loopupper));
  ASTtools::setForLoopTripleValues(2,newforstmt, buildVarRefExp(loopstride));

     //TODO; for (int i=x,..), substitute SgVariableDeclaration with SgExprStatement 
        // for SgForInitStatement of a SgForStatement
  //variable substitution for private,shared, and reduction variables in the loop body
  if (ompattribute != NULL)
  {
   // only work on the loop, not the whole bb1!! Otherwise may corrupt the inserted calls
    //variableSubstituting(decl,ompattribute,bb1);
    SgStatementPtrList &stmtlist = bb1->get_statements();
    Rose_STL_Container<SgStatement*>::reverse_iterator i=stmtlist.rbegin();

    variableSubstituting(decl,ompattribute,*i);

   //TODO performance concern, add tags for reduction, lastprivate clause,
        // only call addXXX() if the tags are true, low priority though
    if(ompattribute->hasReduction) addReductionCalls(decl,ompattribute,bb1);
  // handle lastprivate right after reduction and before barrier
  //It can handle variables in both first/last private lists now
    if (ompattribute->hasLastprivate) addLastprivateStmts(decl,ompattribute,bb1);
  } 

  // add barrier TODO consider nowait
  if (!ompattribute->nowait) {
    SgExprListExp * parameters2 = buildExprListExp();
    SgExprStatement * barriercallstmt= buildFunctionCallStmt( 
        SgName("_ompc_barrier"),buildVoidType(),parameters2, bb1);
    appendStatement(barriercallstmt,bb1);
  }

  popScopeStack();
  OmpMidend::replacePragmaBlock(decl,bb1);
//  cout<<"debug:ompfor:"<<bb1->unparseToString()<<endl;
  return 0;
}

//----------------------------
// translation function for combined omp parallel sections
// tasks:
//      split combined omp parallel sections into separated omp parallel and omp sections
//      bottom up translation then:
//              call transSections() first, 
//              then transParallelRegion()
// TODO consider moving this into frontend, pragma proprecessing or normalization phase
//      the OmpMidend only sees simpler separated case
int OmpMidend::transParallelSections(SgPragmaDeclaration *decl)
{
  ROSE_ASSERT(OmpFrontend::recognizePragma(decl)==e_parallel_sections);
  splitCombinedParallelForSections(decl);

  ROSE_ASSERT(OmpFrontend::recognizePragma(decl)==e_parallel);
  SgBasicBlock *bb = isSgBasicBlock(getNextStatement(decl)); 
  ROSE_ASSERT(bb != NULL);

  SgStatementPtrList &stmtptrlist = bb ->get_statements();
  SgPragmaDeclaration * sectionsdecl= isSgPragmaDeclaration(*(stmtptrlist.begin()));
  ROSE_ASSERT(sectionsdecl != NULL);

  transSections(sectionsdecl);
//cout<<"--------2519,debug,transPrallelRegion:"<<endl<<decl->unparseToString()<<endl;
  transParallelRegion(decl);

  return 0;
}

//----------------------------
// translation function for combined omp parallel for
// tasks:
//      split combined omp parallel for into separated omp parallel and omp for
//      bottom up translation then:
//              call transOmpFor() first, 
//              then transParallelRegion()
// TODO consider moving this into frontend, pragma proprecessing or normalization phase
int OmpMidend::transParallelFor(SgPragmaDeclaration *decl)
{
  ROSE_ASSERT(OmpFrontend::recognizePragma(decl)==e_parallel_for);
  splitCombinedParallelForSections(decl);

  ROSE_ASSERT(OmpFrontend::recognizePragma(decl)==e_parallel);
  SgBasicBlock *bb = isSgBasicBlock(getNextStatement(decl)); 
  ROSE_ASSERT(bb != NULL);
  SgStatementPtrList &stmtptrlist = bb ->get_statements();
  SgPragmaDeclaration * fordecl= isSgPragmaDeclaration(*(stmtptrlist.begin()));
  ROSE_ASSERT(fordecl != NULL);

  transOmpFor(fordecl);

  transParallelRegion(decl);

  return 0;
}

//----------------------------
// Transformation for 'omp master': generate code like
//  {
//   if (_ompc_is_master ())
//      {
//        printf ("Running using %d threads..\n", omp_get_num_threads ());
//      }
//  }
int OmpMidend::transMaster(SgPragmaDeclaration * decl)
{

// _ompc_is_master () conditional statement for if statement
  SgScopeStatement * globalscope = isSgScopeStatement(SageInterface::getGlobalScope(decl));
  ROSE_ASSERT(globalscope);
  SgExprListExp * exp_list_exp1 = buildExprListExp();

  SgName varName("_ompc_is_master"); 
  SgExprStatement * expStmt1= buildFunctionCallStmt( 
        varName, buildIntType(), exp_list_exp1, globalscope); 
// true _body, deep copy of the original 
  SgBasicBlock *truebody1 =  buildBasicBlock();
  SgBasicBlock *falsebody1 = buildBasicBlock(); 

  SgStatement* nextstmt = getNextStatement(decl);
  ASTtools::deepCopy(nextstmt,truebody1);

// if statement, must set falsebody1 here, passing NULL instead will cause runtime error!!
  SgIfStmt *ifstmt = buildIfStmt(isSgStatement(expStmt1), truebody1, falsebody1);

// replace with the two new statements
  SgStatement *parentBB = isSgStatement(decl->get_parent());
  ROSE_ASSERT(parentBB != NULL);

  moveUpPreprocessingInfo(decl,nextstmt);
  LowLevelRewrite::remove(nextstmt);
  insertStatement (decl, ifstmt, true);
  moveUpPreprocessingInfo(ifstmt,decl);
  LowLevelRewrite::remove(decl);
  return 0;
}

//----------------------------
// Transformation for 'omp single': generate code like
//  {
//   if (_ompc_do_single ())
//      {
//        printf ("Running using %d threads..\012", omp_get_num_threads ());
//      }
//    _ompc_barrier ();
//  }
// It also handles clauses for single: private(), firstprivate(), copyprivate(), nowait,etc
int OmpMidend::transSingle(SgPragmaDeclaration * decl)
{

  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  OmpAttributeOld *  ompattribute= dynamic_cast<OmpAttributeOld* > (astattribute);

// _ompc_do_single () conditional statement for if statement
  SgScopeStatement * globalscope = isSgScopeStatement(SageInterface::getGlobalScope(decl));
  SgExprListExp * exp_list_exp1 = buildExprListExp();

  SgExprStatement * expStmt1= buildFunctionCallStmt( 
        SgName("_ompc_do_single"),buildIntType(),exp_list_exp1, globalscope); 
// true _body, deep copy of the original 
  SgBasicBlock *truebody1 =  buildBasicBlock();
  SgBasicBlock *falsebody1 =  buildBasicBlock();

  SgStatement* nextstmt = getNextStatement(decl);
        // add declarations for private/firstprivate variables
  addPrivateVarDeclarations(decl,ompattribute,truebody1);
  
  ASTtools::deepCopy(nextstmt,truebody1);
  //performance concern , add if() later on
  variableSubstituting(decl, ompattribute, truebody1);

// if statement
  SgIfStmt *ifstmt = buildIfStmt(isSgStatement(expStmt1), truebody1, falsebody1);

// replace with the two new statements
  SgStatement *parentBB = isSgStatement(decl->get_parent());
  ROSE_ASSERT(parentBB != NULL);
                                                                                                
  LowLevelRewrite::remove(nextstmt);
  insertStatement (decl, ifstmt, true);
  moveUpPreprocessingInfo(ifstmt,decl);
  // _ompc_barrier() only if there is no nowait clause
  if(!ompattribute->nowait)
  {
    SgExprStatement * expStmt2= buildFunctionCallStmt( 
        SgName("_ompc_barrier"),buildVoidType(),exp_list_exp1,globalscope);
    insertStatement (decl, expStmt2, true);
  }
  LowLevelRewrite::remove(decl);
  return 0;
}
//----------------------------
// Transformation for 'omp barrier': replace pragma with 
//    _ompc_barrier ();
int OmpMidend::transBarrier(SgPragmaDeclaration * decl)
{
// _ompc_barrier()
  SgScopeStatement * globalscope = isSgScopeStatement(SageInterface::getGlobalScope(decl));
  SgExprListExp * exp_list_exp1 = buildExprListExp();
  SgExprStatement * expStmt2= buildFunctionCallStmt(
        SgName("_ompc_barrier"),buildVoidType(), exp_list_exp1,globalscope);
// replace with the two new statements
  SgStatement *parentBB = isSgStatement(decl->get_parent());
  ROSE_ASSERT(parentBB != NULL);
  moveUpPreprocessingInfo(expStmt2,decl);   
  insertStatement (decl, expStmt2, true);
  LowLevelRewrite::remove(decl);
  return 0;
}

//--------------------------------------------------
// Translation for omp flush [list]
// Two major cases:
// a. If it is followed by an empty variable list,
//    replace the pragma statement with 
//        _ompc_flush(0,0)
// b. It has a variable list with n variables
//    replace the pragma statement with n 
//       _ompc_flush(n2,size1)
//       _ompc_flush(n2,size2)
// Special concern: 
//   preserve preprocessing information attached to pragma statement.
//   4/24/2008
int OmpMidend::transFlush(SgPragmaDeclaration * decl)
{
  ROSE_ASSERT(decl!=NULL);
  SgScopeStatement * scope = SageInterface::getScope(decl);
  ROSE_ASSERT(scope!=NULL);
  pushScopeStack(scope);
  int move_counter=0; // only move preprocessing Info. once
  SgStatement* last_statement=decl;

  // retrieve the var_list after omp flush
  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  OmpAttributeOld *  ompattribute= dynamic_cast<OmpAttributeOld* > (astattribute); 
  int var_count = ompattribute->var_list.size();

  SgType* return_type = buildVoidType(); 

  // case a first
  if (var_count==0)
  {
      // argument list (0,0)
    SgIntVal * arg1 = buildIntVal(0); // NULL address for C
    SgIntVal * arg2 = buildIntVal(0); 
    SgExprListExp* arg_list = buildExprListExp();
    appendExpression(arg_list,arg1);
    appendExpression(arg_list,arg2);

    SgExprStatement * call_stmt = buildFunctionCallStmt(
      SgName("_ompc_flush"),return_type,arg_list);
    
    moveUpPreprocessingInfo(call_stmt,decl);
    insertStatementAfter(decl,call_stmt);
  } else
  //case b: 1 or multiple variables
  {
    for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();\
        i!=ompattribute->var_list.end();i++ )
    {
      SgInitializedName *initname = isSgInitializedName((*i)->origVar);
      //arg1: addressOf(var)
      SgAddressOfOp* arg_1= buildAddressOfOp(buildVarRefExp(initname->get_name(),scope));

      //arg2: sizeof(var)
      SgVarRefExp* arg_2_1 = buildVarRefExp(initname->get_name(),scope);
      SgExprListExp * arg_2_exp = buildExprListExp();
      appendExpression(arg_2_exp,arg_2_1);
      SgFunctionCallExp * arg_2 = buildFunctionCallExp(SgName("sizeof"),
                buildIntType(),arg_2_exp);

      SgExprListExp* arg_list = buildExprListExp();
      appendExpression(arg_list,arg_1);
      appendExpression(arg_list,arg_2);

      SgExprStatement * call_stmt = buildFunctionCallStmt(
         SgName("_ompc_flush"),return_type,arg_list);
      if (move_counter ==0) // only move preprocessing info. once
         moveUpPreprocessingInfo(call_stmt,last_statement);
      insertStatementAfter(last_statement,call_stmt);
      move_counter++;  
      last_statement=call_stmt; 
    }
  } //end of if-else. case 2

  popScopeStack();
  LowLevelRewrite::remove(decl);  
  return 0;
}

//----------------------------
// generate two calls to enclose the critical region
// void *__ompc_lock_critical is in ompcLib.h
//    _ompc_enter_critical (&__ompc_lock_critical);
//     ...........
//    _ompc_exit_critical (&__ompc_lock_critical);
// delete pragma afterwards
// TODO  consider named critical section, should be easy to extend
int OmpMidend::transCritical(SgPragmaDeclaration *pragDecl)
{

  SgScopeStatement *scope= getScope(pragDecl);

  //SgExprListExp, 1 parameter (&__ompc_lock_critical)
  SgVarRefExp * varref1= buildVarRefExp(SgName("__ompc_lock_critical"),scope);
  SgAddressOfOp *address1= buildAddressOfOp(varref1);
  
  SgExprListExp * exp_list_exp = buildExprListExp();
  appendExpression(exp_list_exp,address1);

  SgExprStatement * expStmt = buildFunctionCallStmt( 
                SgName("_ompc_enter_critical"), buildVoidType(),exp_list_exp, scope);

  SgExprStatement * expStmt2= buildFunctionCallStmt( 
                SgName("_ompc_exit_critical"), buildVoidType(),exp_list_exp, scope);

// enclose the critical region using two calls and remove the pragma statement
  SgStatement *targetStmt = getNextStatement(pragDecl);

  insertStatement (targetStmt, expStmt, true);
  insertStatement (targetStmt, expStmt2, false);
  LowLevelRewrite::remove(pragDecl);
  return 0;
}

//----------------------------
// generate two calls to enclose the atomic region
// TODO validate this method
//    _ompc_atomic_lock();
//     ...........
//    _ompc_atomic_unlock();
// delete pragma afterwards
//
// TODO  add semantic check  in frontend for the statement after omp atomic
// Omni uses a rather complex way to translate atomic, 
// which I don't quite understand the reason (TODO)
//    {
//      auto int *t_0;
//      (t_0) = (&(_p_i));
//      _ompc_atomic_lock ();
//      (*t_0)++;
//      _ompc_atomic_unlock ();
//    }
//
int OmpMidend::transAtomic(SgPragmaDeclaration *pragDecl)
{

          //RTL functions have global scope
  SgScopeStatement *scope= isSgScopeStatement(SageInterface::getGlobalScope(pragDecl));

  //SgExprListExp, 1 parameter ()
  SgExprListExp * exp_list_exp = buildExprListExp();

  SgExprStatement * expStmt = buildFunctionCallStmt( 
                SgName("_ompc_atomic_lock"), buildVoidType(), exp_list_exp, scope);

  SgExprStatement * expStmt2= buildFunctionCallStmt( 
                SgName("_ompc_atomic_unlock"), buildVoidType(), exp_list_exp, scope);

// enclose the critical region using two calls and remove the pragma statement
  SgStatement *targetStmt = getNextStatement(pragDecl);

  SgStatement *parentBB = isSgStatement(pragDecl->get_parent());
  ROSE_ASSERT(parentBB != NULL);
  
  insertStatement (targetStmt, expStmt, true);
  insertStatement (targetStmt, expStmt2, false);
  LowLevelRewrite::remove(pragDecl);
  return 0;
}

//----------------------------
//tasks:
// * find the main entry for the application
// * add (int argc, char *argv[]) if not exist(?)
// * add runtime system init code at the begin 
// * find all return points and append cleanup code
// * add global declarations for threadprivate variables
// * add global declarations for lock variables

int OmpMidend::insertRTLinitAndCleanCode(SgProject* project, OmpFrontend *ompfrontend)
{
  bool hasMain= false;
//find the main entry
  SgFunctionDefinition* mainDef=NULL;

// DQ (1/6/2007): The correct qualified name for "main" is "::main", at least in C++.
// however for C is should be "main".  Our name qualification is not language specific,
// however, for C is makes no sense to as for the qualified name, so the name we
// want to search for could be language specific.  The test code test2007_07.C 
// demonstrates that the function "main" can exist in both classes (as member functions)
// and in namespaces (as more meaningfully qualified names).  Because of this C++
// would have to qualify the global main function as "::main", I think.  Fixing 
// this details correctly turns on "hasMain" in the code below and fixes the linking
// problem we were having!
// string mainName = "main";
   string mainName = "::main";

  const SgFilePtrList& fileptrlist = project->get_fileList();

  for (std::vector<SgFile*>::const_iterator i = fileptrlist.begin();i!=fileptrlist.end();i++) 
  {
    SgSourceFile* sgfile = isSgSourceFile(*i);
    ROSE_ASSERT(sgfile != NULL);

  SgFunctionDeclaration * mainDecl=findMain(sgfile);
  if (mainDecl!= NULL)
   {
    // printf ("Found main function setting hasMain == true \n");
     mainDef = mainDecl->get_definition();
     hasMain = true;
   }

  // declare pointers for threadprivate variables and global lock
  addGlobalOmpDeclarations(ompfrontend, sgfile->get_globalScope(), hasMain );

  if (hasMain){  // only insert into main function
  // add parameter  int argc , char* argv[] if not exist
    SgInitializedNamePtrList args = mainDef->get_declaration()->get_args();
    SgType * intType=  SgTypeInt::createType();
    SgType *charType=  SgTypeChar::createType();
#if 1
  if (args.size()==0){
    SgFunctionParameterList *parameterList = mainDef->get_declaration()->get_parameterList();
    ROSE_ASSERT(parameterList); 
    //mainDef->get_declaration()->set_parameterList(parameterList);
   //  parameterList->set_parent(mainDef->get_declaration());

    // int argc
      SgName name1("argc");
    SgInitializedName *arg1 = buildInitializedName(name1,intType);

    //char* argv[]
    SgName name2("argv");
    SgPointerType *pType1= new SgPointerType(charType);
    SgPointerType *pType2= new SgPointerType(pType1);
    SgInitializedName *arg2 = buildInitializedName(name2,pType2);
 
    appendArg(parameterList,arg1);
    appendArg(parameterList,arg2);

    } // end if (args.size() ==0)
#endif  
  // add statements to prepare the runtime system
    //int status=0;
   SgIntVal * intVal = buildIntVal(0);

  SgAssignInitializer * init2=buildAssignInitializer(intVal);
    SgName *name1 = new SgName("status"); 
    SgVariableDeclaration* varDecl1 = buildVariableDeclaration(*name1, SgTypeInt::createType(),init2, mainDef->get_body());

 // cout<<"debug:"<<varDecl1->unparseToString()<<endl;

    //_ompc_init(argc, argv);
    SgType* voidtype =SgTypeVoid::createType();
  SgFunctionType *myFuncType= new SgFunctionType(voidtype,false);
  ROSE_ASSERT(myFuncType != NULL);
                                                                                                               
       //SgExprListExp, two parameters (argc, argv)
        // look up symbol tables for symbols
  SgScopeStatement * currentscope = mainDef->get_body();

  SgInitializedNamePtrList mainArgs = mainDef->get_declaration()->get_parameterList()->get_args();
  Rose_STL_Container <SgInitializedName*>::iterator i= mainArgs.begin();
  ROSE_ASSERT(mainArgs.size()==2);

  SgVarRefExp *var1 = buildVarRefExp(isSgInitializedName(*i), mainDef->get_body());
  SgVarRefExp *var2 = buildVarRefExp(isSgInitializedName(*++i), mainDef->get_body());

  SgExprListExp * exp_list_exp = buildExprListExp();
  appendExpression(exp_list_exp,var1);
  appendExpression(exp_list_exp,var2);

  SgExprStatement * expStmt=  buildFunctionCallStmt (SgName("_ompc_init"),
           buildVoidType(), exp_list_exp,currentscope);

//  cout<<"debug:"<<expStmt->unparseToString()<<endl;
  //prepend to main body
  prependStatement(expStmt,currentscope);
  prependStatement(varDecl1,currentscope);

//  cout<<"debug:"<<mainDef->unparseToString()<<endl;

  // search all return statements and add terminate() before them
  //the body of this function is empty in the runtime library
    // _ompc_terminate(status);
                                                                                                               
  //SgExprListExp, 1 parameters (status) 
    SgInitializedName *initName1= varDecl1->get_decl_item(*name1);
    ROSE_ASSERT(initName1);

  SgVarRefExp *var3 = buildVarRefExp(initName1,currentscope);
  SgExprListExp * exp_list_exp2 = buildExprListExp();
  appendExpression(exp_list_exp2,var3);
                                                                                                               
  //build call exp stmt
   SgExprStatement * expStmt2= buildFunctionCallStmt (SgName("_ompc_terminate"), 
      buildVoidType(),exp_list_exp2,mainDef->get_body());

  // find return statement, insert before it
  Rose_STL_Container<SgNode*> rtList = NodeQuery::querySubTree(mainDef, V_SgReturnStmt);
  if (rtList.size()>0)
  {
    for(Rose_STL_Container<SgNode*>::iterator i= rtList.begin();i!=rtList.end();i++)
    {
      SgStatement *targetBB= isSgStatement((*i)->get_parent()); 
      ROSE_ASSERT(targetBB != NULL);
      insertStatement(isSgStatement(*i),expStmt2);
    } 
  }
  else //if not found append to function body
  {
    appendStatement(expStmt2,currentscope);
  }
  // cout<<"debug terminate:"<<expStmt2->unparseToString()<<endl;
  //   AstPostProcessing(mainDef->get_declaration());
  return 0;
 } // end if hasMain

 } //end for sgfile
  return 0;
}

//----------------------------
// generate call like _ompc_do_parallel(__ompc_func_x, NULL);
// Also, if there are shared variables inside the parallel region
//   we have to generate the code to wrap addresses of all shared variables 
//   used in this parallel region. They may come from the union of several 'omp for'
//       void * __ompc_argv[2];
//      *(__ompc_argv +0)=(void*)(&share1);
//      *(__ompc_argv +1)=(void*)(&share2);
//      _ompc_do_parallel(__ompc_func_x, __ompc_argv);
//
// return:  a basic block with several statements
SgBasicBlock* OmpMidend::generateParallelRTLcall(SgPragmaDeclaration* pragDecl, SgFunctionDeclaration *outlinedFunc, OmpAttributeOld * ompattribute)
{

  ROSE_ASSERT(outlinedFunc!=NULL);
  SgBasicBlock * bb1= buildBasicBlock();
  pushScopeStack(bb1);
  //0. get persistent attribute 
  ROSE_ASSERT(ompattribute!=NULL);
  // ompattribute->print();

  SgInitializedName *name_ompc_argv=NULL;
  SgPointerType * pointertype =NULL;
  SgArrayType* mytype = NULL;

  // only add this statement when shared variables needing wrapping exist
  if (ompattribute->wrapperCount>0) { 
  //1. build the wrapping statements
     // void * __ompc_argv[2];
    pointertype= new SgPointerType( SgTypeVoid::createType());
    mytype =buildArrayType(pointertype, buildUnsignedLongVal(ompattribute->wrapperCount));

    SgName argName("__ompc_argv");
  SgVariableDeclaration* ompc_argv=buildVariableDeclaration(argName, mytype,NULL,bb1);
    name_ompc_argv = ompc_argv->get_decl_item(argName);
    ROSE_ASSERT(name_ompc_argv);
  //cout<<"debug:"<<ompc_argv->unparseToString()<<endl;
    appendStatement(ompc_argv,bb1);
  }
    //2.   *(__ompc_argv+0)=(void*)(&share1);
  SgScopeStatement* currentscope= isSgFunctionDeclaration(\
                getEnclosingFunctionDeclaration(pragDecl))\
                ->get_definition()->get_body();
  int counter=0;
  for (Rose_STL_Container<OmpSymbol* >::iterator i= ompattribute->var_list.begin();i!=ompattribute->var_list.end();i++ )
  { //TODO handle all reduction cases
    SgInitializedName *initname = isSgInitializedName((*i)->origVar);
    SgScopeStatement *varscope = isSgScopeStatement(ASTtools::get_scope(isSgNode((*i)->origVar)));
    // scope of function parameters is weird, special handling
    if (((currentscope==varscope) ||\
         (isSgFunctionParameterList(initname->get_parent())))
        &&
         (((*i)->ompType==e_shared)||((*i)->ompType==e_firstprivate)\
         ||((*i)->ompType==e_lastprivate)||((*i)->ompType==e_reduction_plus)) )
    {
      SgAddOp * addop= buildAddOp(buildVarRefExp(name_ompc_argv,bb1), buildIntVal(counter)); //pointertype or arraytype here?
      SgPointerDerefExp *lhs = buildPointerDerefExp(addop);

      SgInitializedName *rhsinitname= isSgInitializedName((*i)->origVar);
      SgVarRefExp *rhsvar = buildVarRefExp(rhsinitname,bb1);
      SgCastExp * rhs = buildCastExp( \
                 buildAddressOfOp(rhsvar), \
                pointertype,SgCastExp::e_C_style_cast);

      SgExprStatement * expstmti= buildAssignStatement(lhs,rhs);
      //cout<<"debug:"<<expstmti->unparseToString()<<endl;
      appendStatement(expstmti,bb1);
      counter ++;
    } //end if
  }//end for


  // 1.5 save the object pointer to _pp_globalojbect, which is used by the wrapper func.
  // Only do this for outlined member function
  // _pp_globalobject= (void *) this;
  SgClassDefinition * classdef;
  classdef = getEnclosingClassDefinition(isSgNode(pragDecl));
  if (classdef != NULL)
  {
    SgPointerType * vartype= new SgPointerType( SgTypeVoid::createType());

    SgScopeStatement *myscope= isSgScopeStatement(SageInterface::getGlobalScope(classdef));
  
    SgVariableSymbol * varsymbol1= isSgVariableSymbol(myscope->lookup_symbol\
           (SgName("_pp_globalobject")));
     ROSE_ASSERT(varsymbol1);
    SgVarRefExp *lhsvar = new SgVarRefExp(TRANS_FILE, varsymbol1);

    SgThisExp * thisexp = new SgThisExp (TRANS_FILE, \
                new SgClassSymbol(classdef->get_declaration()));
    SgCastExp * rhs = buildCastExp(\
           thisexp,vartype,SgCastExp::e_C_style_cast);

      SgExprStatement * expstmti= buildAssignStatement(lhsvar,rhs);
    //cout<<"debug:"<<expstmti->unparseToString()<<endl;
    appendStatement(expstmti,bb1);
  }

  //3. --- build the runtime call
  // SgFunctionRefExp: _ompc_do_parallel()
  SgType* voidtype = SgTypeVoid::createType();
   // we can afford inaccurate function type here since they are runtime calls without declarations in AST currently
  SgFunctionType *myFuncType= new SgFunctionType(voidtype,false);

  //SgExprListExp, two parameters (__ompc_func_x, __ompc_argv)
  if (classdef != NULL) 
 // for class member function, the first argument is the wrapper function instead
  {
    SgGlobal * myglobal= SageInterface::getGlobalScope(pragDecl);
    ROSE_ASSERT(myglobal != NULL);

   SgFunctionSymbol * func_symbol2;
   // find the forward declaration of the wrapper function
    SgDeclarationStatementPtrList testList = myglobal->get_declarations();
    string targetName=(outlinedFunc->get_name()).getString()+"_wrapper";
    SgFunctionDeclaration * returnFuncDef=NULL;

    for (Rose_STL_Container<SgDeclarationStatement*>::iterator i=testList.begin(); \
                i!= testList.end(); i++)
    {
      if (isSgFunctionDeclaration(*i)!=NULL){
        string itemName=(isSgFunctionDeclaration(*i)->get_name()).getString();
        if (itemName==targetName) {
           returnFuncDef=isSgFunctionDeclaration(*i);
           break;
        }
       }// end if func decl
    }// end for
    ROSE_ASSERT(returnFuncDef != NULL);
    func_symbol2= new SgFunctionSymbol(returnFuncDef);
  } 
    //func_symbol2= new SgFunctionSymbol(outlinedFunc);

  SgFunctionType * func_type2 = outlinedFunc->get_type();
 
// bb1 is not attached to AST, so pass the scope for omp parallel as a start scope!!
//  SgFunctionRefExp * func_ref_exp2 = buildFunctionRefExp(outlinedFunc->get_name(),func_type2,bb1);
  SgFunctionRefExp * func_ref_exp2 = buildFunctionRefExp(outlinedFunc->get_name(),
                          func_type2,currentscope);
  SgExprListExp * exp_list_exp = buildExprListExp();
  appendExpression(exp_list_exp,func_ref_exp2);
  
        // TODO if hasSharedVariables 2nd parameter is __ompc_argv
  if (ompattribute->wrapperCount>0)
    appendExpression(exp_list_exp, buildVarRefExp(name_ompc_argv,bb1)); 
  else
    appendExpression(exp_list_exp, buildIntVal(0));
  
  SgExprStatement * expStmt= buildFunctionCallStmt(SgName("_ompc_do_parallel"),
   myFuncType, exp_list_exp,currentscope); // same reason for not using bb1 as scope
  
  appendStatement(expStmt,bb1);
  popScopeStack();
  return  bb1;
}

//----------------------------
// insert the outlined function declaration at the same scope of 
// its parent function declaration right before it.
void OmpMidend::insertOutlinedFunction(SgPragmaDeclaration* decl, \
                        SgFunctionDeclaration *outlinedFunc)
{
  SgFunctionDeclaration *targetDecl=isSgFunctionDeclaration\
        (getEnclosingFunctionDeclaration(decl));
  ROSE_ASSERT(targetDecl !=NULL);
  
  SgMemberFunctionDeclaration* memdecl = \
        isSgMemberFunctionDeclaration(targetDecl);
  if (memdecl == NULL) { // regular function, add to global scope
    SgGlobal *globalScope= SageInterface::getGlobalScope(decl);
    ROSE_ASSERT(globalScope != NULL);
    moveUpPreprocessingInfo(isSgStatement(outlinedFunc), \
                                isSgStatement(targetDecl));

//      isSgStatement(outlinedFunc), true);
    insertStatement(targetDecl,outlinedFunc);
  } else    //class member function here, add to class scope
  { //TODO also consider the preprocessing info. here, though very rare
    SgClassDefinition * classdef = memdecl->get_class_scope();
    ROSE_ASSERT(classdef != NULL);
    classdef->append_member(outlinedFunc);
  }

}

//----------------------------
// replace the pragma and its enclosed code with the compiler-generated basic block
//tasks: 
//     * get the parent basic block containing the pragma
//     * remove the enclosed code(parallel region's body, etc.)
//     * replace the pragma with the runtime call sequences 
// take care of preprocessing info inside the code block also
void OmpMidend::replacePragmaBlock(SgPragmaDeclaration* pragDecl, SgBasicBlock *bb1)
{
  SgStatement *targetBB = isSgStatement(pragDecl->get_parent());
  ROSE_ASSERT(targetBB !=NULL);
 
  SgStatement * oldblock= getNextStatement(pragDecl); 
/*  LowLevelRewrite::remove(oldblock);
  moveUpPreprocessingInfo(bb1,pragDecl);
  LowLevelRewrite::remove(pragDecl);
*/
   insertStatement(oldblock,bb1,true);
//   moveUpPreprocessingInfo(bb1,oldblock);// not correct, should copy it to the outlined function, not the original place!!
   moveUpPreprocessingInfo(bb1,pragDecl);
   LowLevelRewrite::remove(oldblock);
   LowLevelRewrite::remove(pragDecl);
}


//----------------------------
// generate function def from the statement right after the pragma, insert it into func.
// tasks:
//    get the immediate statement into a basic block.
//    variable handling (dereferences for shared variables, replace private variables
//         using local copies, reduction statement ,etc
void OmpMidend::generateOutlinedFunctionDefinition(SgPragmaDeclaration* decl,\
                SgFunctionDeclaration * func, OmpAttributeOld *ompattribute)
{

   ROSE_ASSERT(decl); 
   ROSE_ASSERT(func);
   ROSE_ASSERT(func->get_definition());
   SgBasicBlock * bBlock1 = func->get_definition()->get_body();
   ROSE_ASSERT(bBlock1);

// SgInitializedNamePtrList& argList= func->get_parameterList()->get_args();
// SgInitializedName* parameter1=*(argList.begin()); //at most 1 parameter

   // 1. pointer declarations for shared variables & reduction variables from the same scope
  //            int *_pp_j2;
  //            int *_pp_j;
  //            (_pp_j2) = (((int *) (*__ompc_args + 0)));
  //            (_pp_j) = (((int *) (*((__ompc_args) + (1)))));
  // process shared variable dereferencing first because first/last private variables 
  // need the results later on. 
  addSharedVarDeclarations(decl,func,ompattribute,bBlock1);

  //  int counter=0;
  // 2. variable declarations for shared/private/reduction(plus) OpenMP variables
  //    plus dereference for pointers of shared variables
  //   e.g.     int _p_i;
  //            int _p_k_3;
  addPrivateVarDeclarations(decl,ompattribute,bBlock1);
  addThreadprivateDeclarations(decl,ompattribute,bBlock1);
   //copy the enclosed parallel region code
   SgBasicBlock *bBlock2 = buildBasicBlock();

   appendStatement(bBlock2,bBlock1);
//   bBlock2->set_parent(bBlock1);// bug 192 
   SgStatement* nextStatement = getNextStatement(decl);

// debug only!
   ASTtools::deepCopy(nextStatement,bBlock2);
   moveUpPreprocessingInfo(bBlock2,nextStatement); 
  // 3. variable substitute for shared, private, and reduction variables
//cout<<"-------------L3077"<<nextStatement->unparseToString()<<endl;
//cout<<"ompattribute:is:"<<endl;
//ompattribute->print();
  variableSubstituting(decl, ompattribute, bBlock2);

  //4. add ompc_reduction() call for reduction variables
  if(ompattribute->hasReduction) addReductionCalls(decl,ompattribute,bBlock1);
}

//----------------------------
// generate an outlined function from a basic block or a single statement following a
// pragma representing a parallel region
// It also generates a class member function
SgFunctionDeclaration* OmpMidend::generateOutlinedFunction(SgPragmaDeclaration* decl)
{
// DQ (1/4/2007): Initialized these pointer to NULL.
  SgClassDefinition * classdef        = NULL;

  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  ROSE_ASSERT(astattribute != NULL);// Do we really need this assert?
  OmpAttributeOld *  ompattribute= dynamic_cast<OmpAttributeOld* > (astattribute);
  ROSE_ASSERT(ompattribute != NULL);

  classdef = getEnclosingClassDefinition(isSgNode(decl));

// return type
  SgType* func_return_type =  SgTypeVoid::createType();
// name
  SgName func_name;
  counter++;//increase static variable to get unique function id
  stringstream ss;
  ss<<counter;
  func_name=SgName("__ompc_func_"+ss.str());
 
 // parameter list  void **__ompc_args
   SgFunctionParameterList * parameterList = buildFunctionParameterList();

   SgName var1_name = "__ompc_args";
   SgPointerType* pType1= new SgPointerType(func_return_type); 
   SgPointerType* pType2 = new SgPointerType(pType1);
   SgInitializer * var1_initializer = NULL;

  if (classdef != NULL) // member function
  {
  // current scope, should be in a class scope in C++,  
//  SgScopeStatement* cscope=decl->get_scope();
  SgBasicBlock *func_body = buildBasicBlock();

// DQ (1/4/2007): Moved the declaration of mfunc to this local scope (since it was 
 // not referenced anywhere else (also initialized it to NULL).
    SgMemberFunctionDeclaration * mfunc = NULL;
    SgFunctionType * func_type = new SgFunctionType(func_return_type, false);
    SgFunctionDefinition * func_def = new SgFunctionDefinition(TRANS_FILE,mfunc);
    mfunc = new SgMemberFunctionDeclaration(TRANS_FILE,func_name, func_type,func_def);

    func_type->set_orig_return_type(func_return_type);
    func_def->set_body(func_body);

 // DQ (1/4/2007): This is required!  Only the constructors will set the parents of input IR nodes,
 // the semantics of the access functions to set internal pointers forbids side-effects.  So since
 // the body is set using the set_body() member function the parent of the body must be explicitly 
 // set.
    func_body->set_parent(func_def);

    func_def->set_parent(mfunc); //necessary or not?
   SgInitializedName * var1_init_name = new SgInitializedName(var1_name, pType2,var1_initializer,NULL);
   var1_init_name->set_file_info(TRANS_FILE);
   parameterList->append_arg(var1_init_name);
    mfunc->set_parameterList(parameterList);
    mfunc->set_scope(classdef);  
    mfunc->set_parent(classdef);  

   //set to public for class member function since the wrapper function will access it
    //(isSgDeclarationStatement(mfunc)->get_declarationModifier ()).
    (mfunc->get_declarationModifier ()).get_accessModifier().setPublic();

  // generate and insert wrapper function for outlined member function
    generateWrapperFunction(classdef,mfunc);

    generateOutlinedFunctionDefinition(decl,mfunc,ompattribute);

    return isSgFunctionDeclaration(mfunc);
 }
  else   // regular function
  {
    SgGlobal * globalscope = SageInterface::getGlobalScope(decl);
    ROSE_ASSERT(globalscope);

    SgInitializedName* arg1 = buildInitializedName(var1_name,pType2);
    appendArg(parameterList,arg1);
    SgFunctionDeclaration * func = buildDefiningFunctionDeclaration (func_name, \
        func_return_type,parameterList,globalscope);

    ((func->get_declarationModifier()).get_storageModifier()).setStatic();

    generateOutlinedFunctionDefinition(decl,func,ompattribute);

    return func;
  }

}

//----------------------------
//tasks for transforming a parallel region
// *. generate outlined function
// *. replace parallel region with the runtime call to the outlined function
// *. add runtime system init() and cleanup() code into main function
int OmpMidend::transParallelRegion(SgPragmaDeclaration * decl)
{
  // cout<<"getting OmpAttributeOld..."<<endl;
  AstAttribute* astattribute=decl->getAttribute("OmpAttributeOld");
  ROSE_ASSERT(astattribute != NULL);// Do we really need this?
  OmpAttributeOld *  ompattribute= dynamic_cast<OmpAttributeOld* > (astattribute);
  ROSE_ASSERT(ompattribute != NULL);

  SgFunctionDeclaration *outFuncDecl= generateOutlinedFunction(decl);
  ROSE_ASSERT(outFuncDecl !=NULL);
  insertOutlinedFunction(decl, outFuncDecl);

#if 1
  SgBasicBlock *rtlCall= generateParallelRTLcall(decl, outFuncDecl,ompattribute);
  ROSE_ASSERT(rtlCall !=NULL );
  replacePragmaBlock(decl, rtlCall);
#endif
  return 0;
}
     

//----------------------------------------------------------------------
// bottomup process of AST tree & translate OpenMP constructs along the way
// Do we need more complext AstBottomUpProcessing or even AstTopDownBottomUpProcessing?
// Note: Not in use yet. 
class OmpProcessor: public AstSimpleProcessing
{
protected:
  void virtual visit(SgNode* n);
};

void OmpProcessor::visit(SgNode * node)
{
  if (isSgPragmaDeclaration(node) != NULL) 
  {
    if (OmpFrontend::recognizePragma(isSgPragmaDeclaration(node))==e_parallel)
       OmpMidend::transParallelRegion(isSgPragmaDeclaration(node));

  }
}

/************************************************************************/
int main(int argc, char* argv[])
{
  OmpFrontend ompfrontend;
  SgProject* project = frontend(argc, argv);
  ROSE_ASSERT(project !=NULL);

  //topdown traversal to build persistent OmpAttributeOld for OpenMP pragma nodes
  //ompfrontend.traverseInputFiles(project,preorder);
  ompfrontend.traverse(project,preorder);

  //cout<<"3663, frontend print()"<<endl;
  //ompfrontend.print();

  //insert #include "ompcLib.h", the interface of runtime library to the compiler
  OmpMidend::insertHeaders(project);  

  //add runtime init and cleanup code into main() function
  OmpMidend::insertRTLinitAndCleanCode(project, &ompfrontend);

  //Actual OpenMP translations
  OmpMidend::bottomupProcessing(project);

  // post processing tasks
  // bottomup translation generates variable references before variable declarations,
  fixVariableReferences(project);
//  AstPostProcessing(project);

  AstTests::runAllTests(project);

  return backend(project);
}

