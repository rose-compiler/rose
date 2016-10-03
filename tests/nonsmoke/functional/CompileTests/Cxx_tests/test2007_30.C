/*
Hi Dan,

the following is a 4 line test case that fails under Rose build with gcc4.1.1 and gcc3.4.6 .

The error message is attached below.

thanks,
Thomas


03:35 PM :~/development/roseTest/testBug_02 > ~/development/ROSE-0107/gcc3.4.6/bin/identityTranslator testCase02.C
function name = f definingDeclaration = (nil) nondefiningDeclaration = 0xb7edd9bc parent = (nil)
identityTranslator: ../../../../../ROSE/src/frontend/SageIII/astPostProcessing/resetParentPointers.C:1873: virtual void ResetParentPointersInMemoryPool::visit(SgNode*): Assertion locatedNode->get_parent() != __null failed.
Aborted
*/

struct A {
  static void m() {f();}
  friend void f();
};
