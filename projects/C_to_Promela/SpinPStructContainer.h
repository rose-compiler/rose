#ifndef SPINPSTRUCTCONTAINER_H
#define SPINPSTRUCTCONTAINER_H

#include <string>
#include <vector>

class SpinPStructContainer
{
	public:
	// construction phase
	SpinPStructContainer(std::string name);
	SgClassDeclaration * contructASTNodes();
	SgClassDeclaration * getStructSubTree();
	void addVariable(SgVariableDeclaration * var);
	SgVariableDeclaration*create_pid_decl();
	// hide non-essential code form unparsing or not
	void hideShugar();
	void showShugar();
	// create stuff
	SgVariableDeclaration * createPStructInstanciation(bool visible);
	// getClassType
	SgType * getPStructType();
	SgType * getPointerOfPStructType();
	// create and return the statement for PprocnameType nad Pprocname
	SgVariableDeclaration *  getPStructVarDeclaration();
	SgVariableDeclaration *  getPStructVarInstanziationDeclaration();
	// create varRefExpr of
	SgVarRefExp * createNewVarRefOfPStructVar();
	SgVarRefExp * createNewVarRefPStructVarInstanziation();
	SgVarRefExp * getProcVarRef(){return createNewVarRefPStructVarInstanziation();}
	SgVarRefExp * get_pidVarRef();
	protected:
	SgClassDeclaration * pStruct;
	SgType * tPStruct,*tPPStruct;
	std::string procName;
	SgVariableDeclaration *_pidDecl;
	SgVariableDeclaration * psInstanceDeclaration,* psPointerDeclaration;

	//specialized constructors
	void createPStructInstanceDeclaration();
	void createPStructPointerDeclaration();
};
#endif
