/****************************************************
 * RoseBin :: Run-time Instrumentation of source code for Error Checking
 * Author : tps
 * Date : 28Apr08
 ****************************************************/

#ifndef __RuntimeInstrumentation_
#define __RuntimeInstrumentation_

#include <rose.h>
#include <sstream>



class RuntimeInstrumentation : public AstSimpleProcessing { 
 private:
  //  typedef rose_hash::hash_map <SgVarRefExp*,std::string> varRefList_Type;
  typedef std::map <SgNode*,std::string> varRefList_Type;
  varRefList_Type varRefList;
  void insertCheck(SgVarRefExp* n,std::string desc);
  bool isRightHandSide(SgNode* n);

  SgGlobal* globalScope;

  template <class T>
    inline std::string to_string (const T& t)
    {
      std::stringstream ss;
      ss << t;
      return ss.str();
    }

  SgStatement* getSurroundingStatement(SgNode* n);

 public:
  
  RuntimeInstrumentation(){
    varRefList.clear();
  };


  /****************************************************
   * visit function for each node
   ****************************************************/
  virtual void visit(SgNode* n);

  void run(SgNode* project);



};

#endif


