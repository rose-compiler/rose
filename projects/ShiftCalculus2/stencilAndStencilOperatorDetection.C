
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// #include "shiftCalculusCompiler.h"
#include "stencilAndStencilOperatorDetection.h"

#include "dslSupport.h"

using namespace std;

using namespace DSL_Support;

Detection_InheritedAttribute::Detection_InheritedAttribute()
   {
     isStencilDeclaration = false;
     isStencilOperatorDeclaration = false;
     isShiftExpression    = false;

  // DQ (2/18/2015): Added to support debugging by restricting traversals to main function.
     inFunction = false;
     inDSL_Function = false;
   }

Detection_InheritedAttribute::Detection_InheritedAttribute( const Detection_InheritedAttribute & X )
   {
     isStencilDeclaration         = X.isStencilDeclaration;
     isStencilOperatorDeclaration = X.isStencilOperatorDeclaration;
     isShiftExpression            = X.isShiftExpression;

  // DQ (2/18/2015): Added to support debugging by restricting traversals to main function.
     inFunction     = X.inFunction;
     inDSL_Function = X.inDSL_Function;
   }

void
Detection_InheritedAttribute::set_StencilDeclaration(bool value)
   {
     isStencilDeclaration = value;
   }

bool
Detection_InheritedAttribute::get_StencilDeclaration()
   {
     return isStencilDeclaration;
   }

void
Detection_InheritedAttribute::set_StencilOperatorDeclaration(bool value)
   {
     isStencilOperatorDeclaration = value;
   }

bool
Detection_InheritedAttribute::get_StencilOperatorDeclaration()
   {
     return isStencilOperatorDeclaration;
   }

void
Detection_InheritedAttribute::set_StencilOperatorRef(bool value)
   {
     isStencilOperatorRef = value;
   }

bool
Detection_InheritedAttribute::get_StencilOperatorRef()
   {
     return isStencilOperatorRef;
   }



#if 1
// DQ (2/16/2015): We want to support saving the AST node, but this default constructor is required by the AstProcessing.h file.
Detection_SynthesizedAttribute::Detection_SynthesizedAttribute()
   : node(NULL)
   {
     stencilOperatorTransformed = false;
   }
#endif

Detection_SynthesizedAttribute::Detection_SynthesizedAttribute( SgNode* astNode )
   : node(astNode)
   {
     stencilOperatorTransformed = false;
   }

Detection_SynthesizedAttribute::Detection_SynthesizedAttribute( const Detection_SynthesizedAttribute & X )
   : node(X.node)
   {
     stencilOperatorTransformed = X.stencilOperatorTransformed;

     boxVarRefExpList         = X.boxVarRefExpList;
     rectMDArrayVarRefExpList = X.rectMDArrayVarRefExpList;
     stencilVarRefExpList     = X.stencilVarRefExpList;

  // copy the accumulated children
     dslChildren = X.dslChildren;
   }

void
Detection_SynthesizedAttribute::set_stencilOperatorTransformed(bool value)
   {
     stencilOperatorTransformed = value;
   }

bool
Detection_SynthesizedAttribute::get_stencilOperatorTransformed()
   {
     return stencilOperatorTransformed;
   }



DetectionTraversal::DetectionTraversal()
   {
   }

std::vector<SgInitializedName*> & 
DetectionTraversal::get_stencilInputInitializedNameList()
   {
     return stencilInputInitializedNameList;
   }

std::vector<SgInitializedName*> & 
DetectionTraversal::get_stencilOperatorInitializedNameList()
   {
     return stencilOperatorInitializedNameList;
   }

std::vector<SgFunctionCallExp*> & 
DetectionTraversal::get_stencilOperatorFunctionCallList()
   {
     return stencilOperatorFunctionCallList;
   }

void
DetectionTraversal::display()
   {
#if 1
     printf ("In DetectionTraversal::display(): stencilInputInitializedNameList.size() = %zu \n",stencilInputInitializedNameList.size());
#endif
#if 1
     printf ("In DetectionTraversal::display(): stencilInputInitializedNameList.size() = %zu \n",stencilInputInitializedNameList.size());
     for (size_t i = 0; i < stencilInputInitializedNameList.size(); i++)
        {
          printf ("   --- stencilInputInitializedNameList[%zu]->get_name() = %s \n",i,stencilInputInitializedNameList[i]->get_name().str());
        }
#endif
#if 1
     printf ("In DetectionTraversal::display(): stencilInitializedNameList.size() = %zu \n",stencilInitializedNameList.size());
     for (size_t i = 0; i < stencilInitializedNameList.size(); i++)
        {
          printf ("   --- stencilInitializedNameList[%zu]->get_name() = %s \n",i,stencilInitializedNameList[i]->get_name().str());
        }
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }

DSL_Attribute::DSL_Attribute() {}

DSL_Attribute::~DSL_Attribute() {}

string
DSL_Attribute::toString()
   {
     return "DSL_Attribute";
   }

string 
DSL_Attribute::additionalNodeOptions()
   {
  // Base class additionalNodeOptions() function should be an error to be called.

     printf ("Error: base class function called: needs an implementation on the derived function \n");
     ROSE_ASSERT(false);

  // return "fillcolor=\"green\",style=filled";
     return "";
   }

string
DSL_Attribute::get_name()
   {
     return "DSL-child";
   }

string
DSL_Attribute::get_color()
   {
#if 0
  // Base class get_color() function should be an error to be called.

     printf ("Error: base class function called: needs an implementation on the derived function \n");
     ROSE_ASSERT(false);

     return "";
#else
     return "blue";
#endif
   }


vector<AstAttribute::AttributeEdgeInfo>
DSL_Attribute::additionalEdgeInfo()
   {
     vector<AstAttribute::AttributeEdgeInfo> v;

     vector<SgNode*>::iterator i = dslChildren.begin();
     while ( i != dslChildren.end() )
        {
          ROSE_ASSERT(currentNode != NULL);
#if 0
          printf ("Adding an edge from %p = %s to %p = %s \n",currentNode,currentNode->class_name().c_str(),*i,(*i)->class_name().c_str());
#endif
          string name  = get_name();
          string color = get_color();

       // string options = " arrowsize=7.0 style=\"setlinewidth(7)\" constraint=false color=" + color + " ";
          string options = " arrowsize=4.0 style=\"setlinewidth(7)\" constraint=true color=" + color + " ";

          AstAttribute::AttributeEdgeInfo additional_edge ( (SgNode*) currentNode,*i,name,options);

          v.push_back(additional_edge);

          i++;
        }

     return v;
   }


