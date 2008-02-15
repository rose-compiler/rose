//-*-Mode: C++;-*-
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifndef _arrayDeclInfo_h_
#define _arrayDeclInfo_h_

#if USE_SAGE3
#include <sage3.h>
#else
#include <sage2.h>
#endif

#include "arrayInfo.h"

// CW: this class holds all the data related to an A++ array declaration
// which is required to apply a padding transformation
// for each A++ array one object of the class must be created 

// CW: an array declaration is unique identified with a SgVariableDeclaration
// and an the variable name. The name is required for cases where more
// than one variable is defined in one statement (like "int i,j;")

// CW: the arrayInfo object stores all data which is required for the
// array padding library to figure out a good padding.

class arrayDeclInfo
{
  public:

	arrayDeclInfo(const arrayInfo& info, SgVariableDeclaration *decl, const SgName& name);
	arrayDeclInfo(const arrayDeclInfo& rhs);
	~arrayDeclInfo();
	
	arrayDeclInfo& operator=(const arrayDeclInfo& rhs);
	
	SgVariableDeclaration* getArrayDeclaration() const { return declaration; };
	arrayInfo getArrayInfo() const { return aInfo;};
	SgName getArrayName() const { return arrayName; };
	
  private:
	
	arrayInfo aInfo;
	SgVariableDeclaration* declaration;
	SgName arrayName;
};

#endif
