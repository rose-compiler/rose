// Interfaces of UPC_Translation namespace 
#ifndef ROSE_UPC_TRANSLATION_INTERFACE
#define ROSE_UPC_TRANSLATION_INTERFACE

//#include "rose.h"
#include <string>

#include <vector>

/* Support anonymous upc barrier, 
 * Should be consistent with upcr_barrier.h */
#define  UPCR_BARRIERFLAG_ANONYMOUS 1 
#define  UPCR_BARRIERVAL_ANONYMOUS (0xdeadbeef)
//#define  UPCR_BARRIERVAL_ANONYMOUS (0xFFC1A43B)

/*!
  \defgroup midendUPCTranslation UPC Translation Interfaces
  \ingroup rose_midend
  \brief This namespace contains interfaces of UPC related translation. 

  \authors Chunhua Liao (6/26/2008) Last modified (9/3/2008)
  
*/

namespace upcTranslation{

  //! Initialize translation environment . e.g. set upcc size
  void initTranslation();
  
  //! Add headers and preprocessor declarations
  void addHeadersAndPreprocessorDecls(SgGlobal* global);

  //! Insert helper function prototypes: upcrt_gcd() etc.
  void addHelperFunctionPrototypes(SgFile* sg_file);

  //! Add UPCR_BEGIN_FUNCTION() and UPCR_EXIT_FUNCTION() functions into functions
  /*!
   * Any function that intends to call any of the entry points provided by BUPC runtime.
   */
  void addUpcrBeginAndExitFunctions(SgFunctionDefinition* funcdef);
  
  //! Set output file name from .upc to .c
  void setOutputFileName(SgFile* sg_file);

  //! Linking stage processing: insert per-file alloc/init functions etc.
  void transProject(SgNode* node);

  //! A driver to traverse AST trees and invoke individual translators for UPC constructs
  //! Postorder is preferred. 
  class translationDriver: public AstSimpleProcessing
  {
    protected:
    void visit(SgNode*);
  }; //translationDriver

  //! Translate MYTHREAD to upcr_mythread()
  void transUpcMythread(SgNode *node);

  //! Translate THREADS to upcr_threads()
  void transUpcThreads(SgNode *node);

  //! Translate upc_barrier exp_opt
  void transUpcBarrier(SgNode* node);

  //! Rename main() to 'extern user_main()'
  void renameMainToUserMain(SgFunctionDeclaration* sg_func);

  //! Generate new main() function: it calls bupc_init_reentrant(&argc, &argv, &user_main);
  void generateNewMainFunction(SgFile* sg_file);

  //! Generate per-file memory allocation functions: perfile_allocs() and perfile_inits()
  
  //! Translate statically allocated variables: replace them with generic shared pointers, generate allocation and initialization functions. Return the number of variables handled. 
  /*!
   * This function implements the translation of Statically-allocated Data 
   * (shared or unshared) defined in The Berkeley UPC Runtime Specification V 3.9. 
   */
  int transStaticallyAllocatedData(SgNode* global);

  /*!
   * This function implements the translation of Statically-allocated shared Data (SSD), 
   * defined in The Berkeley UPC Runtime Specification V 3.9. It returns the number of 
   * variables handled. 
   */
  int transStaticallyAllocatedSharedData(Rose_STL_Container<SgVariableDeclaration*> ssd_list);

  /*!
   * This function implements the translation of Statically-allocated unshared Data, 
   * namely Thread-Local Data (TLD)
   * defined in The Berkeley UPC Runtime Specification V 3.9. It returns the number of 
   * variables handled. 
   */
  int transThreadLocalData(Rose_STL_Container<SgVariableDeclaration*> tld_list,SgGlobal* global);

  //! Check if a variable declaration declares a thread local datum, including 
  // unshared global and static local variables
  bool isThreadLocalDataDecl(SgVariableDeclaration* decl);

  //! Check if a variable reference is referring to a thread local data
  bool isThreadLocalDataRef(SgVarRefExp* exp);

  //! Check if a SgVarRefExp is a parameter of a function call expression named UPCR_TLD_ADDR
  // used to avoid duplicated variable substitution for shared variable accesses
  bool isParameterOfFunctionCall(const std::string &funcName, SgVarRefExp* varRef); 

  //! Check if an initializer involves addresses of other TLD data, used for generating 
  // void UPCRI_INIT_filename_xxx()
  bool hasAddressOfOtherTLD (SgInitializer* initor);

  /*!
   * Translate a variable declaration, shared or unshared. 
   *
   * For unshared variables (thread-local data)
   *
   * Modifying AST tree during translation causes some unexpected error.
   * Use int transThreadLocalData(SgNode* global) instead
   */
  //void transVariableDeclaration(SgNode* decl);

  /*! 
   * Translate a variable reference expression, mostly replace it with another expression
   * Variable substitution: 
   * for thread-local data (TLD): treat as pointer deference  
   *   *((int *) UPCR_TLD_ADDR (counter))
   */
  void transVarRefExp(SgNode* exp);
  
