#include "rose.h"
#include "MPISpinReplacement.h"
#include <list>
#include <iostream>
#include "tools.h"
#include "MPICallListHelper.h"
#include "MPI_ASTAttrib.h"
using namespace std;


MPISpinReplacement::MPISpinReplacement()
{
	initMPICallToIdMap();
	foundMPICalls=changedMPICalls=0;
}

void MPISpinReplacement::registerMPICall(std::string name,MPISpinReplacement::MPICallType typeID)
{
	mpiCallToIdMap[name]=typeID;
	idMapToMpiCall[typeID]=name;
}

void MPISpinReplacement::notImplemented(MPISpinReplacement::MPICallType type,MPISpinReplacement::NotImplementedReason reason)
{
	cerr<<"\nAborting because ";
	switch(reason)
	{
		case MPISpinReplacement::NotSuported:
			cerr<<idMapToMpiCall[type]<<" is not supprted by this version"<<endl;
			break;
		case MPISpinReplacement::NotImplemented:
			cerr<<"process"<<idMapToMpiCall[type]<<" is not implemented"<<endl;
			break;
		case MPISpinReplacement::NotSupportedByMPISpin:
			cerr<<idMapToMpiCall[type]<<" is not supported by MPI-Spin"<<endl;
			break;
		default:
		cerr<<" of unkown reason"<<endl;
	}
	exit(-1);
}

//#define ADD_MPI_CALL_MAKRO(name) mpiCallToIdMap[" name " ]=MPISpinReplacement:: $name _CALL
void MPISpinReplacement::initMPICallToIdMap()
{
	// initally use only names for mapping lateron the mangled name should be used
	MPI_RESTIER_ALL_CALLS;
//	#include "mpiRegisterFunctions.inc"
	//ADD_MPI_CALL_MAKRO(MPI_Finalize);
/*	registerMPICall("MPI_Init",MPISpinReplacement::MPI_Init_CALL);
	registerMPICall("MPI_Finalize",MPISpinReplacement::MPI_Finalize_CALL);
	registerMPICall("MPI_Comm_rank",MPISpinReplacement::MPI_Comm_rank_CALL);
	registerMPICall("MPI_Comm_size",MPISpinReplacement::MPI_Comm_size_CALL);
	registerMPICall("MPI_Send",MPISpinReplacement::MPI_Send_CALL);
	registerMPICall("MPI_Recv",MPISpinReplacement::MPI_Recv_CALL);
	registerMPICall("MPI_Bcast",MPISpinReplacement::MPI_Bcast_CALL);
	registerMPICall("MPI_Reduce",MPISpinReplacement::MPI_Reduce_CALL);*/
/*	mpiCallToIdMap[""]=MPISpinReplacement::_CALL;
	mpiCallToIdMap[""]=MPISpinReplacement::_CALL;
	mpiCallToIdMap[""]=MPISpinReplacement::_CALL;
	*/
	cout<<"mpiCallToIdMap = {"<<endl;
	for (std::map<std::string,MPISpinReplacement::MPICallType>::iterator i =mpiCallToIdMap.begin();
		i!=mpiCallToIdMap.end();
                i++)
	{
		cout <<(*i).first<<" -> "<<(*i).second<<endl;
	}
	cout <<"}"<<endl;
}

SgFunctionCallExp * MPISpinReplacement::createNamedFunctioncall(string name,SgExprListExp * exprList)
{
	SgFunctionDeclaration * decl=	new SgFunctionDeclaration(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode(),SgName(name),new SgFunctionType(new SgTypeInt()));
	decl->set_endOfConstruct (Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
	decl->setForward ();
	
	SgFunctionRefExp * funcName = new SgFunctionRefExp(Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode(),	new SgFunctionSymbol(decl),new SgFunctionType(new SgTypeInt()));
	funcName->set_endOfConstruct (Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
	
	SgFunctionCallExp * call= new SgFunctionCallExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),funcName,exprList);
	call->set_endOfConstruct (Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
	return call;			
}

SgVarRefExp * MPISpinReplacement::createNamedVarRef(string name)
{
	
	SgInitializedName * ini=new SgInitializedName(SgName(name),new SgTypeInt());
	// the scope of this decl should be set here, but not now
	SgVarRefExp *ref= new SgVarRefExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),new	SgVariableSymbol (ini));
	ref->set_endOfConstruct (Sg_File_Info::generateDefaultFileInfoForCompilerGeneratedNode());
	return ref;
}

