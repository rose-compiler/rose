#include "aptScope.h"
#include "rose_debug.h"

aptScope::aptScope(aptScopeType sType,aptScope* pScope)
:parentScope(pScope),type(sType)
{
	// CW: if we have a global scope we assume that we do not have
	// a parent scope
	ROSE_ASSERT((type!=aptGlobalScopeTag)||(parentScope==NULL));
}

aptScope::aptScope(const aptScope& rhs)
:parentScope(rhs.parentScope),type(rhs.type),subScopes(rhs.subScopes),
arrayDeclarations(rhs.arrayDeclarations),
arrayReferences(rhs.arrayReferences)
{
}

aptScope::~aptScope()
{
}

arrayDeclInfo* aptScope::addDeclaration(const arrayDeclInfo& newDeclInfo)
{
	// CW: if the list class is better we don't have to
	// do this ugly thing.
	arrayDeclInfo *tmp=new arrayDeclInfo(newDeclInfo);
	arrayDeclarations.addElement(*tmp);
	return tmp;
}

arrayRefInfo* aptScope::addReference(const arrayRefInfo& newRefInfo)
{
	arrayRefInfo *tmp=new arrayRefInfo(newRefInfo);
	arrayReferences.addElement(*tmp);
	return tmp;
}

aptScope* aptScope::addSubScope(const aptScope& newScope)
{
	aptScope *tmp=new aptScope(newScope);
	subScopes.addElement(*tmp);
	return tmp;
}

aptScope & aptScope::getSubScope(int i) const
{
	ROSE_ASSERT(i>=0);
	ROSE_ASSERT(i<subScopes.getLength());
	return subScopes[i];
}

arrayRefInfo& aptScope::getArrayRefInfo(int i) const
{
	ROSE_ASSERT(i>=0);
	ROSE_ASSERT(i<arrayReferences.getLength());
	return arrayReferences[i];
}

arrayDeclInfo& aptScope::getArrayDeclInfo(int i) const
{
	ROSE_ASSERT(i>=0);
	ROSE_ASSERT(i<arrayDeclarations.getLength());
	return arrayDeclarations[i];
}
