#include "testSupport.h"

bool isNull(SgNode* n){
  return (n == NULL);
}


CleanFunctionTypeTableFixture::CleanFunctionTypeTableFixture(){
    // Dirty hack to overcome the global variable in ROSE..
    SgFunctionTypeTable *p = SgNode::get_globalFunctionTypeTable();
    assert(p != NULL);
    SgSymbolTable * pp = p->get_function_type_table();
    while(pp->size() > 0){
      pp->remove(pp->find_any());
    }
//    pp->get_table()->delete_elements();
    assert(pp->size() == 0);
}