MPISpinReplacement::MPICallType MPISpinReplacement::mapFunctionCall(SgFunctionCallExp * call)
{
	SgExpression *fExp=call->get_function ();
	if (isSgFunctionRefExp(fExp))
	{
		SgFunctionRefExp *fRExp=isSgFunctionRefExp(fExp);
		SgName functionName=fRExp->get_symbol ()->get_declaration ()->get_name ();
		cout <<"function name is "<<functionName.getString()<<endl;
		if (mpiCallToIdMap.count(functionName.getString()))
		{
			cout <<"\tis MPI-Call with ID="<<mpiCallToIdMap[functionName.getString()]<<endl;
			return mpiCallToIdMap[functionName.getString()];
		}
	}
	return MPISpinReplacement::NO_MPI_CALL;
}
	
int MPISpinReplacement::replaceIn(SgNode * replacementArea,SpinPStructContainer* spsc)
{
	bool mpiFound,mpiChanged=false;
	cout <<"searching mpi function calls"<<endl;
	list<SgNode*> callSiteList=NodeQuery::querySubTree(replacementArea,V_SgFunctionCallExp);
	for (list<SgNode*>::iterator functionCall=callSiteList.begin();functionCall!=callSiteList.end();functionCall++)
	{
		mpiFound,mpiChanged=true;
		if (isSgFunctionCallExp(*functionCall))
		{
			SgFunctionCallExp *call=isSgFunctionCallExp(*functionCall);
			switch(mapFunctionCall(isSgFunctionCallExp(*functionCall)))
			{
				case MPISpinReplacement::MPI_Init_CALL:
					processMPI_Init(call,spsc);
					break;
				case MPISpinReplacement::MPI_Finalize_CALL:
					processMPI_Finalize(call,spsc);
					break;
				case MPISpinReplacement::MPI_Comm_rank_CALL:
					processMPI_Comm_rank(call,spsc);
					break;
				case MPISpinReplacement::MPI_Comm_size_CALL:
					processMPI_Comm_size(call,spsc);
					break;
				case MPISpinReplacement::MPI_Send_CALL:
					processMPI_Send(call,spsc);
					break;
				case MPISpinReplacement::MPI_Recv_CALL:
					processMPI_Recv(call,spsc);
					break;
				case MPISpinReplacement::MPI_Bcast_CALL	:
					notImplemented(MPISpinReplacement::MPI_Bcast_CALL,MPISpinReplacement::NotSupportedByMPISpin);
//				processMPI_Bcast(call,spsc);
					break;
				case MPISpinReplacement::MPI_Reduce_CALL	:
					notImplemented(MPISpinReplacement::MPI_Reduce_CALL,MPISpinReplacement::NotSupportedByMPISpin);
//					processMPI_Reduce(call,spsc);
					break;
				/*
				case MPISpinReplacement::	:
					processMPI_(call,spsc);
					break;
					*/
				case MPISpinReplacement::NO_MPI_CALL:
					cout << (*functionCall)->unparseToString() << " is not a MPI-Call"<<endl;
					mpiFound=false;
					mpiChanged=false;
					break;
				default:
					notImplemented(mapFunctionCall(isSgFunctionCallExp(*functionCall)),MPISpinReplacement::NotImplemented);
					mpiChanged=false;
					break;
			}
			if (mpiFound)
			{
				foundMPICalls++;
				

			}
			if (mpiChanged)changedMPICalls++;
		}
	}
	return 0;
}


