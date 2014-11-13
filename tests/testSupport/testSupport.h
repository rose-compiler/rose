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

  CleanFunctionTypeTableFixture();

};

#endif
