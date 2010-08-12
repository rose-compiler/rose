#include "rose.h"
#include "SpinPStructContainer.h"
#include <string>
#include <iostream>
using namespace std;

SpinPStructContainer::SpinPStructContainer(std::string name):procName(name)
{	
	pStruct=contructASTNodes();
	tPStruct=new SgClassType(pStruct);
	tPPStruct=new SgPointerType (tPStruct);

	_pidDecl=create_pid_decl();
	addVariable(_pidDecl);
	// create the InstanceDeclaration
	createPStructInstanceDeclaration();
	createPStructPointerDeclaration();
}

SgClassDeclaration * SpinPStructContainer::contructASTNodes()
{
	SgClassDeclaration * retVal=new SgClassDeclaration(
				Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
				SgName(string("P")+(procName)+"Type"),
				SgClassDeclaration::e_struct,
				NULL,
				new SgClassDefinition(Sg_File_Info::generateDefaultFileInfoForTransformationNode())	);
				
	retVal->get_definition()->set_declaration (retVal);
	retVal->get_definition()->set_endOfConstruct(retVal->get_definition()->get_startOfConstruct());
	retVal->set_firstNondefiningDeclaration(retVal);
	retVal->set_endOfConstruct(retVal->get_startOfConstruct());
	retVal->unsetForward ();
	return retVal;										
}

SgClassDeclaration * SpinPStructContainer::getStructSubTree()
{
	return pStruct;
}
void SpinPStructContainer::addVariable(SgVariableDeclaration * var)
{
	
	pStruct->get_definition()->append_member(var);
	var->set_parent(pStruct->get_definition());
}


// create the variable declaration for the _pid field ind the pStructure
SgVariableDeclaration * SpinPStructContainer::create_pid_decl()
{
	SgVariableDeclaration * retVal=new SgVariableDeclaration(
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		SgName("_pid"),
		new SgTypeInt());
	if (retVal==0)
	{
		std::cerr<<"creation of _pid failed"<<endl;
	}
	return retVal;
}

SgVariableDeclaration * SpinPStructContainer::createPStructInstanciation(bool visible)
{
	SgVariableDeclaration * retVal;
	Sg_File_Info * ptrFI=Sg_File_Info::generateDefaultFileInfoForTransformationNode();
	return retVal;	
}

SgVariableDeclaration *  SpinPStructContainer::getPStructVarDeclaration()
{	
	return psPointerDeclaration;
}
SgVariableDeclaration *  SpinPStructContainer::getPStructVarInstanziationDeclaration()
{
	return psInstanceDeclaration;
}
SgType * SpinPStructContainer::getPStructType()
{
	return tPStruct;
}
SgType * SpinPStructContainer::getPointerOfPStructType()
{
	return tPPStruct;
}


void SpinPStructContainer::createPStructInstanceDeclaration()
{
	SgName instanceName=string("P")+procName+"Instance";
	psInstanceDeclaration=
		new SgVariableDeclaration(
			Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			SgName(instanceName),
			tPStruct);
	
}
void SpinPStructContainer::createPStructPointerDeclaration()
{
	SgName instanceName=string("P")+procName;
	// create the initializer
	SgAddressOfOp *aoOp= new SgAddressOfOp (
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		isSgExpression(createNewVarRefOfPStructVar()),
		tPPStruct
		);
	SgInitializer * ptrInitializer=new SgAssignInitializer(
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		aoOp);	

	

	// create the declaration
	psPointerDeclaration=
		new SgVariableDeclaration(
			Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			SgName(instanceName),
			tPPStruct,
			ptrInitializer);	

/*	psPointerDeclaration=
		new SgVariableDeclaration(
			Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
			SgName(instanceName),
			tPPStruct);	*/
}

SgVarRefExp * SpinPStructContainer::createNewVarRefOfPStructVar()
{
	return new  SgVarRefExp (
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		new SgVariableSymbol (psInstanceDeclaration->get_definition()->get_vardefn ())
		);	
}
SgVarRefExp * SpinPStructContainer::createNewVarRefPStructVarInstanziation()
{
	return new  SgVarRefExp (
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		new SgVariableSymbol (psPointerDeclaration->get_definition()->get_vardefn ())
		);	
}
SgVarRefExp * SpinPStructContainer::get_pidVarRef()
{
	return new  SgVarRefExp (
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
                new SgVariableSymbol (_pidDecl->get_definition()->get_vardefn ())
                );

}

void SpinPStructContainer::hideShugar()
{
	pStruct->get_file_info()->unsetOutputInCodeGeneration ();
//	tPPStruct->get_file_info()->unsetOutputInCodeGeneration ();
//	tPStruct->get_file_info()->unsetOutputInCodeGeneration ();
	psInstanceDeclaration->get_file_info()->unsetOutputInCodeGeneration ();
	psPointerDeclaration->get_file_info()->unsetOutputInCodeGeneration ();
//	->unsetOutputInCodeGeneration ();
//	->unsetOutputInCodeGeneration ();
}