void MPISpinReplacement::processMPI_Init(SgFunctionCallExp *call,SpinPStructContainer* spsc)
{
	// MPI_Init(proc,rank)
	// proc = P followed by the proc-name
	// rank = Pproducer->_pid - 1
	// for mpi init only the parameters need to be modified..
	markCallStatement(call);
	SgExprListExp * oldArgList=call->get_args ();
	SgExprListExp * newArgList=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	call->set_args(newArgList);
	newArgList->set_parent(call);
	delete(oldArgList);
	
	// create a sgVarRefExp for var
	newArgList->append_expression(spsc->getProcVarRef());
	SgArrowExp * pidExp=new SgArrowExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		spsc->getProcVarRef(),spsc->get_pidVarRef());
		
	newArgList->append_expression(
		new SgSubtractOp(
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		pidExp,
		new SgIntVal (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),1))
		);
	
	
	// add var->_pid
	
}
void MPISpinReplacement::processMPI_Finalize(SgFunctionCallExp *call,SpinPStructContainer* spsc)
{
	markCallStatement(call);
	// for mpi finalize the Process-Vartiable needs to be passed
	SgExprListExp * newArgList=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	
	call->set_args(newArgList);
	newArgList->set_parent(call);
	
	// create a sgVarRefExp for var
	newArgList->append_expression(spsc->getProcVarRef());	
}


void MPISpinReplacement::processMPI_Comm_rank(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	// get the second parameter of the call &(...) and get the expression of address of

	SgExprListExp  * exprList=call->get_args ();
	SgExpression  * lhs=*(++(exprList->get_expressions ().begin()));
	if (isSgAddressOfOp(lhs))
	{
		lhs=isSgAddressOfOp(lhs)->get_operand ();
	}
	else
	{
		lhs=new SgPointerDerefExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs);
	}
	//fake a  functionscall
	SgExprListExp * arg=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	arg->append_expression(spsc->getProcVarRef());
	SgFunctionCallExp * rhs=createNamedFunctioncall(string("RANK"),arg);
	
	SgAssignOp * assign=new	SgAssignOp(
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
		lhs,rhs/*
		new SgIntVal (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),1)*/
	);
	SgExprStatement  * newStmt= new SgExprStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),assign);
	// replace the old mpi call with the new stuff
	cout <<"\t\tUSING LowLevelRewrite::replace"<<endl;
	LowLevelRewrite::replace(isSgStatement(call->get_parent()),newStmt);
	cout <<"\t\tforcing c_code nesting"<<endl;
	// and for transformation into a c_code
	forceCCodeNesting(newStmt);
	cout <<"\t\tconverting MPI_Comm_rank done"<<endl;
	/*
	if (toConvert(newStmt))	
		cout <<"setting stmt to c_code failed"<<endl;
	else
		cout <<"setting stmt succeeded"<<endl;
	cout <<"statment unprases to "<<newStmt->unparseToString()<<endl;
	if (isSgStatement(newStmt))
	{
		cout <<" all ok, is a statement"<<endl;
	}
	else
	{
		cout <<"error, this is not a statement"<<endl;
	}
	*/
}
void MPISpinReplacement::processMPI_Comm_size(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	SgExprListExp  * exprList=call->get_args ();
	SgExpression  * lhs=*(++(exprList->get_expressions ().begin()));
	if (isSgAddressOfOp(lhs))
	{
		lhs=isSgAddressOfOp(lhs)->get_operand ();
	}
	else
	{
		lhs=new SgPointerDerefExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs);
	}
	SgVarRefExp * rhs=createNamedVarRef("NPROCS");
	SgAssignOp * assign=new SgAssignOp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs,rhs);
	
	SgExprStatement  * newStmt= new SgExprStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),assign);
	// replace the old mpi call with the new stuff
	LowLevelRewrite::replace(isSgStatement(call->get_parent()),newStmt);	
}



