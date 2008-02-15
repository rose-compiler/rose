#include "arrayDeclInfo.h"
#include "rose_debug.h"

// CW:
// - info : array information for that array (includes sizes, elementsize etc.)
// - decl: pointer/reference to variable declaration Sage subtree
// - name: array variable name
// The last to uniquely identify the array declaration. The first
// is the collection of the data which is collected at run-time
arrayDeclInfo::arrayDeclInfo(const arrayInfo & info, SgVariableDeclaration *decl, const SgName& name)
:aInfo(info),declaration(decl),arrayName(name)
{
	ROSE_ASSERT(declaration!=NULL);	
}

// CW: copy constructor (makes a deep copy, except for sage subtree)
arrayDeclInfo::arrayDeclInfo(const arrayDeclInfo& rhs)
:aInfo(rhs.aInfo),declaration(rhs.declaration),arrayName(rhs.arrayName)
{
	ROSE_ASSERT(declaration!=NULL);
}

arrayDeclInfo& arrayDeclInfo::operator=(const arrayDeclInfo& rhs)
{
	if(&rhs==this) return *this;

	aInfo=rhs.aInfo;
	declaration=rhs.declaration;	
	arrayName=rhs.arrayName;
}

// CW: Sage subtree is not deleted
arrayDeclInfo::~arrayDeclInfo(){}

