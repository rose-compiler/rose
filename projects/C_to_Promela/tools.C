#include "rose.h"
#include "tools.h"
#include "CToProMeLaASTAttrib.h"
#include "StateVecVarAttrib.h"
#include "MPI_ASTAttrib.h"


using namespace std;
/*******************************************************************************************/
/*                                 variable ast attribute                                  */
/*******************************************************************************************/

void markAsPromelaStateVar(SgNode *node,bool global)
{
	
	StateVecVarASTAttrib * attr=NULL;//new (C2PASTAttribute(true);
	// if the attribute exists
	if (node->attributeExists(STATEVECVARASTATTRIBSTR))
	{
		// get and change it
		attr=dynamic_cast<StateVecVarASTAttrib*>(node->getAttribute(STATEVECVARASTATTRIBSTR));
		attr->setGlobal(global);
		attr->setInState(true);
	}
	else
	{
		attr=new StateVecVarASTAttrib(true);
		attr->setGlobal(global);
	}
	// and put it back in the AST
	node->setAttribute(STATEVECVARASTATTRIBSTR,attr);	
	
}
bool isLocalPromelaStateVar(SgVarRefExp * exp)
{
	
	if (nodeHasStateVecVarAttrib(exp))
	{
		
		return getStateVecVarAttrib(exp)->isLocalState();
	}
	return false;
}
bool isGlobalPromelaStateVar(SgVarRefExp * exp)
{
	if (nodeHasStateVecVarAttrib(exp))
	{
		return getStateVecVarAttrib(exp)->isGlobalState();
	}
	return false;
}
/*******************************************************************************************/
/*                                 other                                                   */
/*******************************************************************************************/



///////////////////
vector<SgFunctionDefinition *> findProcessFunctions(SgProject* project)
{
	vector<SgFunctionDefinition *> processCandidates;
	
	list < SgNode * >functionDef = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
	for (list<SgNode*>::iterator i=functionDef.begin();i!=functionDef.end();i++)
	{
		SgFunctionDeclaration *	dec=isSgFunctionDefinition(*i)->get_declaration();
		if (dec->get_name().getString()==string("main"))
		{
			cout <<"Found Main"<<endl;
			processCandidates.push_back(isSgFunctionDefinition(*i));
		}
	}
	return processCandidates;
}


void forceCCodeNesting(SgNode * node)
{
	C2PASTAttribute * attr=NULL;//new (C2PASTAttribute(true);
	if (node->attributeExists(C2PASTATTRIBSTR))
	{
//		cout <<"Attrib exists"<<endl;
		attr=dynamic_cast<C2PASTAttribute*>(node->getAttribute(C2PASTATTRIBSTR));	
	}
	else
	{
//		cout <<"attrib does not exist"<<endl;
		attr=new C2PASTAttribute(false);
	}
	cout <<"forcing "<<node->unparseToString()<<" to be nested in a c_code-block"<<endl;
	attr->doNotConvert();
	cout <<"attr value is now "<< attr->toConvert()<<endl;
//	cout <<"Setting Attrib for "<<node->class_name()<<endl;
	node->setAttribute(C2PASTATTRIBSTR,attr);	
	
}

void markForPromelaTransformation(SgNode * node)
{
	C2PASTAttribute * attr=NULL;//new (C2PASTAttribute(true);
	if (node->attributeExists(C2PASTATTRIBSTR))
	{
//		cout <<"Attrib exists"<<endl;
		attr=dynamic_cast<C2PASTAttribute*>(node->getAttribute(C2PASTATTRIBSTR));
		if (attr->toConvert()==false) cout <<"chaning to convert for node "<<node->unparseToString()<<endl;
		attr->convert();
	}
	else
	{
//		cout <<"attrib does not exist"<<endl;
		attr=new C2PASTAttribute(true);
	}
//	cout <<"Setting Attrib for "<<node->class_name()<<endl;
	node->setAttribute(C2PASTATTRIBSTR,attr);	
}

