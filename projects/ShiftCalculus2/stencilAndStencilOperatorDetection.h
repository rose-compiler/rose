
// DQ (2/17/2015): Include Shift Calculus DSL abstraction implementation.
#include <iostream>
#include <cassert>
#include <cmath>
#include <vector>
#include <memory>
#include <stdio.h>
#include <fstream>
#include "Shift.H"
#include "Stencil.H" 
#include "PowerItoI.H"

#include "Box.H"

#include "RectMDArray.H"


class Detection_InheritedAttribute
   {
     private:
          bool isShiftExpression;

          bool isStencilDeclaration;
          bool isStencilOperatorDeclaration;
          bool isStencilOperatorRef;

     public:
       // DQ (2/18/2015): Added to support debugging by restricting traversals to main function.
          bool inFunction;
          bool inDSL_Function;
      
     public:
          Detection_InheritedAttribute();
          Detection_InheritedAttribute( const Detection_InheritedAttribute & X );

          void set_ShiftExpression(bool value);
          bool get_ShiftExpression();

          void set_StencilDeclaration(bool value);
          bool get_StencilDeclaration();
          void set_StencilOperatorDeclaration(bool value);
          bool get_StencilOperatorDeclaration();

          void set_StencilOperatorRef(bool value);
          bool get_StencilOperatorRef();
   };

class Detection_SynthesizedAttribute
   {
     private:
          bool stencilOperatorTransformed;

     public:
          SgNode* node;

          std::vector<SgVarRefExp*> boxVarRefExpList;
          std::vector<SgVarRefExp*> rectMDArrayVarRefExpList;
          std::vector<SgVarRefExp*> stencilVarRefExpList;

          std::vector<SgNode*> dslChildren;

     public:
          Detection_SynthesizedAttribute();
          Detection_SynthesizedAttribute( SgNode* n );
          Detection_SynthesizedAttribute( const Detection_SynthesizedAttribute & X );

          void set_stencilOperatorTransformed(bool value);
          bool get_stencilOperatorTransformed();
   };


class DSL_Attribute: public AstAttribute 
   {
     public:
          DSL_Attribute();
          virtual ~DSL_Attribute();

          std::string toString();
          std::string additionalNodeOptions();

       // Most attributes will have children that define there embedding into the AST.
          std::vector<SgNode*> dslChildren;

          SgNode* currentNode;
          std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();

          std::string get_name();
          std::string get_color();
   };

