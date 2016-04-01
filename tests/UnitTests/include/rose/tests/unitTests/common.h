#ifndef ROSE_UNIT_TESTS_COMMON_H_
#define ROSE_UNIT_TESTS_COMMON_H_

#include <rose.h>
#include <Sawyer/Assert.h>

namespace Rose {
namespace Tests {

/** Unit Tests for the ROSE compiler API.
 *
 *  Unit tests are grouped into logical namespaces under Rose::Tests::UnitTests.
 *  (The namespacing scheme mirrors the relative namespace within ROSE.)
 *
 *  Each namespace encapsulates a logical set of unit tests, which are run
 *  by a namespace-specific RunUnitTests() function.
 **/
namespace UnitTests {

#define check(COND) ASSERT_always_require(COND)
#define check2(COND, MESG) ASSERT_always_require2(COND, MESG)

template<typename T>
bool is(SgNode* t){
 return (dynamic_cast<T*>(t) != NULL);
}

bool isNull(SgNode* n) {
  return (n == NULL);
}

namespace Diagnostics {
using namespace rose::Diagnostics;

void initialize() {
  rose::Diagnostics::initialize();
  rose::Diagnostics::mfacilities.control("all");
}

}//::Rose::Tests::UnitTests::Diagnostics

using namespace UnitTests::Diagnostics;
}//::Rose::Tests::UnitTests
}//::Rose::Tests
}//::Rose

#endif
