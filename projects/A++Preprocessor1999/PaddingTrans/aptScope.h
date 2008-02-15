//-*-Mode: C++;-*-
#ifndef _aptScope_h_
#define _aptScope_h_

#include "tlist.h"

#include "aptScopeType.h"
#include "arrayDeclInfo.h"
#include "arrayRefInfo.h"


// CW: a scope is a set of array declarations, array references, and
// a set of subscopes. So, a scope can be seen as a tree which stores
// information (about references and declarations) at his nodes.
// 
class aptScope
{
  public:
  
    aptScope(aptScopeType sType,aptScope* pScope);
	aptScope(const aptScope& rhs);
    ~aptScope();


	// CW: return parent scope
	aptScope * getParentScope() const { return parentScope; };
	
	// return amount of array declarations in scope
	int getNoOfArrayDeclInfos() const { return arrayDeclarations.getLength(); };
	// return a reference to an array variable declaration (starting from 0)
	arrayDeclInfo& getArrayDeclInfo(int i) const;
	// CW: add an A++ array variable declaration to this scope
	// the parameter is copied
	arrayDeclInfo* addDeclaration(const arrayDeclInfo& newDeclInfo);

	// CW: return amount of array references in scope
	int getNoOfArrayRefInfos() const { return arrayReferences.getLength(); };
	// CW: access reference i (starting from 0)
	arrayRefInfo& getArrayRefInfo(int i) const;
	arrayRefInfo* addReference(const arrayRefInfo& newRefInfo);

	// CW: returns number of subscopes
	int getNoOfSubScopes() const { return subScopes.getLength(); };
	// CW:: access subscope i (starting from 0)
	aptScope & getSubScope(int i) const;
	// CW: add a new sub scope
	aptScope* addSubScope(const aptScope& newSubScope);

  private:
	
	aptScope *parentScope;
	List<arrayDeclInfo> arrayDeclarations;
	List<arrayRefInfo> arrayReferences;
	List<aptScope> subScopes;

	aptScopeType type;
};

#endif
