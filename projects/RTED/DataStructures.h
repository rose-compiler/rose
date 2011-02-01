#ifndef RTED_DS_H
#define RTED_DS_H

#include <string>
#include <vector>

#include <rose.h>

#include "CppRuntimeSystem/rted_typedefs.h"


/* -----------------------------------------------------------
 * This class represents a runtime array
 * it stores information about the dimension of the array
 * whether its allocated on the stack or heap
 * and the size for both dimensions
 * Finally, there is a boolean to indicate if this array is created with malloc
 * -----------------------------------------------------------*/
struct RtedArray
{
		SgInitializedName*         initName;
		SgStatement*               surroundingStatement;
		AllocKind                  allocKind;
		SgExpression*              size;
		std::vector<SgExpression*> indices;

		RtedArray ( SgInitializedName* init,
								SgStatement* stmt,
								AllocKind _allocKind,
								SgExpression* _size = NULL
							)
		: initName(init), surroundingStatement(stmt), allocKind(_allocKind), size(_size), indices()
		{
			ROSE_ASSERT(initName && surroundingStatement && (allocKind != akUndefined));
			ROSE_ASSERT((allocKind == akStack) || size);
		}

		virtual ~RtedArray() {}

		std::vector<SgExpression*>&       getIndices()       { return indices; }
		const std::vector<SgExpression*>& getIndices() const { return indices; }

		int getDimension() const { return indices.size(); }

		std::string unparseToString() const
		{
			std::string                                  res;
			std::vector< SgExpression* >::const_iterator i = indices.begin();

			while( i != indices.end() ) {
						res += (*i) -> unparseToString();
						++i;
						if( i != indices.end() )
								res += ", ";
			}

			return res;
		}

	private:
	  RtedArray();
};



/* -----------------------------------------------------------
 * This class stores information about one Element
 * in a class definition
 * We want to know types and sizes and offsets
 * of all the variables in a class and struct
 * -----------------------------------------------------------*/
class RtedClassElement {
 public:
	std::string manglElementName;
	std::string elementType;
  SgDeclarationStatement* sgElement;

  RtedClassElement( std::string _elementName,
		                std::string _elementType,
		                SgDeclarationStatement* _sgElement
									)
	: manglElementName(_elementName), elementType(_elementType), sgElement(_sgElement)
  {
		ROSE_ASSERT(sgElement);
  }

  virtual ~RtedClassElement() {}

  virtual size_t extraArgSize() { return 0; }
	virtual RtedArray* get_array() { return NULL; }
};

class RtedClassArrayElement : public RtedClassElement {
      RtedArray* array;
  public:
      RtedClassArrayElement( std::string elementName,
														 std::string elementType,
														 SgDeclarationStatement* sgElement,
														 RtedArray* arr
						               )
			: RtedClassElement(elementName, elementType, sgElement), array(arr)
			{
				ROSE_ASSERT(array);
			}

      size_t extraArgSize() {
          // dimensionality, then each dimension
          return (array -> getDimension() + 1);
      }

			RtedArray* get_array() { return array; }
};


/* -----------------------------------------------------------
 * This class stores information about all class definitions
 * We want to know types and sizes and offsets
 * of all the variables in a class and struct
 * -----------------------------------------------------------*/
struct RtedClassDefinition
{
  SgClassDefinition*             classDef;
  std::string                    manglClassName;
  std::string                    classType;
  SgExpression*                  sizeClass;
  std::vector<RtedClassElement*> elems;

  RtedClassDefinition( SgClassDefinition* _classDef,
		                   std::string _className,
		                   std::string _classType,
                       SgExpression* _sizeClass,
		                   const std::vector<RtedClassElement*>& _elements
										 )
  : classDef(_classDef), manglClassName(_className), classType(_classType),
		sizeClass(_sizeClass), elems(_elements)
	{
    ROSE_ASSERT(classDef);
  }

	size_t nrOfElems() const
	{
		return elems.size();
	}

  virtual ~RtedClassDefinition(){}
};


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
struct RtedArguments {
  // The arguments hold either a FunctionCall
  // or a stackcall, if it is a function call
  // we need f_name, if it is a stack call
  // we use d_name for the variable that is put on stack
  // but we also use the func name to avoid certain functions
  // for being checked
	SgStatement*               stmt;
  std::string                f_name;
  std::string                f_mangled_name;
  std::string                d_name;
  std::string                d_mangled_name;
  SgInitializedName*         initName;
  SgExpression*              varRefExp;
  SgExpression*              leftHandSideAssignmentExpr;
  SgExpression*              leftHandSideAssignmentExprStr;
	SgExprListExp*             argumentList;
	SgExpressionPtrList        arguments;

  RtedArguments( SgStatement* stm,
	               const std::string& ffuncn,
		             const std::string& fmangl,
		             const std::string& funcn,
		             const std::string& mangl,
		             SgExpression* var,
		             SgExpression* leftHandAssignStr,
		             SgExpression* leftHandAssign,
		             SgExprListExp* exprList,
								 const SgExpressionPtrList& args
		           )
	: stmt(stm),
		f_name(ffuncn), f_mangled_name(fmangl), d_name(funcn), d_mangled_name(mangl),
		initName(NULL), varRefExp(var),
		leftHandSideAssignmentExpr(leftHandAssign), leftHandSideAssignmentExprStr (leftHandAssignStr),
		argumentList(exprList), arguments(args)
	{
		ROSE_ASSERT(var);

		if (isSgVarRefExp(var)) {
			initName = isSgVarRefExp(var)->get_symbol()->get_declaration();
			ROSE_ASSERT(initName);
		}
	}

	std::string toString()
	{
    return ( "func name: " + f_name + "  func mangl_name: " + f_mangled_name
		       + "data name: " + d_name + "  data mangl_name: " + d_mangled_name
					 + " varRefExp : " + varRefExp->unparseToString() + " at addr: "
					 + RoseBin_support::ToString(varRefExp)
					 + "  stmt: "+stmt->unparseToString() + " at addr: " + RoseBin_support::ToString(stmt)
					 );
  }
};


class RtedForStmtProcessed : public AstAttribute {
    public:
        static const std::string Key;


        RtedForStmtProcessed( SgExpression* _exp ) : exp( _exp ) {}


        SgExpression* get_exp() { return exp; }
        SgExpression* set_exp( SgExpression* e ) { exp = e; return exp; }


    private:
        /// This is the expression to replace when we add expressions.  E.g. if
        /// we want to add expression e, we replace exp with bitwise or of exp
        /// and e.
        SgExpression* exp;
};

#endif
