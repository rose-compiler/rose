/*
The folowing example:

class nsINodeInfo
{
public:
 nsINodeInfo()
   : mInner(0)
 {
 }
 class nsNodeInfoInner
 {
 public:
   nsNodeInfoInner(nsINodeInfo *aName)
   {
   }

 };

 nsNodeInfoInner mInner;

};


gives the following error:

Error: non-defining memberFunctionDeclaration with parent not set to class scope
lt-identityTranslator:
/home/andreas/REPOSITORY-SRC/ROSE/June-25a-Unsafe/NEW_ROSE/src/midend/astDiagnostics/AstConsistencyTests.C:3317:
virtual void TestChildPointersInMemoryPool::visit(SgNode*): Assertion false failed. 
*/

class nsINodeInfo
{
public:
 nsINodeInfo()
   : mInner(0)
 {
 }
 class nsNodeInfoInner
 {
 public:
   nsNodeInfoInner(nsINodeInfo *aName)
   {
   }

 };

 nsNodeInfoInner mInner;

};

