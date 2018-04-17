
#include <iostream>

#include "sideEffect.h"

using namespace std;

class testSideEffectAttribute
{
 public:

  // Constructors

  testSideEffectAttribute() : mValid(false) { }
  testSideEffectAttribute ( SgNode* astNode ) : mValid(false) { }
  testSideEffectAttribute(const testSideEffectAttribute& X) :
    mValid(X.mValid), 
    mParentName(X.mParentName) { }

  string getParentName() const { return mParentName; }

  void setParentName(string name) { mParentName = name; mValid = true; }

  bool getValid() { return mValid; }

 private:
  bool                mValid;
  string              mParentName;
};

class testSideEffectTraversal
: public SgTopDownProcessing<testSideEffectAttribute>
{
 public:
  testSideEffectTraversal() : mUniquifier(0) { }

  testSideEffectTraversal(SideEffectAnalysis *setSideEffect) : 
    mSideEffectPtr(setSideEffect), mUniquifier(0) { }

  testSideEffectAttribute evaluateInheritedAttribute(SgNode* astNode,
						  testSideEffectAttribute inheritedAttribute) 
  {

    typedef list<const char* >::iterator iter;
    mUniquifier++;

    switch(astNode->variantT())
      {
      case V_SgFunctionDefinition: 
	{

	  // found a function definition
	  SgFunctionDeclaration *funcDec = 
	    isSgFunctionDeclaration(astNode->get_parent());

	  string funcName = getQualifiedFunctionName(funcDec);

	  list<const char* > sideEffects = mSideEffectPtr->getGMOD(funcName.c_str());
	  if (sideEffects.begin() != sideEffects.end()) {
	    cout << endl;
	    cout << "Function  Side Effects " << funcName << ": ";
	    for(iter i2 = sideEffects.begin(); i2 != sideEffects.end(); ++i2)
	      cout << *i2 << " ";
	    cout << endl;
    }

	  inheritedAttribute.setParentName(funcName);

	} break;
      case V_SgFunctionCallExp: 
	{
#if 0
	  string funcName = getQualifiedFunctionName((SgFunctionCallExp*)astNode);

	  cout << funcName << ": ";
	  list<const char* > sideEffects = mSideEffectPtr->getGMOD(funcName.c_str());
	  for(iter i2 = sideEffects.begin(); i2 != sideEffects.end(); ++i2)
	    cout << *i2 << " ";
	  cout << endl;
#endif


	} // fall through
      default: 
	{
	  if ( (astNode->get_file_info() != NULL) &&
               (inheritedAttribute.getValid()) ) {

#if 0
	    char stmt[512];
	    sprintf(stmt, "%s-%s-%ld", 
		    inheritedAttribute.getParentName().c_str(),
		    astNode->get_file_info()->get_filename(), 
		    mUniquifier);
	    list<const char* > sideEffects = mSideEffectPtr->getDMOD(stmt);
#else
	    string stmt = mSideEffectPtr->getNodeIdentifier(astNode);
	    list<const char* > sideEffects = mSideEffectPtr->getDMOD(stmt.c_str());
#endif

	    if (sideEffects.begin() != sideEffects.end()) {
	      cout << "Statement Side Effects " << stmt << ": ";
	      for(iter i2 = sideEffects.begin(); i2 != sideEffects.end(); ++i2)
                cout << *i2 << " ";
	      cout << endl;
            }

	  }
	} break;
      }

    testSideEffectAttribute returnAttribute(inheritedAttribute);
    return returnAttribute;
  }
  
 private:
  SideEffectAnalysis *mSideEffectPtr;
  int mUniquifier;
};