bool toConvert(SgNode*node)
{
	bool retVal=false;
	if (node->attributeExists (C2PASTATTRIBSTR))
	{
		C2PASTAttribute* attr=static_cast<C2PASTAttribute*>(node->getAttribute(C2PASTATTRIBSTR));
		if (attr==NULL)
		{
			cerr<<"ATTRIBUTE is NULL"<<endl;
			assert(attr!=NULL);
		}
		retVal= attr->toConvert();
//		cout <<"class = " << node->class_name () <<endl;
/*		if (retVal==false)
		cout <<"toConvert("<<node->unparseToString()<<")="<<retVal<<endl;*/
	}
/*	else
	{
		cout <<"toConvert("<<node->unparseToString()<<"): attributeExists= FALSE"<<endl;
	}*/
/*	if (retVal)
		cout <<"toConvert=true"<<endl;
	else
	 	cout <<"toConvert=false"<<endl;*/
	return retVal;

}

void flattenScopes(SgFunctionDefinition * fDef)
{
	// * 3.3: find all variables in the function,give them a unique name and move them to the bgeinning of the function, this is allowed because this is for C only!!!. This would not work for classes which must be constructed
	unsigned int idNr=1;
	// rename variables
	list<SgNode*> varDeclList=NodeQuery::querySubTree(fDef,V_SgInitializedName);
	for (list<SgNode*>::iterator varDecl=varDeclList.begin();varDecl!=varDeclList.end();varDecl++)
	{
		string varName=isSgInitializedName(*varDecl)->get_name().getString();
		char numberCString[255];
		sprintf(numberCString,"%i",idNr);
		string newVarName=string("PML")+string(numberCString)+string("_")+varName;
		idNr++;

		isSgInitializedName(*varDecl)->set_name(SgName(newVarName.c_str()));		
	}					

	list<SgNode*> newDeclList;
	varDeclList.clear();
	varDeclList=NodeQuery::querySubTree(fDef,V_SgVariableDeclaration);
	// the move the variable declaration to the function begin and replace the old declaration site with a definition
	for (list<SgNode*>::iterator varDecl=varDeclList.begin();varDecl!=varDeclList.end();varDecl++)
	{
		SgVariableDeclaration * varDeclStmt=isSgVariableDeclaration(*varDecl);
		SgVariableDefinition * varDef=varDeclStmt->get_definition();
		SgInitializedName *iniName=isSgInitializedName(varDef->get_vardefn());
		if (iniName->get_initializer () !=NULL)
		{
			//				cout <<"VarDecl >"<<iniName->get_name().getString()<<"< has initilizer >"<<iniName->get_initializer ()->unparseToString()<<"<"<<endl;
			// determine if it is safe to separate initializer from decl
			// for now true
			if (1)
			{
				Sg_File_Info  * fi=Sg_File_Info::generateDefaultFileInfoForTransformationNode();
				SgVarRefExp * varRef=new	SgVarRefExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),new SgVariableSymbol(iniName));
				SgType * type=isSgAssignInitializer(iniName->get_initializer())->get_type ();
				SgAssignInitializer * sai=isSgAssignInitializer(iniName->get_initializer());
				if (sai==NULL)
				{
					cerr<<"isSgAssignInitializer(iniName->get_initializer())=NULL"<<endl;
					exit(-1);
				}
				SgExpression *lhs,*rhs;
				lhs=varRef;
				rhs=sai->get_operand ();
				if (lhs==NULL)
				{
					cerr<<"lhs=NULL"<<endl;
					exit(-1);
				}
				if (rhs==NULL)
				{
					cerr<<"rhs=NULL"<<endl;
					exit(-1);
				}
				SgAssignOp * assignment=new SgAssignOp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lhs,rhs);
				SgExprStatement * expr=new SgExprStatement(fi,assignment);
				if (expr==NULL)
				{
					cerr<<"construction of expr failed"<<endl;
				}
				isSgAssignInitializer(iniName->get_initializer ())->set_operand(NULL);
				delete(iniName->get_initializer ());
				iniName->set_initializer (NULL);
				// put the iniName in the list
				newDeclList.push_back(varDeclStmt);
				if (isSgStatement(varDeclStmt)==NULL)
				{
					cerr<<"isSgStatement(varDeclStmt)==NULL"<<endl;
				}
				LowLevelRewrite::replace(isSgStatement(varDeclStmt),isSgStatement(expr));					
			}				
		}
		else
		{
			cout <<"VarDecl >"<<iniName->get_name().getString()<<"> is uninitialized"<<endl;
			newDeclList.push_back(varDeclStmt);
			LowLevelRewrite::remove(isSgStatement(varDeclStmt));
		}
	}

	for (list<SgNode*>::iterator varDecl=newDeclList.begin();varDecl!=newDeclList.end();varDecl++)
	{
		fDef->prepend_statement(isSgStatement(*varDecl));
	}
	return ;
}


