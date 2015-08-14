#ifndef MATLAB_UNPARSER_TRAVERSAL_H
#define MATLAB_UNPARSER_TRAVERSAL_H

#include <string>
typedef std::string SynthesizedAttribute;

class MatlabUnparserBottomUpTraversal : public AstBottomUpProcessing<SynthesizedAttribute>
{
public:
  
  SynthesizedAttribute evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList childAttributes);

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