DSL_ValueAttribute::DSL_ValueAttribute() {}

DSL_ValueAttribute::~DSL_ValueAttribute() {}

string
DSL_ValueAttribute::toString()
   {
     return "DSL_ValueAttribute";
   }

string 
DSL_ValueAttribute::additionalNodeOptions()
   {
  // Base class additionalNodeOptions() function should be an error to be called.

     printf ("Error: base class function called: needs an implementation on the derived function \n");
     ROSE_ASSERT(false);

     return "";
   }






Point_Attribute::Point_Attribute() {}

Point_Attribute::~Point_Attribute() {}

string
Point_Attribute::toString()
   {
     return "Point_Attribute";
   }

string 
Point_Attribute::additionalNodeOptions()
   {
     return "fillcolor=\"orange\",style=filled";
   }


PointValue_Attribute::PointValue_Attribute() {}

PointValue_Attribute::~PointValue_Attribute() {}

string
PointValue_Attribute::toString()
   {
     return "PointValue_Attribute";
   }

string 
PointValue_Attribute::additionalNodeOptions()
   {
      return "";
   }




RectMDArray_Attribute::RectMDArray_Attribute() {}

RectMDArray_Attribute::~RectMDArray_Attribute() {}

string
RectMDArray_Attribute::toString()
   {
     return "RectMDArray_Attribute";
   }

string 
RectMDArray_Attribute::additionalNodeOptions()
   {
     return "fillcolor=\"purple\",style=filled";
   }


RectMDArrayValue_Attribute::RectMDArrayValue_Attribute() {}

RectMDArrayValue_Attribute::~RectMDArrayValue_Attribute() {}

string
RectMDArrayValue_Attribute::toString()
   {
     return "RectMDArrayValue_Attribute";
   }

string 
RectMDArrayValue_Attribute::additionalNodeOptions()
   {
     return "";
   }


Stencil_Attribute::Stencil_Attribute() {}

Stencil_Attribute::~Stencil_Attribute() {}

string
Stencil_Attribute::toString()
   {
     return "Stencil_Attribute";
   }

string 
Stencil_Attribute::additionalNodeOptions()
   {
     return "fillcolor=\"red\",style=filled";
   }


Array_Attribute::Array_Attribute() {}

Array_Attribute::~Array_Attribute() {}

string
Array_Attribute::toString()
   {
     return "Array_Attribute";
   }

string 
Array_Attribute::additionalNodeOptions()
   {
     return "fillcolor=\"green\",style=filled";
   }


Box_Attribute::Box_Attribute() {}

Box_Attribute::~Box_Attribute() {}

string
Box_Attribute::toString()
   {
     return "Box_Attribute";
   }

string 
Box_Attribute::additionalNodeOptions()
   {
     return "fillcolor=\"aquamarine\",style=filled";
   }

BoxValue_Attribute::BoxValue_Attribute() {}

BoxValue_Attribute::~BoxValue_Attribute() {}

string
BoxValue_Attribute::toString()
   {
     return "BoxValue_Attribute";
   }

string 
BoxValue_Attribute::additionalNodeOptions()
   {
     return "";
   }





StencilUpdate_Attribute::StencilUpdate_Attribute() {}

StencilUpdate_Attribute::~StencilUpdate_Attribute() {}

string
StencilUpdate_Attribute::toString()
   {
     return "StencilUpdate_Attribute";
   }

string 
StencilUpdate_Attribute::additionalNodeOptions()
   {
     return "shape=hexagon,fillcolor=\"red\",style=filled";
   }


OperatorCarot_Attribute::OperatorCarot_Attribute() {}

OperatorCarot_Attribute::~OperatorCarot_Attribute() {}

string
OperatorCarot_Attribute::toString()
   {
     return "OperatorCarot_Attribute";
   }

string 
OperatorCarot_Attribute::additionalNodeOptions()
   {
     return "shape=pentagon,fillcolor=\"lightgreen\",style=filled";
   }


PointOperatorMultiply_Attribute::PointOperatorMultiply_Attribute() {}

PointOperatorMultiply_Attribute::~PointOperatorMultiply_Attribute() {}

string
PointOperatorMultiply_Attribute::toString()
   {
     return "OperatorMultiply_Attribute";
   }

string 
PointOperatorMultiply_Attribute::additionalNodeOptions()
   {
     return "shape=hexagon,fillcolor=\"turquoise2\",style=filled";
   }


ScalarShiftOperatorMultiply_Attribute::ScalarShiftOperatorMultiply_Attribute() {}

ScalarShiftOperatorMultiply_Attribute::~ScalarShiftOperatorMultiply_Attribute() {}

string
ScalarShiftOperatorMultiply_Attribute::toString()
   {
     return "ScalarShiftOperatorMultiply_Attribute";
   }

string 
ScalarShiftOperatorMultiply_Attribute::additionalNodeOptions()
   {
     return "shape=hexagon,fillcolor=\"turquoise4\",style=filled";
   }

PointShiftOperatorMultiply_Attribute::PointShiftOperatorMultiply_Attribute() {}

PointShiftOperatorMultiply_Attribute::~PointShiftOperatorMultiply_Attribute() {}

string
PointShiftOperatorMultiply_Attribute::toString()
   {
     return "PointShiftOperatorMultiply_Attribute";
   }

string 
PointShiftOperatorMultiply_Attribute::additionalNodeOptions()
   {
     return "shape=hexagon,fillcolor=\"turquoise3\",style=filled";
   }


StencilApplyOperator_Attribute::StencilApplyOperator_Attribute() {}

StencilApplyOperator_Attribute::~StencilApplyOperator_Attribute() {}

string
StencilApplyOperator_Attribute::toString()
   {
     return "StencilApplyOperator_Attribute";
   }

