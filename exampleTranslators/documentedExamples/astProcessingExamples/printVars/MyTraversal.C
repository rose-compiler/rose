// Author: Markus Schordan, Vienna University of Technology, 2004.
// $Id: MyTraversal.C,v 1.3 2006/04/24 00:21:26 dquinlan Exp $

#include "rose.h"
#include "MyTraversal.h"

void
MyTraversal::visit(SgNode* node) {

  //! test for base class information of AST node
  if(dynamic_cast<SgBinaryOp*>(node)) {
    //! the number of binary operators is accumulated in the member variable binaryOpCount
    binaryOpCount++;
  }

  //! handle concrete objects of AST nodes
  switch(node->variantT()) {
    /*! use predefined enums for variants to switch on variant
        naming scheme for variants: V_<classname>
    */
    case V_SgVarRefExp:
     {
    /*! down-cast node from SgNode* to SgVarRefExp*
        would return 0 if it cannot be down-casted (which cannot happen here)
    */
    SgVarRefExp* varRefExp=isSgVarRefExp(node);

    //! obtain the variable name
    SgVariableSymbol* varSym=varRefExp->get_symbol();
    SgName varName=varSym->get_name();

    //! print class name and variable name
    cout << node->sage_class_name() << ": " << varName << endl;

    //! Some other useful access functions (dead code, for demo only)
    SgType* varRefExpType   = varRefExp->get_type();
    SgInitializedName* decl = varSym->get_declaration();
    break;
    }

 // DQ (3/13/2006): Added code
    default:
       {
      // Avoid g++ warning about unhandled cases
       }
  }
}

unsigned int
MyTraversal::getBinaryOpCount() {
  return binaryOpCount;
}