void replace(SgNode * parent, SgNode * current,SgNode *newNode)
{
	std::vector< std::pair<	SgNode **, std::string > > internalList=parent->returnDataMemberReferenceToPointers ();
	for (int i=0;i<internalList.size();i++)
	{
//		cout << i << " is "<< internalList[i].second<<endl;
		if ((*internalList[i].first)==current)
		{
			if (isSgVarRefExp(*(internalList[i].first)))
			{
//				cout <<"is sgVarRefexp"<<endl;
			}
//			cout <<"
//			cout <<" replacing "<<internalList[i].second<<" "<<(*internalList[i].first)->unparseToString()<<" with "<<newNode->unparseToString()<<endl;
	//		cout <<" replacing "<<internalList[i].second<<" "<<(*internalList[i].first)->unparseToString()<<endl;
			*(internalList[i].first)=newNode;
			newNode->set_parent(parent);
		}
	}
}

void refactorSgVarRefExpInCBlocks(SgFunctionDefinition * fDef,SgVariableDeclaration* vDec)
{
	SgName name=string("P")+(fDef)->get_declaration()->get_name().getString();
        list<SgNode*> stmtList=NodeQuery::querySubTree(fDef,V_SgStatement);	
        for (list<SgNode*>::iterator stmtIt=stmtList.begin();stmtIt!=stmtList.end();stmtIt++)
	{
		if (isSgWhileStmt(*stmtIt) || isSgIfStmt(*stmtIt) || isSgFunctionDefinition(*stmtIt)||isSgBasicBlock(*stmtIt))
	        {
			cout <<"while or if, don't process"<<endl;
			continue;
                }
		// this stmt remains in c
		if (!toConvert(*stmtIt))
		{
			cout <<"stmt changes from :"<<(*stmtIt)->unparseToString()<<endl;
			list<SgNode*> varRefExpList=NodeQuery::querySubTree(*stmtIt,V_SgVarRefExp);
			// for all var refs in this one
			for (list<SgNode*>::iterator refIt=varRefExpList.begin();refIt!=varRefExpList.end();refIt++)
			{
				SgVarRefExp * ref=isSgVarRefExp(*refIt);
				if (isLocalPromelaStateVar(ref))
				{
					SgNode * parent=ref->get_parent();
					cout <<ref->get_symbol()->get_declaration ()->get_name ().getString()<<" is "<<name<<" local"<<endl;
					// replace it with P${procName}->${VarName}
					SgArrowExp *arowExp=new SgArrowExp (
					Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
					new SgVarRefExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),new SgVariableSymbol (vDec->get_definition()->get_vardefn ())),
					ref,NULL);
//					cout <<"replacing "<<ref->unparseToString()<<endl;
					replace(parent,ref,arowExp);
//					cout <<"is now "<<arowExp->unparseToString();
					

				}
				else if (isGlobalPromelaStateVar(ref))
				{
					cout <<ref->get_symbol()->get_declaration ()->get_name ().getString()<<" is gobal"<<endl;
				}
				else
				{
					cout <<ref->get_symbol()->get_declaration ()->get_name ().getString()<<" is c_block local"<<endl;
				}
			}
			cout <<"stmt changes to   :"<<(*stmtIt)->unparseToString()<<endl;
			
		}
	}
}


