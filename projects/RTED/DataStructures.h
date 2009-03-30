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

#endif