// milki (07/07/2010) This traversal isn't used
class testNodeSideEffectTraversal
: public SgTopDownProcessing<testSideEffectAttribute>
{
 public:
  testNodeSideEffectTraversal() : mUniquifier(0) { }

  testNodeSideEffectTraversal(SideEffectAnalysis *setSideEffect) : 
    mSideEffectPtr(setSideEffect), mUniquifier(0) { }

  testSideEffectAttribute evaluateInheritedAttribute(SgNode* astNode,
						  testSideEffectAttribute inheritedAttribute) 
  {

    typedef list<const char* >::iterator iter;
    mUniquifier++;

    switch(astNode->variantT())
      {
      case V_SgFunctionDefinition: 
	{

	  // found a function definition
	  SgFunctionDeclaration *funcDec = 
	    isSgFunctionDeclaration(astNode->get_parent());

	  string funcName = getQualifiedFunctionName(funcDec);

#if 0
	  mSideEffectPtr->calcSideEffect(*astNode);
	  list<const char* > sideEffects = mSideEffectPtr->getGMOD(funcName.c_str());
	  if (sideEffects.begin() != sideEffects.end()) {
	    cout << endl;
	    cout << "Function  Side Effects " << funcName << ": ";
	    for(iter i2 = sideEffects.begin(); i2 != sideEffects.end(); ++i2)
	      cout << *i2 << " ";
	    cout << endl;
	  }
#endif
	  inheritedAttribute.setParentName(funcName);

	} break;
      case V_SgForStatement:
	{

	  mSideEffectPtr->calcSideEffect(*astNode);

	} // fall through
      default: 
	{

	  if ( (astNode->get_file_info() != NULL) &&
               (inheritedAttribute.getValid()) ) {

#if 0
	    mUniquifier = (long)astNode;
	    char stmt[512];
	    sprintf(stmt, "%s-%s-%ld", 
		    inheritedAttribute.getParentName().c_str(),
		    astNode->get_file_info()->get_filename(), 
		    mUniquifier);
	    

	    list<const char* > sideEffects = mSideEffectPtr->getDMOD(stmt);
#else
	    string stmt = mSideEffectPtr->getNodeIdentifier(astNode);
	    list<const char* > sideEffects = mSideEffectPtr->getDMOD(stmt.c_str());
#endif
	    if (sideEffects.begin() != sideEffects.end()) {
	      cout << "Statement Side Effects " << stmt << ": ";
	      for(iter i2 = sideEffects.begin(); i2 != sideEffects.end(); ++i2)
		cout << *i2 << " ";
	      cout << endl;
	    }

	  }
	} break;
      }

    testSideEffectAttribute returnAttribute(inheritedAttribute);
    return returnAttribute;
  }
  
 private:
  SideEffectAnalysis *mSideEffectPtr;
  int mUniquifier;
};


int main(int argc, char **argv) {

  SgProject* project = frontend(argc, argv);

  cout << "Call side effects on project: " << endl;
  SideEffectAnalysis *sideEffect;

  sideEffect = SideEffectAnalysis::create();
  sideEffect->calcSideEffect(*project);

  cout << endl << endl;

  testSideEffectTraversal treeTraversal(sideEffect);
  testSideEffectAttribute inheritedAttribute(project);
  treeTraversal.traverse( project, inheritedAttribute );

#ifdef ALTERNATE_METHOD
  // alternatively to performing a graph traversal to find function call
  // names and query their side effects, we could have asked the
  // side effect object directly for the visited functions.
  list<const char* > calledFuncs = sideEffect->getCalledFunctions();

  typedef list<const char* >::iterator iter;

  for(iter i = calledFuncs.begin(); i != calledFuncs.end(); ++i) {
    cout << *i << ": ";
    list<const char* > sideEffects = sideEffect->getGMOD(*i);
    for(iter i2 = sideEffects.begin(); i2 != sideEffects.end(); ++i2)
      cout << *i2 << " ";
    cout << endl;
  }
#endif

  delete sideEffect;

  SgFile *file = &(project->get_file(0));

  cout << endl << endl << "Call side effects on file: " << endl;
  sideEffect = SideEffectAnalysis::create();
  sideEffect->calcSideEffect(*file);

  cout << endl << endl;

  testSideEffectTraversal fileTreeTraversal(sideEffect);
  testSideEffectAttribute fileInheritedAttribute(file);
  fileTreeTraversal.traverse( file, fileInheritedAttribute );

  delete sideEffect;

  cout << endl << endl << "Call side effects within for loop: " << endl;
  sideEffect = SideEffectAnalysis::create();

  testNodeSideEffectTraversal nodeTreeTraversal(sideEffect);
  testSideEffectAttribute nodeInheritedAttribute(project);
  nodeTreeTraversal.traverse( project, nodeInheritedAttribute );

  delete sideEffect;

  return 0;
}