void removeReturnStmts(SgFunctionDefinition * fDef)
{


	bool replaceReturnStmts=true;
	string functionName=fDef->get_declaration()->get_name().getString();
	string labelName=string("C2P_ENDOF_")+functionName;
	list<SgNode*> returnStmts= NodeQuery::querySubTree(fDef,V_SgReturnStmt);
	if (returnStmts.size())
	{
		//found return statements, need a exit-label
		SgLabelStatement * lbl=new SgLabelStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),SgName(labelName.c_str()));
		markForPromelaTransformation(lbl);

		cout <<"creating exit label >"<<labelName<<"< for function >"<<functionName<<"<"<<endl;
		// special case:
		if (returnStmts.size()==1)
		{
			//check if the return stmt is the last in the body
			SgBasicBlock  * bb=fDef->get_body();
			SgStatementPtrList sm=bb->get_statements ();
			cout <<"only one return, which should be at the end..."<<endl;
			cout <<"rbegin is : "<<(*(sm.rbegin()))->unparseToString()<<endl;
			if ((*(sm.rbegin()))==(*(returnStmts.begin())))
			{
				cout <<"return is last"<<endl;
				replaceReturnStmts=false;
				LowLevelRewrite::remove(isSgStatement(*(returnStmts.begin())));
			}
		}
		if (replaceReturnStmts)
		{
			for (list<SgNode*>::iterator j=returnStmts.begin();j!=returnStmts.end();j++)
			{
				cout <<"replacing return with goto to the end of the function"<<endl;
				SgGotoStatement *gotoStmt;
				string filename=(*j)->get_file_info()->get_filenameString();
				//					gotoStmt=new SgGotoStatement((*j)->get_file_info(),lbl);
				//					gotoStmt=new SgGotoStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lbl);
				gotoStmt=new SgGotoStatement(Sg_File_Info::generateFileInfoForTransformationNode(filename),lbl);
				markForPromelaTransformation(gotoStmt);
				LowLevelRewrite::markForOutputInCodeGenerationForRewrite(gotoStmt);
				// replace the return stmt with a goto
				LowLevelRewrite::replace(isSgStatement(*j),gotoStmt);
				// this failes
				//					LowLevelRewrite::replace(isSgStatement(*j),new SgGotoStatement(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),lbl));
			}
			//				SgBasicBlock  * bb=fDef->get_body();
			//				SgStatementPtrList sm=bb->get_statements ();

			//				for (SgStatementPtrList::iterator
			// and finally ad as a last steh the last label
			fDef->append_statement(lbl);
			SgBasicBlock *saftyBlock=new SgBasicBlock (Sg_File_Info::generateDefaultFileInfoForTransformationNode());
			markForPromelaTransformation(saftyBlock);
			fDef->append_statement(saftyBlock);
		}
	}
}

SgStatement * getStatement(SgNode*node)
{
	SgStatement * stmt=isSgStatement(node);
	if (stmt!=NULL) return stmt;
	SgNode * cand=node->get_parent();
	while(!isSgStatement(cand))
	{
		cand=cand->get_parent();
	}
	return isSgStatement(cand);
	
}

bool isSideEffektingStmt(SgStatement * stmt)
{
	if (isSgWhileStmt(stmt)||
			isSgIfStmt(stmt))
	{
		return false;
	}
	//TODO
	return true;
}
bool isPromelaCompatible(SgStatement * stmt)
{
	return false;
}
bool isPromelaCompatibleVariableType(SgVariableDeclaration * node)
{
	return false;
}
bool isPromelaCompatibleVariableType(SgVarRefExp * node)
{
	return false;
}