string 
StencilApplyOperator_Attribute::additionalNodeOptions()
   {
     return "shape=hexagon,fillcolor=\"violetred\",style=filled";
   }

OperatorZero_Attribute::OperatorZero_Attribute() {}

OperatorZero_Attribute::~OperatorZero_Attribute() {}

string
OperatorZero_Attribute::toString()
   {
     return "OperatorZero_Attribute";
   }

string 
OperatorZero_Attribute::additionalNodeOptions()
   {
     return "shape=rectangle,fillcolor=\"orange\",style=filled";
   }

OperatorOnes_Attribute::OperatorOnes_Attribute() {}

OperatorOnes_Attribute::~OperatorOnes_Attribute() {}

string
OperatorOnes_Attribute::toString()
   {
     return "OperatorOnes_Attribute";
   }

string 
OperatorOnes_Attribute::additionalNodeOptions()
   {
     return "shape=rectangle,fillcolor=\"orange\",style=filled";
   }

OperatorUnit_Attribute::OperatorUnit_Attribute() {}

OperatorUnit_Attribute::~OperatorUnit_Attribute() {}

string
OperatorUnit_Attribute::toString()
   {
     return "OperatorUnit_Attribute";
   }

string 
OperatorUnit_Attribute::additionalNodeOptions()
   {
     return "shape=rectangle,fillcolor=\"orange\",style=filled";
   }

ForLoopOperator_Attribute::ForLoopOperator_Attribute() {}

ForLoopOperator_Attribute::~ForLoopOperator_Attribute() {}

string
ForLoopOperator_Attribute::toString()
   {
     return "ForLoopOperator_Attribute";
   }

string 
ForLoopOperator_Attribute::additionalNodeOptions()
   {
     return "shape=doubleoctagon,fillcolor=\"red\",style=filled";
   }

GrowOperator_Attribute::GrowOperator_Attribute() {}

GrowOperator_Attribute::~GrowOperator_Attribute() {}

string
GrowOperator_Attribute::toString()
   {
     return "GrowOperator_Attribute";
   }

string 
GrowOperator_Attribute::additionalNodeOptions()
   {
     return "shape=rectangle,fillcolor=\"aquamarine\",style=filled";
   }

OperatorShiftVec_Attribute::OperatorShiftVec_Attribute() {}

OperatorShiftVec_Attribute::~OperatorShiftVec_Attribute() {}

string
OperatorShiftVec_Attribute::toString()
   {
     return "OperatorShiftVec_Attribute";
   }

string 
OperatorShiftVec_Attribute::additionalNodeOptions()
   {
     return "shape=rectangle,fillcolor=\"green\",style=filled";
   }













StencilVariableRefExp_Attribute::StencilVariableRefExp_Attribute() {}

StencilVariableRefExp_Attribute::~StencilVariableRefExp_Attribute() {}

string
StencilVariableRefExp_Attribute::toString()
   {
     return "StencilVariableRefExp_Attribute";
   }

string 
StencilVariableRefExp_Attribute::additionalNodeOptions()
   {
     return "shape=diamond,fillcolor=\"red\",style=filled";
   }


PointVariableRefExp_Attribute::PointVariableRefExp_Attribute() {}

PointVariableRefExp_Attribute::~PointVariableRefExp_Attribute() {}

string
PointVariableRefExp_Attribute::toString()
   {
     return "PointVariableRefExp_Attribute";
   }

string 
PointVariableRefExp_Attribute::additionalNodeOptions()
   {
     return "shape=diamond,fillcolor=\"orange\",style=filled";
   }


ArrayVariableRefExp_Attribute::ArrayVariableRefExp_Attribute() {}

ArrayVariableRefExp_Attribute::~ArrayVariableRefExp_Attribute() {}

string
ArrayVariableRefExp_Attribute::toString()
   {
     return "PointVariableRefExp_Attribute";
   }

string 
ArrayVariableRefExp_Attribute::additionalNodeOptions()
   {
     return "shape=diamond,fillcolor=\"green\",style=filled";
   }


RectMDArrayVariableRefExp_Attribute::RectMDArrayVariableRefExp_Attribute() {}

RectMDArrayVariableRefExp_Attribute::~RectMDArrayVariableRefExp_Attribute() {}

string
RectMDArrayVariableRefExp_Attribute::toString()
   {
     return "RectMDArrayVariableRefExp_Attribute";
   }

string 
RectMDArrayVariableRefExp_Attribute::additionalNodeOptions()
   {
  // return "";
     return "shape=diamond,fillcolor=\"purple\",style=filled";
   }

BoxVariableRefExp_Attribute::BoxVariableRefExp_Attribute() {}

BoxVariableRefExp_Attribute::~BoxVariableRefExp_Attribute() {}

string
BoxVariableRefExp_Attribute::toString()
   {
     return "BoxVariableRefExp_Attribute";
   }

string 
BoxVariableRefExp_Attribute::additionalNodeOptions()
   {
     return "shape=diamond,fillcolor=\"aquamarine\",style=filled";
   }


ConstantExpression_Attribute::ConstantExpression_Attribute() {}

ConstantExpression_Attribute::~ConstantExpression_Attribute() {}

string
ConstantExpression_Attribute::toString()
   {
     return "ConstantExpression_Attribute";
   }

string 
ConstantExpression_Attribute::additionalNodeOptions()
   {
     return "shape=triangle,fillcolor=\"bisque2\",style=filled";
   }


IntegerValue_Attribute::IntegerValue_Attribute() {}

IntegerValue_Attribute::~IntegerValue_Attribute() {}

string
IntegerValue_Attribute::toString()
   {
     return "IntegerValue_Attribute";
   }

string 
IntegerValue_Attribute::additionalNodeOptions()
   {
  // return "shape=triangle,fillcolor=\"bisque2\",style=filled";
     return "";
   }

DoubleValue_Attribute::DoubleValue_Attribute() {}

DoubleValue_Attribute::~DoubleValue_Attribute() {}

string
DoubleValue_Attribute::toString()
   {
     return "DoubleValue_Attribute";
   }

