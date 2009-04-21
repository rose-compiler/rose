#ifndef RTED_DS_H
#define RTED_DS_H
#include <string>

class RTedArray {
 public:
  bool stack;
  int dimension;
  SgInitializedName* initName;
  SgExpression* indx1;
  SgExpression* indx2;
  RTedArray(bool s, int dim, SgInitializedName* init, SgExpression* idx1, SgExpression* idx2) {
	  stack = s;
	  dimension = dim;
	  initName = init;
	  indx1=idx1;
	  indx2=idx2;
	  //ROSE_ASSERT(indx1);
	  //ROSE_ASSERT(indx2);
  }
  virtual ~RTedArray() {}

  void getIndices(std::vector  <SgExpression*>& vec ) {
	  vec.push_back(indx1);
	  vec.push_back(indx2);
  }

  std::string unparseToString() {
	  std::string res = "";
	  if (indx1!=NULL)
		  res = indx1->unparseToString();
	  if (indx2!=NULL)
		res+=", "+indx2->unparseToString();
	  return res;
  }
};


class RTedFunctionCall {
 public:
  SgInitializedName* initName;
  SgVarRefExp* varRefExp;
  std::string name;
  std::string mangled_name;
  RTedFunctionCall(SgInitializedName* init, SgVarRefExp* var,
		   std::string n, std::string m_n) {
    initName=init;
    varRefExp=var;
    name=n;
    mangled_name=m_n;
  }
  virtual ~RTedFunctionCall() {}

};

class RtedArguments {
 public:
  std::string name;
  std::string mangled_name;
  SgInitializedName* initName;
  SgExpression* varRefExp;
  std::vector<SgExpression*> arguments;
  RtedArguments(std::string funcn, 
		std::string mangl,
		SgExpression* var,
		std::vector<SgExpression*> args) {
    name=funcn;
    mangled_name=mangl;
    //initName=init;
    varRefExp=var;
    arguments = args;
    ROSE_ASSERT(var);
    if (isSgVarRefExp(var)) {
      initName = isSgVarRefExp(var)->get_symbol()->get_declaration();
      ROSE_ASSERT(initName);
    }
 }
  virtual ~RtedArguments() {}
};


#endif
