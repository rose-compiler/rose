/****************************************************
 * RoseBin :: Run-time Instrumentation of source code for Error Checking
 * Author : tps
 * Date : 28Apr08
 ****************************************************/

#ifndef __RuntimeInstrumentation_
#define __RuntimeInstrumentation_

#include <rose.h>


class RuntimeInstrumentation : public AstSimpleProcessing { 
 private:

 public:
  
  RuntimeInstrumentation(){
  };


  /****************************************************
   * visit function for each node
   ****************************************************/
  virtual void visit(SgNode* n);

  void run(SgNode* project);



};

#endif


