/*!
 * Translation (directive lowering) support for OpenMP 3.0 C/C++
 *
 *  Like other OpenMP related work within ROSE, 
 *  all things go to the OmpSupport namespace to avoid conflicts
 * Liao, 8/10/2009
 */
#ifndef OMP_LOWERING_H
#define OMP_LOWERING_H 
namespace OmpSupport
{
  //! The type of target runtime libraries
  // We support both Omni and GCC OpenMP runtime libraries
  enum omp_rtl_enum {
    e_omni = 0,
    e_gcc,
    e_last_rtl
  };

  extern omp_rtl_enum rtl_type; 

  void commandLineProcessing(std::vector<std::string> &argvList);

  //! The top level interface
  //void lower_omp(SgProject*);
  void lower_omp(SgSourceFile*);

  //! Insert #include "xxx.h", the interface of runtime library to the compiler
  //void insertRTLHeaders(SgProject*);
  void insertRTLHeaders(SgSourceFile*);

  //! Only needed for Omni
  // int insertRTLinitAndCleanCode(SgProject* project); 


  //! A driver to traverse AST trees and invoke individual translators for OpenMP constructs
  //! Postorder is preferred. 
  class translationDriver: public AstSimpleProcessing
  { 
    protected:
      void visit(SgNode*);
  }; //translationDriver

  void transParallelRegion (SgNode*);
  // I decided to reuse the existing Outliner work instead of coding a new one
  //SgFunctionDeclaration* generateOutlinedFunction(SgNode* node);

} // end namespace OmpSupport  

#endif //OMP_LOWERING_H
