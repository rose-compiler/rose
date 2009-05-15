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
  SgStatement* stmt;
  std::string name;
  std::string mangled_name;
  SgInitializedName* initName;
  SgExpression* varRefExp;
  std::vector<SgExpression*> arguments;
  RtedArguments(std::string funcn, 
		std::string mangl,
		SgExpression* var,
		SgStatement* stm,
		std::vector<SgExpression*> args) {
    ROSE_ASSERT(var);
    stmt = stm;
    name=funcn;
    mangled_name=mangl;
    varRefExp=var;
    arguments = args;
    if (isSgVarRefExp(var)) {
      initName = isSgVarRefExp(var)->get_symbol()->get_declaration();
      ROSE_ASSERT(initName);
    }
 }
  std::string toString() {
    return "name: " + name + "  mangl_name: " +mangled_name +  " varRefExp : " + 
      varRefExp->unparseToString() + " at addr: " + RoseBin_support::ToString(varRefExp)+
      "  stmt: "+stmt->unparseToString() + " at addr: " + RoseBin_support::ToString(stmt);
  }
  virtual ~RtedArguments() {}
};


#endif
