#ifndef ROSETEST_UNITTESTING_TESTSUPPORT
#define ROSETEST_UNITTESTING_TESTSUPPORT


/*
 * File to include to make use of the ROSE unit testing support
 * infrastructure
 */

#include "rose.h"
#include "gtest/gtest.h"

template<typename T>
bool is(SgNode* t){
 return (dynamic_cast<T*>(t) != NULL);
}

bool isNull(SgNode* n);


#if 0
/*
 * Test fixture which is meant to be used if the user needs a SgProject
 */
class EmptyCMainFunctionClassFixture : public ::testing::Test {
public:

};
#endif

#endif
