// MACHINE GENERATED SOURCE FILE WITH ROSE (Grammar.h)--- DO NOT MODIFY!

#include "sage3basic.h"

#include "AST_FILE_IO.h"

// The header file ("rose_config.h") should only be included by source files that require it.
#include "rose_config.h"

#if _MSC_VER
#define USE_CPP_NEW_DELETE_OPERATORS 0
#endif


using namespace std;

// Simplify code by using std namespace (never put into header files since it effects users) 
using namespace std;

vector<SgNode*>
SgNode::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgNode::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgNode::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgNode::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgNode::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgSupport::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgSupport::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgSupport::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgSupport::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgSupport::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgModifier::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgModifier" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgModifier" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgModifier::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgModifier" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgModifier::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgModifier" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgModifier::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgModifier" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgModifierNodes::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgModifierNodes::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgModifierNodes::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgModifierNodes::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgModifierNodes" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgModifierNodes::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgModifierNodes" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgConstVolatileModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgConstVolatileModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgConstVolatileModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgConstVolatileModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgConstVolatileModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgConstVolatileModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgConstVolatileModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgStorageModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgStorageModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgStorageModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgStorageModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgStorageModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgStorageModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgStorageModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAccessModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAccessModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAccessModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAccessModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAccessModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAccessModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAccessModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFunctionModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFunctionModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFunctionModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFunctionModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFunctionModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFunctionModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFunctionModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgUPC_AccessModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUPC_AccessModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUPC_AccessModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUPC_AccessModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUPC_AccessModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUPC_AccessModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUPC_AccessModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgSpecialFunctionModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgSpecialFunctionModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgSpecialFunctionModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgSpecialFunctionModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgSpecialFunctionModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgSpecialFunctionModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgSpecialFunctionModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgElaboratedTypeModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgElaboratedTypeModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgElaboratedTypeModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgElaboratedTypeModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgElaboratedTypeModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgElaboratedTypeModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgElaboratedTypeModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgLinkageModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgLinkageModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgLinkageModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgLinkageModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgLinkageModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgLinkageModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgLinkageModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgBaseClassModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgBaseClassModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgBaseClassModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgBaseClassModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgBaseClassModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgBaseClassModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgBaseClassModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTypeModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgTypeModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgTypeModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgTypeModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgTypeModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgTypeModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgTypeModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDeclarationModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDeclarationModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDeclarationModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDeclarationModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDeclarationModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDeclarationModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDeclarationModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOpenclAccessModeModifier::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOpenclAccessModeModifier::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOpenclAccessModeModifier::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOpenclAccessModeModifier::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOpenclAccessModeModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOpenclAccessModeModifier::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOpenclAccessModeModifier" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgName::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgName::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgName::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgName::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgName" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgName::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgName" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgSymbolTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgSymbolTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgSymbolTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgSymbolTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgSymbolTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgSymbolTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgSymbolTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAttribute::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAttribute::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAttribute::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAttribute::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAttribute::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgPragma::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgPragma::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgPragma::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgPragma::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgPragma" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgPragma::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgPragma" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgBitAttribute::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBitAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgBitAttribute::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBitAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgBitAttribute::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBitAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgBitAttribute::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBitAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgBitAttribute::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBitAttribute" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgFuncDecl_attr::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFuncDecl_attr::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFuncDecl_attr::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFuncDecl_attr::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFuncDecl_attr" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFuncDecl_attr::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFuncDecl_attr" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgClassDecl_attr::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgClassDecl_attr::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgClassDecl_attr::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgClassDecl_attr::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgClassDecl_attr" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgClassDecl_attr::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgClassDecl_attr" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
Sg_File_Info::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
Sg_File_Info::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
Sg_File_Info::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
Sg_File_Info::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "Sg_File_Info" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
Sg_File_Info::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "Sg_File_Info" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFile::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgFile" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgFile::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgFile" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgFile::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgFile" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgFile::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgFile" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgFile::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgFile" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgSourceFile::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgGlobal* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_globalScope);
return traversalSuccessorContainer;
}
vector<string>
SgSourceFile::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_globalScope");
return traversalSuccessorContainer;
}
size_t
SgSourceFile::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgSourceFile::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_globalScope;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgSourceFile::get_childIndex(SgNode *child) {
if (child == p_globalScope) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgBinaryComposite::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmGenericFileList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_genericFileList);
/* typeString = SgAsmInterpretationList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_interpretations);
return traversalSuccessorContainer;
}
vector<string>
SgBinaryComposite::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_genericFileList");
traversalSuccessorContainer.push_back("p_interpretations");
return traversalSuccessorContainer;
}
size_t
SgBinaryComposite::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgBinaryComposite::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_genericFileList;
case 1: return p_interpretations;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBinaryComposite::get_childIndex(SgNode *child) {
if (child == p_genericFileList) return 0;
else if (child == p_interpretations) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgUnknownFile::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgGlobal* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_globalScope);
return traversalSuccessorContainer;
}
vector<string>
SgUnknownFile::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_globalScope");
return traversalSuccessorContainer;
}
size_t
SgUnknownFile::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnknownFile::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_globalScope;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnknownFile::get_childIndex(SgNode *child) {
if (child == p_globalScope) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgProject::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgFileList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_fileList_ptr);
return traversalSuccessorContainer;
}
vector<string>
SgProject::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_fileList_ptr");
return traversalSuccessorContainer;
}
size_t
SgProject::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgProject::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_fileList_ptr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgProject::get_childIndex(SgNode *child) {
if (child == p_fileList_ptr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOptions::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOptions::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOptions::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOptions::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOptions" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOptions::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOptions" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgUnparse_Info::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUnparse_Info::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUnparse_Info::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUnparse_Info::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUnparse_Info" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUnparse_Info::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUnparse_Info" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgBaseClass::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgClassDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_base_class);
return traversalSuccessorContainer;
}
vector<string>
SgBaseClass::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_base_class");
return traversalSuccessorContainer;
}
size_t
SgBaseClass::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgBaseClass::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_base_class;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBaseClass::get_childIndex(SgNode *child) {
if (child == p_base_class) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgTypedefSeq::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgTypedefSeq::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgTypedefSeq::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgTypedefSeq::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgTypedefSeq" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgTypedefSeq::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgTypedefSeq" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTemplateParameter::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_defaultExpressionParameter);
/* typeString = SgTemplateDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_templateDeclaration);
/* typeString = SgTemplateDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_defaultTemplateDeclarationParameter);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateParameter::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
traversalSuccessorContainer.push_back("p_defaultExpressionParameter");
traversalSuccessorContainer.push_back("p_templateDeclaration");
traversalSuccessorContainer.push_back("p_defaultTemplateDeclarationParameter");
return traversalSuccessorContainer;
}
size_t
SgTemplateParameter::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgTemplateParameter::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
case 1: return p_defaultExpressionParameter;
case 2: return p_templateDeclaration;
case 3: return p_defaultTemplateDeclarationParameter;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateParameter::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else if (child == p_defaultExpressionParameter) return 1;
else if (child == p_templateDeclaration) return 2;
else if (child == p_defaultTemplateDeclarationParameter) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateArgument::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
/* typeString = SgTemplateDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_templateDeclaration);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateArgument::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
traversalSuccessorContainer.push_back("p_templateDeclaration");
return traversalSuccessorContainer;
}
size_t
SgTemplateArgument::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgTemplateArgument::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
case 1: return p_templateDeclaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateArgument::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else if (child == p_templateDeclaration) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDirectory::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgFileList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_fileList);
/* typeString = SgDirectoryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_directoryList);
return traversalSuccessorContainer;
}
vector<string>
SgDirectory::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_fileList");
traversalSuccessorContainer.push_back("p_directoryList");
return traversalSuccessorContainer;
}
size_t
SgDirectory::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDirectory::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_fileList;
case 1: return p_directoryList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDirectory::get_childIndex(SgNode *child) {
if (child == p_fileList) return 0;
else if (child == p_directoryList) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgFileList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_listOfFiles.size() + 0);
/* typeString = SgFilePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgFilePtrList::iterator iter;
     for (iter = p_listOfFiles.begin(); iter != p_listOfFiles.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgFileList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgFilePtrList::iterator  iter;
     for (iter = p_listOfFiles.begin(); iter != p_listOfFiles.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgFileList::get_numberOfTraversalSuccessors() {
return p_listOfFiles.size() + 0;
}
SgNode *
SgFileList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_listOfFiles.size());
return p_listOfFiles[idx];
}
size_t
SgFileList::get_childIndex(SgNode *child) {
SgFilePtrList::iterator itr = find(p_listOfFiles.begin(), p_listOfFiles.end(), child);
if (itr != p_listOfFiles.end()) return itr - p_listOfFiles.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgDirectoryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_listOfDirectories.size() + 0);
/* typeString = SgDirectoryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgDirectoryPtrList::iterator iter;
     for (iter = p_listOfDirectories.begin(); iter != p_listOfDirectories.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgDirectoryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgDirectoryPtrList::iterator  iter;
     for (iter = p_listOfDirectories.begin(); iter != p_listOfDirectories.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgDirectoryList::get_numberOfTraversalSuccessors() {
return p_listOfDirectories.size() + 0;
}
SgNode *
SgDirectoryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_listOfDirectories.size());
return p_listOfDirectories[idx];
}
size_t
SgDirectoryList::get_childIndex(SgNode *child) {
SgDirectoryPtrList::iterator itr = find(p_listOfDirectories.begin(), p_listOfDirectories.end(), child);
if (itr != p_listOfDirectories.end()) return itr - p_listOfDirectories.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgFunctionParameterTypeList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFunctionParameterTypeList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFunctionParameterTypeList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFunctionParameterTypeList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFunctionParameterTypeList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFunctionParameterTypeList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFunctionParameterTypeList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgQualifiedName::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgQualifiedName::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgQualifiedName::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgQualifiedName::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgQualifiedName" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgQualifiedName::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgQualifiedName" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTemplateArgumentList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_args.size() + 0);
/* typeString = SgTemplateArgumentPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgTemplateArgumentPtrList::iterator iter;
     for (iter = p_args.begin(); iter != p_args.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgTemplateArgumentList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgTemplateArgumentPtrList::iterator  iter;
     for (iter = p_args.begin(); iter != p_args.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgTemplateArgumentList::get_numberOfTraversalSuccessors() {
return p_args.size() + 0;
}
SgNode *
SgTemplateArgumentList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_args.size());
return p_args[idx];
}
size_t
SgTemplateArgumentList::get_childIndex(SgNode *child) {
SgTemplateArgumentPtrList::iterator itr = find(p_args.begin(), p_args.end(), child);
if (itr != p_args.end()) return itr - p_args.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateParameterList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_args.size() + 0);
/* typeString = SgTemplateParameterPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgTemplateParameterPtrList::iterator iter;
     for (iter = p_args.begin(); iter != p_args.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgTemplateParameterList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgTemplateParameterPtrList::iterator  iter;
     for (iter = p_args.begin(); iter != p_args.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgTemplateParameterList::get_numberOfTraversalSuccessors() {
return p_args.size() + 0;
}
SgNode *
SgTemplateParameterList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_args.size());
return p_args[idx];
}
size_t
SgTemplateParameterList::get_childIndex(SgNode *child) {
SgTemplateParameterPtrList::iterator itr = find(p_args.begin(), p_args.end(), child);
if (itr != p_args.end()) return itr - p_args.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgGraph::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgGraph::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgGraph::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgGraph::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgGraph::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgIncidenceDirectedGraph::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIncidenceDirectedGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgIncidenceDirectedGraph::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIncidenceDirectedGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgIncidenceDirectedGraph::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIncidenceDirectedGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgIncidenceDirectedGraph::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIncidenceDirectedGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgIncidenceDirectedGraph::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIncidenceDirectedGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgBidirectionalGraph::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBidirectionalGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgBidirectionalGraph::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBidirectionalGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgBidirectionalGraph::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBidirectionalGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgBidirectionalGraph::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBidirectionalGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgBidirectionalGraph::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBidirectionalGraph" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgStringKeyedBidirectionalGraph::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgStringKeyedBidirectionalGraph::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgStringKeyedBidirectionalGraph::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgStringKeyedBidirectionalGraph::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgStringKeyedBidirectionalGraph" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgStringKeyedBidirectionalGraph::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgStringKeyedBidirectionalGraph" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIntKeyedBidirectionalGraph::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIntKeyedBidirectionalGraph::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIntKeyedBidirectionalGraph::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIntKeyedBidirectionalGraph::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIntKeyedBidirectionalGraph" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIntKeyedBidirectionalGraph::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIntKeyedBidirectionalGraph" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIncidenceUndirectedGraph::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIncidenceUndirectedGraph::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIncidenceUndirectedGraph::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIncidenceUndirectedGraph::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIncidenceUndirectedGraph" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIncidenceUndirectedGraph::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIncidenceUndirectedGraph" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgGraphNode::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgGraphNode::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgGraphNode::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgGraphNode::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgGraphNode" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgGraphNode::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgGraphNode" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgGraphEdge::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraphEdge" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgGraphEdge::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraphEdge" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgGraphEdge::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraphEdge" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgGraphEdge::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraphEdge" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgGraphEdge::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgGraphEdge" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgDirectedGraphEdge::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDirectedGraphEdge::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDirectedGraphEdge::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDirectedGraphEdge::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDirectedGraphEdge" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDirectedGraphEdge::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDirectedGraphEdge" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgUndirectedGraphEdge::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUndirectedGraphEdge::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUndirectedGraphEdge::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUndirectedGraphEdge::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUndirectedGraphEdge" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUndirectedGraphEdge::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUndirectedGraphEdge" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgGraphNodeList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgGraphNodeList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgGraphNodeList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgGraphNodeList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgGraphNodeList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgGraphNodeList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgGraphNodeList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgGraphEdgeList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgGraphEdgeList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgGraphEdgeList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgGraphEdgeList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgGraphEdgeList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgGraphEdgeList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgGraphEdgeList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTypeTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgTypeTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgTypeTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgTypeTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgTypeTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgTypeTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgTypeTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgNameGroup::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgNameGroup::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgNameGroup::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgNameGroup::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgNameGroup" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgNameGroup::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgNameGroup" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDimensionObject::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDimensionObject::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDimensionObject::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDimensionObject::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDimensionObject" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDimensionObject::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDimensionObject" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFormatItem::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFormatItem::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFormatItem::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFormatItem::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFormatItem" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFormatItem::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFormatItem" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFormatItemList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFormatItemList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFormatItemList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFormatItemList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFormatItemList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFormatItemList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFormatItemList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDataStatementGroup::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDataStatementGroup::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDataStatementGroup::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDataStatementGroup::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDataStatementGroup" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDataStatementGroup::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDataStatementGroup" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDataStatementObject::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDataStatementObject::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDataStatementObject::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDataStatementObject::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDataStatementObject" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDataStatementObject::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDataStatementObject" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDataStatementValue::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDataStatementValue::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDataStatementValue::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDataStatementValue::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDataStatementValue" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDataStatementValue::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDataStatementValue" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgType::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgType::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgType::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgType::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgTypeUnknown::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeUnknown::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeUnknown::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeUnknown::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeUnknown::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeChar::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeChar::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeChar::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeChar::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeChar::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeSignedChar::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeSignedChar::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeSignedChar::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeSignedChar::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeSignedChar::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeUnsignedChar::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeUnsignedChar::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeUnsignedChar::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeUnsignedChar::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeUnsignedChar::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeShort::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeShort::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeShort::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeShort::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeShort::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeSignedShort::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeSignedShort::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeSignedShort::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeSignedShort::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeSignedShort::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeUnsignedShort::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeUnsignedShort::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeUnsignedShort::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeUnsignedShort::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeUnsignedShort::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeInt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeInt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeInt::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeInt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeInt::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeSignedInt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeSignedInt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeSignedInt::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeSignedInt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeSignedInt::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeUnsignedInt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeUnsignedInt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeUnsignedInt::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeUnsignedInt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeUnsignedInt::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeLong::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeLong::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeLong::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeLong::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeLong::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeSignedLong::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeSignedLong::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeSignedLong::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeSignedLong::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeSignedLong::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeUnsignedLong::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeUnsignedLong::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeUnsignedLong::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeUnsignedLong::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeUnsignedLong::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeVoid::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeVoid::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeVoid::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeVoid::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeVoid::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeGlobalVoid::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeGlobalVoid::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeGlobalVoid::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeGlobalVoid::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeGlobalVoid::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeWchar::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeWchar::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeWchar::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeWchar::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeWchar::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeFloat::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeFloat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeFloat::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeFloat::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeFloat::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeDouble::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeDouble::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeDouble::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeDouble::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeDouble::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeLongLong::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeLongLong::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeLongLong::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeLongLong::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeLongLong::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeSignedLongLong::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeSignedLongLong::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeSignedLongLong::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeSignedLongLong::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeSignedLongLong::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeUnsignedLongLong::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeUnsignedLongLong::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeUnsignedLongLong::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeUnsignedLongLong::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeUnsignedLongLong::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeLongDouble::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeLongDouble::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeLongDouble::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeLongDouble::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeLongDouble::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeString::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lengthExpression);
return traversalSuccessorContainer;
}
vector<string>
SgTypeString::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_lengthExpression");
return traversalSuccessorContainer;
}
size_t
SgTypeString::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeString::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_lengthExpression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeString::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_lengthExpression) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeBool::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeBool::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeBool::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeBool::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeBool::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgPointerType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgPointerType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgPointerType::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgPointerType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPointerType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgPointerMemberType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgPointerMemberType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgPointerMemberType::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgPointerMemberType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPointerMemberType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgReferenceType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgReferenceType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgReferenceType::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgReferenceType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgReferenceType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgNamedType::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNamedType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgNamedType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNamedType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgNamedType::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNamedType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgNamedType::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNamedType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgNamedType::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgNamedType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgClassType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
/* typeString = SgDeclarationStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgClassType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgClassType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgClassType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
case 6: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgClassType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else if (child == p_declaration) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgEnumType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
/* typeString = SgDeclarationStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgEnumType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgEnumType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgEnumType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
case 6: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgEnumType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else if (child == p_declaration) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgTypedefType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
/* typeString = SgDeclarationStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgTypedefType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgTypedefType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgTypedefType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
case 6: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypedefType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else if (child == p_declaration) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgModifierType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgModifierType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgModifierType::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgModifierType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgModifierType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgFunctionType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_return_type);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_orig_return_type);
return traversalSuccessorContainer;
}
vector<string>
SgFunctionType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_return_type");
traversalSuccessorContainer.push_back("p_orig_return_type");
return traversalSuccessorContainer;
}
size_t
SgFunctionType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgFunctionType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_return_type;
case 6: return p_orig_return_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFunctionType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_return_type) return 5;
else if (child == p_orig_return_type) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgMemberFunctionType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_return_type);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_orig_return_type);
return traversalSuccessorContainer;
}
vector<string>
SgMemberFunctionType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_return_type");
traversalSuccessorContainer.push_back("p_orig_return_type");
return traversalSuccessorContainer;
}
size_t
SgMemberFunctionType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgMemberFunctionType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_return_type;
case 6: return p_orig_return_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMemberFunctionType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_return_type) return 5;
else if (child == p_orig_return_type) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgPartialFunctionType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_return_type);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_orig_return_type);
return traversalSuccessorContainer;
}
vector<string>
SgPartialFunctionType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_return_type");
traversalSuccessorContainer.push_back("p_orig_return_type");
return traversalSuccessorContainer;
}
size_t
SgPartialFunctionType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgPartialFunctionType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_return_type;
case 6: return p_orig_return_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPartialFunctionType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_return_type) return 5;
else if (child == p_orig_return_type) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgPartialFunctionModifierType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(8);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_return_type);
/* typeString = SgType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_orig_return_type);
/* typeString = static SgPartialFunctionModifierType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgPartialFunctionModifierType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_return_type");
traversalSuccessorContainer.push_back("p_orig_return_type");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgPartialFunctionModifierType::get_numberOfTraversalSuccessors() {
return 8;
}
SgNode *
SgPartialFunctionModifierType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_return_type;
case 6: return p_orig_return_type;
case 7: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPartialFunctionModifierType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_return_type) return 5;
else if (child == p_orig_return_type) return 6;
else if (child == p_builtin_type) return 7;
else return (size_t) -1;
}
vector<SgNode*>
SgArrayType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(7);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_index);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dim_info);
return traversalSuccessorContainer;
}
vector<string>
SgArrayType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_index");
traversalSuccessorContainer.push_back("p_dim_info");
return traversalSuccessorContainer;
}
size_t
SgArrayType::get_numberOfTraversalSuccessors() {
return 7;
}
SgNode *
SgArrayType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_index;
case 6: return p_dim_info;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgArrayType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_index) return 5;
else if (child == p_dim_info) return 6;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeEllipse::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeEllipse::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeEllipse::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeEllipse::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeEllipse::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgTemplateType::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgTemplateType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgQualifiedNameType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgQualifiedNameType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgQualifiedNameType::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgQualifiedNameType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgQualifiedNameType::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeComplex::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgTypeComplex::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgTypeComplex::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgTypeComplex::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeComplex::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeImaginary::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
return traversalSuccessorContainer;
}
vector<string>
SgTypeImaginary::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
return traversalSuccessorContainer;
}
size_t
SgTypeImaginary::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgTypeImaginary::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeImaginary::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeDefault::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeDefault::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeDefault::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeDefault::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeDefault::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeCAFTeam::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeCAFTeam::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeCAFTeam::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeCAFTeam::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeCAFTeam::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeCrayPointer::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeCrayPointer::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeCrayPointer::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeCrayPointer::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeCrayPointer::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeLabel::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgReferenceType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ref_to);
/* typeString = SgPointerType* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ptr_to);
/* typeString = SgModifierNodes* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_modifiers);
/* typeString = SgTypedefSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_typedefs);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_type_kind);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgTypeLabel::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ref_to");
traversalSuccessorContainer.push_back("p_ptr_to");
traversalSuccessorContainer.push_back("p_modifiers");
traversalSuccessorContainer.push_back("p_typedefs");
traversalSuccessorContainer.push_back("p_type_kind");
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgTypeLabel::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgTypeLabel::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ref_to;
case 1: return p_ptr_to;
case 2: return p_modifiers;
case 3: return p_typedefs;
case 4: return p_type_kind;
case 5: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeLabel::get_childIndex(SgNode *child) {
if (child == p_ref_to) return 0;
else if (child == p_ptr_to) return 1;
else if (child == p_modifiers) return 2;
else if (child == p_typedefs) return 3;
else if (child == p_type_kind) return 4;
else if (child == p_builtin_type) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgLocatedNode::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgLocatedNode::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgLocatedNode::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgLocatedNode::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgLocatedNode::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgLocatedNodeSupport::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNodeSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgLocatedNodeSupport::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNodeSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgLocatedNodeSupport::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNodeSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgLocatedNodeSupport::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNodeSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgLocatedNodeSupport::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgLocatedNodeSupport" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgCommonBlockObject::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_variable_reference_list);
return traversalSuccessorContainer;
}
vector<string>
SgCommonBlockObject::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_variable_reference_list");
return traversalSuccessorContainer;
}
size_t
SgCommonBlockObject::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgCommonBlockObject::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_variable_reference_list;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCommonBlockObject::get_childIndex(SgNode *child) {
if (child == p_variable_reference_list) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgInitializedName::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgInitializer* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_initptr);
return traversalSuccessorContainer;
}
vector<string>
SgInitializedName::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_initptr");
return traversalSuccessorContainer;
}
size_t
SgInitializedName::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgInitializedName::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_initptr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgInitializedName::get_childIndex(SgNode *child) {
if (child == p_initptr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgInterfaceBody::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgInterfaceBody::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgInterfaceBody::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgInterfaceBody::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgInterfaceBody" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgInterfaceBody::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgInterfaceBody" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgRenamePair::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgRenamePair::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgRenamePair::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgRenamePair::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgRenamePair" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgRenamePair::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgRenamePair" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpClause::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgOmpClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgOmpClause::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgOmpClause::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgOmpClause::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgOmpOrderedClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOmpOrderedClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOmpOrderedClause::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOmpOrderedClause::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOmpOrderedClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOmpOrderedClause::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOmpOrderedClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpNowaitClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOmpNowaitClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOmpNowaitClause::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOmpNowaitClause::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOmpNowaitClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOmpNowaitClause::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOmpNowaitClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpUntiedClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOmpUntiedClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOmpUntiedClause::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOmpUntiedClause::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOmpUntiedClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOmpUntiedClause::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOmpUntiedClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpDefaultClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOmpDefaultClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOmpDefaultClause::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOmpDefaultClause::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOmpDefaultClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOmpDefaultClause::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOmpDefaultClause" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpExpressionClause::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpExpressionClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgOmpExpressionClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpExpressionClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgOmpExpressionClause::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpExpressionClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgOmpExpressionClause::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpExpressionClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgOmpExpressionClause::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpExpressionClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgOmpCollapseClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgOmpCollapseClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgOmpCollapseClause::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpCollapseClause::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpCollapseClause::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpIfClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgOmpIfClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgOmpIfClause::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpIfClause::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpIfClause::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpNumThreadsClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgOmpNumThreadsClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgOmpNumThreadsClause::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpNumThreadsClause::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpNumThreadsClause::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpVariablesClause::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpVariablesClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgOmpVariablesClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpVariablesClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgOmpVariablesClause::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpVariablesClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgOmpVariablesClause::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpVariablesClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgOmpVariablesClause::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpVariablesClause" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgOmpCopyprivateClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpCopyprivateClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpCopyprivateClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpCopyprivateClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpCopyprivateClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpPrivateClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpPrivateClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpPrivateClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpPrivateClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpPrivateClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpFirstprivateClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpFirstprivateClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpFirstprivateClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpFirstprivateClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpFirstprivateClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpSharedClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpSharedClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpSharedClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpSharedClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpSharedClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpCopyinClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpCopyinClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpCopyinClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpCopyinClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpCopyinClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpLastprivateClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpLastprivateClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpLastprivateClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpLastprivateClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpLastprivateClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpReductionClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpReductionClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpReductionClause::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpReductionClause::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpReductionClause::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpScheduleClause::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_chunk_size);
return traversalSuccessorContainer;
}
vector<string>
SgOmpScheduleClause::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_chunk_size");
return traversalSuccessorContainer;
}
size_t
SgOmpScheduleClause::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpScheduleClause::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_chunk_size;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpScheduleClause::get_childIndex(SgNode *child) {
if (child == p_chunk_size) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgScopeStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgScopeStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgScopeStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgScopeStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgScopeStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgScopeStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgScopeStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgScopeStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgScopeStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgScopeStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgGlobal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_declarations.size() + 0);
/* typeString = SgDeclarationStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgDeclarationStatementPtrList::iterator iter;
     for (iter = p_declarations.begin(); iter != p_declarations.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgGlobal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgDeclarationStatementPtrList::iterator  iter;
     for (iter = p_declarations.begin(); iter != p_declarations.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgGlobal::get_numberOfTraversalSuccessors() {
return p_declarations.size() + 0;
}
SgNode *
SgGlobal::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_declarations.size());
return p_declarations[idx];
}
size_t
SgGlobal::get_childIndex(SgNode *child) {
SgDeclarationStatementPtrList::iterator itr = find(p_declarations.begin(), p_declarations.end(), child);
if (itr != p_declarations.end()) return itr - p_declarations.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgBasicBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_statements.size() + 0);
/* typeString = SgStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgStatementPtrList::iterator iter;
     for (iter = p_statements.begin(); iter != p_statements.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgBasicBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgStatementPtrList::iterator  iter;
     for (iter = p_statements.begin(); iter != p_statements.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgBasicBlock::get_numberOfTraversalSuccessors() {
return p_statements.size() + 0;
}
SgNode *
SgBasicBlock::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_statements.size());
return p_statements[idx];
}
size_t
SgBasicBlock::get_childIndex(SgNode *child) {
SgStatementPtrList::iterator itr = find(p_statements.begin(), p_statements.end(), child);
if (itr != p_statements.end()) return itr - p_statements.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgIfStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_conditional);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_true_body);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_false_body);
return traversalSuccessorContainer;
}
vector<string>
SgIfStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_conditional");
traversalSuccessorContainer.push_back("p_true_body");
traversalSuccessorContainer.push_back("p_false_body");
return traversalSuccessorContainer;
}
size_t
SgIfStmt::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgIfStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_conditional;
case 1: return p_true_body;
case 2: return p_false_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgIfStmt::get_childIndex(SgNode *child) {
if (child == p_conditional) return 0;
else if (child == p_true_body) return 1;
else if (child == p_false_body) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgForStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgForInitStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_for_init_stmt);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_test);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_increment);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_loop_body);
return traversalSuccessorContainer;
}
vector<string>
SgForStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_for_init_stmt");
traversalSuccessorContainer.push_back("p_test");
traversalSuccessorContainer.push_back("p_increment");
traversalSuccessorContainer.push_back("p_loop_body");
return traversalSuccessorContainer;
}
size_t
SgForStatement::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgForStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_for_init_stmt;
case 1: return p_test;
case 2: return p_increment;
case 3: return p_loop_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgForStatement::get_childIndex(SgNode *child) {
if (child == p_for_init_stmt) return 0;
else if (child == p_test) return 1;
else if (child == p_increment) return 2;
else if (child == p_loop_body) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgFunctionDefinition::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgFunctionDefinition::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgFunctionDefinition::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgFunctionDefinition::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFunctionDefinition::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgClassDefinition::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_members.size() + 0);
/* typeString = SgDeclarationStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgDeclarationStatementPtrList::iterator iter;
     for (iter = p_members.begin(); iter != p_members.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgClassDefinition::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgDeclarationStatementPtrList::iterator  iter;
     for (iter = p_members.begin(); iter != p_members.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgClassDefinition::get_numberOfTraversalSuccessors() {
return p_members.size() + 0;
}
SgNode *
SgClassDefinition::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_members.size());
return p_members[idx];
}
size_t
SgClassDefinition::get_childIndex(SgNode *child) {
SgDeclarationStatementPtrList::iterator itr = find(p_members.begin(), p_members.end(), child);
if (itr != p_members.end()) return itr - p_members.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateInstantiationDefn::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_members.size() + 0);
/* typeString = SgDeclarationStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgDeclarationStatementPtrList::iterator iter;
     for (iter = p_members.begin(); iter != p_members.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgTemplateInstantiationDefn::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgDeclarationStatementPtrList::iterator  iter;
     for (iter = p_members.begin(); iter != p_members.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgTemplateInstantiationDefn::get_numberOfTraversalSuccessors() {
return p_members.size() + 0;
}
SgNode *
SgTemplateInstantiationDefn::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_members.size());
return p_members[idx];
}
size_t
SgTemplateInstantiationDefn::get_childIndex(SgNode *child) {
SgDeclarationStatementPtrList::iterator itr = find(p_members.begin(), p_members.end(), child);
if (itr != p_members.end()) return itr - p_members.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgWhileStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_condition);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgWhileStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_condition");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgWhileStmt::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgWhileStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_condition;
case 1: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgWhileStmt::get_childIndex(SgNode *child) {
if (child == p_condition) return 0;
else if (child == p_body) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDoWhileStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_condition);
return traversalSuccessorContainer;
}
vector<string>
SgDoWhileStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
traversalSuccessorContainer.push_back("p_condition");
return traversalSuccessorContainer;
}
size_t
SgDoWhileStmt::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDoWhileStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
case 1: return p_condition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDoWhileStmt::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else if (child == p_condition) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgSwitchStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_item_selector);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgSwitchStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_item_selector");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgSwitchStatement::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgSwitchStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_item_selector;
case 1: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgSwitchStatement::get_childIndex(SgNode *child) {
if (child == p_item_selector) return 0;
else if (child == p_body) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgCatchOptionStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgVariableDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_condition);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgCatchOptionStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_condition");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgCatchOptionStmt::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgCatchOptionStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_condition;
case 1: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCatchOptionStmt::get_childIndex(SgNode *child) {
if (child == p_condition) return 0;
else if (child == p_body) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgNamespaceDefinitionStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_declarations.size() + 0);
/* typeString = SgDeclarationStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgDeclarationStatementPtrList::iterator iter;
     for (iter = p_declarations.begin(); iter != p_declarations.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgNamespaceDefinitionStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgDeclarationStatementPtrList::iterator  iter;
     for (iter = p_declarations.begin(); iter != p_declarations.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgNamespaceDefinitionStatement::get_numberOfTraversalSuccessors() {
return p_declarations.size() + 0;
}
SgNode *
SgNamespaceDefinitionStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_declarations.size());
return p_declarations[idx];
}
size_t
SgNamespaceDefinitionStatement::get_childIndex(SgNode *child) {
SgDeclarationStatementPtrList::iterator itr = find(p_declarations.begin(), p_declarations.end(), child);
if (itr != p_declarations.end()) return itr - p_declarations.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgBlockDataStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgBlockDataStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgBlockDataStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgBlockDataStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgBlockDataStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgBlockDataStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgBlockDataStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAssociateStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgVariableDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_variable_declaration);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAssociateStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_variable_declaration");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAssociateStatement::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAssociateStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_variable_declaration;
case 1: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAssociateStatement::get_childIndex(SgNode *child) {
if (child == p_variable_declaration) return 0;
else if (child == p_body) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgFortranDo::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_initialization);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_bound);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_increment);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgFortranDo::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_initialization");
traversalSuccessorContainer.push_back("p_bound");
traversalSuccessorContainer.push_back("p_increment");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgFortranDo::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgFortranDo::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_initialization;
case 1: return p_bound;
case 2: return p_increment;
case 3: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFortranDo::get_childIndex(SgNode *child) {
if (child == p_initialization) return 0;
else if (child == p_bound) return 1;
else if (child == p_increment) return 2;
else if (child == p_body) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgFortranNonblockedDo::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_initialization);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_bound);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_increment);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgFortranNonblockedDo::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_initialization");
traversalSuccessorContainer.push_back("p_bound");
traversalSuccessorContainer.push_back("p_increment");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgFortranNonblockedDo::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgFortranNonblockedDo::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_initialization;
case 1: return p_bound;
case 2: return p_increment;
case 3: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFortranNonblockedDo::get_childIndex(SgNode *child) {
if (child == p_initialization) return 0;
else if (child == p_bound) return 1;
else if (child == p_increment) return 2;
else if (child == p_body) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgForAllStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_forall_header);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgForAllStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_forall_header");
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgForAllStatement::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgForAllStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_forall_header;
case 1: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgForAllStatement::get_childIndex(SgNode *child) {
if (child == p_forall_header) return 0;
else if (child == p_body) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcForAllStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgForInitStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_for_init_stmt);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_test);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_increment);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_affinity);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_loop_body);
return traversalSuccessorContainer;
}
vector<string>
SgUpcForAllStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_for_init_stmt");
traversalSuccessorContainer.push_back("p_test");
traversalSuccessorContainer.push_back("p_increment");
traversalSuccessorContainer.push_back("p_affinity");
traversalSuccessorContainer.push_back("p_loop_body");
return traversalSuccessorContainer;
}
size_t
SgUpcForAllStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgUpcForAllStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_for_init_stmt;
case 1: return p_test;
case 2: return p_increment;
case 3: return p_affinity;
case 4: return p_loop_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcForAllStatement::get_childIndex(SgNode *child) {
if (child == p_for_init_stmt) return 0;
else if (child == p_test) return 1;
else if (child == p_increment) return 2;
else if (child == p_affinity) return 3;
else if (child == p_loop_body) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgCAFWithTeamStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgCAFWithTeamStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgCAFWithTeamStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgCAFWithTeamStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCAFWithTeamStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgFunctionTypeTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFunctionTypeTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFunctionTypeTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFunctionTypeTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFunctionTypeTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFunctionTypeTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFunctionTypeTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDeclarationStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgDeclarationStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgDeclarationStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgDeclarationStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgDeclarationStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgFunctionParameterList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_args.size() + 0);
/* typeString = SgInitializedNamePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgInitializedNamePtrList::iterator iter;
     for (iter = p_args.begin(); iter != p_args.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgFunctionParameterList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgInitializedNamePtrList::iterator  iter;
     for (iter = p_args.begin(); iter != p_args.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgFunctionParameterList::get_numberOfTraversalSuccessors() {
return p_args.size() + 0;
}
SgNode *
SgFunctionParameterList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_args.size());
return p_args[idx];
}
size_t
SgFunctionParameterList::get_childIndex(SgNode *child) {
SgInitializedNamePtrList::iterator itr = find(p_args.begin(), p_args.end(), child);
if (itr != p_args.end()) return itr - p_args.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgVariableDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 1);
traversalSuccessorContainer.push_back(compute_baseTypeDefiningDeclaration());
/* typeString = SgInitializedNamePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgInitializedNamePtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgVariableDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_baseTypeDefiningDeclaration");
   {
     SgInitializedNamePtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgVariableDeclaration::get_numberOfTraversalSuccessors() {
return p_variables.size() + 1;
}
SgNode *
SgVariableDeclaration::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return compute_baseTypeDefiningDeclaration();
else return p_variables[idx-1];
}
size_t
SgVariableDeclaration::get_childIndex(SgNode *child) {
if (child == compute_baseTypeDefiningDeclaration()) return 0;
else {
SgInitializedNamePtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return (itr - p_variables.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgVariableDefinition::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgInitializedName* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_vardefn);
/* typeString = SgUnsignedLongVal* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_bitfield);
return traversalSuccessorContainer;
}
vector<string>
SgVariableDefinition::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_vardefn");
traversalSuccessorContainer.push_back("p_bitfield");
return traversalSuccessorContainer;
}
size_t
SgVariableDefinition::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgVariableDefinition::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_vardefn;
case 1: return p_bitfield;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgVariableDefinition::get_childIndex(SgNode *child) {
if (child == p_vardefn) return 0;
else if (child == p_bitfield) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgClinkageDeclarationStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgClinkageDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgClinkageDeclarationStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgClinkageDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgClinkageDeclarationStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgClinkageDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgClinkageDeclarationStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgClinkageDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgClinkageDeclarationStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgClinkageDeclarationStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgClinkageStartStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgClinkageStartStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgClinkageStartStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgClinkageStartStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgClinkageStartStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgClinkageStartStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgClinkageStartStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgClinkageEndStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgClinkageEndStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgClinkageEndStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgClinkageEndStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgClinkageEndStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgClinkageEndStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgClinkageEndStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgEnumDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_enumerators.size() + 0);
/* typeString = SgInitializedNamePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgInitializedNamePtrList::iterator iter;
     for (iter = p_enumerators.begin(); iter != p_enumerators.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgEnumDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgInitializedNamePtrList::iterator  iter;
     for (iter = p_enumerators.begin(); iter != p_enumerators.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgEnumDeclaration::get_numberOfTraversalSuccessors() {
return p_enumerators.size() + 0;
}
SgNode *
SgEnumDeclaration::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_enumerators.size());
return p_enumerators[idx];
}
size_t
SgEnumDeclaration::get_childIndex(SgNode *child) {
SgInitializedNamePtrList::iterator itr = find(p_enumerators.begin(), p_enumerators.end(), child);
if (itr != p_enumerators.end()) return itr - p_enumerators.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_operands.size() + 0);
/* typeString = SgExpressionPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgExpressionPtrList::iterator iter;
     for (iter = p_operands.begin(); iter != p_operands.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgExpressionPtrList::iterator  iter;
     for (iter = p_operands.begin(); iter != p_operands.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmStmt::get_numberOfTraversalSuccessors() {
return p_operands.size() + 0;
}
SgNode *
SgAsmStmt::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_operands.size());
return p_operands[idx];
}
size_t
SgAsmStmt::get_childIndex(SgNode *child) {
SgExpressionPtrList::iterator itr = find(p_operands.begin(), p_operands.end(), child);
if (itr != p_operands.end()) return itr - p_operands.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAttributeSpecificationStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAttributeSpecificationStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAttributeSpecificationStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAttributeSpecificationStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAttributeSpecificationStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAttributeSpecificationStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAttributeSpecificationStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFormatStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFormatStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFormatStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFormatStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFormatStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFormatStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFormatStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTemplateDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgTemplateDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgTemplateDeclaration::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgTemplateDeclaration::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgTemplateDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgTemplateDeclaration::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgTemplateDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTemplateInstantiationDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgDeclarationStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateInstantiationDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgTemplateInstantiationDirectiveStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgTemplateInstantiationDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateInstantiationDirectiveStatement::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUseStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_rename_list.size() + 0);
/* typeString = SgRenamePairPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgRenamePairPtrList::iterator iter;
     for (iter = p_rename_list.begin(); iter != p_rename_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgUseStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgRenamePairPtrList::iterator  iter;
     for (iter = p_rename_list.begin(); iter != p_rename_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgUseStatement::get_numberOfTraversalSuccessors() {
return p_rename_list.size() + 0;
}
SgNode *
SgUseStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_rename_list.size());
return p_rename_list[idx];
}
size_t
SgUseStatement::get_childIndex(SgNode *child) {
SgRenamePairPtrList::iterator itr = find(p_rename_list.begin(), p_rename_list.end(), child);
if (itr != p_rename_list.end()) return itr - p_rename_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgParameterStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgParameterStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgParameterStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgParameterStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgParameterStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgParameterStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgParameterStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgNamespaceDeclarationStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgNamespaceDefinitionStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
return traversalSuccessorContainer;
}
vector<string>
SgNamespaceDeclarationStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgNamespaceDeclarationStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgNamespaceDeclarationStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_definition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgNamespaceDeclarationStatement::get_childIndex(SgNode *child) {
if (child == p_definition) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgEquivalenceStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgEquivalenceStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgEquivalenceStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgEquivalenceStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgEquivalenceStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgEquivalenceStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgEquivalenceStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgInterfaceStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_interface_body_list.size() + 0);
/* typeString = SgInterfaceBodyPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgInterfaceBodyPtrList::iterator iter;
     for (iter = p_interface_body_list.begin(); iter != p_interface_body_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgInterfaceStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgInterfaceBodyPtrList::iterator  iter;
     for (iter = p_interface_body_list.begin(); iter != p_interface_body_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgInterfaceStatement::get_numberOfTraversalSuccessors() {
return p_interface_body_list.size() + 0;
}
SgNode *
SgInterfaceStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_interface_body_list.size());
return p_interface_body_list[idx];
}
size_t
SgInterfaceStatement::get_childIndex(SgNode *child) {
SgInterfaceBodyPtrList::iterator itr = find(p_interface_body_list.begin(), p_interface_body_list.end(), child);
if (itr != p_interface_body_list.end()) return itr - p_interface_body_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgNamespaceAliasDeclarationStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgNamespaceAliasDeclarationStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgNamespaceAliasDeclarationStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgNamespaceAliasDeclarationStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgNamespaceAliasDeclarationStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgNamespaceAliasDeclarationStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgNamespaceAliasDeclarationStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgCommonBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_block_list.size() + 0);
/* typeString = SgCommonBlockObjectPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgCommonBlockObjectPtrList::iterator iter;
     for (iter = p_block_list.begin(); iter != p_block_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgCommonBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgCommonBlockObjectPtrList::iterator  iter;
     for (iter = p_block_list.begin(); iter != p_block_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgCommonBlock::get_numberOfTraversalSuccessors() {
return p_block_list.size() + 0;
}
SgNode *
SgCommonBlock::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_block_list.size());
return p_block_list[idx];
}
size_t
SgCommonBlock::get_childIndex(SgNode *child) {
SgCommonBlockObjectPtrList::iterator itr = find(p_block_list.begin(), p_block_list.end(), child);
if (itr != p_block_list.end()) return itr - p_block_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgTypedefDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
traversalSuccessorContainer.push_back(compute_baseTypeDefiningDeclaration());
return traversalSuccessorContainer;
}
vector<string>
SgTypedefDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgTypedefDeclaration::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgTypedefDeclaration::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return compute_baseTypeDefiningDeclaration();
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypedefDeclaration::get_childIndex(SgNode *child) {
if (child == compute_baseTypeDefiningDeclaration()) return 0;
else if (child == p_declaration) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgStatementFunctionStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgFunctionDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_function);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgStatementFunctionStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_function");
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgStatementFunctionStatement::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgStatementFunctionStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_function;
case 1: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgStatementFunctionStatement::get_childIndex(SgNode *child) {
if (child == p_function) return 0;
else if (child == p_expression) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgCtorInitializerList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_ctors.size() + 0);
/* typeString = SgInitializedNamePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgInitializedNamePtrList::iterator iter;
     for (iter = p_ctors.begin(); iter != p_ctors.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgCtorInitializerList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgInitializedNamePtrList::iterator  iter;
     for (iter = p_ctors.begin(); iter != p_ctors.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgCtorInitializerList::get_numberOfTraversalSuccessors() {
return p_ctors.size() + 0;
}
SgNode *
SgCtorInitializerList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_ctors.size());
return p_ctors[idx];
}
size_t
SgCtorInitializerList::get_childIndex(SgNode *child) {
SgInitializedNamePtrList::iterator itr = find(p_ctors.begin(), p_ctors.end(), child);
if (itr != p_ctors.end()) return itr - p_ctors.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgPragmaDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgPragma* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_pragma);
return traversalSuccessorContainer;
}
vector<string>
SgPragmaDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_pragma");
return traversalSuccessorContainer;
}
size_t
SgPragmaDeclaration::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgPragmaDeclaration::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_pragma;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPragmaDeclaration::get_childIndex(SgNode *child) {
if (child == p_pragma) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUsingDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUsingDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUsingDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUsingDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUsingDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUsingDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUsingDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgClassDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
traversalSuccessorContainer.push_back(compute_classDefinition());
return traversalSuccessorContainer;
}
vector<string>
SgClassDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgClassDeclaration::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgClassDeclaration::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return compute_classDefinition();
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgClassDeclaration::get_childIndex(SgNode *child) {
if (child == compute_classDefinition()) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateInstantiationDecl::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
traversalSuccessorContainer.push_back(compute_classDefinition());
return traversalSuccessorContainer;
}
vector<string>
SgTemplateInstantiationDecl::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgTemplateInstantiationDecl::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgTemplateInstantiationDecl::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return compute_classDefinition();
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateInstantiationDecl::get_childIndex(SgNode *child) {
if (child == compute_classDefinition()) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgDerivedTypeStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgClassDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
return traversalSuccessorContainer;
}
vector<string>
SgDerivedTypeStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgDerivedTypeStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgDerivedTypeStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_definition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDerivedTypeStatement::get_childIndex(SgNode *child) {
if (child == p_definition) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgModuleStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgClassDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
return traversalSuccessorContainer;
}
vector<string>
SgModuleStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgModuleStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgModuleStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_definition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgModuleStatement::get_childIndex(SgNode *child) {
if (child == p_definition) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgImplicitStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgInitializedNamePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgInitializedNamePtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgImplicitStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgInitializedNamePtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgImplicitStatement::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgImplicitStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgImplicitStatement::get_childIndex(SgNode *child) {
SgInitializedNamePtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgUsingDeclarationStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUsingDeclarationStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUsingDeclarationStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUsingDeclarationStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUsingDeclarationStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUsingDeclarationStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUsingDeclarationStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgNamelistStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgNamelistStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgNamelistStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgNamelistStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgNamelistStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgNamelistStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgNamelistStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgImportStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgImportStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgImportStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgImportStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgImportStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgImportStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgImportStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFunctionDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
return traversalSuccessorContainer;
}
vector<string>
SgFunctionDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgFunctionDeclaration::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgFunctionDeclaration::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFunctionDeclaration::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgMemberFunctionDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
/* typeString = SgCtorInitializerList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_CtorInitializerList);
return traversalSuccessorContainer;
}
vector<string>
SgMemberFunctionDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
traversalSuccessorContainer.push_back("p_CtorInitializerList");
return traversalSuccessorContainer;
}
size_t
SgMemberFunctionDeclaration::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgMemberFunctionDeclaration::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
case 2: return p_CtorInitializerList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMemberFunctionDeclaration::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else if (child == p_CtorInitializerList) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateInstantiationMemberFunctionDecl::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
/* typeString = SgCtorInitializerList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_CtorInitializerList);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateInstantiationMemberFunctionDecl::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
traversalSuccessorContainer.push_back("p_CtorInitializerList");
return traversalSuccessorContainer;
}
size_t
SgTemplateInstantiationMemberFunctionDecl::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgTemplateInstantiationMemberFunctionDecl::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
case 2: return p_CtorInitializerList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateInstantiationMemberFunctionDecl::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else if (child == p_CtorInitializerList) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateInstantiationFunctionDecl::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateInstantiationFunctionDecl::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgTemplateInstantiationFunctionDecl::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgTemplateInstantiationFunctionDecl::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateInstantiationFunctionDecl::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgProgramHeaderStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
return traversalSuccessorContainer;
}
vector<string>
SgProgramHeaderStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
return traversalSuccessorContainer;
}
size_t
SgProgramHeaderStatement::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgProgramHeaderStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgProgramHeaderStatement::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgProcedureHeaderStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
/* typeString = SgInitializedName* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_result_name);
return traversalSuccessorContainer;
}
vector<string>
SgProcedureHeaderStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
traversalSuccessorContainer.push_back("p_result_name");
return traversalSuccessorContainer;
}
size_t
SgProcedureHeaderStatement::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgProcedureHeaderStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
case 2: return p_result_name;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgProcedureHeaderStatement::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else if (child == p_result_name) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgEntryStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgFunctionParameterList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_parameterList);
/* typeString = SgFunctionDefinition* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_definition);
/* typeString = SgInitializedName* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_result_name);
return traversalSuccessorContainer;
}
vector<string>
SgEntryStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_parameterList");
traversalSuccessorContainer.push_back("p_definition");
traversalSuccessorContainer.push_back("p_result_name");
return traversalSuccessorContainer;
}
size_t
SgEntryStatement::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgEntryStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_parameterList;
case 1: return p_definition;
case 2: return p_result_name;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgEntryStatement::get_childIndex(SgNode *child) {
if (child == p_parameterList) return 0;
else if (child == p_definition) return 1;
else if (child == p_result_name) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgContainsStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgContainsStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgContainsStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgContainsStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgContainsStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgContainsStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgContainsStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgC_PreprocessorDirectiveStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgC_PreprocessorDirectiveStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgC_PreprocessorDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgC_PreprocessorDirectiveStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgC_PreprocessorDirectiveStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgC_PreprocessorDirectiveStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgC_PreprocessorDirectiveStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgC_PreprocessorDirectiveStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgC_PreprocessorDirectiveStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgC_PreprocessorDirectiveStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgIncludeDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIncludeDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIncludeDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIncludeDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIncludeDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIncludeDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIncludeDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDefineDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDefineDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDefineDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDefineDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDefineDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDefineDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDefineDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgUndefDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUndefDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUndefDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUndefDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUndefDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUndefDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUndefDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIfdefDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIfdefDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIfdefDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIfdefDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIfdefDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIfdefDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIfdefDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIfndefDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIfndefDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIfndefDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIfndefDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIfndefDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIfndefDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIfndefDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIfDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIfDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIfDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIfDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIfDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIfDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIfDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgDeadIfDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDeadIfDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDeadIfDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDeadIfDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDeadIfDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDeadIfDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDeadIfDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgElseDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgElseDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgElseDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgElseDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgElseDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgElseDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgElseDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgElseifDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgElseifDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgElseifDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgElseifDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgElseifDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgElseifDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgElseifDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgEndifDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgEndifDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgEndifDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgEndifDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgEndifDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgEndifDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgEndifDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgLineDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgLineDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgLineDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgLineDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgLineDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgLineDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgLineDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgWarningDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgWarningDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgWarningDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgWarningDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgWarningDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgWarningDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgWarningDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgErrorDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgErrorDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgErrorDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgErrorDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgErrorDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgErrorDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgErrorDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgEmptyDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgEmptyDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgEmptyDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgEmptyDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgEmptyDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgEmptyDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgEmptyDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIncludeNextDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIncludeNextDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIncludeNextDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIncludeNextDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIncludeNextDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIncludeNextDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIncludeNextDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIdentDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIdentDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIdentDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIdentDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIdentDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIdentDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIdentDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgLinemarkerDirectiveStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgLinemarkerDirectiveStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgLinemarkerDirectiveStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgLinemarkerDirectiveStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgLinemarkerDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgLinemarkerDirectiveStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgLinemarkerDirectiveStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpThreadprivateStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpThreadprivateStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpThreadprivateStatement::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpThreadprivateStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpThreadprivateStatement::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgFortranIncludeLine::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFortranIncludeLine::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFortranIncludeLine::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFortranIncludeLine::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFortranIncludeLine" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFortranIncludeLine::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFortranIncludeLine" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgJavaImportStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgJavaImportStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgJavaImportStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgJavaImportStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgJavaImportStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgJavaImportStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgJavaImportStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgExprStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgExprStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgExprStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgExprStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgExprStatement::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgLabelStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgLabelStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgLabelStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgLabelStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgLabelStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgLabelStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgLabelStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgCaseOptionStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_key);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_key_range_end);
return traversalSuccessorContainer;
}
vector<string>
SgCaseOptionStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_key");
traversalSuccessorContainer.push_back("p_body");
traversalSuccessorContainer.push_back("p_key_range_end");
return traversalSuccessorContainer;
}
size_t
SgCaseOptionStmt::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgCaseOptionStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_key;
case 1: return p_body;
case 2: return p_key_range_end;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCaseOptionStmt::get_childIndex(SgNode *child) {
if (child == p_key) return 0;
else if (child == p_body) return 1;
else if (child == p_key_range_end) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgTryStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgCatchStatementSeq* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_catch_statement_seq_root);
return traversalSuccessorContainer;
}
vector<string>
SgTryStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
traversalSuccessorContainer.push_back("p_catch_statement_seq_root");
return traversalSuccessorContainer;
}
size_t
SgTryStmt::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgTryStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
case 1: return p_catch_statement_seq_root;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTryStmt::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else if (child == p_catch_statement_seq_root) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDefaultOptionStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgDefaultOptionStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgDefaultOptionStmt::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgDefaultOptionStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDefaultOptionStmt::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgBreakStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgBreakStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgBreakStmt::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgBreakStmt::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgBreakStmt" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgBreakStmt::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgBreakStmt" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgContinueStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgContinueStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgContinueStmt::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgContinueStmt::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgContinueStmt" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgContinueStmt::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgContinueStmt" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgReturnStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgReturnStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgReturnStmt::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgReturnStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgReturnStmt::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgGotoStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgGotoStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgGotoStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgGotoStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgGotoStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgGotoStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgGotoStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgSpawnStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgFunctionCallExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_the_func);
return traversalSuccessorContainer;
}
vector<string>
SgSpawnStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_the_func");
return traversalSuccessorContainer;
}
size_t
SgSpawnStmt::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgSpawnStmt::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_the_func;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgSpawnStmt::get_childIndex(SgNode *child) {
if (child == p_the_func) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgNullStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgNullStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgNullStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgNullStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgNullStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgNullStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgNullStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgVariantStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgVariantStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgVariantStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgVariantStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgVariantStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgVariantStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgVariantStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgForInitStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_init_stmt.size() + 0);
/* typeString = SgStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgStatementPtrList::iterator iter;
     for (iter = p_init_stmt.begin(); iter != p_init_stmt.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgForInitStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgStatementPtrList::iterator  iter;
     for (iter = p_init_stmt.begin(); iter != p_init_stmt.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgForInitStatement::get_numberOfTraversalSuccessors() {
return p_init_stmt.size() + 0;
}
SgNode *
SgForInitStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_init_stmt.size());
return p_init_stmt[idx];
}
size_t
SgForInitStatement::get_childIndex(SgNode *child) {
SgStatementPtrList::iterator itr = find(p_init_stmt.begin(), p_init_stmt.end(), child);
if (itr != p_init_stmt.end()) return itr - p_init_stmt.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgCatchStatementSeq::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_catch_statement_seq.size() + 0);
/* typeString = SgStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgStatementPtrList::iterator iter;
     for (iter = p_catch_statement_seq.begin(); iter != p_catch_statement_seq.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgCatchStatementSeq::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgStatementPtrList::iterator  iter;
     for (iter = p_catch_statement_seq.begin(); iter != p_catch_statement_seq.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgCatchStatementSeq::get_numberOfTraversalSuccessors() {
return p_catch_statement_seq.size() + 0;
}
SgNode *
SgCatchStatementSeq::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_catch_statement_seq.size());
return p_catch_statement_seq[idx];
}
size_t
SgCatchStatementSeq::get_childIndex(SgNode *child) {
SgStatementPtrList::iterator itr = find(p_catch_statement_seq.begin(), p_catch_statement_seq.end(), child);
if (itr != p_catch_statement_seq.end()) return itr - p_catch_statement_seq.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgStopOrPauseStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgStopOrPauseStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgStopOrPauseStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgStopOrPauseStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgStopOrPauseStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgStopOrPauseStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgStopOrPauseStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgIOStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIOStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgIOStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIOStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgIOStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIOStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgIOStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIOStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgIOStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgIOStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgPrintStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_format);
return traversalSuccessorContainer;
}
vector<string>
SgPrintStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
traversalSuccessorContainer.push_back("p_format");
return traversalSuccessorContainer;
}
size_t
SgPrintStatement::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgPrintStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
case 5: return p_format;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPrintStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else if (child == p_format) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgReadStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(13);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_format);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rec);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_end);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_namelist);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_advance);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_size);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_eor);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_asynchronous);
return traversalSuccessorContainer;
}
vector<string>
SgReadStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
traversalSuccessorContainer.push_back("p_format");
traversalSuccessorContainer.push_back("p_rec");
traversalSuccessorContainer.push_back("p_end");
traversalSuccessorContainer.push_back("p_namelist");
traversalSuccessorContainer.push_back("p_advance");
traversalSuccessorContainer.push_back("p_size");
traversalSuccessorContainer.push_back("p_eor");
traversalSuccessorContainer.push_back("p_asynchronous");
return traversalSuccessorContainer;
}
size_t
SgReadStatement::get_numberOfTraversalSuccessors() {
return 13;
}
SgNode *
SgReadStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
case 5: return p_format;
case 6: return p_rec;
case 7: return p_end;
case 8: return p_namelist;
case 9: return p_advance;
case 10: return p_size;
case 11: return p_eor;
case 12: return p_asynchronous;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgReadStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else if (child == p_format) return 5;
else if (child == p_rec) return 6;
else if (child == p_end) return 7;
else if (child == p_namelist) return 8;
else if (child == p_advance) return 9;
else if (child == p_size) return 10;
else if (child == p_eor) return 11;
else if (child == p_asynchronous) return 12;
else return (size_t) -1;
}
vector<SgNode*>
SgWriteStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(10);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_format);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rec);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_namelist);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_advance);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_asynchronous);
return traversalSuccessorContainer;
}
vector<string>
SgWriteStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
traversalSuccessorContainer.push_back("p_format");
traversalSuccessorContainer.push_back("p_rec");
traversalSuccessorContainer.push_back("p_namelist");
traversalSuccessorContainer.push_back("p_advance");
traversalSuccessorContainer.push_back("p_asynchronous");
return traversalSuccessorContainer;
}
size_t
SgWriteStatement::get_numberOfTraversalSuccessors() {
return 10;
}
SgNode *
SgWriteStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
case 5: return p_format;
case 6: return p_rec;
case 7: return p_namelist;
case 8: return p_advance;
case 9: return p_asynchronous;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgWriteStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else if (child == p_format) return 5;
else if (child == p_rec) return 6;
else if (child == p_namelist) return 7;
else if (child == p_advance) return 8;
else if (child == p_asynchronous) return 9;
else return (size_t) -1;
}
vector<SgNode*>
SgOpenStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(18);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_file);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_status);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_access);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_form);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_recl);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_blank);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_position);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_action);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_delim);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_pad);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_round);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sign);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_asynchronous);
return traversalSuccessorContainer;
}
vector<string>
SgOpenStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
traversalSuccessorContainer.push_back("p_file");
traversalSuccessorContainer.push_back("p_status");
traversalSuccessorContainer.push_back("p_access");
traversalSuccessorContainer.push_back("p_form");
traversalSuccessorContainer.push_back("p_recl");
traversalSuccessorContainer.push_back("p_blank");
traversalSuccessorContainer.push_back("p_position");
traversalSuccessorContainer.push_back("p_action");
traversalSuccessorContainer.push_back("p_delim");
traversalSuccessorContainer.push_back("p_pad");
traversalSuccessorContainer.push_back("p_round");
traversalSuccessorContainer.push_back("p_sign");
traversalSuccessorContainer.push_back("p_asynchronous");
return traversalSuccessorContainer;
}
size_t
SgOpenStatement::get_numberOfTraversalSuccessors() {
return 18;
}
SgNode *
SgOpenStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
case 5: return p_file;
case 6: return p_status;
case 7: return p_access;
case 8: return p_form;
case 9: return p_recl;
case 10: return p_blank;
case 11: return p_position;
case 12: return p_action;
case 13: return p_delim;
case 14: return p_pad;
case 15: return p_round;
case 16: return p_sign;
case 17: return p_asynchronous;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOpenStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else if (child == p_file) return 5;
else if (child == p_status) return 6;
else if (child == p_access) return 7;
else if (child == p_form) return 8;
else if (child == p_recl) return 9;
else if (child == p_blank) return 10;
else if (child == p_position) return 11;
else if (child == p_action) return 12;
else if (child == p_delim) return 13;
else if (child == p_pad) return 14;
else if (child == p_round) return 15;
else if (child == p_sign) return 16;
else if (child == p_asynchronous) return 17;
else return (size_t) -1;
}
vector<SgNode*>
SgCloseStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(6);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_status);
return traversalSuccessorContainer;
}
vector<string>
SgCloseStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
traversalSuccessorContainer.push_back("p_status");
return traversalSuccessorContainer;
}
size_t
SgCloseStatement::get_numberOfTraversalSuccessors() {
return 6;
}
SgNode *
SgCloseStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
case 5: return p_status;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCloseStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else if (child == p_status) return 5;
else return (size_t) -1;
}
vector<SgNode*>
SgInquireStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
return traversalSuccessorContainer;
}
vector<string>
SgInquireStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
return traversalSuccessorContainer;
}
size_t
SgInquireStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgInquireStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgInquireStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgFlushStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
return traversalSuccessorContainer;
}
vector<string>
SgFlushStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
return traversalSuccessorContainer;
}
size_t
SgFlushStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgFlushStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFlushStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgBackspaceStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
return traversalSuccessorContainer;
}
vector<string>
SgBackspaceStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
return traversalSuccessorContainer;
}
size_t
SgBackspaceStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgBackspaceStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBackspaceStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgRewindStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
return traversalSuccessorContainer;
}
vector<string>
SgRewindStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
return traversalSuccessorContainer;
}
size_t
SgRewindStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgRewindStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgRewindStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgEndfileStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
return traversalSuccessorContainer;
}
vector<string>
SgEndfileStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
return traversalSuccessorContainer;
}
size_t
SgEndfileStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgEndfileStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgEndfileStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgWaitStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(5);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_stmt_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unit);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iostat);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_err);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iomsg);
return traversalSuccessorContainer;
}
vector<string>
SgWaitStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_stmt_list");
traversalSuccessorContainer.push_back("p_unit");
traversalSuccessorContainer.push_back("p_iostat");
traversalSuccessorContainer.push_back("p_err");
traversalSuccessorContainer.push_back("p_iomsg");
return traversalSuccessorContainer;
}
size_t
SgWaitStatement::get_numberOfTraversalSuccessors() {
return 5;
}
SgNode *
SgWaitStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_stmt_list;
case 1: return p_unit;
case 2: return p_iostat;
case 3: return p_err;
case 4: return p_iomsg;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgWaitStatement::get_childIndex(SgNode *child) {
if (child == p_io_stmt_list) return 0;
else if (child == p_unit) return 1;
else if (child == p_iostat) return 2;
else if (child == p_err) return 3;
else if (child == p_iomsg) return 4;
else return (size_t) -1;
}
vector<SgNode*>
SgWhereStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_condition);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgElseWhereStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_elsewhere);
return traversalSuccessorContainer;
}
vector<string>
SgWhereStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_condition");
traversalSuccessorContainer.push_back("p_body");
traversalSuccessorContainer.push_back("p_elsewhere");
return traversalSuccessorContainer;
}
size_t
SgWhereStatement::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgWhereStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_condition;
case 1: return p_body;
case 2: return p_elsewhere;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgWhereStatement::get_childIndex(SgNode *child) {
if (child == p_condition) return 0;
else if (child == p_body) return 1;
else if (child == p_elsewhere) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgElseWhereStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_condition);
/* typeString = SgBasicBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgElseWhereStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_elsewhere);
return traversalSuccessorContainer;
}
vector<string>
SgElseWhereStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_condition");
traversalSuccessorContainer.push_back("p_body");
traversalSuccessorContainer.push_back("p_elsewhere");
return traversalSuccessorContainer;
}
size_t
SgElseWhereStatement::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgElseWhereStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_condition;
case 1: return p_body;
case 2: return p_elsewhere;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgElseWhereStatement::get_childIndex(SgNode *child) {
if (child == p_condition) return 0;
else if (child == p_body) return 1;
else if (child == p_elsewhere) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgNullifyStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_pointer_list);
return traversalSuccessorContainer;
}
vector<string>
SgNullifyStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_pointer_list");
return traversalSuccessorContainer;
}
size_t
SgNullifyStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgNullifyStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_pointer_list;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgNullifyStatement::get_childIndex(SgNode *child) {
if (child == p_pointer_list) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgArithmeticIfStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_conditional);
return traversalSuccessorContainer;
}
vector<string>
SgArithmeticIfStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_conditional");
return traversalSuccessorContainer;
}
size_t
SgArithmeticIfStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgArithmeticIfStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_conditional;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgArithmeticIfStatement::get_childIndex(SgNode *child) {
if (child == p_conditional) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAssignStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_value);
return traversalSuccessorContainer;
}
vector<string>
SgAssignStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_value");
return traversalSuccessorContainer;
}
size_t
SgAssignStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAssignStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_value;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAssignStatement::get_childIndex(SgNode *child) {
if (child == p_value) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgComputedGotoStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_labelList);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_label_index);
return traversalSuccessorContainer;
}
vector<string>
SgComputedGotoStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_labelList");
traversalSuccessorContainer.push_back("p_label_index");
return traversalSuccessorContainer;
}
size_t
SgComputedGotoStatement::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgComputedGotoStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_labelList;
case 1: return p_label_index;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgComputedGotoStatement::get_childIndex(SgNode *child) {
if (child == p_labelList) return 0;
else if (child == p_label_index) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAssignedGotoStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_targets);
return traversalSuccessorContainer;
}
vector<string>
SgAssignedGotoStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_targets");
return traversalSuccessorContainer;
}
size_t
SgAssignedGotoStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAssignedGotoStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_targets;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAssignedGotoStatement::get_childIndex(SgNode *child) {
if (child == p_targets) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAllocateStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expr_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_stat_expression);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_errmsg_expression);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_source_expression);
return traversalSuccessorContainer;
}
vector<string>
SgAllocateStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expr_list");
traversalSuccessorContainer.push_back("p_stat_expression");
traversalSuccessorContainer.push_back("p_errmsg_expression");
traversalSuccessorContainer.push_back("p_source_expression");
return traversalSuccessorContainer;
}
size_t
SgAllocateStatement::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgAllocateStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expr_list;
case 1: return p_stat_expression;
case 2: return p_errmsg_expression;
case 3: return p_source_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAllocateStatement::get_childIndex(SgNode *child) {
if (child == p_expr_list) return 0;
else if (child == p_stat_expression) return 1;
else if (child == p_errmsg_expression) return 2;
else if (child == p_source_expression) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgDeallocateStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expr_list);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_stat_expression);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_errmsg_expression);
return traversalSuccessorContainer;
}
vector<string>
SgDeallocateStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expr_list");
traversalSuccessorContainer.push_back("p_stat_expression");
traversalSuccessorContainer.push_back("p_errmsg_expression");
return traversalSuccessorContainer;
}
size_t
SgDeallocateStatement::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgDeallocateStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expr_list;
case 1: return p_stat_expression;
case 2: return p_errmsg_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDeallocateStatement::get_childIndex(SgNode *child) {
if (child == p_expr_list) return 0;
else if (child == p_stat_expression) return 1;
else if (child == p_errmsg_expression) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcNotifyStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_notify_expression);
return traversalSuccessorContainer;
}
vector<string>
SgUpcNotifyStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_notify_expression");
return traversalSuccessorContainer;
}
size_t
SgUpcNotifyStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcNotifyStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_notify_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcNotifyStatement::get_childIndex(SgNode *child) {
if (child == p_notify_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcWaitStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_wait_expression);
return traversalSuccessorContainer;
}
vector<string>
SgUpcWaitStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_wait_expression");
return traversalSuccessorContainer;
}
size_t
SgUpcWaitStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcWaitStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_wait_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcWaitStatement::get_childIndex(SgNode *child) {
if (child == p_wait_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcBarrierStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_barrier_expression);
return traversalSuccessorContainer;
}
vector<string>
SgUpcBarrierStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_barrier_expression");
return traversalSuccessorContainer;
}
size_t
SgUpcBarrierStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcBarrierStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_barrier_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcBarrierStatement::get_childIndex(SgNode *child) {
if (child == p_barrier_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcFenceStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgUpcFenceStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgUpcFenceStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgUpcFenceStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgUpcFenceStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgUpcFenceStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgUpcFenceStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpBarrierStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOmpBarrierStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOmpBarrierStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOmpBarrierStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOmpBarrierStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOmpBarrierStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOmpBarrierStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpTaskwaitStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgOmpTaskwaitStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgOmpTaskwaitStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgOmpTaskwaitStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgOmpTaskwaitStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgOmpTaskwaitStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgOmpTaskwaitStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgOmpFlushStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_variables.size() + 0);
/* typeString = SgVarRefExpPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgVarRefExpPtrList::iterator iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpFlushStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgVarRefExpPtrList::iterator  iter;
     for (iter = p_variables.begin(); iter != p_variables.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpFlushStatement::get_numberOfTraversalSuccessors() {
return p_variables.size() + 0;
}
SgNode *
SgOmpFlushStatement::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_variables.size());
return p_variables[idx];
}
size_t
SgOmpFlushStatement::get_childIndex(SgNode *child) {
SgVarRefExpPtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);
if (itr != p_variables.end()) return itr - p_variables.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgOmpBodyStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgOmpBodyStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgOmpBodyStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgOmpBodyStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgOmpBodyStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgOmpAtomicStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgOmpAtomicStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgOmpAtomicStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpAtomicStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpAtomicStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpMasterStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgOmpMasterStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgOmpMasterStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpMasterStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpMasterStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpOrderedStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgOmpOrderedStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgOmpOrderedStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpOrderedStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpOrderedStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpCriticalStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgOmpCriticalStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgOmpCriticalStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpCriticalStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpCriticalStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpSectionStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgOmpSectionStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgOmpSectionStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpSectionStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpSectionStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpWorkshareStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgOmpWorkshareStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgOmpWorkshareStatement::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgOmpWorkshareStatement::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOmpWorkshareStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgOmpClauseBodyStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClauseBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgOmpClauseBodyStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClauseBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgOmpClauseBodyStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClauseBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgOmpClauseBodyStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClauseBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgOmpClauseBodyStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgOmpClauseBodyStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgOmpParallelStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_clauses.size() + 1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgOmpClausePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgOmpClausePtrList::iterator iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpParallelStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_body");
   {
     SgOmpClausePtrList::iterator  iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpParallelStatement::get_numberOfTraversalSuccessors() {
return p_clauses.size() + 1;
}
SgNode *
SgOmpParallelStatement::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return p_body;
else return p_clauses[idx-1];
}
size_t
SgOmpParallelStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else {
SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);
if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgOmpSingleStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_clauses.size() + 1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgOmpClausePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgOmpClausePtrList::iterator iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpSingleStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_body");
   {
     SgOmpClausePtrList::iterator  iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpSingleStatement::get_numberOfTraversalSuccessors() {
return p_clauses.size() + 1;
}
SgNode *
SgOmpSingleStatement::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return p_body;
else return p_clauses[idx-1];
}
size_t
SgOmpSingleStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else {
SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);
if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgOmpTaskStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_clauses.size() + 1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgOmpClausePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgOmpClausePtrList::iterator iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpTaskStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_body");
   {
     SgOmpClausePtrList::iterator  iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpTaskStatement::get_numberOfTraversalSuccessors() {
return p_clauses.size() + 1;
}
SgNode *
SgOmpTaskStatement::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return p_body;
else return p_clauses[idx-1];
}
size_t
SgOmpTaskStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else {
SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);
if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgOmpForStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_clauses.size() + 1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgOmpClausePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgOmpClausePtrList::iterator iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpForStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_body");
   {
     SgOmpClausePtrList::iterator  iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpForStatement::get_numberOfTraversalSuccessors() {
return p_clauses.size() + 1;
}
SgNode *
SgOmpForStatement::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return p_body;
else return p_clauses[idx-1];
}
size_t
SgOmpForStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else {
SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);
if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgOmpDoStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_clauses.size() + 1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgOmpClausePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgOmpClausePtrList::iterator iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpDoStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_body");
   {
     SgOmpClausePtrList::iterator  iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpDoStatement::get_numberOfTraversalSuccessors() {
return p_clauses.size() + 1;
}
SgNode *
SgOmpDoStatement::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return p_body;
else return p_clauses[idx-1];
}
size_t
SgOmpDoStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else {
SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);
if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgOmpSectionsStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_clauses.size() + 1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
/* typeString = SgOmpClausePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgOmpClausePtrList::iterator iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgOmpSectionsStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 1;
traversalSuccessorContainer.push_back("p_body");
   {
     SgOmpClausePtrList::iterator  iter;
     for (iter = p_clauses.begin(); iter != p_clauses.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgOmpSectionsStatement::get_numberOfTraversalSuccessors() {
return p_clauses.size() + 1;
}
SgNode *
SgOmpSectionsStatement::get_traversalSuccessorByIndex(size_t idx) {
if (idx == 0) return p_body;
else return p_clauses[idx-1];
}
size_t
SgOmpSectionsStatement::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else {
SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);
if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;
else return (size_t) -1;
}
}
vector<SgNode*>
SgSequenceStatement::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgSequenceStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgSequenceStatement::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgSequenceStatement::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgSequenceStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgSequenceStatement::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgSequenceStatement" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgExpression::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgExpression::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgExpression::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgExpression::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgUnaryOp::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgUnaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgUnaryOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgUnaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgUnaryOp::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgUnaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgUnaryOp::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgUnaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgUnaryOp::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgUnaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgExpressionRoot::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgExpressionRoot::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgExpressionRoot::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgExpressionRoot::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgExpressionRoot::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgMinusOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgMinusOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgMinusOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgMinusOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMinusOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnaryAddOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgUnaryAddOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgUnaryAddOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnaryAddOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnaryAddOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgNotOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgNotOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgNotOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgNotOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgNotOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgPointerDerefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgPointerDerefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgPointerDerefExp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgPointerDerefExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPointerDerefExp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAddressOfOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgAddressOfOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgAddressOfOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAddressOfOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAddressOfOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgMinusMinusOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgMinusMinusOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgMinusMinusOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgMinusMinusOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMinusMinusOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgPlusPlusOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgPlusPlusOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgPlusPlusOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgPlusPlusOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPlusPlusOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgBitComplementOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgBitComplementOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgBitComplementOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgBitComplementOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBitComplementOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgCastExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgCastExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgCastExp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgCastExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
case 1: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCastExp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else if (child == p_originalExpressionTree) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgThrowOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgThrowOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgThrowOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgThrowOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgThrowOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgRealPartOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgRealPartOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgRealPartOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgRealPartOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgRealPartOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgImagPartOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgImagPartOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgImagPartOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgImagPartOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgImagPartOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgConjugateOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgConjugateOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgConjugateOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgConjugateOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgConjugateOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUserDefinedUnaryOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgUserDefinedUnaryOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgUserDefinedUnaryOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUserDefinedUnaryOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUserDefinedUnaryOp::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgBinaryOp::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBinaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgBinaryOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBinaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgBinaryOp::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBinaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgBinaryOp::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBinaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgBinaryOp::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgBinaryOp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgArrowExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgArrowExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgArrowExp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgArrowExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgArrowExp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDotExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgDotExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgDotExp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDotExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDotExp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDotStarOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgDotStarOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgDotStarOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDotStarOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDotStarOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgArrowStarOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgArrowStarOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgArrowStarOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgArrowStarOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgArrowStarOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgEqualityOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgEqualityOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgEqualityOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgEqualityOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgEqualityOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgLessThanOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgLessThanOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgLessThanOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgLessThanOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLessThanOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgGreaterThanOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgGreaterThanOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgGreaterThanOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgGreaterThanOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgGreaterThanOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgNotEqualOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgNotEqualOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgNotEqualOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgNotEqualOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgNotEqualOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgLessOrEqualOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgLessOrEqualOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgLessOrEqualOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgLessOrEqualOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLessOrEqualOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgGreaterOrEqualOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgGreaterOrEqualOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgGreaterOrEqualOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgGreaterOrEqualOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgGreaterOrEqualOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAddOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgAddOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgAddOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAddOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAddOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgSubtractOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgSubtractOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgSubtractOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgSubtractOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgSubtractOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgMultiplyOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgMultiplyOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgMultiplyOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgMultiplyOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMultiplyOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDivideOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgDivideOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgDivideOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDivideOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDivideOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgIntegerDivideOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgIntegerDivideOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgIntegerDivideOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgIntegerDivideOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgIntegerDivideOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgModOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgModOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgModOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgModOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgModOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAndOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgAndOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgAndOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAndOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAndOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgOrOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgOrOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgOrOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgOrOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgOrOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgBitXorOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgBitXorOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgBitXorOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgBitXorOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBitXorOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgBitAndOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgBitAndOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgBitAndOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgBitAndOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBitAndOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgBitOrOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgBitOrOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgBitOrOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgBitOrOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBitOrOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgCommaOpExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgCommaOpExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgCommaOpExp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgCommaOpExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCommaOpExp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgLshiftOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgLshiftOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgLshiftOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgLshiftOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLshiftOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgRshiftOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgRshiftOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgRshiftOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgRshiftOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgRshiftOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgPntrArrRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgPntrArrRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgPntrArrRefExp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgPntrArrRefExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPntrArrRefExp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgScopeOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgScopeOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgScopeOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgScopeOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgScopeOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgPlusAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgPlusAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgPlusAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgPlusAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPlusAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgMinusAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgMinusAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgMinusAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgMinusAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMinusAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAndAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgAndAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgAndAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAndAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAndAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgIorAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgIorAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgIorAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgIorAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgIorAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgMultAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgMultAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgMultAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgMultAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMultAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgDivAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgDivAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgDivAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDivAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDivAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgModAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgModAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgModAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgModAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgModAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgXorAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgXorAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgXorAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgXorAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgXorAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgLshiftAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgLshiftAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgLshiftAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgLshiftAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLshiftAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgRshiftAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgRshiftAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgRshiftAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgRshiftAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgRshiftAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgExponentiationOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgExponentiationOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgExponentiationOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgExponentiationOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgExponentiationOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgConcatenationOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgConcatenationOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgConcatenationOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgConcatenationOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgConcatenationOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgPointerAssignOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgPointerAssignOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgPointerAssignOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgPointerAssignOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgPointerAssignOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgUserDefinedBinaryOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand_i);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgUserDefinedBinaryOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand_i");
traversalSuccessorContainer.push_back("p_rhs_operand_i");
return traversalSuccessorContainer;
}
size_t
SgUserDefinedBinaryOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgUserDefinedBinaryOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand_i;
case 1: return p_rhs_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUserDefinedBinaryOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand_i) return 0;
else if (child == p_rhs_operand_i) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgExprListExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_expressions.size() + 0);
/* typeString = SgExpressionPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgExpressionPtrList::iterator iter;
     for (iter = p_expressions.begin(); iter != p_expressions.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgExprListExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgExpressionPtrList::iterator  iter;
     for (iter = p_expressions.begin(); iter != p_expressions.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgExprListExp::get_numberOfTraversalSuccessors() {
return p_expressions.size() + 0;
}
SgNode *
SgExprListExp::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_expressions.size());
return p_expressions[idx];
}
size_t
SgExprListExp::get_childIndex(SgNode *child) {
SgExpressionPtrList::iterator itr = find(p_expressions.begin(), p_expressions.end(), child);
if (itr != p_expressions.end()) return itr - p_expressions.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgVarRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgVarRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgVarRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgVarRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgVarRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgVarRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgVarRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgClassNameRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgClassNameRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgClassNameRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgClassNameRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgClassNameRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgClassNameRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgClassNameRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFunctionRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFunctionRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFunctionRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFunctionRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFunctionRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFunctionRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFunctionRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgMemberFunctionRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgMemberFunctionRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgMemberFunctionRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgMemberFunctionRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgMemberFunctionRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgMemberFunctionRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgMemberFunctionRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgValueExp::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgValueExp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgValueExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgValueExp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgValueExp::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgValueExp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgValueExp::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgValueExp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgValueExp::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgValueExp" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgBoolValExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgBoolValExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgBoolValExp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgBoolValExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgBoolValExp::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgStringVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgStringVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgStringVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgStringVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgStringVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgShortVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgShortVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgShortVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgShortVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgShortVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgCharVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgCharVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgCharVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgCharVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCharVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnsignedCharVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUnsignedCharVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUnsignedCharVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnsignedCharVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnsignedCharVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgWcharVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgWcharVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgWcharVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgWcharVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgWcharVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnsignedShortVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUnsignedShortVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUnsignedShortVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnsignedShortVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnsignedShortVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgIntVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgIntVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgIntVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgIntVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgIntVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgEnumVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgEnumVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgEnumVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgEnumVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgEnumVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnsignedIntVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUnsignedIntVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUnsignedIntVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnsignedIntVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnsignedIntVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgLongIntVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgLongIntVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgLongIntVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgLongIntVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLongIntVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgLongLongIntVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgLongLongIntVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgLongLongIntVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgLongLongIntVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLongLongIntVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnsignedLongLongIntVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUnsignedLongLongIntVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUnsignedLongLongIntVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnsignedLongLongIntVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnsignedLongLongIntVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnsignedLongVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUnsignedLongVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUnsignedLongVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUnsignedLongVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnsignedLongVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgFloatVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgFloatVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgFloatVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgFloatVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFloatVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgDoubleVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgDoubleVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgDoubleVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgDoubleVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDoubleVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgLongDoubleVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgLongDoubleVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgLongDoubleVal::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgLongDoubleVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLongDoubleVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgComplexVal::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
/* typeString = SgValueExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_real_value);
/* typeString = SgValueExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_imaginary_value);
return traversalSuccessorContainer;
}
vector<string>
SgComplexVal::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
traversalSuccessorContainer.push_back("p_real_value");
traversalSuccessorContainer.push_back("p_imaginary_value");
return traversalSuccessorContainer;
}
size_t
SgComplexVal::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgComplexVal::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
case 1: return p_real_value;
case 2: return p_imaginary_value;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgComplexVal::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else if (child == p_real_value) return 1;
else if (child == p_imaginary_value) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcThreads::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUpcThreads::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUpcThreads::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcThreads::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcThreads::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcMythread::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_originalExpressionTree);
return traversalSuccessorContainer;
}
vector<string>
SgUpcMythread::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_originalExpressionTree");
return traversalSuccessorContainer;
}
size_t
SgUpcMythread::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcMythread::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_originalExpressionTree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcMythread::get_childIndex(SgNode *child) {
if (child == p_originalExpressionTree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgFunctionCallExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_function);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_args);
return traversalSuccessorContainer;
}
vector<string>
SgFunctionCallExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_function");
traversalSuccessorContainer.push_back("p_args");
return traversalSuccessorContainer;
}
size_t
SgFunctionCallExp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgFunctionCallExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_function;
case 1: return p_args;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFunctionCallExp::get_childIndex(SgNode *child) {
if (child == p_function) return 0;
else if (child == p_args) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgSizeOfOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_expr);
return traversalSuccessorContainer;
}
vector<string>
SgSizeOfOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_expr");
return traversalSuccessorContainer;
}
size_t
SgSizeOfOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgSizeOfOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_expr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgSizeOfOp::get_childIndex(SgNode *child) {
if (child == p_operand_expr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcLocalsizeofExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgUpcLocalsizeofExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgUpcLocalsizeofExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcLocalsizeofExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcLocalsizeofExpression::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcBlocksizeofExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgUpcBlocksizeofExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgUpcBlocksizeofExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcBlocksizeofExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcBlocksizeofExpression::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUpcElemsizeofExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgUpcElemsizeofExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgUpcElemsizeofExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgUpcElemsizeofExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUpcElemsizeofExpression::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgTypeIdOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_expr);
return traversalSuccessorContainer;
}
vector<string>
SgTypeIdOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_expr");
return traversalSuccessorContainer;
}
size_t
SgTypeIdOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgTypeIdOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_expr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypeIdOp::get_childIndex(SgNode *child) {
if (child == p_operand_expr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgConditionalExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_conditional_exp);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_true_exp);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_false_exp);
return traversalSuccessorContainer;
}
vector<string>
SgConditionalExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_conditional_exp");
traversalSuccessorContainer.push_back("p_true_exp");
traversalSuccessorContainer.push_back("p_false_exp");
return traversalSuccessorContainer;
}
size_t
SgConditionalExp::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgConditionalExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_conditional_exp;
case 1: return p_true_exp;
case 2: return p_false_exp;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgConditionalExp::get_childIndex(SgNode *child) {
if (child == p_conditional_exp) return 0;
else if (child == p_true_exp) return 1;
else if (child == p_false_exp) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgNewExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_placement_args);
/* typeString = SgConstructorInitializer* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_constructor_args);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_args);
return traversalSuccessorContainer;
}
vector<string>
SgNewExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_placement_args");
traversalSuccessorContainer.push_back("p_constructor_args");
traversalSuccessorContainer.push_back("p_builtin_args");
return traversalSuccessorContainer;
}
size_t
SgNewExp::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgNewExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_placement_args;
case 1: return p_constructor_args;
case 2: return p_builtin_args;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgNewExp::get_childIndex(SgNode *child) {
if (child == p_placement_args) return 0;
else if (child == p_constructor_args) return 1;
else if (child == p_builtin_args) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgDeleteExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_variable);
return traversalSuccessorContainer;
}
vector<string>
SgDeleteExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_variable");
return traversalSuccessorContainer;
}
size_t
SgDeleteExp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgDeleteExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_variable;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDeleteExp::get_childIndex(SgNode *child) {
if (child == p_variable) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgThisExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgThisExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgThisExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgThisExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgThisExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgThisExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgThisExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgInitializer::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgInitializer" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgInitializer::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgInitializer" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgInitializer::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgInitializer" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgInitializer::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgInitializer" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgInitializer::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgInitializer" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAggregateInitializer::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_initializers);
return traversalSuccessorContainer;
}
vector<string>
SgAggregateInitializer::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_initializers");
return traversalSuccessorContainer;
}
size_t
SgAggregateInitializer::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAggregateInitializer::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_initializers;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAggregateInitializer::get_childIndex(SgNode *child) {
if (child == p_initializers) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgConstructorInitializer::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_args);
return traversalSuccessorContainer;
}
vector<string>
SgConstructorInitializer::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_args");
return traversalSuccessorContainer;
}
size_t
SgConstructorInitializer::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgConstructorInitializer::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_args;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgConstructorInitializer::get_childIndex(SgNode *child) {
if (child == p_args) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAssignInitializer::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_i);
return traversalSuccessorContainer;
}
vector<string>
SgAssignInitializer::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_i");
return traversalSuccessorContainer;
}
size_t
SgAssignInitializer::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAssignInitializer::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_i;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAssignInitializer::get_childIndex(SgNode *child) {
if (child == p_operand_i) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgDesignatedInitializer::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_designatorList);
/* typeString = SgInitializer* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_memberInit);
return traversalSuccessorContainer;
}
vector<string>
SgDesignatedInitializer::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_designatorList");
traversalSuccessorContainer.push_back("p_memberInit");
return traversalSuccessorContainer;
}
size_t
SgDesignatedInitializer::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgDesignatedInitializer::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_designatorList;
case 1: return p_memberInit;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgDesignatedInitializer::get_childIndex(SgNode *child) {
if (child == p_designatorList) return 0;
else if (child == p_memberInit) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgVarArgStartOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand);
return traversalSuccessorContainer;
}
vector<string>
SgVarArgStartOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand");
traversalSuccessorContainer.push_back("p_rhs_operand");
return traversalSuccessorContainer;
}
size_t
SgVarArgStartOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgVarArgStartOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand;
case 1: return p_rhs_operand;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgVarArgStartOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand) return 0;
else if (child == p_rhs_operand) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgVarArgOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_expr);
return traversalSuccessorContainer;
}
vector<string>
SgVarArgOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_expr");
return traversalSuccessorContainer;
}
size_t
SgVarArgOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgVarArgOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_expr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgVarArgOp::get_childIndex(SgNode *child) {
if (child == p_operand_expr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgVarArgEndOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_expr);
return traversalSuccessorContainer;
}
vector<string>
SgVarArgEndOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_expr");
return traversalSuccessorContainer;
}
size_t
SgVarArgEndOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgVarArgEndOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_expr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgVarArgEndOp::get_childIndex(SgNode *child) {
if (child == p_operand_expr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgVarArgCopyOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs_operand);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs_operand);
return traversalSuccessorContainer;
}
vector<string>
SgVarArgCopyOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs_operand");
traversalSuccessorContainer.push_back("p_rhs_operand");
return traversalSuccessorContainer;
}
size_t
SgVarArgCopyOp::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgVarArgCopyOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs_operand;
case 1: return p_rhs_operand;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgVarArgCopyOp::get_childIndex(SgNode *child) {
if (child == p_lhs_operand) return 0;
else if (child == p_rhs_operand) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgVarArgStartOneOperandOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand_expr);
return traversalSuccessorContainer;
}
vector<string>
SgVarArgStartOneOperandOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand_expr");
return traversalSuccessorContainer;
}
size_t
SgVarArgStartOneOperandOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgVarArgStartOneOperandOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand_expr;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgVarArgStartOneOperandOp::get_childIndex(SgNode *child) {
if (child == p_operand_expr) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgNullExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgNullExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgNullExpression::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgNullExpression::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgNullExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgNullExpression::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgNullExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgVariantExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgVariantExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgVariantExpression::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgVariantExpression::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgVariantExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgVariantExpression::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgVariantExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgSubscriptExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lowerBound);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_upperBound);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_stride);
return traversalSuccessorContainer;
}
vector<string>
SgSubscriptExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lowerBound");
traversalSuccessorContainer.push_back("p_upperBound");
traversalSuccessorContainer.push_back("p_stride");
return traversalSuccessorContainer;
}
size_t
SgSubscriptExpression::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgSubscriptExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lowerBound;
case 1: return p_upperBound;
case 2: return p_stride;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgSubscriptExpression::get_childIndex(SgNode *child) {
if (child == p_lowerBound) return 0;
else if (child == p_upperBound) return 1;
else if (child == p_stride) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgColonShapeExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgColonShapeExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgColonShapeExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgColonShapeExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgColonShapeExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgColonShapeExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgColonShapeExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsteriskShapeExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsteriskShapeExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsteriskShapeExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsteriskShapeExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsteriskShapeExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsteriskShapeExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsteriskShapeExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgImpliedDo::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_do_var_initialization);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_last_val);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_increment);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_object_list);
return traversalSuccessorContainer;
}
vector<string>
SgImpliedDo::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_do_var_initialization");
traversalSuccessorContainer.push_back("p_last_val");
traversalSuccessorContainer.push_back("p_increment");
traversalSuccessorContainer.push_back("p_object_list");
return traversalSuccessorContainer;
}
size_t
SgImpliedDo::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgImpliedDo::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_do_var_initialization;
case 1: return p_last_val;
case 2: return p_increment;
case 3: return p_object_list;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgImpliedDo::get_childIndex(SgNode *child) {
if (child == p_do_var_initialization) return 0;
else if (child == p_last_val) return 1;
else if (child == p_increment) return 2;
else if (child == p_object_list) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgIOItemExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_io_item);
return traversalSuccessorContainer;
}
vector<string>
SgIOItemExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_io_item");
return traversalSuccessorContainer;
}
size_t
SgIOItemExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgIOItemExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_io_item;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgIOItemExpression::get_childIndex(SgNode *child) {
if (child == p_io_item) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgStatementExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_statement);
return traversalSuccessorContainer;
}
vector<string>
SgStatementExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_statement");
return traversalSuccessorContainer;
}
size_t
SgStatementExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgStatementExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_statement;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgStatementExpression::get_childIndex(SgNode *child) {
if (child == p_statement) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmOp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgAsmOp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgAsmOp::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmOp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmOp::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgLabelRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgLabelRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgLabelRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgLabelRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgLabelRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgLabelRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgLabelRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgActualArgumentExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression);
return traversalSuccessorContainer;
}
vector<string>
SgActualArgumentExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_expression");
return traversalSuccessorContainer;
}
size_t
SgActualArgumentExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgActualArgumentExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_expression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgActualArgumentExpression::get_childIndex(SgNode *child) {
if (child == p_expression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgUnknownArrayOrFunctionReference::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_named_reference);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_expression_list);
return traversalSuccessorContainer;
}
vector<string>
SgUnknownArrayOrFunctionReference::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_named_reference");
traversalSuccessorContainer.push_back("p_expression_list");
return traversalSuccessorContainer;
}
size_t
SgUnknownArrayOrFunctionReference::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgUnknownArrayOrFunctionReference::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_named_reference;
case 1: return p_expression_list;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgUnknownArrayOrFunctionReference::get_childIndex(SgNode *child) {
if (child == p_named_reference) return 0;
else if (child == p_expression_list) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgPseudoDestructorRefExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgPseudoDestructorRefExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgPseudoDestructorRefExp::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgPseudoDestructorRefExp::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgPseudoDestructorRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgPseudoDestructorRefExp::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgPseudoDestructorRefExp" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgCAFCoExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_referData);
return traversalSuccessorContainer;
}
vector<string>
SgCAFCoExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_referData");
return traversalSuccessorContainer;
}
size_t
SgCAFCoExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgCAFCoExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_referData;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCAFCoExpression::get_childIndex(SgNode *child) {
if (child == p_referData) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgCudaKernelCallExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_function);
/* typeString = SgExprListExp* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_args);
/* typeString = SgCudaKernelExecConfig* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_config);
return traversalSuccessorContainer;
}
vector<string>
SgCudaKernelCallExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_function");
traversalSuccessorContainer.push_back("p_args");
traversalSuccessorContainer.push_back("p_exec_config");
return traversalSuccessorContainer;
}
size_t
SgCudaKernelCallExp::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgCudaKernelCallExp::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_function;
case 1: return p_args;
case 2: return p_exec_config;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCudaKernelCallExp::get_childIndex(SgNode *child) {
if (child == p_function) return 0;
else if (child == p_args) return 1;
else if (child == p_exec_config) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgCudaKernelExecConfig::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_grid);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_blocks);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_shared);
/* typeString = SgExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_stream);
return traversalSuccessorContainer;
}
vector<string>
SgCudaKernelExecConfig::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_grid");
traversalSuccessorContainer.push_back("p_blocks");
traversalSuccessorContainer.push_back("p_shared");
traversalSuccessorContainer.push_back("p_stream");
return traversalSuccessorContainer;
}
size_t
SgCudaKernelExecConfig::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgCudaKernelExecConfig::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_grid;
case 1: return p_blocks;
case 2: return p_shared;
case 3: return p_stream;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgCudaKernelExecConfig::get_childIndex(SgNode *child) {
if (child == p_grid) return 0;
else if (child == p_blocks) return 1;
else if (child == p_shared) return 2;
else if (child == p_stream) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgToken::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgToken::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgToken::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgToken::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgToken" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgToken::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgToken" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgSymbol::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgSymbol::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgSymbol::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgSymbol::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgVariableSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgVariableSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgVariableSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgVariableSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgVariableSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgVariableSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgVariableSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgFunctionSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgFunctionDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgFunctionSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgFunctionSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgFunctionSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgFunctionSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgMemberFunctionSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgFunctionDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgMemberFunctionSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgMemberFunctionSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgMemberFunctionSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgMemberFunctionSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgRenameSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgFunctionDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
/* typeString = SgSymbol* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_original_symbol);
return traversalSuccessorContainer;
}
vector<string>
SgRenameSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
traversalSuccessorContainer.push_back("p_original_symbol");
return traversalSuccessorContainer;
}
size_t
SgRenameSymbol::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgRenameSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
case 1: return p_original_symbol;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgRenameSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else if (child == p_original_symbol) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgFunctionTypeSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgFunctionTypeSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgFunctionTypeSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgFunctionTypeSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgFunctionTypeSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgFunctionTypeSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgFunctionTypeSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgClassSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgClassDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgClassSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgClassSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgClassSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgClassSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgTemplateSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgTemplateDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgTemplateSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgTemplateSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgTemplateSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTemplateSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgEnumSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgEnumDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgEnumSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgEnumSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgEnumSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgEnumSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgEnumFieldSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgEnumFieldSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgEnumFieldSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgEnumFieldSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgEnumFieldSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgEnumFieldSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgEnumFieldSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgTypedefSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgTypedefDeclaration* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgTypedefSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgTypedefSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgTypedefSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgTypedefSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgLabelSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgLabelStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgLabelSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgLabelSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgLabelSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgLabelSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgDefaultSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgDefaultSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgDefaultSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgDefaultSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgDefaultSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgDefaultSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgDefaultSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgNamespaceSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgNamespaceDeclarationStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
/* typeString = SgNamespaceAliasDeclarationStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_aliasDeclaration);
return traversalSuccessorContainer;
}
vector<string>
SgNamespaceSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
traversalSuccessorContainer.push_back("p_aliasDeclaration");
return traversalSuccessorContainer;
}
size_t
SgNamespaceSymbol::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgNamespaceSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
case 1: return p_aliasDeclaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgNamespaceSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else if (child == p_aliasDeclaration) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgIntrinsicSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgIntrinsicSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgIntrinsicSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgIntrinsicSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgIntrinsicSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgIntrinsicSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgIntrinsicSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgModuleSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgModuleStatement* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_declaration);
return traversalSuccessorContainer;
}
vector<string>
SgModuleSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_declaration");
return traversalSuccessorContainer;
}
size_t
SgModuleSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgModuleSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_declaration;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgModuleSymbol::get_childIndex(SgNode *child) {
if (child == p_declaration) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgInterfaceSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgInterfaceSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgInterfaceSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgInterfaceSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgInterfaceSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgInterfaceSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgInterfaceSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgCommonSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgCommonSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgCommonSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgCommonSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgCommonSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgCommonSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgCommonSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAliasSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgSymbol* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_alias);
return traversalSuccessorContainer;
}
vector<string>
SgAliasSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_alias");
return traversalSuccessorContainer;
}
size_t
SgAliasSymbol::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAliasSymbol::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_alias;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAliasSymbol::get_childIndex(SgNode *child) {
if (child == p_alias) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryAddressSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryAddressSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryAddressSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmBinaryAddressSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmBinaryAddressSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmBinaryAddressSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmBinaryAddressSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmBinaryDataSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryDataSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryDataSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmBinaryDataSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmBinaryDataSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmBinaryDataSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmBinaryDataSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmNode::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmNode::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmNode::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmNode::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmNode::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmNode" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmStatement::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmStatement::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmStatement::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmStatement::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmStatement::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmStatement" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmDeclaration::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDeclaration" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDeclaration" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmDeclaration::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDeclaration" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmDeclaration::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDeclaration" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmDeclaration::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDeclaration" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmDataStructureDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDataStructureDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDataStructureDeclaration::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDataStructureDeclaration::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDataStructureDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDataStructureDeclaration::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDataStructureDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmFunctionDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_statementList.size() + 0);
/* typeString = SgAsmStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmStatementPtrList::iterator iter;
     for (iter = p_statementList.begin(); iter != p_statementList.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmFunctionDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmStatementPtrList::iterator  iter;
     for (iter = p_statementList.begin(); iter != p_statementList.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmFunctionDeclaration::get_numberOfTraversalSuccessors() {
return p_statementList.size() + 0;
}
SgNode *
SgAsmFunctionDeclaration::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_statementList.size());
return p_statementList[idx];
}
size_t
SgAsmFunctionDeclaration::get_childIndex(SgNode *child) {
SgAsmStatementPtrList::iterator itr = find(p_statementList.begin(), p_statementList.end(), child);
if (itr != p_statementList.end()) return itr - p_statementList.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmFieldDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmFieldDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmFieldDeclaration::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmFieldDeclaration::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmFieldDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmFieldDeclaration::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmFieldDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_statementList.size() + 0);
/* typeString = SgAsmStatementPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmStatementPtrList::iterator iter;
     for (iter = p_statementList.begin(); iter != p_statementList.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmStatementPtrList::iterator  iter;
     for (iter = p_statementList.begin(); iter != p_statementList.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmBlock::get_numberOfTraversalSuccessors() {
return p_statementList.size() + 0;
}
SgNode *
SgAsmBlock::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_statementList.size());
return p_statementList[idx];
}
size_t
SgAsmBlock::get_childIndex(SgNode *child) {
SgAsmStatementPtrList::iterator itr = find(p_statementList.begin(), p_statementList.end(), child);
if (itr != p_statementList.end()) return itr - p_statementList.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmInstruction::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmOperandList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operandList);
return traversalSuccessorContainer;
}
vector<string>
SgAsmInstruction::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operandList");
return traversalSuccessorContainer;
}
size_t
SgAsmInstruction::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmInstruction::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operandList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmInstruction::get_childIndex(SgNode *child) {
if (child == p_operandList) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmx86Instruction::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmOperandList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operandList);
return traversalSuccessorContainer;
}
vector<string>
SgAsmx86Instruction::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operandList");
return traversalSuccessorContainer;
}
size_t
SgAsmx86Instruction::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmx86Instruction::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operandList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmx86Instruction::get_childIndex(SgNode *child) {
if (child == p_operandList) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmArmInstruction::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmOperandList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operandList);
return traversalSuccessorContainer;
}
vector<string>
SgAsmArmInstruction::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operandList");
return traversalSuccessorContainer;
}
size_t
SgAsmArmInstruction::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmArmInstruction::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operandList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmArmInstruction::get_childIndex(SgNode *child) {
if (child == p_operandList) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPowerpcInstruction::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmOperandList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operandList);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPowerpcInstruction::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operandList");
return traversalSuccessorContainer;
}
size_t
SgAsmPowerpcInstruction::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmPowerpcInstruction::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operandList;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPowerpcInstruction::get_childIndex(SgNode *child) {
if (child == p_operandList) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmExpression::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmExpression::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmExpression::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmExpression::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmValueExpression::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmValueExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmValueExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmValueExpression::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmValueExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmValueExpression::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmValueExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmValueExpression::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmValueExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmByteValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmByteValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmByteValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmByteValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmByteValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmWordValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmWordValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmWordValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmWordValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmWordValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDoubleWordValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDoubleWordValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmDoubleWordValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDoubleWordValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDoubleWordValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmQuadWordValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmQuadWordValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmQuadWordValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmQuadWordValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmQuadWordValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmSingleFloatValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmSingleFloatValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmSingleFloatValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmSingleFloatValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmSingleFloatValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDoubleFloatValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDoubleFloatValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmDoubleFloatValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDoubleFloatValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDoubleFloatValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmVectorValueExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmValueExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_unfolded_expression_tree);
return traversalSuccessorContainer;
}
vector<string>
SgAsmVectorValueExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_unfolded_expression_tree");
return traversalSuccessorContainer;
}
size_t
SgAsmVectorValueExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmVectorValueExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_unfolded_expression_tree;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmVectorValueExpression::get_childIndex(SgNode *child) {
if (child == p_unfolded_expression_tree) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryExpression::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmBinaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmBinaryExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmBinaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmBinaryExpression::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmBinaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmBinaryExpression::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmBinaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmBinaryExpression::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmBinaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmBinaryAdd::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryAdd::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryAdd::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryAdd::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryAdd::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinarySubtract::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinarySubtract::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinarySubtract::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinarySubtract::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinarySubtract::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryMultiply::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryMultiply::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryMultiply::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryMultiply::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryMultiply::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryDivide::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryDivide::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryDivide::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryDivide::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryDivide::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryMod::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryMod::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryMod::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryMod::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryMod::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryAddPreupdate::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryAddPreupdate::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryAddPreupdate::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryAddPreupdate::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryAddPreupdate::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinarySubtractPreupdate::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinarySubtractPreupdate::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinarySubtractPreupdate::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinarySubtractPreupdate::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinarySubtractPreupdate::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryAddPostupdate::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryAddPostupdate::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryAddPostupdate::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryAddPostupdate::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryAddPostupdate::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinarySubtractPostupdate::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinarySubtractPostupdate::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinarySubtractPostupdate::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinarySubtractPostupdate::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinarySubtractPostupdate::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryLsl::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryLsl::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryLsl::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryLsl::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryLsl::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryLsr::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryLsr::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryLsr::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryLsr::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryLsr::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryAsr::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryAsr::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryAsr::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryAsr::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryAsr::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmBinaryRor::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_lhs);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rhs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmBinaryRor::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_lhs");
traversalSuccessorContainer.push_back("p_rhs");
return traversalSuccessorContainer;
}
size_t
SgAsmBinaryRor::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmBinaryRor::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_lhs;
case 1: return p_rhs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmBinaryRor::get_childIndex(SgNode *child) {
if (child == p_lhs) return 0;
else if (child == p_rhs) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmUnaryExpression::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmUnaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmUnaryExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmUnaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmUnaryExpression::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmUnaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmUnaryExpression::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmUnaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmUnaryExpression::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmUnaryExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmUnaryPlus::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand);
return traversalSuccessorContainer;
}
vector<string>
SgAsmUnaryPlus::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand");
return traversalSuccessorContainer;
}
size_t
SgAsmUnaryPlus::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmUnaryPlus::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmUnaryPlus::get_childIndex(SgNode *child) {
if (child == p_operand) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmUnaryMinus::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand);
return traversalSuccessorContainer;
}
vector<string>
SgAsmUnaryMinus::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand");
return traversalSuccessorContainer;
}
size_t
SgAsmUnaryMinus::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmUnaryMinus::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmUnaryMinus::get_childIndex(SgNode *child) {
if (child == p_operand) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmUnaryRrx::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand);
return traversalSuccessorContainer;
}
vector<string>
SgAsmUnaryRrx::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand");
return traversalSuccessorContainer;
}
size_t
SgAsmUnaryRrx::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmUnaryRrx::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmUnaryRrx::get_childIndex(SgNode *child) {
if (child == p_operand) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmUnaryArmSpecialRegisterList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_operand);
return traversalSuccessorContainer;
}
vector<string>
SgAsmUnaryArmSpecialRegisterList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_operand");
return traversalSuccessorContainer;
}
size_t
SgAsmUnaryArmSpecialRegisterList::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmUnaryArmSpecialRegisterList::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_operand;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmUnaryArmSpecialRegisterList::get_childIndex(SgNode *child) {
if (child == p_operand) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmMemoryReferenceExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_address);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment);
return traversalSuccessorContainer;
}
vector<string>
SgAsmMemoryReferenceExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_address");
traversalSuccessorContainer.push_back("p_segment");
return traversalSuccessorContainer;
}
size_t
SgAsmMemoryReferenceExpression::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmMemoryReferenceExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_address;
case 1: return p_segment;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmMemoryReferenceExpression::get_childIndex(SgNode *child) {
if (child == p_address) return 0;
else if (child == p_segment) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmRegisterReferenceExpression::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmRegisterReferenceExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmRegisterReferenceExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmRegisterReferenceExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmRegisterReferenceExpression::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmRegisterReferenceExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmRegisterReferenceExpression::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmRegisterReferenceExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmRegisterReferenceExpression::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmRegisterReferenceExpression" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmx86RegisterReferenceExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmx86RegisterReferenceExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmx86RegisterReferenceExpression::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmx86RegisterReferenceExpression::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmx86RegisterReferenceExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmx86RegisterReferenceExpression::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmx86RegisterReferenceExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmArmRegisterReferenceExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmArmRegisterReferenceExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmArmRegisterReferenceExpression::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmArmRegisterReferenceExpression::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmArmRegisterReferenceExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmArmRegisterReferenceExpression::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmArmRegisterReferenceExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmPowerpcRegisterReferenceExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPowerpcRegisterReferenceExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPowerpcRegisterReferenceExpression::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPowerpcRegisterReferenceExpression::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPowerpcRegisterReferenceExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPowerpcRegisterReferenceExpression::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPowerpcRegisterReferenceExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmControlFlagsExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmControlFlagsExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmControlFlagsExpression::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmControlFlagsExpression::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmControlFlagsExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmControlFlagsExpression::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmControlFlagsExpression" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmCommonSubExpression::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmExpression* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_subexpression);
return traversalSuccessorContainer;
}
vector<string>
SgAsmCommonSubExpression::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_subexpression");
return traversalSuccessorContainer;
}
size_t
SgAsmCommonSubExpression::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmCommonSubExpression::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_subexpression;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmCommonSubExpression::get_childIndex(SgNode *child) {
if (child == p_subexpression) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmExprListExp::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_expressions.size() + 0);
/* typeString = SgAsmExpressionPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmExpressionPtrList::iterator iter;
     for (iter = p_expressions.begin(); iter != p_expressions.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmExprListExp::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmExpressionPtrList::iterator  iter;
     for (iter = p_expressions.begin(); iter != p_expressions.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmExprListExp::get_numberOfTraversalSuccessors() {
return p_expressions.size() + 0;
}
SgNode *
SgAsmExprListExp::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_expressions.size());
return p_expressions[idx];
}
size_t
SgAsmExprListExp::get_childIndex(SgNode *child) {
SgAsmExpressionPtrList::iterator itr = find(p_expressions.begin(), p_expressions.end(), child);
if (itr != p_expressions.end()) return itr - p_expressions.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmInterpretation::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmDwarfCompilationUnitList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dwarf_info);
/* typeString = SgAsmBlock* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_global_block);
return traversalSuccessorContainer;
}
vector<string>
SgAsmInterpretation::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_dwarf_info");
traversalSuccessorContainer.push_back("p_global_block");
return traversalSuccessorContainer;
}
size_t
SgAsmInterpretation::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmInterpretation::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_dwarf_info;
case 1: return p_global_block;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmInterpretation::get_childIndex(SgNode *child) {
if (child == p_dwarf_info) return 0;
else if (child == p_global_block) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmOperandList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_operands.size() + 0);
/* typeString = SgAsmExpressionPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmExpressionPtrList::iterator iter;
     for (iter = p_operands.begin(); iter != p_operands.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmOperandList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmExpressionPtrList::iterator  iter;
     for (iter = p_operands.begin(); iter != p_operands.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmOperandList::get_numberOfTraversalSuccessors() {
return p_operands.size() + 0;
}
SgNode *
SgAsmOperandList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_operands.size());
return p_operands[idx];
}
size_t
SgAsmOperandList::get_childIndex(SgNode *child) {
SgAsmExpressionPtrList::iterator itr = find(p_operands.begin(), p_operands.end(), child);
if (itr != p_operands.end()) return itr - p_operands.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmType::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmType::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmType::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmType::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmType" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmTypeByte::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeByte::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeByte::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeByte::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeByte::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeWord::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeWord::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeWord::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeWord::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeWord::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeDoubleWord::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeDoubleWord::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeDoubleWord::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeDoubleWord::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeDoubleWord::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeQuadWord::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeQuadWord::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeQuadWord::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeQuadWord::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeQuadWord::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeDoubleQuadWord::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeDoubleQuadWord::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeDoubleQuadWord::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeDoubleQuadWord::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeDoubleQuadWord::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmType80bitFloat::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmType80bitFloat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmType80bitFloat::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmType80bitFloat::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmType80bitFloat::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmType128bitFloat::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmType128bitFloat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmType128bitFloat::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmType128bitFloat::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmType128bitFloat::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeSingleFloat::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeSingleFloat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeSingleFloat::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeSingleFloat::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeSingleFloat::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeDoubleFloat::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = static $CLASSNAME* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_builtin_type);
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeDoubleFloat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_builtin_type");
return traversalSuccessorContainer;
}
size_t
SgAsmTypeDoubleFloat::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmTypeDoubleFloat::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_builtin_type;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmTypeDoubleFloat::get_childIndex(SgNode *child) {
if (child == p_builtin_type) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmTypeVector::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmTypeVector::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmTypeVector::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmTypeVector::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmTypeVector" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmTypeVector::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmTypeVector" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmExecutableFileFormat::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExecutableFileFormat" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmExecutableFileFormat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExecutableFileFormat" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmExecutableFileFormat::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExecutableFileFormat" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmExecutableFileFormat::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExecutableFileFormat" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmExecutableFileFormat::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmExecutableFileFormat" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmGenericDLL::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericDLL::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmGenericDLL::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmGenericDLL::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmGenericDLL" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmGenericDLL::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmGenericDLL" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericFormat::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericFormat::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmGenericFormat::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmGenericFormat::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmGenericFormat" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmGenericFormat::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmGenericFormat" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericDLLList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericDLLList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmGenericDLLList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmGenericDLLList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmGenericDLLList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmGenericDLLList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmGenericDLLList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfEHFrameEntryFD::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfEHFrameEntryFD::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfEHFrameEntryFD::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfEHFrameEntryFD::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfEHFrameEntryFD" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfEHFrameEntryFD::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfEHFrameEntryFD" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericFile::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmGenericHeaderList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_headers);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_holes);
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericFile::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_headers");
traversalSuccessorContainer.push_back("p_holes");
return traversalSuccessorContainer;
}
size_t
SgAsmGenericFile::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmGenericFile::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_headers;
case 1: return p_holes;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmGenericFile::get_childIndex(SgNode *child) {
if (child == p_headers) return 0;
else if (child == p_holes) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmGenericSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmGenericSection::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmGenericSection::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmGenericSection" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmGenericSection::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmGenericSection" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmGenericFormat* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_format);
/* typeString = SgAsmGenericDLLList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dlls);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sections);
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_exec_format");
traversalSuccessorContainer.push_back("p_dlls");
traversalSuccessorContainer.push_back("p_sections");
return traversalSuccessorContainer;
}
size_t
SgAsmGenericHeader::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmGenericHeader::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_exec_format;
case 1: return p_dlls;
case 2: return p_sections;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmGenericHeader::get_childIndex(SgNode *child) {
if (child == p_exec_format) return 0;
else if (child == p_dlls) return 1;
else if (child == p_sections) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEFileHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgAsmGenericFormat* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_format);
/* typeString = SgAsmGenericDLLList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dlls);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sections);
/* typeString = SgAsmPERVASizePairList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_rvasize_pairs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEFileHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_exec_format");
traversalSuccessorContainer.push_back("p_dlls");
traversalSuccessorContainer.push_back("p_sections");
traversalSuccessorContainer.push_back("p_rvasize_pairs");
return traversalSuccessorContainer;
}
size_t
SgAsmPEFileHeader::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgAsmPEFileHeader::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_exec_format;
case 1: return p_dlls;
case 2: return p_sections;
case 3: return p_rvasize_pairs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEFileHeader::get_childIndex(SgNode *child) {
if (child == p_exec_format) return 0;
else if (child == p_dlls) return 1;
else if (child == p_sections) return 2;
else if (child == p_rvasize_pairs) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmLEFileHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(9);
/* typeString = SgAsmGenericFormat* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_format);
/* typeString = SgAsmGenericDLLList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dlls);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sections);
/* typeString = SgAsmDOSExtendedHeader* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dos2_header);
/* typeString = SgAsmLEPageTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_page_table);
/* typeString = SgAsmLENameTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_resname_table);
/* typeString = SgAsmLENameTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_nonresname_table);
/* typeString = SgAsmLEEntryTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entry_table);
/* typeString = SgAsmLERelocTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_reloc_table);
return traversalSuccessorContainer;
}
vector<string>
SgAsmLEFileHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_exec_format");
traversalSuccessorContainer.push_back("p_dlls");
traversalSuccessorContainer.push_back("p_sections");
traversalSuccessorContainer.push_back("p_dos2_header");
traversalSuccessorContainer.push_back("p_page_table");
traversalSuccessorContainer.push_back("p_resname_table");
traversalSuccessorContainer.push_back("p_nonresname_table");
traversalSuccessorContainer.push_back("p_entry_table");
traversalSuccessorContainer.push_back("p_reloc_table");
return traversalSuccessorContainer;
}
size_t
SgAsmLEFileHeader::get_numberOfTraversalSuccessors() {
return 9;
}
SgNode *
SgAsmLEFileHeader::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_exec_format;
case 1: return p_dlls;
case 2: return p_sections;
case 3: return p_dos2_header;
case 4: return p_page_table;
case 5: return p_resname_table;
case 6: return p_nonresname_table;
case 7: return p_entry_table;
case 8: return p_reloc_table;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmLEFileHeader::get_childIndex(SgNode *child) {
if (child == p_exec_format) return 0;
else if (child == p_dlls) return 1;
else if (child == p_sections) return 2;
else if (child == p_dos2_header) return 3;
else if (child == p_page_table) return 4;
else if (child == p_resname_table) return 5;
else if (child == p_nonresname_table) return 6;
else if (child == p_entry_table) return 7;
else if (child == p_reloc_table) return 8;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmNEFileHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(8);
/* typeString = SgAsmGenericFormat* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_format);
/* typeString = SgAsmGenericDLLList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dlls);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sections);
/* typeString = SgAsmDOSExtendedHeader* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dos2_header);
/* typeString = SgAsmNENameTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_resname_table);
/* typeString = SgAsmNENameTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_nonresname_table);
/* typeString = SgAsmNEModuleTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_module_table);
/* typeString = SgAsmNEEntryTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entry_table);
return traversalSuccessorContainer;
}
vector<string>
SgAsmNEFileHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_exec_format");
traversalSuccessorContainer.push_back("p_dlls");
traversalSuccessorContainer.push_back("p_sections");
traversalSuccessorContainer.push_back("p_dos2_header");
traversalSuccessorContainer.push_back("p_resname_table");
traversalSuccessorContainer.push_back("p_nonresname_table");
traversalSuccessorContainer.push_back("p_module_table");
traversalSuccessorContainer.push_back("p_entry_table");
return traversalSuccessorContainer;
}
size_t
SgAsmNEFileHeader::get_numberOfTraversalSuccessors() {
return 8;
}
SgNode *
SgAsmNEFileHeader::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_exec_format;
case 1: return p_dlls;
case 2: return p_sections;
case 3: return p_dos2_header;
case 4: return p_resname_table;
case 5: return p_nonresname_table;
case 6: return p_module_table;
case 7: return p_entry_table;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmNEFileHeader::get_childIndex(SgNode *child) {
if (child == p_exec_format) return 0;
else if (child == p_dlls) return 1;
else if (child == p_sections) return 2;
else if (child == p_dos2_header) return 3;
else if (child == p_resname_table) return 4;
else if (child == p_nonresname_table) return 5;
else if (child == p_module_table) return 6;
else if (child == p_entry_table) return 7;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDOSFileHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(4);
/* typeString = SgAsmGenericFormat* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_format);
/* typeString = SgAsmGenericDLLList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dlls);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sections);
/* typeString = SgAsmGenericSection* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_relocs);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDOSFileHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_exec_format");
traversalSuccessorContainer.push_back("p_dlls");
traversalSuccessorContainer.push_back("p_sections");
traversalSuccessorContainer.push_back("p_relocs");
return traversalSuccessorContainer;
}
size_t
SgAsmDOSFileHeader::get_numberOfTraversalSuccessors() {
return 4;
}
SgNode *
SgAsmDOSFileHeader::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_exec_format;
case 1: return p_dlls;
case 2: return p_sections;
case 3: return p_relocs;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDOSFileHeader::get_childIndex(SgNode *child) {
if (child == p_exec_format) return 0;
else if (child == p_dlls) return 1;
else if (child == p_sections) return 2;
else if (child == p_relocs) return 3;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfFileHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmGenericFormat* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exec_format);
/* typeString = SgAsmGenericDLLList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_dlls);
/* typeString = SgAsmGenericSectionList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_sections);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfFileHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_exec_format");
traversalSuccessorContainer.push_back("p_dlls");
traversalSuccessorContainer.push_back("p_sections");
return traversalSuccessorContainer;
}
size_t
SgAsmElfFileHeader::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfFileHeader::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_exec_format;
case 1: return p_dlls;
case 2: return p_sections;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfFileHeader::get_childIndex(SgNode *child) {
if (child == p_exec_format) return 0;
else if (child == p_dlls) return 1;
else if (child == p_sections) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSection::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmElfSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymbolSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfSymbolList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_symbols);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymbolSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_symbols");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymbolSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfSymbolSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_symbols;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSymbolSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_symbols) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfRelocSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfRelocEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfRelocSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfRelocSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfRelocSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfRelocSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfDynamicSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfDynamicEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfDynamicSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfDynamicSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfDynamicSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfDynamicSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfStringSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfStringSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
return traversalSuccessorContainer;
}
size_t
SgAsmElfStringSection::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmElfStringSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfStringSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfNoteSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfNoteEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfNoteSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfNoteSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfNoteSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfNoteSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfEHFrameSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfEHFrameEntryCIList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ci_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfEHFrameSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_ci_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfEHFrameSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfEHFrameSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_ci_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfEHFrameSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_ci_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfSymverEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfSymverSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSymverSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverDefinedSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfSymverDefinedEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverDefinedSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverDefinedSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfSymverDefinedSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSymverDefinedSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverNeededSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmElfSectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmElfSegmentTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_segment_entry);
/* typeString = SgAsmElfSymverNeededEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverNeededSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_segment_entry");
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverNeededSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmElfSymverNeededSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_segment_entry;
case 2: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSymverNeededSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_segment_entry) return 1;
else if (child == p_entries) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSectionTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSectionTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSectionTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSectionTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSectionTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSegmentTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSegmentTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSegmentTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSegmentTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSegmentTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSegmentTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSegmentTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmPESection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmPESectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPESection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
return traversalSuccessorContainer;
}
size_t
SgAsmPESection::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmPESection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPESection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmPESectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmPEImportDirectoryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_import_directories);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_import_directories");
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportSection::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmPEImportSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_import_directories;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEImportSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_import_directories) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEExportSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmPESectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
/* typeString = SgAsmPEExportDirectory* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_export_dir);
/* typeString = SgAsmPEExportEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_exports);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEExportSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
traversalSuccessorContainer.push_back("p_export_dir");
traversalSuccessorContainer.push_back("p_exports");
return traversalSuccessorContainer;
}
size_t
SgAsmPEExportSection::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmPEExportSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
case 1: return p_export_dir;
case 2: return p_exports;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEExportSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else if (child == p_export_dir) return 1;
else if (child == p_exports) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEStringSection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmPESectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_section_entry);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEStringSection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_section_entry");
return traversalSuccessorContainer;
}
size_t
SgAsmPEStringSection::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmPEStringSection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_section_entry;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEStringSection::get_childIndex(SgNode *child) {
if (child == p_section_entry) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPESectionTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPESectionTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPESectionTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPESectionTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPESectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPESectionTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPESectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDOSExtendedHeader::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDOSExtendedHeader::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDOSExtendedHeader::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDOSExtendedHeader::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDOSExtendedHeader" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDOSExtendedHeader::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDOSExtendedHeader" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmCoffSymbolTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmCoffSymbolList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_symbols);
return traversalSuccessorContainer;
}
vector<string>
SgAsmCoffSymbolTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_symbols");
return traversalSuccessorContainer;
}
size_t
SgAsmCoffSymbolTable::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmCoffSymbolTable::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_symbols;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmCoffSymbolTable::get_childIndex(SgNode *child) {
if (child == p_symbols) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmNESection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmNESectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_st_entry);
/* typeString = SgAsmNERelocTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_reloc_table);
return traversalSuccessorContainer;
}
vector<string>
SgAsmNESection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_st_entry");
traversalSuccessorContainer.push_back("p_reloc_table");
return traversalSuccessorContainer;
}
size_t
SgAsmNESection::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmNESection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_st_entry;
case 1: return p_reloc_table;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmNESection::get_childIndex(SgNode *child) {
if (child == p_st_entry) return 0;
else if (child == p_reloc_table) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmNESectionTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmNESectionTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmNESectionTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmNESectionTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmNESectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmNESectionTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmNESectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmNENameTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmNENameTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmNENameTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmNENameTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmNENameTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmNENameTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmNENameTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmNEModuleTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmNEStringTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_strtab);
return traversalSuccessorContainer;
}
vector<string>
SgAsmNEModuleTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_strtab");
return traversalSuccessorContainer;
}
size_t
SgAsmNEModuleTable::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmNEModuleTable::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_strtab;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmNEModuleTable::get_childIndex(SgNode *child) {
if (child == p_strtab) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmNEStringTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmNEStringTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmNEStringTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmNEStringTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmNEStringTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmNEStringTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmNEStringTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmNEEntryTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmNEEntryPointPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmNEEntryPointPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmNEEntryTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmNEEntryPointPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmNEEntryTable::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmNEEntryTable::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmNEEntryTable::get_childIndex(SgNode *child) {
SgAsmNEEntryPointPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmNERelocTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmNERelocEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmNERelocEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmNERelocTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmNERelocEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmNERelocTable::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmNERelocTable::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmNERelocTable::get_childIndex(SgNode *child) {
SgAsmNERelocEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmLESection::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmLESectionTableEntry* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_st_entry);
return traversalSuccessorContainer;
}
vector<string>
SgAsmLESection::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_st_entry");
return traversalSuccessorContainer;
}
size_t
SgAsmLESection::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmLESection::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_st_entry;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmLESection::get_childIndex(SgNode *child) {
if (child == p_st_entry) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmLESectionTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmLESectionTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmLESectionTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmLESectionTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmLESectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmLESectionTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmLESectionTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmLENameTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmLENameTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmLENameTable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmLENameTable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmLENameTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmLENameTable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmLENameTable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmLEPageTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmLEPageTableEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmLEPageTableEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmLEPageTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmLEPageTableEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmLEPageTable::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmLEPageTable::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmLEPageTable::get_childIndex(SgNode *child) {
SgAsmLEPageTableEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmLEEntryTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmLEEntryPointPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmLEEntryPointPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmLEEntryTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmLEEntryPointPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmLEEntryTable::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmLEEntryTable::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmLEEntryTable::get_childIndex(SgNode *child) {
SgAsmLEEntryPointPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmLERelocTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmLERelocEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmLERelocEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmLERelocTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmLERelocEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmLERelocTable::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmLERelocTable::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmLERelocTable::get_childIndex(SgNode *child) {
SgAsmLERelocEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmGenericSymbol::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmGenericSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmGenericSymbol::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmGenericSymbol::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmGenericSymbol::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericSymbol" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmCoffSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmCoffSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmCoffSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmCoffSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmCoffSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmCoffSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmCoffSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSymbol::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymbol::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymbol::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSymbol::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSymbol::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSymbol" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericStrtab::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericStrtab" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmGenericStrtab::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericStrtab" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmGenericStrtab::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericStrtab" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmGenericStrtab::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericStrtab" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmGenericStrtab::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericStrtab" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmElfStrtab::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfStrtab::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfStrtab::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfStrtab::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfStrtab" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfStrtab::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfStrtab" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmCoffStrtab::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmCoffStrtab::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmCoffStrtab::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmCoffStrtab::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmCoffStrtab" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmCoffStrtab::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmCoffStrtab" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericSymbolList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericSymbolList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmGenericSymbolList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmGenericSymbolList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmGenericSymbolList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmGenericSymbolList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmGenericSymbolList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmGenericSectionList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_sections.size() + 0);
/* typeString = SgAsmGenericSectionPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmGenericSectionPtrList::iterator iter;
     for (iter = p_sections.begin(); iter != p_sections.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericSectionList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmGenericSectionPtrList::iterator  iter;
     for (iter = p_sections.begin(); iter != p_sections.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmGenericSectionList::get_numberOfTraversalSuccessors() {
return p_sections.size() + 0;
}
SgNode *
SgAsmGenericSectionList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_sections.size());
return p_sections[idx];
}
size_t
SgAsmGenericSectionList::get_childIndex(SgNode *child) {
SgAsmGenericSectionPtrList::iterator itr = find(p_sections.begin(), p_sections.end(), child);
if (itr != p_sections.end()) return itr - p_sections.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmGenericHeaderList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_headers.size() + 0);
/* typeString = SgAsmGenericHeaderPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmGenericHeaderPtrList::iterator iter;
     for (iter = p_headers.begin(); iter != p_headers.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericHeaderList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmGenericHeaderPtrList::iterator  iter;
     for (iter = p_headers.begin(); iter != p_headers.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmGenericHeaderList::get_numberOfTraversalSuccessors() {
return p_headers.size() + 0;
}
SgNode *
SgAsmGenericHeaderList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_headers.size());
return p_headers[idx];
}
size_t
SgAsmGenericHeaderList::get_childIndex(SgNode *child) {
SgAsmGenericHeaderPtrList::iterator itr = find(p_headers.begin(), p_headers.end(), child);
if (itr != p_headers.end()) return itr - p_headers.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmGenericString::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericString" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmGenericString::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericString" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmGenericString::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericString" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmGenericString::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericString" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmGenericString::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmGenericString" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmBasicString::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmBasicString::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmBasicString::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmBasicString::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmBasicString" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmBasicString::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmBasicString" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmStoredString::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmStoredString::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmStoredString::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmStoredString::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmStoredString" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmStoredString::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmStoredString" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSectionTableEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSectionTableEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSectionTableEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSectionTableEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSectionTableEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSegmentTableEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSegmentTableEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSegmentTableEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSegmentTableEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSegmentTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSegmentTableEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSegmentTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSymbolList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_symbols.size() + 0);
/* typeString = SgAsmElfSymbolPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSymbolPtrList::iterator iter;
     for (iter = p_symbols.begin(); iter != p_symbols.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymbolList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSymbolPtrList::iterator  iter;
     for (iter = p_symbols.begin(); iter != p_symbols.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymbolList::get_numberOfTraversalSuccessors() {
return p_symbols.size() + 0;
}
SgNode *
SgAsmElfSymbolList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_symbols.size());
return p_symbols[idx];
}
size_t
SgAsmElfSymbolList::get_childIndex(SgNode *child) {
SgAsmElfSymbolPtrList::iterator itr = find(p_symbols.begin(), p_symbols.end(), child);
if (itr != p_symbols.end()) return itr - p_symbols.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportILTEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportILTEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportILTEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPEImportILTEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPEImportILTEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPEImportILTEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPEImportILTEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfRelocEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfRelocEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfRelocEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfRelocEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfRelocEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfRelocEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfRelocEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfRelocEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfRelocEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfRelocEntryList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfRelocEntryList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfRelocEntryList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfRelocEntryList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfRelocEntryList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmPEExportEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmGenericString* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_name);
/* typeString = SgAsmGenericString* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_forwarder);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEExportEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_name");
traversalSuccessorContainer.push_back("p_forwarder");
return traversalSuccessorContainer;
}
size_t
SgAsmPEExportEntry::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmPEExportEntry::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_name;
case 1: return p_forwarder;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEExportEntry::get_childIndex(SgNode *child) {
if (child == p_name) return 0;
else if (child == p_forwarder) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEExportEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_exports.size() + 0);
/* typeString = SgAsmPEExportEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmPEExportEntryPtrList::iterator iter;
     for (iter = p_exports.begin(); iter != p_exports.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEExportEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmPEExportEntryPtrList::iterator  iter;
     for (iter = p_exports.begin(); iter != p_exports.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmPEExportEntryList::get_numberOfTraversalSuccessors() {
return p_exports.size() + 0;
}
SgNode *
SgAsmPEExportEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_exports.size());
return p_exports[idx];
}
size_t
SgAsmPEExportEntryList::get_childIndex(SgNode *child) {
SgAsmPEExportEntryPtrList::iterator itr = find(p_exports.begin(), p_exports.end(), child);
if (itr != p_exports.end()) return itr - p_exports.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfDynamicEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfDynamicEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfDynamicEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfDynamicEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfDynamicEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfDynamicEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfDynamicEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfDynamicEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfDynamicEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfDynamicEntryList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfDynamicEntryList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfDynamicEntryList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfDynamicEntryList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfDynamicEntryList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSegmentTableEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmElfSegmentTableEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSegmentTableEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSegmentTableEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSegmentTableEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSegmentTableEntryList::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmElfSegmentTableEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmElfSegmentTableEntryList::get_childIndex(SgNode *child) {
SgAsmElfSegmentTableEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmStringStorage::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmStringStorage::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmStringStorage::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmStringStorage::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmStringStorage" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmStringStorage::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmStringStorage" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfNoteEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfNoteEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfNoteEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfNoteEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfNoteEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfNoteEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfNoteEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfNoteEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfNoteEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfNoteEntryList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfNoteEntryList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfNoteEntryList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfNoteEntryList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfNoteEntryList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSymverEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSymverEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSymverEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSymverEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSymverEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSymverEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmElfSymverEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSymverEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSymverEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverEntryList::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmElfSymverEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmElfSymverEntryList::get_childIndex(SgNode *child) {
SgAsmElfSymverEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverDefinedEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmElfSymverDefinedAuxList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverDefinedEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverDefinedEntry::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmElfSymverDefinedEntry::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSymverDefinedEntry::get_childIndex(SgNode *child) {
if (child == p_entries) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverDefinedEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmElfSymverDefinedEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSymverDefinedEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverDefinedEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSymverDefinedEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverDefinedEntryList::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmElfSymverDefinedEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmElfSymverDefinedEntryList::get_childIndex(SgNode *child) {
SgAsmElfSymverDefinedEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverDefinedAux::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverDefinedAux::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverDefinedAux::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSymverDefinedAux::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSymverDefinedAux" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSymverDefinedAux::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSymverDefinedAux" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSymverDefinedAuxList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmElfSymverDefinedAuxPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSymverDefinedAuxPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverDefinedAuxList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSymverDefinedAuxPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverDefinedAuxList::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmElfSymverDefinedAuxList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmElfSymverDefinedAuxList::get_childIndex(SgNode *child) {
SgAsmElfSymverDefinedAuxPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverNeededEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmElfSymverNeededAuxList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverNeededEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverNeededEntry::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmElfSymverNeededEntry::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfSymverNeededEntry::get_childIndex(SgNode *child) {
if (child == p_entries) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverNeededEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmElfSymverNeededEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSymverNeededEntryPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverNeededEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSymverNeededEntryPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverNeededEntryList::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmElfSymverNeededEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmElfSymverNeededEntryList::get_childIndex(SgNode *child) {
SgAsmElfSymverNeededEntryPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfSymverNeededAux::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverNeededAux::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverNeededAux::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfSymverNeededAux::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfSymverNeededAux" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfSymverNeededAux::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfSymverNeededAux" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfSymverNeededAuxList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmElfSymverNeededAuxPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmElfSymverNeededAuxPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfSymverNeededAuxList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmElfSymverNeededAuxPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmElfSymverNeededAuxList::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmElfSymverNeededAuxList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmElfSymverNeededAuxList::get_childIndex(SgNode *child) {
SgAsmElfSymverNeededAuxPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportDirectory::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(2);
/* typeString = SgAsmPEImportLookupTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_ilt);
/* typeString = SgAsmPEImportLookupTable* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_iat);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportDirectory::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_ilt");
traversalSuccessorContainer.push_back("p_iat");
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportDirectory::get_numberOfTraversalSuccessors() {
return 2;
}
SgNode *
SgAsmPEImportDirectory::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_ilt;
case 1: return p_iat;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEImportDirectory::get_childIndex(SgNode *child) {
if (child == p_ilt) return 0;
else if (child == p_iat) return 1;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportHNTEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportHNTEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportHNTEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPEImportHNTEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPEImportHNTEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPEImportHNTEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPEImportHNTEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmPESectionTableEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPESectionTableEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPESectionTableEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPESectionTableEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPESectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPESectionTableEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPESectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmPEExportDirectory::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEExportDirectory::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPEExportDirectory::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPEExportDirectory::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPEExportDirectory" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPEExportDirectory::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPEExportDirectory" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmPERVASizePair::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmPERVASizePair::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmPERVASizePair::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmPERVASizePair::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmPERVASizePair" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmPERVASizePair::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmPERVASizePair" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmCoffSymbolList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_symbols.size() + 0);
/* typeString = SgAsmCoffSymbolPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmCoffSymbolPtrList::iterator iter;
     for (iter = p_symbols.begin(); iter != p_symbols.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmCoffSymbolList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmCoffSymbolPtrList::iterator  iter;
     for (iter = p_symbols.begin(); iter != p_symbols.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmCoffSymbolList::get_numberOfTraversalSuccessors() {
return p_symbols.size() + 0;
}
SgNode *
SgAsmCoffSymbolList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_symbols.size());
return p_symbols[idx];
}
size_t
SgAsmCoffSymbolList::get_childIndex(SgNode *child) {
SgAsmCoffSymbolPtrList::iterator itr = find(p_symbols.begin(), p_symbols.end(), child);
if (itr != p_symbols.end()) return itr - p_symbols.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPERVASizePairList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_pairs.size() + 0);
/* typeString = SgAsmPERVASizePairPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmPERVASizePairPtrList::iterator iter;
     for (iter = p_pairs.begin(); iter != p_pairs.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmPERVASizePairList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmPERVASizePairPtrList::iterator  iter;
     for (iter = p_pairs.begin(); iter != p_pairs.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmPERVASizePairList::get_numberOfTraversalSuccessors() {
return p_pairs.size() + 0;
}
SgNode *
SgAsmPERVASizePairList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_pairs.size());
return p_pairs[idx];
}
size_t
SgAsmPERVASizePairList::get_childIndex(SgNode *child) {
SgAsmPERVASizePairPtrList::iterator itr = find(p_pairs.begin(), p_pairs.end(), child);
if (itr != p_pairs.end()) return itr - p_pairs.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmElfEHFrameEntryCI::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmElfEHFrameEntryFDList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_fd_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfEHFrameEntryCI::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_fd_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmElfEHFrameEntryCI::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmElfEHFrameEntryCI::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_fd_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmElfEHFrameEntryCI::get_childIndex(SgNode *child) {
if (child == p_fd_entries) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportHNTEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_hintnames.size() + 0);
/* typeString = SgAsmPEImportHNTEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmPEImportHNTEntryPtrList::iterator iter;
     for (iter = p_hintnames.begin(); iter != p_hintnames.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportHNTEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmPEImportHNTEntryPtrList::iterator  iter;
     for (iter = p_hintnames.begin(); iter != p_hintnames.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportHNTEntryList::get_numberOfTraversalSuccessors() {
return p_hintnames.size() + 0;
}
SgNode *
SgAsmPEImportHNTEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_hintnames.size());
return p_hintnames[idx];
}
size_t
SgAsmPEImportHNTEntryList::get_childIndex(SgNode *child) {
SgAsmPEImportHNTEntryPtrList::iterator itr = find(p_hintnames.begin(), p_hintnames.end(), child);
if (itr != p_hintnames.end()) return itr - p_hintnames.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportILTEntryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_vector.size() + 0);
/* typeString = SgAsmPEImportILTEntryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmPEImportILTEntryPtrList::iterator iter;
     for (iter = p_vector.begin(); iter != p_vector.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportILTEntryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmPEImportILTEntryPtrList::iterator  iter;
     for (iter = p_vector.begin(); iter != p_vector.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportILTEntryList::get_numberOfTraversalSuccessors() {
return p_vector.size() + 0;
}
SgNode *
SgAsmPEImportILTEntryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_vector.size());
return p_vector[idx];
}
size_t
SgAsmPEImportILTEntryList::get_childIndex(SgNode *child) {
SgAsmPEImportILTEntryPtrList::iterator itr = find(p_vector.begin(), p_vector.end(), child);
if (itr != p_vector.end()) return itr - p_vector.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportLookupTable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmPEImportILTEntryList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_entries);
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportLookupTable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_entries");
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportLookupTable::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmPEImportLookupTable::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_entries;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmPEImportLookupTable::get_childIndex(SgNode *child) {
if (child == p_entries) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmPEImportDirectoryList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_vector.size() + 0);
/* typeString = SgAsmPEImportDirectoryPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmPEImportDirectoryPtrList::iterator iter;
     for (iter = p_vector.begin(); iter != p_vector.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmPEImportDirectoryList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmPEImportDirectoryPtrList::iterator  iter;
     for (iter = p_vector.begin(); iter != p_vector.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmPEImportDirectoryList::get_numberOfTraversalSuccessors() {
return p_vector.size() + 0;
}
SgNode *
SgAsmPEImportDirectoryList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_vector.size());
return p_vector[idx];
}
size_t
SgAsmPEImportDirectoryList::get_childIndex(SgNode *child) {
SgAsmPEImportDirectoryPtrList::iterator itr = find(p_vector.begin(), p_vector.end(), child);
if (itr != p_vector.end()) return itr - p_vector.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmNEEntryPoint::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmNEEntryPoint::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmNEEntryPoint::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmNEEntryPoint::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmNEEntryPoint" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmNEEntryPoint::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmNEEntryPoint" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmNERelocEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmNERelocEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmNERelocEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmNERelocEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmNERelocEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmNERelocEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmNERelocEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmNESectionTableEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmNESectionTableEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmNESectionTableEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmNESectionTableEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmNESectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmNESectionTableEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmNESectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfEHFrameEntryCIList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfEHFrameEntryCIList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfEHFrameEntryCIList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfEHFrameEntryCIList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfEHFrameEntryCIList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfEHFrameEntryCIList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfEHFrameEntryCIList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmLEPageTableEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmLEPageTableEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmLEPageTableEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmLEPageTableEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmLEPageTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmLEPageTableEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmLEPageTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmLEEntryPoint::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_entries.size() + 0);
/* typeString = SgAsmLEEntryPointPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmLEEntryPointPtrList::iterator iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmLEEntryPoint::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmLEEntryPointPtrList::iterator  iter;
     for (iter = p_entries.begin(); iter != p_entries.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmLEEntryPoint::get_numberOfTraversalSuccessors() {
return p_entries.size() + 0;
}
SgNode *
SgAsmLEEntryPoint::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_entries.size());
return p_entries[idx];
}
size_t
SgAsmLEEntryPoint::get_childIndex(SgNode *child) {
SgAsmLEEntryPointPtrList::iterator itr = find(p_entries.begin(), p_entries.end(), child);
if (itr != p_entries.end()) return itr - p_entries.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmLESectionTableEntry::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmLESectionTableEntry::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmLESectionTableEntry::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmLESectionTableEntry::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmLESectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmLESectionTableEntry::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmLESectionTableEntry" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmElfEHFrameEntryFDList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmElfEHFrameEntryFDList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmElfEHFrameEntryFDList::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmElfEHFrameEntryFDList::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmElfEHFrameEntryFDList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmElfEHFrameEntryFDList::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmElfEHFrameEntryFDList" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfInformation::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfInformation" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmDwarfInformation::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfInformation" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmDwarfInformation::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfInformation" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmDwarfInformation::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfInformation" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmDwarfInformation::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfInformation" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmDwarfMacro::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfMacro::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfMacro::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfMacro::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfMacro" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfMacro::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfMacro" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfMacroList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_macro_list.size() + 0);
/* typeString = SgAsmDwarfMacroPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmDwarfMacroPtrList::iterator iter;
     for (iter = p_macro_list.begin(); iter != p_macro_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfMacroList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmDwarfMacroPtrList::iterator  iter;
     for (iter = p_macro_list.begin(); iter != p_macro_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfMacroList::get_numberOfTraversalSuccessors() {
return p_macro_list.size() + 0;
}
SgNode *
SgAsmDwarfMacroList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_macro_list.size());
return p_macro_list[idx];
}
size_t
SgAsmDwarfMacroList::get_childIndex(SgNode *child) {
SgAsmDwarfMacroPtrList::iterator itr = find(p_macro_list.begin(), p_macro_list.end(), child);
if (itr != p_macro_list.end()) return itr - p_macro_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfLine::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfLine::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfLine::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfLine::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfLine" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfLine::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfLine" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfLineList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_line_list.size() + 0);
/* typeString = SgAsmDwarfLinePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmDwarfLinePtrList::iterator iter;
     for (iter = p_line_list.begin(); iter != p_line_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfLineList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmDwarfLinePtrList::iterator  iter;
     for (iter = p_line_list.begin(); iter != p_line_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfLineList::get_numberOfTraversalSuccessors() {
return p_line_list.size() + 0;
}
SgNode *
SgAsmDwarfLineList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_line_list.size());
return p_line_list[idx];
}
size_t
SgAsmDwarfLineList::get_childIndex(SgNode *child) {
SgAsmDwarfLinePtrList::iterator itr = find(p_line_list.begin(), p_line_list.end(), child);
if (itr != p_line_list.end()) return itr - p_line_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfCompilationUnitList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_cu_list.size() + 0);
/* typeString = SgAsmDwarfCompilationUnitPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmDwarfCompilationUnitPtrList::iterator iter;
     for (iter = p_cu_list.begin(); iter != p_cu_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfCompilationUnitList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmDwarfCompilationUnitPtrList::iterator  iter;
     for (iter = p_cu_list.begin(); iter != p_cu_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfCompilationUnitList::get_numberOfTraversalSuccessors() {
return p_cu_list.size() + 0;
}
SgNode *
SgAsmDwarfCompilationUnitList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_cu_list.size());
return p_cu_list[idx];
}
size_t
SgAsmDwarfCompilationUnitList::get_childIndex(SgNode *child) {
SgAsmDwarfCompilationUnitPtrList::iterator itr = find(p_cu_list.begin(), p_cu_list.end(), child);
if (itr != p_cu_list.end()) return itr - p_cu_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfConstruct::get_traversalSuccessorContainer() {
vector<SgNode*> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfConstruct" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

vector<string>
SgAsmDwarfConstruct::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfConstruct" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return traversalSuccessorContainer;
 }

size_t
SgAsmDwarfConstruct::get_numberOfTraversalSuccessors() {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfConstruct" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

SgNode*
SgAsmDwarfConstruct::get_traversalSuccessorByIndex(size_t) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfConstruct" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return NULL;
 }

size_t
SgAsmDwarfConstruct::get_childIndex(SgNode *) {
   cerr << "Internal error(!): called tree traversal mechanism for illegal object: " << endl
<< "static: SgAsmDwarfConstruct" << endl << "dynamic:  " << this->sage_class_name() << endl;
cerr << "Aborting ..." << endl;
ROSE_ASSERT(false);
return 42;
 }

vector<SgNode*>
SgAsmDwarfArrayType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfArrayType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfArrayType::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfArrayType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfArrayType::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfClassType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfClassType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfClassType::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfClassType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfClassType::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfEntryPoint::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfEntryPoint::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfEntryPoint::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfEntryPoint::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfEntryPoint" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfEntryPoint::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfEntryPoint" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfEnumerationType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfEnumerationType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfEnumerationType::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfEnumerationType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfEnumerationType::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfFormalParameter::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfFormalParameter::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfFormalParameter::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfFormalParameter::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfFormalParameter" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfFormalParameter::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfFormalParameter" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfImportedDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfImportedDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfImportedDeclaration::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfImportedDeclaration::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfImportedDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfImportedDeclaration::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfImportedDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfLabel::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfLabel::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfLabel::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfLabel::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfLabel" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfLabel::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfLabel" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfLexicalBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfLexicalBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfLexicalBlock::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfLexicalBlock::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfLexicalBlock::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfMember::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfMember::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfMember::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfMember::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfMember" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfMember::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfMember" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfPointerType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfPointerType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfPointerType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfPointerType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfPointerType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfPointerType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfPointerType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfReferenceType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfReferenceType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfReferenceType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfReferenceType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfReferenceType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfReferenceType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfReferenceType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfCompilationUnit::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(3);
/* typeString = SgAsmDwarfLineList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_line_info);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_language_constructs);
/* typeString = SgAsmDwarfMacroList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_macro_info);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfCompilationUnit::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_line_info");
traversalSuccessorContainer.push_back("p_language_constructs");
traversalSuccessorContainer.push_back("p_macro_info");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfCompilationUnit::get_numberOfTraversalSuccessors() {
return 3;
}
SgNode *
SgAsmDwarfCompilationUnit::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_line_info;
case 1: return p_language_constructs;
case 2: return p_macro_info;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfCompilationUnit::get_childIndex(SgNode *child) {
if (child == p_line_info) return 0;
else if (child == p_language_constructs) return 1;
else if (child == p_macro_info) return 2;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfStringType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfStringType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfStringType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfStringType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfStringType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfStringType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfStringType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfStructureType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfStructureType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfStructureType::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfStructureType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfStructureType::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfSubroutineType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfSubroutineType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfSubroutineType::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfSubroutineType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfSubroutineType::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfTypedef::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfTypedef::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfTypedef::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfTypedef::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfTypedef" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfTypedef::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfTypedef" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfUnionType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUnionType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUnionType::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfUnionType::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfUnionType::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfUnspecifiedParameters::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUnspecifiedParameters::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUnspecifiedParameters::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfUnspecifiedParameters::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfUnspecifiedParameters" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfUnspecifiedParameters::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfUnspecifiedParameters" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfVariant::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfVariant::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfVariant::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfVariant::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfVariant" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfVariant::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfVariant" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfCommonBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfCommonBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfCommonBlock::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfCommonBlock::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfCommonBlock::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfCommonInclusion::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfCommonInclusion::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfCommonInclusion::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfCommonInclusion::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfCommonInclusion" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfCommonInclusion::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfCommonInclusion" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfInheritance::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfInheritance::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfInheritance::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfInheritance::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfInheritance" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfInheritance::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfInheritance" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfInlinedSubroutine::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfInlinedSubroutine::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfInlinedSubroutine::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfInlinedSubroutine::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfInlinedSubroutine::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfModule::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfModule::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfModule::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfModule::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfModule" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfModule::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfModule" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfPtrToMemberType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfPtrToMemberType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfPtrToMemberType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfPtrToMemberType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfPtrToMemberType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfPtrToMemberType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfPtrToMemberType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfSetType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfSetType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfSetType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfSetType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfSetType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfSetType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfSetType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfSubrangeType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfSubrangeType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfSubrangeType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfSubrangeType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfSubrangeType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfSubrangeType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfSubrangeType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfWithStmt::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfWithStmt::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfWithStmt::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfWithStmt::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfWithStmt" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfWithStmt::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfWithStmt" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfAccessDeclaration::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfAccessDeclaration::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfAccessDeclaration::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfAccessDeclaration::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfAccessDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfAccessDeclaration::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfAccessDeclaration" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfBaseType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfBaseType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfBaseType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfBaseType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfBaseType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfBaseType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfBaseType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfCatchBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfCatchBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfCatchBlock::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfCatchBlock::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfCatchBlock" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfCatchBlock::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfCatchBlock" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfConstType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfConstType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfConstType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfConstType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfConstType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfConstType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfConstType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfConstant::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfConstant::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfConstant::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfConstant::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfConstant" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfConstant::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfConstant" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfEnumerator::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfEnumerator::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfEnumerator::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfEnumerator::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfEnumerator" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfEnumerator::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfEnumerator" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfFileType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfFileType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfFileType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfFileType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfFileType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfFileType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfFileType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfFriend::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfFriend::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfFriend::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfFriend::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfFriend" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfFriend::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfFriend" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfNamelist::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfNamelist::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfNamelist::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfNamelist::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfNamelist" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfNamelist::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfNamelist" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfNamelistItem::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfNamelistItem::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfNamelistItem::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfNamelistItem::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfNamelistItem" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfNamelistItem::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfNamelistItem" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfPackedType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfPackedType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfPackedType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfPackedType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfPackedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfPackedType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfPackedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfSubprogram::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfSubprogram::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfSubprogram::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfSubprogram::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfSubprogram::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfTemplateTypeParameter::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfTemplateTypeParameter::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfTemplateTypeParameter::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfTemplateTypeParameter::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfTemplateTypeParameter" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfTemplateTypeParameter::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfTemplateTypeParameter" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfTemplateValueParameter::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfTemplateValueParameter::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfTemplateValueParameter::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfTemplateValueParameter::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfTemplateValueParameter" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfTemplateValueParameter::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfTemplateValueParameter" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfThrownType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfThrownType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfThrownType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfThrownType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfThrownType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfThrownType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfThrownType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfTryBlock::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfTryBlock::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfTryBlock::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfTryBlock::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfTryBlock" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfTryBlock::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfTryBlock" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfVariantPart::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfVariantPart::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfVariantPart::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfVariantPart::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfVariantPart" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfVariantPart::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfVariantPart" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfVariable::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfVariable::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfVariable::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfVariable::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfVariable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfVariable::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfVariable" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfVolatileType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfVolatileType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfVolatileType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfVolatileType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfVolatileType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfVolatileType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfVolatileType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfDwarfProcedure::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfDwarfProcedure::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfDwarfProcedure::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfDwarfProcedure::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfDwarfProcedure" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfDwarfProcedure::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfDwarfProcedure" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfRestrictType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfRestrictType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfRestrictType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfRestrictType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfRestrictType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfRestrictType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfRestrictType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfInterfaceType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfInterfaceType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfInterfaceType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfInterfaceType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfInterfaceType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfInterfaceType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfInterfaceType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfNamespace::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(1);
/* typeString = SgAsmDwarfConstructList* */
/* isSTLContainer   (typeString) = false */
/* isSTLContainerPtr(typeString) = false */
traversalSuccessorContainer.push_back(p_body);
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfNamespace::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
traversalSuccessorContainer.push_back("p_body");
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfNamespace::get_numberOfTraversalSuccessors() {
return 1;
}
SgNode *
SgAsmDwarfNamespace::get_traversalSuccessorByIndex(size_t idx) {
switch (idx) {
case 0: return p_body;
default: cout << "invalid index " << idx << " in get_traversalSuccessorByIndex()" << endl;
ROSE_ASSERT(false);
return NULL;
}
}
size_t
SgAsmDwarfNamespace::get_childIndex(SgNode *child) {
if (child == p_body) return 0;
else return (size_t) -1;
}
vector<SgNode*>
SgAsmDwarfImportedModule::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfImportedModule::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfImportedModule::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfImportedModule::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfImportedModule" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfImportedModule::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfImportedModule" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfUnspecifiedType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUnspecifiedType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUnspecifiedType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfUnspecifiedType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfUnspecifiedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfUnspecifiedType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfUnspecifiedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfPartialUnit::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfPartialUnit::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfPartialUnit::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfPartialUnit::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfPartialUnit" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfPartialUnit::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfPartialUnit" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfImportedUnit::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfImportedUnit::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfImportedUnit::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfImportedUnit::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfImportedUnit" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfImportedUnit::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfImportedUnit" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfMutableType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfMutableType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfMutableType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfMutableType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfMutableType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfMutableType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfMutableType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfCondition::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfCondition::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfCondition::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfCondition::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfCondition" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfCondition::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfCondition" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfSharedType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfSharedType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfSharedType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfSharedType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfSharedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfSharedType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfSharedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfFormatLabel::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfFormatLabel::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfFormatLabel::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfFormatLabel::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfFormatLabel" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfFormatLabel::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfFormatLabel" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfFunctionTemplate::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfFunctionTemplate::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfFunctionTemplate::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfFunctionTemplate::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfFunctionTemplate" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfFunctionTemplate::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfFunctionTemplate" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfClassTemplate::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfClassTemplate::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfClassTemplate::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfClassTemplate::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfClassTemplate" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfClassTemplate::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfClassTemplate" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfUpcSharedType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUpcSharedType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUpcSharedType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfUpcSharedType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfUpcSharedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfUpcSharedType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfUpcSharedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfUpcStrictType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUpcStrictType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUpcStrictType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfUpcStrictType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfUpcStrictType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfUpcStrictType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfUpcStrictType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfUpcRelaxedType::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUpcRelaxedType::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUpcRelaxedType::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfUpcRelaxedType::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfUpcRelaxedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfUpcRelaxedType::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfUpcRelaxedType" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfUnknownConstruct::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfUnknownConstruct::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfUnknownConstruct::get_numberOfTraversalSuccessors() {
return 0;
}
SgNode *
SgAsmDwarfUnknownConstruct::get_traversalSuccessorByIndex(size_t idx) {
cout << "error: get_traversalSuccessorByIndex called on node of type " << "SgAsmDwarfUnknownConstruct" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return NULL;
}
size_t
SgAsmDwarfUnknownConstruct::get_childIndex(SgNode *child) {
cout << "error: get_childIndex called on node of type " << "SgAsmDwarfUnknownConstruct" << " that has no successors!" << endl;
ROSE_ASSERT(false);
return 0; 
}
vector<SgNode*>
SgAsmDwarfConstructList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_list.size() + 0);
/* typeString = SgAsmDwarfConstructPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmDwarfConstructPtrList::iterator iter;
     for (iter = p_list.begin(); iter != p_list.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmDwarfConstructList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmDwarfConstructPtrList::iterator  iter;
     for (iter = p_list.begin(); iter != p_list.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmDwarfConstructList::get_numberOfTraversalSuccessors() {
return p_list.size() + 0;
}
SgNode *
SgAsmDwarfConstructList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_list.size());
return p_list[idx];
}
size_t
SgAsmDwarfConstructList::get_childIndex(SgNode *child) {
SgAsmDwarfConstructPtrList::iterator itr = find(p_list.begin(), p_list.end(), child);
if (itr != p_list.end()) return itr - p_list.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmInterpretationList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_interpretations.size() + 0);
/* typeString = SgAsmInterpretationPtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmInterpretationPtrList::iterator iter;
     for (iter = p_interpretations.begin(); iter != p_interpretations.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmInterpretationList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmInterpretationPtrList::iterator  iter;
     for (iter = p_interpretations.begin(); iter != p_interpretations.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmInterpretationList::get_numberOfTraversalSuccessors() {
return p_interpretations.size() + 0;
}
SgNode *
SgAsmInterpretationList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_interpretations.size());
return p_interpretations[idx];
}
size_t
SgAsmInterpretationList::get_childIndex(SgNode *child) {
SgAsmInterpretationPtrList::iterator itr = find(p_interpretations.begin(), p_interpretations.end(), child);
if (itr != p_interpretations.end()) return itr - p_interpretations.begin();
else return (size_t) -1;
}
vector<SgNode*>
SgAsmGenericFileList::get_traversalSuccessorContainer() {
  vector<SgNode*> traversalSuccessorContainer;
  traversalSuccessorContainer.reserve(p_files.size() + 0);
/* typeString = SgAsmGenericFilePtrList */
/* isSTLContainer   (typeString) = true */
/* isSTLContainerPtr(typeString) = false */
   {
     SgAsmGenericFilePtrList::iterator iter;
     for (iter = p_files.begin(); iter != p_files.end(); iter++)
          traversalSuccessorContainer.push_back(*iter);
        }
return traversalSuccessorContainer;
}
vector<string>
SgAsmGenericFileList::get_traversalSuccessorNamesContainer() {
vector<string> traversalSuccessorContainer;
int i = 0;
   {
     SgAsmGenericFilePtrList::iterator  iter;
     for (iter = p_files.begin(); iter != p_files.end(); (iter++,i++)) {
char buf[20];
sprintf(buf,"*[%d]",i);
traversalSuccessorContainer.push_back(buf);
        }
   }
return traversalSuccessorContainer;
}
size_t
SgAsmGenericFileList::get_numberOfTraversalSuccessors() {
return p_files.size() + 0;
}
SgNode *
SgAsmGenericFileList::get_traversalSuccessorByIndex(size_t idx) {
ROSE_ASSERT(idx < p_files.size());
return p_files[idx];
}
size_t
SgAsmGenericFileList::get_childIndex(SgNode *child) {
SgAsmGenericFilePtrList::iterator itr = find(p_files.begin(), p_files.end(), child);
if (itr != p_files.end()) return itr - p_files.begin();
else return (size_t) -1;
}