// MPI_Send(message,count,datatype,dest,tag,com) -> MPI_Send(proc,buffer,count,datatype,dest,tag)
void MPISpinReplacement::processMPI_Send(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	markCallStatement(call);
	SgExprListExp * oldArgList=call->get_args ();
	SgExpressionPtrList  oldExprList=oldArgList->get_expressions ();
	SgExpressionPtrList::iterator oldExprIt=oldExprList.begin();
	SgExprListExp * newArgList=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	call->set_args(newArgList);
	newArgList->set_parent(call);
	// add PStruct	
	newArgList->append_expression(spsc->getProcVarRef());
	// add the message
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the count
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the datatype
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the dest
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the tag
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// do not add the commonuicatorobect
	//CI MPI_Comm
}
// MPI_Recv(message,count,datatype,source,tag,com,status) -> MPI_Recv(proc,message,count,datatye,source,tag,status)
void MPISpinReplacement::processMPI_Recv(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	markCallStatement(call);
	SgExprListExp * oldArgList=call->get_args ();
	SgExpressionPtrList  oldExprList=oldArgList->get_expressions ();
	SgExpressionPtrList::iterator oldExprIt=oldExprList.begin();
	SgExprListExp * newArgList=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	call->set_args(newArgList);
	newArgList->set_parent(call);
	// add PStruct	
	newArgList->append_expression(spsc->getProcVarRef());
	// add the message
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the count
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the datatype
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the source
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the tag
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// do not add the commonuicatorobect
	//CI MPI_Comm
	oldExprIt++;
	// add the status
	newArgList->append_expression(*oldExprIt);
//	(*oldExprIt)->set_parent(newArgList);
	oldExprIt++;
}


void MPISpinReplacement::processMPI_Initialized(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	// get the first argument 
	SgExprListExp  * exprList=call->get_args ();
	// get the lefthand side 
	SgExpression  * lhs=*(exprList->get_expressions ().begin());
	if (isSgAddressOfOp(lhs))
	{
		lhs=isSgAddressOfOp(lhs)->get_operand ();
	}
	else
	{
		lhs=new SgPointerDerefExp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs);
	}

	// generate the right hand side
	SgExprListExp * args=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	args->append_expression(spsc->getProcVarRef());	
	call->set_args(args);
	
	SgFunctionCallExp * rhs=call;	
	SgAssignOp * assign=new SgAssignOp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs,rhs);
	SgExprStatement  * newStmt= new SgExprStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),assign);

	// replace the old mpi call with the new stuff
	LowLevelRewrite::replace(isSgStatement(call->get_parent()),newStmt);	
}
void MPISpinReplacement::processMPI_Finalized(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	processMPI_Initialized(call,spsc);
}

void MPISpinReplacement::genericISend(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	markCallStatement(call);
	SgExprListExp * oldArgList=call->get_args ();
	SgExpressionPtrList  oldExprList=oldArgList->get_expressions ();
	SgExpressionPtrList::iterator oldExprIt=oldExprList.begin();
	SgExprListExp * newArgList=new SgExprListExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
	call->set_args(newArgList);
	newArgList->set_parent(call);
	// add PStruct	
	newArgList->append_expression(spsc->getProcVarRef());
	// add the message
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the count
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the datatype
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the dest
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// add the tag
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;
	// do not add the commonuicatorobect
	//CI MPI_Comm
	oldExprIt++;
	// add the request
	newArgList->append_expression(*oldExprIt);
	oldExprIt++;	
}


void MPISpinReplacement::processMPI_ISend(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	genericISend(call,spsc);
}


/*
// MPI_Bcast(message,count,datatype,tag,comm)
void MPISpinReplacement::processMPI_Bcast(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
	 	
}
//MPI_Reduce(operand,recvbuf,recvounts[],datatype,operator,comm) ->
//MPI_Reduce(
void MPISpinReplacement::processMPI_Reduce(SgFunctionCallExp *call,SpinPStructContainer * spsc)
{
}*/

void MPISpinReplacement::markCallStatement(SgFunctionCallExp*call)
{
	SgNode *tmp=call;
	while(tmp && !isSgStatement(tmp)) tmp=tmp->get_parent();
	markNodeAsMPI(tmp);
}
