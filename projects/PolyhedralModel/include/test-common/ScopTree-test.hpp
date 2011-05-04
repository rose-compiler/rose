/**
 * \file include/test-common/ScopTree-test.hpp
 * \brief Specialization of ScopTree for test purpose.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "common/ScopTree.hpp"
#include "test-common/PolyhedricContainer-test.hpp"

#include <vector>

#ifndef _SCOPTREE_TEST_HPP_
#define _SCOPTREE_TEST_HPP_

namespace ScopTree_ {
using namespace ScopTree_;

typedef ScopTree_::ScopTree<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestScopTree;
typedef ScopTree_::ScopRoot<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestScopRoot;
typedef ScopTree_::ScopLoop<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestScopLoop;
typedef ScopTree_::ScopConditionnal<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestScopConditionnal;
typedef ScopTree_::ScopStatement<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestScopStatement;

}

#endif /* _SCOPTREE_TEST_HPP_ */

