#ifndef MATLAB_UNPARSER_TRAVERSAL_H
#define MATLAB_UNPARSER_TRAVERSAL_H

#include "sage3basic.h"
#include <string>

typedef std::string SynthesizedAttribute;

class MatlabUnparserBottomUpTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
{
public:
  
  SynthesizedAttribute evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList childAttributes)
  {
    switch(node->variantT())
      {
      case V_SgDoubleVal:
	return handleDoubleVal(node, childAttributes);
	break;
	
      case V_SgExprListExp:
	return handleExprListExp(node, childAttributes);
	break;

      case V_SgMatrixExp:
	return handleMatrixExp(node, childAttributes);
	break;

      case V_SgMagicColonExp:
	return handleMagicColonExp(node, childAttributes);
	break;
	
      case V_SgRangeExp:
	return handleRangeExp(node, childAttributes);
	break;
	
      case V_SgVarRefExp:
	return handleVarRefExp(node, childAttributes);
       break;

      case V_SgInitializedName:
	return handleInitializedName(node, childAttributes);
	break;

      case V_SgFunctionCallExp:
	return handleFunctionCallExp(node, childAttributes);
	break;
	
      case V_SgAssignOp:
	return handleAssignOp(node, childAttributes);
	break;

      case V_SgAddOp:
	return handleBinaryOp(node, childAttributes, "+");
	break;

      case V_SgElementwiseAddOp:
	return handleBinaryOp(node, childAttributes, ".+");
	break;

      case V_SgMultiplyOp:
	return handleBinaryOp(node, childAttributes, "*");
	break;

      case V_SgElementwiseMultiplyOp:
	return handleBinaryOp(node, childAttributes, ".*");
	break;
	
      case V_SgSubtractOp:
	return handleBinaryOp(node, childAttributes, "-");
	break;

      case V_SgElementwiseSubtractOp:
	return handleBinaryOp(node, childAttributes, ".-");
	break;
	
      case V_SgDivideOp:
	return handleBinaryOp(node, childAttributes, "/");
	break;

      case V_SgElementwiseDivideOp:
	return handleBinaryOp(node, childAttributes, "./");
	break;

      case V_SgLeftDivideOp:
	return handleBinaryOp(node, childAttributes, "\\");
	break;

      case V_SgElementwiseLeftDivideOp:
	return handleBinaryOp(node, childAttributes, ".\\");
	break;

      case V_SgPowerOp:
	return handleBinaryOp(node, childAttributes, "^");
	break;

      case V_SgElementwisePowerOp:
	return handleBinaryOp(node, childAttributes, ".^");
	break;

      case V_SgLessThanOp:
	return handleBinaryOp(node, childAttributes, "<");
	break;

      case V_SgLessOrEqualOp:
	return handleBinaryOp(node, childAttributes, "<=");
	break;

      case V_SgEqualityOp:
	return handleBinaryOp(node, childAttributes, "==");
	break;

      case V_SgGreaterOrEqualOp:
	return handleBinaryOp(node, childAttributes, ">=");
	break;

      case V_SgGreaterThanOp:
	return handleBinaryOp(node, childAttributes, ">");
	break;                                             

      case V_SgNotEqualOp:
	return handleBinaryOp(node, childAttributes, "~=");
	break;

      case V_SgBitAndOp:
	return handleBinaryOp(node, childAttributes, "&");
	break;

      case V_SgBitOrOp:
	return handleBinaryOp(node, childAttributes, "|");
	break;

      case V_SgOrOp:
	return handleBinaryOp(node, childAttributes, "||");
	break;
	
      case V_SgAndOp:
	return handleBinaryOp(node, childAttributes, "&&");
	break;

      case V_SgLshiftOp:
	return handleBinaryOp(node, childAttributes, "<<");
	break;

      case V_SgRshiftOp:
	return handleBinaryOp(node, childAttributes, ">>");
	break;
	
      case V_SgMatrixTransposeOp:
	return handleMatrixTransposeOp(node, childAttributes);
	break;

      case V_SgMinusOp:
	return handlePrefixOp(node, childAttributes, "-");
	break;
	
      case V_SgReturnStmt:
	return handleReturnStmt(node, childAttributes);
	break;
	
      case V_SgExprStatement:
	return handleExprStatement(node, childAttributes);
	break;

      case V_SgBasicBlock:
	return handleBasicBlock(node, childAttributes);
	break;

      case V_SgFunctionDefinition:
	return handleFunctionDefinition(node, childAttributes);
	break;

      case V_SgFunctionParameterList:
	return handleFunctionParameterList(node, childAttributes);
	break;

      case V_SgFunctionDeclaration:
	return handleFunctionDeclaration(node, childAttributes);
	break;

      case V_SgMatlabForStatement:
	return handleMatlabForStatement(node, childAttributes);
	break;

      case V_SgIfStmt:
	return handleIfStmt(node, childAttributes);
	break;
	
      case V_SgGlobal:
	return handleGlobal(node, childAttributes);
	break;

      case V_SgSourceFile:
	return handleSourceFile(node, childAttributes);
	break;
	
      default:
	return "";
	break;
      }
  }

private:
  std::string handleDoubleVal(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleExprListExp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleMatrixExp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleMagicColonExp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleRangeExp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleVarRefExp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleInitializedName(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleFunctionCallExp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handlePrefixOp(SgNode *node, SynthesizedAttributesList childAttributes, std::string operatorString);
  std::string handleAssignOp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleBinaryOp(SgNode *node, SynthesizedAttributesList childAttributes, std::string operatorString);
  std::string handleMatrixTransposeOp(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleReturnStmt(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleExprStatement(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleBasicBlock(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleFunctionDefinition(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleFunctionParameterList(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleFunctionDeclaration(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleMatlabForStatement(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleIfStmt(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleGlobal(SgNode *node, SynthesizedAttributesList childAttributes);
  std::string handleSourceFile(SgNode *node, SynthesizedAttributesList childAttributes);

  /*
   *The Attribute that has string representation for the return statement.
   *It is supposed to be attached to a function declaration
  */
  class ReturnStmtAttribute : public AstAttribute
  {
  public:
    std::string returnStmtString;

    void attach_to(SgNode *node)
    {
      node->setAttribute("RETURN_STRING", this);
    }

    static ReturnStmtAttribute* get_attribute(SgNode *node)
    {
      if(node->attributeExists("RETURN_STRING"))
	{
	  return  (ReturnStmtAttribute*)(node->getAttribute("RETURN_STRING"));
	}
      else
	{
	  return NULL;
	}
    }
  };
};
#endif