string 
DoubleValue_Attribute::additionalNodeOptions()
   {
  // return "shape=triangle,fillcolor=\"bisque2\",style=filled";
     return "";
   }

ShiftValue_Attribute::ShiftValue_Attribute() {}

ShiftValue_Attribute::~ShiftValue_Attribute() {}

string
ShiftValue_Attribute::toString()
   {
     return "ShiftValue_Attribute";
   }

string 
ShiftValue_Attribute::additionalNodeOptions()
   {
  // return "shape=triangle,fillcolor=\"bisque2\",style=filled";
     return "";
   }


ArrayValue_Attribute::ArrayValue_Attribute() {}

ArrayValue_Attribute::~ArrayValue_Attribute() {}

string
ArrayValue_Attribute::toString()
   {
     return "ArrayValue_Attribute";
   }

string 
ArrayValue_Attribute::additionalNodeOptions()
   {
  // return "shape=triangle,fillcolor=\"bisque2\",style=filled";
     return "";
   }

StencilValue_Attribute::StencilValue_Attribute() {}

StencilValue_Attribute::~StencilValue_Attribute() {}

string
StencilValue_Attribute::toString()
   {
     return "StencilValue_Attribute";
   }

string 
StencilValue_Attribute::additionalNodeOptions()
   {
  // return "shape=triangle,fillcolor=\"bisque2\",style=filled";
     return "";
   }





#if 0
// Attribute names (need better names)
const string DetectionTraversal::scalar_multiply_name = "Scalar OperatorMultiply operator*";
const string DetectionTraversal::OperatorZero       = "OperatorZero";
const string DetectionTraversal::OperatorOnes       = "OperatorOnes";
const string DetectionTraversal::OperatorUnit       = "OperatorUnit";
const string DetectionTraversal::ForLoopOperator    = "ForLoopOperator";
const string DetectionTraversal::GrowOperator       = "GrowOperator";
const string DetectionTraversal::ConstantExpression = "ConstantExpression";
const string DetectionTraversal::OperatorShiftVec   = "OperatorShiftVec";
#else
const string dsl_type_string = "dsl_type";

// Names used for types of operators
// const string DetectionTraversal::scalar_multiply_name = dsl_type_string;
const string DetectionTraversal::PointOperatorMultiply       = dsl_type_string;
const string DetectionTraversal::ScalarShiftOperatorMultiply = dsl_type_string;
const string DetectionTraversal::PointShiftOperatorMultiply  = dsl_type_string;
const string DetectionTraversal::OperatorZero         = dsl_type_string;
const string DetectionTraversal::OperatorOnes         = dsl_type_string;
const string DetectionTraversal::OperatorUnit         = dsl_type_string;
const string DetectionTraversal::ForLoopOperator      = dsl_type_string;
const string DetectionTraversal::GrowOperator         = dsl_type_string;
const string DetectionTraversal::ConstantExpression   = dsl_type_string;
const string DetectionTraversal::OperatorShiftVec     = dsl_type_string;

// Names used for types of variables
const string DetectionTraversal::PointVariable        = dsl_type_string;
const string DetectionTraversal::BoxVariable          = dsl_type_string;
const string DetectionTraversal::ShiftVariable        = dsl_type_string;
const string DetectionTraversal::ArrayVariable        = dsl_type_string;
const string DetectionTraversal::StencilVariable      = dsl_type_string;
const string DetectionTraversal::RectMDArrayVariable  = dsl_type_string;
#endif

