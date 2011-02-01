#include "rose.h"
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
 
#include "GlobalIndexing.h"

void GlobalIndexing :: visit(SgNode* node){
  if(isSgBasicBlock(node)!=NULL){
    list<SgStatement*> stmts = isSgBasicBlock(node)->get_statements();
    
    for(list<SgStatement*>::const_iterator it = stmts.begin(); it!=stmts.end(); ++it){

      list_of_statements.push_front(isSgNode(*it));
      if(!(*it)->attribute.exists("global_index")){
        (*it)->attribute.add("global_index", new GlobalIndex(0));
        global_index++;
        (*it)->attribute.set("global_index", (AstAttribute*)global_index);
        //cout << "globalindex="<< global_index << endl;
      }
    }
  }
}
