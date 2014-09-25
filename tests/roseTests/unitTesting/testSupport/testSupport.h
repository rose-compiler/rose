#ifndef ROSETEST_UNITTESTING_TESTSUPPORT
#define ROSETEST_UNITTESTING_TESTSUPPORT


/*
 * File to include to make use of the ROSE unit testing support
 * infrastructure
 */

#include "rose.h"
#include "gtest/gtest.h"
#include <cassert>

template<typename T>
bool is(SgNode* t){
 return (dynamic_cast<T*>(t) != NULL);
}

bool isNull(SgNode* n);


/*
 * This Fixture provides the user with a cleaned SgFunctionTypeTable
 */
class CleanFunctionTypeTableFixture : public ::testing::Test {
public:

  CleanFunctionTypeTableFixture(){
    // Dirty hack to overcome the global variable in ROSE..
    SgFunctionTypeTable *p = SgNode::get_globalFunctionTypeTable();
    assert(p != NULL);
    SgSymbolTable * pp = p->get_function_type_table();
    while(pp->size() > 0){
      pp->remove(pp->find_any());
    }
    assert(pp->size() == 0);
  }

};

#endif