void
DetectionTraversal::findAndAddAttributeToConstantExpression ( SgFunctionCallExp* functionCallExp )
   {
  // This DSL operator takes a scale, so we want to locate it in the argument list.
  // This might be better to do in the evaluateSynthesizedAttribute() function.
     SgExprListExp* exprListExp = functionCallExp->get_args();
     ROSE_ASSERT(exprListExp != NULL);

  // SgExpressionPtrList & get_expressions();
     SgExpressionPtrList & expressions = exprListExp->get_expressions();

  // ROSE_ASSERT(expressions.size() == 1);
#if 0
     printf ("WARNING: In findAndAddAttributeToConstantExpression(): expressions.size() = %zu \n",expressions.size());
#endif
     ROSE_ASSERT(expressions.size() >= 1);

     for (SgExpressionPtrList::iterator i = expressions.begin(); i != expressions.end(); i++)
        {
          SgExpression* scalarExpression = *i;
          ROSE_ASSERT(scalarExpression != NULL);
#if 0
          printf ("scalarExpression = %p = %s \n",scalarExpression,scalarExpression->class_name().c_str());
#endif
          SgValueExp*  valueExp  = isSgValueExp(scalarExpression);
          SgVarRefExp* varRefExp = isSgVarRefExp(scalarExpression);
          SgUnaryOp*   unaryOp   = isSgUnaryOp(scalarExpression);

       // Output warning if this might not be an expression can be can evaluated to be a constant at compile time.
       // We will do the constant folding a bit later to verify that these are evaluatable to constants.
          if (valueExp == NULL)
             {
               printf ("WARNING: this should at least be an expression that can be evaluated to be a constant \n");
             }

       // This needs to be made more general logic, OK for now.
          if (valueExp != NULL || varRefExp != NULL || unaryOp != NULL)
             {
               ConstantExpression_Attribute* dslAttribute = new ConstantExpression_Attribute();
#if 0
               printf ("Adding (ConstantExpression) dslAttribute2 = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               scalarExpression->addNewAttribute(ConstantExpression,dslAttribute);
             }
        }
   }



void
checkAndResetToMakeConsistantCompilerGenerated ( SgInitializedName* initializedName )
   {
  // DQ (2/15/2015): This makes up for a bug in the ROSE AST (to be fixed later).
  // It seems that the compiler generated mode is not set uniformally between the 
  // starting and the ending source position construct.

     ROSE_ASSERT(initializedName->get_startOfConstruct() != NULL);
     bool isCompilerGenerated_result = initializedName->get_startOfConstruct()->isCompilerGenerated();
     ROSE_ASSERT(initializedName->get_endOfConstruct() != NULL);
     if (isCompilerGenerated_result != initializedName->get_endOfConstruct()->isCompilerGenerated())
        {
#if 0
          initializedName->get_endOfConstruct()->display("In DetectionTraversal::evaluateInheritedAttribute(): error: startOfConstruct()->isCompilerGenerated() != endOfConstruct()->isCompilerGenerated(): debug");
#endif
          initializedName->get_endOfConstruct()->setCompilerGenerated();
        }
     ROSE_ASSERT(isCompilerGenerated_result == initializedName->get_endOfConstruct()->isCompilerGenerated());
   }


Detection_InheritedAttribute
DetectionTraversal::evaluateInheritedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute )
   {
#if 1
     printf ("In DetectionTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

  // DQ (2/18/2015): Added to support debugging by restricting traversals to main function.
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
  // if (functionDeclaration != NULL && functionDeclaration->get_name() == "main")
     if (functionDeclaration != NULL)
        {
          inheritedAttribute.inFunction = true;

       // We could make this a list of acceptable function names as an alternative.
          if (functionDeclaration->get_name() == "main")
             {
               inheritedAttribute.inDSL_Function = true;
#if 1
               printf ("Calling constant folding on the function containing DSL specifications: root is functionDeclaration = %s \n",functionDeclaration->get_name().str());
               ConstantFolding::constantFoldingOptimization(astNode,false);
               printf ("DONE: Calling constant folding on the function containing DSL specifications \n");
#if 0
               printf ("Exiting as a test after calling ConstantFolding::constantFoldingOptimization() \n");
               ROSE_ASSERT(false);
#endif
#endif
             }
        }

     if (inheritedAttribute.inFunction == true)
        {
          if (inheritedAttribute.inDSL_Function == false)
             {
#if 0
               printf ("Found IR node in non DSL function: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
               return Detection_InheritedAttribute(inheritedAttribute);
             }
            else
             {
#if 0
               printf ("Found IR node in DSL function: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
             }
        }
       else
        {
#if 0
          printf ("Found IR node in non function: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
        }

  // Detection of stencil declaration and stencil operator.
  // Where the stencil specification is using std::vectors as parameters to the constructor, we have to first
  // find the stencil declaration and read the associated SgVarRefExp to get the variable names used.  
  // Then a finite state machine can be constructed for each of the input variables so that we can 
  // interpret the state when the stencil operator is constructed.
     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
     if (variableDeclaration != NULL)
        {
       // Get the SgInitializedName from the SgVariableDeclaration.
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);
#if 0
          printf ("In DetectionTraversal::evaluateInheritedAttribute(): case SgVariableDeclaration: initializedName->get_type() = %p = %s \n",initializedName->get_type(),initializedName->get_type()->class_name().c_str());
#endif
       // bool foundStencilVariable = false;

          SgType* base_type = initializedName->get_type()->findBaseType();
          ROSE_ASSERT(base_type != NULL);

       // SgClassType* classType = isSgClassType(initializedName->get_type());
          SgClassType* classType = isSgClassType(base_type);

          if (classType != NULL)
             {
#if 0
               printf ("In DetectionTraversal::evaluateInheritedAttribute(): case SgClassType: class name = %s \n",classType->get_name().str());
#endif
            // Check if this is associated with a template instantiation.
               SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(classType->get_declaration());
               if (templateInstantiationDecl != NULL)
                  {
#if 0
                    printf ("case SgTemplateInstaiationDecl: class name = %s \n",classType->get_name().str());
                    printf ("case SgTemplateInstaiationDecl: templateInstantiationDecl->get_templateName() = %s \n",templateInstantiationDecl->get_templateName().str());
#endif
                 // inheritedAttribute.set_StencilDeclaration(templateInstantiationDecl->get_templateName() == "Stencil");
                 // inheritedAttribute.set_StencilOperatorDeclaration(templateInstantiationDecl->get_templateName() == "StencilOperator");

                    if (templateInstantiationDecl->get_templateName() == "Stencil")
                       {
                      // DQ (2/8/2015): Ignore compiler generated IR nodes (from template instantiations, etc.).
                      // Note that simpleCNS.cpp generates one of these from it's use of the tuple template and associated template instantations.

                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                           // Save the SgInitializedName associated with the stencil.
                              stencilInitializedNameList.push_back(initializedName);
                              inheritedAttribute.set_StencilDeclaration(true);
                           // foundStencilVariable = true;
#if 0
                              printf ("Detected Stencil<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
#if 0
                              initializedName->get_file_info()->display("In DetectionTraversal::evaluateInheritedAttribute(): initializedName : debug");
#endif
                              Stencil_Attribute* dslAttribute = new Stencil_Attribute();
#if 0
                              printf ("Adding (Stencil) dslAttribute = %p \n",dslAttribute);
#endif
                              ROSE_ASSERT(dslAttribute != NULL);

                           // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                              initializedName->addNewAttribute(StencilVariable,dslAttribute);
                            }
                       }

                 // Detect variables of type RectMDArray
                    if (templateInstantiationDecl->get_templateName() == "RectMDArray")
                       {
                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                              RectMDArrayInitializedNameList.push_back(initializedName);
#if 0
                              printf ("Detected RectMDArray<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
                              RectMDArray_Attribute* dslAttribute = new RectMDArray_Attribute();
                              printf ("Adding (RectMDArray) dslAttribute = %p \n",dslAttribute);
                              ROSE_ASSERT(dslAttribute != NULL);

                              initializedName->addNewAttribute(RectMDArrayVariable,dslAttribute);
                            }
                       }

                 // Detect variables of type array
                    if (templateInstantiationDecl->get_templateName() == "array")
                       {
                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                              arrayInitializedNameList.push_back(initializedName);
#if 0
                              printf ("Detected C++11 array<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
                              Array_Attribute* dslAttribute = new Array_Attribute();
                              printf ("Adding (array) dslAttribute = %p \n",dslAttribute);
                              ROSE_ASSERT(dslAttribute != NULL);

                              initializedName->addNewAttribute(ArrayVariable,dslAttribute);
                            }
                       }
                  }

               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               if (classDeclaration != NULL)
                  {
                    if (classDeclaration->get_name() == "Point")
                       {
                      // Save the SgInitializedName associated with the Point type.
#if 0
                         printf ("Detected Point<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                              pointInitializedNameList.push_back(initializedName);

                              Point_Attribute* dslAttribute = new Point_Attribute();
                              printf ("Adding (Point) dslAttribute = %p \n",dslAttribute);
                              ROSE_ASSERT(dslAttribute != NULL);

                           // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                              initializedName->addNewAttribute(PointVariable,dslAttribute);
                            }
                       }

                    if (classDeclaration->get_name() == "Box")
                       {
                      // Save the SgInitializedName associated with the Point type.
#if 0
                         printf ("Detected Point<> typed variable: initializedName = %p name = %s \n",initializedName,initializedName->get_name().str());
#endif
                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                              boxInitializedNameList.push_back(initializedName);

                              Box_Attribute* dslAttribute = new Box_Attribute();
                              printf ("Adding (Box) dslAttribute = %p \n",dslAttribute);
                              ROSE_ASSERT(dslAttribute != NULL);

                           // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                              initializedName->addNewAttribute(BoxVariable,dslAttribute);
                            }
                       }
                  }
             }

#if 0
          printf ("case inheritedAttribute.get_StencilDeclaration() == true: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
#if 0
          printf ("Found a stencil declaration \n");
          variableDeclaration->get_file_info()->display("location of stencil declaration: debug");
#endif
        }

  // Find the invocation of the stencil operator (via a SgFunctionCallExp).
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
       // Here we make assumptions on how the stencil is specified in the DSL.
          SgDotExp* dotExp = isSgDotExp(functionCallExp->get_function());
          if (dotExp != NULL)
             {
               SgVarRefExp* varRefExp = isSgVarRefExp(dotExp->get_lhs_operand());
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());

               bool stencilOperatorFound      = false;
            // bool stencilApplyOperatorFound = false;
               bool pointOperatorFound        = false;
               bool growOperatorFound         = false;

               if (varRefExp != NULL && memberFunctionRefExp != NULL)
                  {
                 // if (initializedNameList.find(initializedName) != initializedNameList.end())
                    SgVariableSymbol* variableSymbol = isSgVariableSymbol(varRefExp->get_symbol());
                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
#if 0
                    printf ("memberFunctionSymbol->get_name()  = %s \n",memberFunctionSymbol->get_name().str());
                    printf ("initializedNameList.size()        = %zu \n",initializedNameList.size());
                    printf ("stencilInitializedNameList.size() = %zu \n",stencilInitializedNameList.size());
#endif
                 // Check that this is the correct member function on the correctly typed variable reference.
                    stencilOperatorFound = (find(stencilInitializedNameList.begin(),stencilInitializedNameList.end(),initializedName) != stencilInitializedNameList.end()) && (memberFunctionSymbol->get_name() == "operator+=");
                    pointOperatorFound   = (find(pointInitializedNameList.begin(),pointInitializedNameList.end(),initializedName) != pointInitializedNameList.end()) && (memberFunctionSymbol->get_name() == "operator*");

                    growOperatorFound    = (find(boxInitializedNameList.begin(),boxInitializedNameList.end(),initializedName) != boxInitializedNameList.end()) && (memberFunctionSymbol->get_name() == "grow");

                 // DQ (2/14/2015): It would have been a better design to have this operator be a member function of the stencil class.
                 // stencilApplyOperatorFound = (find(stencilInitializedNameList.begin(),stencilInitializedNameList.end(),initializedName) != stencilInitializedNameList.end()) && (memberFunctionSymbol->get_name() == "apply");
                  }

            // DQ (2/16/2015): Check for operator*() in "const Point hi = getOnes() * scalar;"
            // This is actually the better and most general test for a specific member function 
            // call (since calling off of a variable is not general enough).
               SgFunctionCallExp* nestedFunctionCallExp = isSgFunctionCallExp(dotExp->get_lhs_operand());
               if (nestedFunctionCallExp != NULL)
                  {
                    SgType* returnType = nestedFunctionCallExp->get_type();
                    ROSE_ASSERT(returnType != NULL);
#if 0
                    printf ("returnType = %p = %s \n",returnType,returnType->class_name().c_str());
#endif
                    SgType* base_type = returnType->findBaseType();
                    ROSE_ASSERT(base_type != NULL);

                    SgClassType* classType = isSgClassType(base_type);
                    if (classType != NULL)
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                         if (classDeclaration != NULL)
                            {
                              if (classDeclaration->get_name() == "Point")
                                 {
#if 0
                                   printf ("Found Point member function \n");
#endif
                                   pointOperatorFound = true;
                                 }
                            }
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               if (stencilOperatorFound == true)
                  {
                 // This is the location of the operator where the stencil is to be put in the transformed code.
                    stencilOperatorFunctionCallList.push_back(functionCallExp);

                    StencilUpdate_Attribute* dslAttribute = new StencilUpdate_Attribute();
#if 0
                    printf ("Adding (Stencil operator+=) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute("Stencil operator+=",dslAttribute);
#if 0
                    printf ("Found stencil location: functionCallExp = %p \n",functionCallExp);
                    functionCallExp->get_file_info()->display("location of stencil operator use: debug");
#endif
                  }

               if (pointOperatorFound == true)
                  {
                    PointOperatorMultiply_Attribute* dslAttribute = new PointOperatorMultiply_Attribute();
#if 0
                    printf ("Adding (PointOperatorMultiply operator*) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute(PointOperatorMultiply,dslAttribute);

                    findAndAddAttributeToConstantExpression(functionCallExp);
                  }

               if (growOperatorFound == true)
                  {
                    GrowOperator_Attribute* dslAttribute = new GrowOperator_Attribute();
#if 0
                    printf ("Adding (GrowOperator) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute(GrowOperator,dslAttribute);

                    findAndAddAttributeToConstantExpression(functionCallExp);
                  }
             }

          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionCallExp->get_function());
          if (memberFunctionRefExp != NULL)
             {
            // We might have to narrow these to the specific cases were are interested in.
               SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
               if (memberFunctionSymbol->get_name() == "apply")
                  {
                    StencilApplyOperator_Attribute* dslAttribute = new StencilApplyOperator_Attribute();
#if 0
                    printf ("Adding (StencilApplyOperator) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute("StencilApplyOperator",dslAttribute);
                  }
             }

          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionCallExp->get_function());
          if (functionRefExp != NULL)
             {
            // We might have to narrow these to the specific cases were are interested in.
               SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
               if (functionSymbol->get_name() == "operator^")
                  {
                    OperatorCarot_Attribute* dslAttribute = new OperatorCarot_Attribute();
#if 0
                    printf ("Adding (OperatorCarot operator^) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute("OperatorCarot operator^",dslAttribute);
                  }

            // We might have to narrow these to the specific cases were are interested in.
               if (functionSymbol->get_name() == "operator*")
                  {
                 // We have to distinghish between the scalar operator*() on a Point and that for a Shift.
                 //    template <class T> Stencil<T> operator*(T a_coef, Shift a_shift);
                 // and
                 //    Point operator*(int a_nref) const;
                 // However, the template function will be instantated as two different template instantiations, and we need to handle them seperately.
                 //    T == double, and
                 //    T == Point.

                    SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
                    ROSE_ASSERT(functionDeclaration != NULL);
#if 0
                    printf ("This should be the shift operator* instead of the Point operator*: functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->class_name().c_str());

                    functionDeclaration->get_file_info()->display("This should be the shift operator* instead of the Point operator*: functionDeclaration: debug");

                    SgType* returnType = functionRefExp->get_type();

                    printf ("returnType = %p = %s \n",returnType,returnType->class_name().c_str());
#endif
                    SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
                    ROSE_ASSERT(templateInstantiationFunctionDecl != NULL);
#if 0
                    printf ("This should be the shift operator* instead of the Point operator*: functionRefExp = %p \n",functionRefExp);
#endif
                    SgFunctionType* functionType = isSgFunctionType(functionRefExp->get_type());
                    ROSE_ASSERT(functionType != NULL);
                    SgFunctionParameterTypeList* functionParameterTypeList = functionType->get_argument_list();
                    ROSE_ASSERT(functionParameterTypeList != NULL);
                    SgTypePtrList & typeList = functionParameterTypeList->get_arguments();
                    ROSE_ASSERT(typeList.empty() == false);
                    SgType* firstArgument = typeList[0];
#if 0
                    printf ("### firstArgument = %p = %s \n",firstArgument,firstArgument->class_name().c_str());
#endif
                    SgTypeDouble* typeDouble = isSgTypeDouble(firstArgument);

                 // It appear that this is always true, why is this?
                    ROSE_ASSERT(typeDouble != NULL);

                    if (typeDouble != NULL)
                       {
                         ScalarShiftOperatorMultiply_Attribute* dslAttribute = new ScalarShiftOperatorMultiply_Attribute();
                         functionCallExp->addNewAttribute(ScalarShiftOperatorMultiply,dslAttribute);
                       }
                      else
                       {
                      // Make sure this is at least a SgClassType, but later we need to verify it is type Point.
                         ROSE_ASSERT(isSgClassType(firstArgument) != NULL);
                         PointShiftOperatorMultiply_Attribute* dslAttribute = new PointShiftOperatorMultiply_Attribute();
                         functionCallExp->addNewAttribute(PointShiftOperatorMultiply,dslAttribute);

#if 1
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }

                 // Mark the scalar with an attribute.
                    findAndAddAttributeToConstantExpression(functionCallExp);
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

            // We might have to narrow these to the specific cases were are interested in.
               if (functionSymbol->get_name() == "getShiftVec")
                  {
                    OperatorShiftVec_Attribute* dslAttribute = new OperatorShiftVec_Attribute();
#if 0
                    printf ("Adding (OperatorShiftVec) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute(OperatorShiftVec,dslAttribute);
                  }

            // We might have to narrow these to the specific cases were are interested in.
               if (functionSymbol->get_name() == "getZeros")
                  {
                    OperatorZero_Attribute* dslAttribute = new OperatorZero_Attribute();
#if 0
                    printf ("Adding (OperatorZero) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute(OperatorZero,dslAttribute);
                  }

            // We might have to narrow these to the specific cases were are interested in.
               if (functionSymbol->get_name() == "getOnes")
                  {
                    OperatorOnes_Attribute* dslAttribute = new OperatorOnes_Attribute();
#if 0
                    printf ("Adding (OperatorOnes) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute(OperatorOnes,dslAttribute);
                  }

            // We might have to narrow these to the specific cases were are interested in.
               if (functionSymbol->get_name() == "getUnitv")
                  {
                    OperatorUnit_Attribute* dslAttribute = new OperatorUnit_Attribute();
#if 0
                    printf ("Adding (OperatorUnit) dslAttribute = %p \n",dslAttribute);
#endif
                    ROSE_ASSERT(dslAttribute != NULL);

                    functionCallExp->addNewAttribute(OperatorUnit,dslAttribute);

                 // Mark the scalar with an attribute.
                    findAndAddAttributeToConstantExpression(functionCallExp);
                  }
             }
        }

     SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
     if (varRefExp != NULL)
        {
          SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
          ROSE_ASSERT(variableSymbol != NULL);
          SgInitializedName* initializedName = variableSymbol->get_declaration();
          ROSE_ASSERT(initializedName != NULL);

          bool stencilVariableFound = (find(stencilInitializedNameList.begin(),stencilInitializedNameList.end(),initializedName) != stencilInitializedNameList.end());
          if (stencilVariableFound == true)
             {
               StencilVariableRefExp_Attribute* dslAttribute = new StencilVariableRefExp_Attribute();
#if 0
               printf ("Adding (StencilVariableRefExp) dslAttribute = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               varRefExp->addNewAttribute("StencilVariableRefExp",dslAttribute);
             }

          bool pointVariableFound = (find(pointInitializedNameList.begin(),pointInitializedNameList.end(),initializedName) != pointInitializedNameList.end());
          if (pointVariableFound == true)
             {
               PointVariableRefExp_Attribute* dslAttribute = new PointVariableRefExp_Attribute();
#if 0
               printf ("Adding (PointVariableRefExp) dslAttribute = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               varRefExp->addNewAttribute("PointVariableRefExp",dslAttribute);
             }

          bool arrayVariableFound = (find(arrayInitializedNameList.begin(),arrayInitializedNameList.end(),initializedName) != arrayInitializedNameList.end());
          if (arrayVariableFound == true)
             {
               ArrayVariableRefExp_Attribute* dslAttribute = new ArrayVariableRefExp_Attribute();
#if 0
               printf ("Adding (ArrayVariableRefExp) dslAttribute = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               varRefExp->addNewAttribute("ArrayVariableRefExp",dslAttribute);
             }

          bool RectMDArrayVariableFound = (find(RectMDArrayInitializedNameList.begin(),RectMDArrayInitializedNameList.end(),initializedName) != RectMDArrayInitializedNameList.end());
          if (RectMDArrayVariableFound == true)
             {
               RectMDArrayVariableRefExp_Attribute* dslAttribute = new RectMDArrayVariableRefExp_Attribute();
#if 0
               printf ("Adding (RectMDArrayVariableRefExp) dslAttribute = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               varRefExp->addNewAttribute("RectMDArrayVariableRefExp",dslAttribute);
             }

          bool boxVariableFound = (find(boxInitializedNameList.begin(),boxInitializedNameList.end(),initializedName) != boxInitializedNameList.end());
          if (boxVariableFound == true)
             {
               BoxVariableRefExp_Attribute* dslAttribute = new BoxVariableRefExp_Attribute();
#if 0
               printf ("Adding (BoxVariableRefExp) dslAttribute = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               varRefExp->addNewAttribute("BoxVariableRefExp",dslAttribute);
             }
        }

#if 0
     printf ("Leaving DetectionTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

  // Construct the return attribute from the modified input attribute.
     return Detection_InheritedAttribute(inheritedAttribute);
   }


Detection_SynthesizedAttribute
DetectionTraversal::evaluateSynthesizedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     ROSE_ASSERT(astNode != NULL);

     Detection_SynthesizedAttribute return_synthesizedAttribute(astNode);

#if 0
     printf ("In evaluateSynthesizedAttribute(): astNode = %p = %s synthesizedAttributeList.size() = %zu dslChildren.size() = %zu \n",
          astNode,astNode->class_name().c_str(),synthesizedAttributeList.size(),return_synthesizedAttribute.dslChildren.size());
#endif

  // At each IR node and across all children, accumulate the dslChildren (child nodes for each of the DSL AST nodes).
     for (SubTreeSynthesizedAttributes::iterator i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
        {
          SgNode* childNode = (*i).node;
       // ROSE_ASSERT(childNode != NULL);
          if (childNode != NULL)
             {
#if 0
               printf ("Identified child node in evaluateSynthesizedAttribute(): childNode = %p = %s \n",childNode,childNode->class_name().c_str());
#endif
            // Insert each list from the child into the accumulated list in the current Synthesized Attribute.
               return_synthesizedAttribute.dslChildren.insert(return_synthesizedAttribute.dslChildren.end(),i->dslChildren.begin(),i->dslChildren.end());
#if 0
               printf ("   --- copying i->dslChildren.size() = %zu into return_synthesizedAttribute.dslChildren.size() = %zu \n",i->dslChildren.size(),return_synthesizedAttribute.dslChildren.size());
#endif
               if (return_synthesizedAttribute.dslChildren.empty() == false)
                  {
#if 0
                    printf ("In evaluateSynthesizedAttribute(): dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
#endif
                  }
             }
        }

  // We have to handle the DSL specific SgForStatement within the evaluateSynthesizedAttribute() function (after traversing children).
     SgForStatement* forStatement = isSgForStatement(astNode);
     if (forStatement != NULL)
        {
       // Detect the case of a for loop over DSL abstractions (we might just want to restrict this to stencil abstractions, not clear yet).
          if (return_synthesizedAttribute.dslChildren.empty() == false)
             {
               ForLoopOperator_Attribute* dslAttribute = new ForLoopOperator_Attribute();
#if 1
               printf ("Adding (ForLoopOperator) dslAttribute = %p \n",dslAttribute);
#endif
               ROSE_ASSERT(dslAttribute != NULL);

               forStatement->addNewAttribute(ForLoopOperator,dslAttribute);
             }
        }

  // For any DSL specific AST nodes (C++ AST nodes containing a DSL_Attribute), initialize the pointers to children of the DSL IR node.
     AstAttributeMechanism* astAttributeContainer = astNode->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
       // I think there should only be one DSL attribute, in the future we can support more on a single IR node.
          if (astAttributeContainer->size() != 1)
             {
               printf ("WARNING: astAttributeContainer->size() != 1: astAttributeContainer->size() = %d \n",astAttributeContainer->size());
             }
          ROSE_ASSERT(astAttributeContainer->size() == 1);

       // Loop over all the attributes at this IR node
       //  Pei-Hung (12/22/15): THe ASTAttributeMechanmsim is changed and has to use new API
       //   for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
          BOOST_FOREACH (const std::string &attributeName, astAttributeContainer->getAttributeIdentifiers()) 
             {
               AstAttribute* attribute = astNode->getAttribute(attributeName);
               ROSE_ASSERT(attribute != NULL);
 
               DSL_Attribute* dslAstAttribute = dynamic_cast<DSL_Attribute*>(attribute);
               ROSE_ASSERT(dslAstAttribute != NULL);
#if 0
               printf ("Identified dslAstAttribute in evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
               printf ("   --- return_synthesizedAttribute.dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
#endif
            // Copy the dsl child data to the dsl attribute.
               dslAstAttribute->currentNode = astNode;
               dslAstAttribute->dslChildren = return_synthesizedAttribute.dslChildren;
             }

       // Clear the dsl attributes becasue we the only collect dsl child attributes at dsl attributed IR nodes and don't pass then further up the tree.          
          return_synthesizedAttribute.dslChildren.clear();

       // Add the current node since it has an attribute.
          return_synthesizedAttribute.dslChildren.push_back(astNode);

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     printf ("Leaving evaluateSynthesizedAttribute(): return_synthesizedAttribute.dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
#endif

     return return_synthesizedAttribute;
   }

