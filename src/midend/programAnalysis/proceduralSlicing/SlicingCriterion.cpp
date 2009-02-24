#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "SlicingCriterion.h"

void SlicingCriterion::visit(SgNode* node){
  // We assume we only have two pragma declarations
  if(isSgPragmaDeclaration(node)!=NULL){
    if(!next)
      next = true;
    else{
      next = false;
      foundall = true;  
    }
  }
  if(next && !foundall && isSgPragma(node)==NULL && isSgPragmaDeclaration(node)==NULL ){
    if(isSgStatement(node))
      stmts.insert(isSgStatement(node));
  }
}