class DSL_ValueAttribute: public DSL_Attribute 
   {
     public:
          DSL_ValueAttribute();
          virtual ~DSL_ValueAttribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class Point_Attribute: public DSL_Attribute 
   {
     public:
          Point_Attribute();
          virtual ~Point_Attribute();

       // Point value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class PointValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          PointValue_Attribute();
          virtual ~PointValue_Attribute();

          Point value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class RectMDArray_Attribute: public DSL_Attribute 
   {
     public:
          RectMDArray_Attribute();
          virtual ~RectMDArray_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class Stencil_Attribute: public DSL_Attribute 
   {
     public:
          Stencil_Attribute();
          virtual ~Stencil_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class Array_Attribute: public DSL_Attribute 
   {
     public:
          Array_Attribute();
          virtual ~Array_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class Box_Attribute: public DSL_Attribute 
   {
     public:
          Box_Attribute();
          virtual ~Box_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class StencilUpdate_Attribute: public DSL_Attribute 
   {
     public:
          StencilUpdate_Attribute();
          virtual ~StencilUpdate_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class OperatorCarot_Attribute: public DSL_Attribute 
   {
     public:
          OperatorCarot_Attribute();
          virtual ~OperatorCarot_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class PointOperatorMultiply_Attribute: public DSL_Attribute 
   {
     public:
          PointOperatorMultiply_Attribute();
          virtual ~PointOperatorMultiply_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class ScalarShiftOperatorMultiply_Attribute: public DSL_Attribute 
   {
     public:
          ScalarShiftOperatorMultiply_Attribute();
          virtual ~ScalarShiftOperatorMultiply_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class PointShiftOperatorMultiply_Attribute: public DSL_Attribute 
   {
     public:
          PointShiftOperatorMultiply_Attribute();
          virtual ~PointShiftOperatorMultiply_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class StencilApplyOperator_Attribute: public DSL_Attribute 
   {
     public:
          StencilApplyOperator_Attribute();
          virtual ~StencilApplyOperator_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class OperatorZero_Attribute: public DSL_Attribute 
   {
     public:
          OperatorZero_Attribute();
          virtual ~OperatorZero_Attribute();

       // Point value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class OperatorOnes_Attribute: public DSL_Attribute 
   {
     public:
          OperatorOnes_Attribute();
          virtual ~OperatorOnes_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class OperatorUnit_Attribute: public DSL_Attribute 
   {
     public:
          OperatorUnit_Attribute();
          virtual ~OperatorUnit_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class ForLoopOperator_Attribute: public DSL_Attribute 
   {
     public:
          ForLoopOperator_Attribute();
          virtual ~ForLoopOperator_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class GrowOperator_Attribute: public DSL_Attribute 
   {
     public:
          GrowOperator_Attribute();
          virtual ~GrowOperator_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class OperatorShiftVec_Attribute: public DSL_Attribute 
   {
     public:
          OperatorShiftVec_Attribute();
          virtual ~OperatorShiftVec_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };












class StencilVariableRefExp_Attribute: public DSL_Attribute 
   {
     public:
          StencilVariableRefExp_Attribute();
          virtual ~StencilVariableRefExp_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class PointVariableRefExp_Attribute: public DSL_Attribute 
   {
     public:
          PointVariableRefExp_Attribute();
          virtual ~PointVariableRefExp_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class ArrayVariableRefExp_Attribute: public DSL_Attribute 
   {
     public:
          ArrayVariableRefExp_Attribute();
          virtual ~ArrayVariableRefExp_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class RectMDArrayVariableRefExp_Attribute: public DSL_Attribute 
   {
     public:
          RectMDArrayVariableRefExp_Attribute();
          virtual ~RectMDArrayVariableRefExp_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class BoxVariableRefExp_Attribute: public DSL_Attribute 
   {
     public:
          BoxVariableRefExp_Attribute();
          virtual ~BoxVariableRefExp_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };


class ConstantExpression_Attribute: public DSL_Attribute 
   {
     public:
          ConstantExpression_Attribute();
          virtual ~ConstantExpression_Attribute();

          std::string toString();
          std::string additionalNodeOptions();
   };

class IntegerValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          IntegerValue_Attribute();
          virtual ~IntegerValue_Attribute();

          int value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class DoubleValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          DoubleValue_Attribute();
          virtual ~DoubleValue_Attribute();

          double value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class ShiftValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          ShiftValue_Attribute();
          virtual ~ShiftValue_Attribute();

          Shift value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class ArrayValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          ArrayValue_Attribute();
          virtual ~ArrayValue_Attribute();

          array<Shift,DIM> value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class StencilValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          StencilValue_Attribute();
          virtual ~StencilValue_Attribute();

          Stencil<double> value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class BoxValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          BoxValue_Attribute();
          virtual ~BoxValue_Attribute();

       // Box value;

          std::string toString();
          std::string additionalNodeOptions();
   };

class RectMDArrayValue_Attribute: public DSL_ValueAttribute 
   {
     public:
          RectMDArrayValue_Attribute();
          virtual ~RectMDArrayValue_Attribute();

       // The value is a single component double precission floating point 
       // multi-dimensional array (array dimensions are evaluated at runtime).
       // RectMDArray<double,1> value;

          std::string toString();
          std::string additionalNodeOptions();
   };



class DetectionTraversal : public SgTopDownBottomUpProcessing<Detection_InheritedAttribute,Detection_SynthesizedAttribute>
   {
     private:
          std::vector<SgInitializedName*> initializedNameList;
       // We want a map of vectors of inputs to stencil declarations later.
       // std::map<SgInitializedName*, std::vector<SgVarRef*> > stencilInputList;
       // std::vector<SgVarRefExp*> stencilInputList;
          std::vector<SgExpression*>      stencilInputExpressionList;
          std::vector<SgInitializedName*> stencilInputInitializedNameList;

       // This is a list of the stencil objects found (there could be more than one).
          std::vector<SgInitializedName*> stencilInitializedNameList;
          std::vector<SgInitializedName*> stencilOperatorInitializedNameList;
          std::vector<SgFunctionCallExp*> stencilOperatorFunctionCallList;

          std::vector<SgInitializedName*> pointInitializedNameList;
          std::vector<SgInitializedName*> arrayInitializedNameList;
          std::vector<SgInitializedName*> RectMDArrayInitializedNameList;
          std::vector<SgInitializedName*> boxInitializedNameList;

     public:
       // Functions required to overload the pure virtual functions in the abstract base class.
          Detection_InheritedAttribute   evaluateInheritedAttribute   (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute );
          Detection_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* astNode, Detection_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );

          void findAndAddAttributeToConstantExpression ( SgFunctionCallExp* functionCallExp );

          static const std::string PointVariable;
          static const std::string BoxVariable;
          static const std::string ShiftVariable;
          static const std::string ArrayVariable;
          static const std::string StencilVariable;
          static const std::string RectMDArrayVariable;

       // static const std::string scalar_multiply_name;
          static const std::string PointOperatorMultiply;
          static const std::string ScalarShiftOperatorMultiply;
          static const std::string PointShiftOperatorMultiply;
          static const std::string OperatorZero;
          static const std::string OperatorOnes;
          static const std::string OperatorUnit;
          static const std::string ForLoopOperator;
          static const std::string GrowOperator;
          static const std::string ConstantExpression;
          static const std::string OperatorShiftVec;

          DetectionTraversal();

          std::vector<SgInitializedName*> & get_stencilInputInitializedNameList();
          std::vector<SgInitializedName*> & get_stencilOperatorInitializedNameList();

          std::vector<SgFunctionCallExp*> & get_stencilOperatorFunctionCallList();

       // Output debugging data.
          void display();
   };

