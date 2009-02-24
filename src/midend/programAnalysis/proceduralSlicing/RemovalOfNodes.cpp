#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "RemovalOfNodes.h"

void RemovalOfNodes::visit(SgNode* node){
  // If the AST node doesn't have the attribute keep, then delete the node.
  if(!node->attribute.exists("keep")){
    SgStatement *stmt = isSgStatement(node);
    if(stmt!=NULL){
      //LowLevelRewrite::remove(stmt);
      MiddleLevelRewrite::remove(stmt); //Still removes #include-statments when removes global variables
#ifdef DEBUG_REMOVAL
      writeMessage(node);
#endif
     }
  }
  // free memory of the ast attribute...
  // else if(node->attribute.exists("keep")){
  //   node->attribute.remove("keep");
  //  }
}

void RemovalOfNodes::writeMessage(SgNode* node){
  cout << "Removing: "
          << node->sage_class_name() <<": " << node->unparseToString() << endl;
}