  //!Translate an assignment operations
  /*!
   * Cases are:
   * a)  lhs is a shared variable being written:
   *    lsscounter=99; //--> is translated into
   *    UPCR_PUT_PSHARED_VAL(_N10_lsscounter_N9_781042953_, 0, 99, 4);
   */
  void transAssignOp(SgNode* exp);

  //! Generate UPCRI_ALLOC_flename_xxx() function for a source file with 
  //global and static local shared variables as par of per-file allocation
  SgFunctionDeclaration * generateUpcriAllocFunc(SgFile* file, 
                    std::vector<SgVariableDeclaration*>global_list, 
                    std::vector<SgVariableDeclaration*>local_list,
                    SgScopeStatement* target_scope);

  //! Generate the aggregate initializer used inside UPCRI_ALLOC_shared_xxx()
  // UPCRT_STARTUP_SHALLOC (array,   40, 20, 1, 8, "A100H_R5_d"), for regular shared
  // UPCRT_STARTUP_PSHALLOC (global_counter, 4, 1, 0, 4, "R1_i"), for phaseless shared
  SgAggregateInitializer*
  generateUpcriAllocFuncInitializer(std::vector<SgVariableDeclaration*>decl_list, 
           bool phaseless, SgScopeStatement* scope); 

  //!Generate the UPCRI_INIT_filename_xxx() function for per-file initialization of
  //shared and unshared (TLD) data
  SgFunctionDeclaration* generateUpcriInitFunc(SgFile* file, 
                    std::vector<SgVariableDeclaration*>shared_list, 
                    std::vector<SgVariableDeclaration*>tld_list, 
                    SgScopeStatement* target_scope);
 
  //!Generate the initialization statements for shared variables and insert them into  UPCRI_INIT_filename_xxx() 
  void generateUpcriInitStmtForShared(std::vector<SgVariableDeclaration*>shared_list,
                       SgBasicBlock* func_body,SgScopeStatement* globalScope);

  //!Generate the initialization statements for TLD and insert them into  UPCRI_INIT_filename_xxx() 
  void generateUpcriInitStmtForTLD(std::vector<SgVariableDeclaration*>tld_list,
                       SgBasicBlock* func_body,SgScopeStatement* globalScope);

  //! Initialize UPC sizes etc. 
  void initUpcTranslation();

  //! Get size of a type, using customized UPC fundamental sizes
  size_t customsizeof(SgType* t);

  //! Get alignment of a type, using customized UPC fundamental sizes
  size_t customalignof(SgType* t);

  //! Get size of an element of an array type, return the regular size for non-array types
  size_t getElementSize(SgType* t);
 
  //! Get number of blocks of a UPC type
  size_t getNumberOfBlocks(SgType* t);

  //! Get the size of a block for a UPC shared type
  size_t getBlockBytes(SgType* t);

  //! Is a type is a UPC shared array with THREADS related dimension?
  bool hasUpcThreadsDimension(SgType* t);

  //! Generate a unique global scope name for a local variable: _name_hash(mangled_name)
  std::string generateUniqueGlobalName(SgVariableDeclaration* decl);

  //! Check if a node is a defining UPCRI_ALLOC_file_xxx() or UPCRI_INIT_file_xxx()
  bool isUpcriAllocInit(SgNode* node, bool checkAlloc);

  //! Find defining UPCRI_ALLCO_file_xxx() or UPCRI_INIT_file_xxx() from a file
  SgFunctionDeclaration* findUpcriAllocInit(SgNode* node, bool isAlloc);

  //! Translate special cases for private-to-shared pointers
  /*!
   * Special cases for private to shared pointers
   *
   * a) in global scope: must have 'extern' modifier
   *  extern shared int * ep2s_p1; // translated into
   *  extern upcr_pshared_ptr_t ep2s_p1;
   *
   * b) in local scope: must not have 'static' modifier
   *   
   *  shared int *lp2s_p;
   *  shared int *lp2s_p1=0;
   *  shared int *lp2s_p2=&global_counter2;
   *  lp2s_p1 = &global_counter2;
   *  // translated into-->
   *    // declaration without initializers
   *   upcr_pshared_ptr_t lp2s_p;
   *   upcr_pshared_ptr_t lp2s_p1;
   *   upcr_pshared_ptr_t lp2s_p2;
   *
   *   // special handling for NULL-valued initializer
   *   UPCR_SETNULL_PSHARED ((_UINT32) & lp2s_p1);
   *   lp2s_p2 = global_counter2;
   *   lp2s_p1 = global_counter2;
   */
   void transSpecialPrivate2Shared(SgVariableDeclaration* decl);

   //! Check if an expression is Null(or 0) valued
   bool isNullValued(SgExpression* exp);
//-----------------------------------------------------------------------
} // end of namespace

#endif //ROSE_UPC_TRANSLATION_INTERFACE




