// This is part of a sub-project within ROSE to define a higher level interface 
// for the construction of the AST.
#include "rose.h"
#include "highLevelInterface.h"


// *******************************************************************************
// ******************************  Support Functions  ****************************
// *******************************************************************************

void setStatementDetails ( SgStatement* stmt, SgNode* parent, SgScopeStatement* scope )
  {
 // This function sets a few of the required details of IR nodes.

 // The scope sometimes has to be explicit set (mostly for SgDeclarationStatement)
    if (scope != NULL)
         stmt->set_scope(scope);

    setLocatedNodeDetails(stmt,parent);
  }

void setExpressionDetails ( SgExpression* expr, SgNode* parent )
  {
 // This function sets a few of the required details of IR nodes.

    setLocatedNodeDetails(expr,parent);

 // Set the operator position if this is an operator!
    if (expr->isOperator() == true)
       {
         expr->set_operatorPosition(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
       }

  }

void setLocatedNodeDetails ( SgLocatedNode* locatedNode, SgNode* parent)
  {
 // This function sets a few of the required details of IR nodes.

 // The parent has to be set (if you use the STL directly)
    locatedNode->set_parent(parent);

 // The position of any SgLocatedNode must be set (in this case the IR nodes is classified
 // as being part of a transformation and to be output in the code generation. Since the
 // High Level Interface to the AST is all about construction of new AST sub-trees, there
 // is not position in the original source code so we mark all new IR nodes as being part
 // of a transformation and to be output in the generated code.
    locatedNode->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
    locatedNode->set_endOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
  }


// *******************************************************************************
// *******************************  Build Functions  *****************************
// *******************************************************************************

void
buildInitializedName( const SgName & name, SgType* type, SgScopeStatment* scope = NULL )
   {
  // If the scope was not specified, then get it from the scope stack.
     if (scope == NULL)
          scope = currentScope.back();

     ROSE_ASSERT(scope != NULL);
     ROSE_ASSERT(name.is_null() == false);
     ROSE_ASSERT(type != NULL);

     SgInitializedName* initializedName = new SgInitializedName(name,type,scope);
   }

void buildVariableDeclaration();
void buildValueExpression();
void buildAddOperator();