void removeContinues(SgWhileStmt * whileStmt)
{
	//TODO
}

void forToWhileTransformation(SgForStatement *it)
{
}

void doWhileToWhileTransformation(SgDoWhileStmt * doWhile)
{
}

void markStatementsWithCStateVariables(SgFunctionDefinition * fDef,set<SgInitializedName *> variables)
{
	// find all variable uses in this function
	list<SgNode*> varRefExpList=NodeQuery::querySubTree(fDef,V_SgVarRefExp);
	//
	for (list<SgNode*>::iterator it=varRefExpList.begin();it!=varRefExpList.end();it++)
	{
		// check if the initialized name is in the list...
		SgVarRefExp * exp=isSgVarRefExp(*it);
		SgInitializedName * iniName=exp->get_symbol ()->get_declaration () ;
		if (!variables.count(iniName))
		{
			cout <<"\t\tnot int stateVec,continue"<<endl;
			continue;
		}
		assert(exp!=NULL);
		// get the statement contining the varRefExp
		SgStatement * containingStatement;
		SgNode * tmp=*it;
		assert(tmp!=NULL);
		while(!isSgStatement(tmp) && tmp) tmp=tmp->get_parent();
		if (!tmp)
		{	
			
			cout <<"ERROR while processing "<<iniName->get_name().getString()<<endl;
			cout<<"\t parent of " <<(*it)->unparseToString()<<" evalueates to NULL"<<endl;
			assert(false);
		
		}
		containingStatement=isSgStatement(tmp);		
		assert(containingStatement!=NULL);

		// mark that statement for transformation in a c_block
		if (toConvert(containingStatement))
			cout <<"Warning: "<<containingStatement->unparseToString()<< " must be nested in a c_code-block"<<endl;
		forceCCodeNesting(containingStatement);		
		// mark the variable expression as a state
		markAsPromelaStateVar(exp);		
	}
}






// revert macros from the 

void revertMPIDefinitions(SgProject * proj)
{
	list<SgNode*> varRefExpList=NodeQuery::querySubTree(proj,V_SgVarRefExp);
	set<SgInitializedName*> renameSet;
	for (list<SgNode*>::iterator it=varRefExpList.begin();it!=varRefExpList.end();it++)
	{
		// check if the initialized name is in the list...
		SgVarRefExp * exp=isSgVarRefExp(*it);
		SgInitializedName * iniName=exp->get_symbol ()->get_declaration () ;
		string name=iniName->get_name ().getString();
		int pos=name.find("lam_mpi",0);
		if (pos==0)
		{
			renameSet.insert(iniName);
			//it is an mpi string -> replace with origina
			replaceSgNode(exp->get_parent(),exp);
			// mark the expression itself, not important..
			markNodeAsMPI(exp);
			markNodeAsMPI(iniName->get_declaration ());
		}
	}
	// rename the back to thoir original name...
	for (set<SgInitializedName*>::iterator it=renameSet.begin();it!=renameSet.end();it++)
	{
		SgInitializedName * iniName=*it;
		string name=iniName->get_name ().getString();
		string mpiName=name.substr(4,name.length());
		for (int i=0;i<mpiName.length();i++)
		{
			mpiName[i]=std::toupper(mpiName[i]);
		}
		iniName->set_name(SgName(mpiName));

	}

}

void replaceSgNode(SgNode * what,SgNode * with)
{
	
	std::vector< std::pair<	SgNode **, std::string > > l= what->get_parent()->returnDataMemberReferenceToPointers ();
	for(int i=0;i<l.size();i++)
	{
		if (*(l[i].first)==what)
		{
			cout <<"found what"<<l[i].second<<endl;
			*l[i].first=with;
			with->set_parent(what->get_parent());
			
		}
	}
}
