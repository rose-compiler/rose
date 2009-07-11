#ifndef RTED_DS_H
#define RTED_DS_H
#include <string>

/* -----------------------------------------------------------
 * This class stores information about one Element
 * in a class definition
 * We want to know types and sizes and offsets
 * of all the variables in a class and struct
 * -----------------------------------------------------------*/
class RtedClassElement {
 public:
  const char* manglElementName;
  const char* elementType;
  SgDeclarationStatement* sgElement;
  
  RtedClassElement(const char* _elementName,
		   const char* _elementType,
		   SgDeclarationStatement* _sgElement) {
    manglElementName = _elementName;
    elementType = _elementType;
    sgElement = _sgElement;
    ROSE_ASSERT(sgElement);
  }
  virtual ~RtedClassElement(){}
};


/* -----------------------------------------------------------
 * This class stores information about all class definitions
 * We want to know types and sizes and offsets
 * of all the variables in a class and struct
 * -----------------------------------------------------------*/
class RtedClassDefinition {
 public:
  SgClassDefinition* classDef;
  const char* manglClassName;
  unsigned int nrOfElements;
  unsigned int sizeClass;
  std::vector<RtedClassElement*> elements;
  
  RtedClassDefinition(SgClassDefinition* _classDef,
		      const char* _className,
		      unsigned int _elementsSize,
		      unsigned int _sizeClass, 
		      std::vector<RtedClassElement*> _elements) {
    classDef = _classDef;
    manglClassName = _className;
    nrOfElements = _elementsSize;
    sizeClass = _sizeClass;
    elements = _elements;
    ROSE_ASSERT(classDef);
  }
  virtual ~RtedClassDefinition(){}
};


#if 1
/* -----------------------------------------------------------
 * This class stores information about all variable declarations
 * We need to know if they were initialized
 * and what type they have so we can make sure that types are correct
 * when the variable is passed to another function e.g.
 * -----------------------------------------------------------*/
class RTedVariableType {
 public:
  SgInitializedName* initName;
  SgExpression* initialized;
  RTedVariableType(SgInitializedName* init,
		   SgExpression* initExp) {
    initName=init;
    initialized=initExp;
  }
  virtual ~RTedVariableType(){}
};
#endif

/* -----------------------------------------------------------
 * This class represents a runtime array
 * it stores information about the dimension of the array
 * whether its allocated on the stack or heap
 * and the size for both dimensions
 * Finally, there is a bollean to indicate if this array is created with malloc
 * -----------------------------------------------------------*/
class RTedArray {
 public:
  bool stack;
  int dimension;
  SgInitializedName* initName;
  SgExpression* indx1;
  SgExpression* indx2;
  bool ismalloc;
  RTedArray(bool s, int dim, SgInitializedName* init, SgExpression* idx1, 
	    SgExpression* idx2, bool mal) {
          stack = s;
	  dimension = dim;
	  initName = init;
	  indx1=idx1;
	  indx2=idx2;
	  ismalloc=mal;
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


/* -----------------------------------------------------------
 * This class holds information about the arguments passed through a function call
 * This class is used to put variables on the stack before a function is called
 * Name : name of the function
 * Mangled_name : Mangled name of the function
 * stmt : The statement of the funtion call
 * VarRefExp: the variable that triggered the function call
 * arguments: Additional arguments used when "interesting functions" are called, such as
 *            strcpy, memcpy
 * -----------------------------------------------------------*/
class RtedArguments {
 public:
  SgStatement* stmt;
  // The arguments hold either a FunctionCall
  // or a stackcall, if it is a function call
  // we need f_name, if it is a stack call
  // we use d_name for the variable that is put on stack
  // but we also use the func name to avoid certain functions
  // for being checked
  std::string f_name;
  std::string f_mangled_name;
  std::string d_name;
  std::string d_mangled_name;
  SgInitializedName* initName;
  SgExpression* varRefExp;
  std::vector<SgExpression*> arguments;
  SgExpression* leftHandSideAssignmentExpr;
  SgExpression* leftHandSideAssignmentExprStr;
  RtedArguments(
		std::string ffuncn, 
		std::string fmangl,
		std::string funcn, 
		std::string mangl,
		SgExpression* var,
		SgStatement* stm,
		std::vector<SgExpression*> args,
		SgExpression* leftHandAssignStr,
		SgExpression* leftHandAssign
		) {
    ROSE_ASSERT(var);
    stmt = stm;
    f_name=ffuncn;
    f_mangled_name=fmangl;
    d_name=funcn;
    d_mangled_name=mangl;
    varRefExp=var;
    arguments = args;
    leftHandSideAssignmentExpr = leftHandAssign;
    leftHandSideAssignmentExprStr = leftHandAssignStr;
    if (isSgVarRefExp(var)) {
      initName = isSgVarRefExp(var)->get_symbol()->get_declaration();
      ROSE_ASSERT(initName);
    }
 }
  std::string toString() {
    return "func name: " + f_name + "  func mangl_name: " +f_mangled_name +
      "data name: " + d_name + "  data mangl_name: " +d_mangled_name +  " varRefExp : " + 
      varRefExp->unparseToString() + " at addr: " + RoseBin_support::ToString(varRefExp)+
      "  stmt: "+stmt->unparseToString() + " at addr: " + RoseBin_support::ToString(stmt);
  }
  virtual ~RtedArguments() {}
};


#endif
