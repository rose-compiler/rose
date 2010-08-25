#include <rose.h>
#include<iostream>

#include "DependenceGraph.h"
#include "CToProMeLa.h"
#include "CToProMeLaUnparser.h"
#include "tools.h"

#include "PromelaMarker.h"
#include "MarkAST4Unparse.h"
#include "CStateASTAttrib.h"
#include "StateVecVarAttrib.h"

#include "IdentifyInlineTargets.h"

#include "SlicingInfo.h"
#include "CreateSliceSet.h"
#include "CreateSlice.h"
#include "MPISpinReplacement.h"
#include "SpinPStructContainer.h"
#include "VisPromelaAST.h"
#include "PropagatePromelaTargetTraversal.h"
#include "MPI_ASTAttrib.h"
#include "changeLoops.h"
using namespace std;

void announceStep(int major,int minor,char * explanation)
{/*
	string str(explanation);
	string step;
	string line("-------------------------------------------------------------------------------");

	step+="\tStep"+major
	if (str.length()>*/
	cout <<"-------------------------------------------------------------------------------"<<endl;
	cout <<"\tStep"<<major;
	if (minor) cout<<"."<<minor;
	cout<<": "<<explanation<<endl;
	cout <<"-------------------------------------------------------------------------------"<<endl;
}

int main(int argc,char ** argv)
{
	SgProject *project = frontend(argc, argv);
	VisPromelaAST * debugPromelaAst=new VisPromelaAST(project);
	cout <<"reverting mpi"<<endl;
	revertMPIDefinitions(project);
	
	announceStep(1,0,"Identify modelchecking targets");
	//! step1: Identify modelchecking targets
	PromelaMarker initialMarker;
	initialMarker.traverse(project, preorder);
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.inital_marking.dot");		
	// since the next step is creating a slice, we must identify all statements which are modelchekcing targets so that we have a valid slice
	SlicingInfo si=SlicingInfo();
	// it might be better to use the AST-Attributes as a method for containing this information, but right now the pragma is sufficient
	si.setSliceTargetString(string("SPIN_TARGET"));
	si.traverse(project, preorder);


	announceStep(2,0,"Create abstractions using sdg and other means");
	// create the sdg
	SystemDependenceGraph * sdg=new SystemDependenceGraph();
	sdg->parseProject(project);
	//TODO: at this point one would implement abstractions, because they alter the code in such a way, that slicing might be able to remove additional statements, which otherwihse would not be removed
	

	announceStep(3,0,"Slice program to reduce state-space and\n\t eliminate dead code from abstractions");
	//! Step2: Slice program to reduce towards abstraction and goal
		
	// create the set of nodes, that are in the slice, in terms of the sdg
	CreateSliceSet sliceSet(sdg,si.getSlicingTargets());

	// using the set of statements that are within the slice construct an unnamed CreateSlice-class and slice the project
	CreateSlice(sliceSet.computeSliceSet()).traverse(project);
	
	// since now the ast is substantially different from the previouse version the sdg is not valid anymore
	delete(sdg);

	//DEVEL: output the sliced statement-graph
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.sliced.dot");		
	//DEVEL: run-ast-consistancy-test to enshure a valid ast
	AstTests::runAllTests(project);


	announceStep(4,1,"Perform sanity-check for non-transformable code like scanf and parameterinput  argc and argv");
	//TODO: implement the sanitiy-checks
	

	announceStep(4,2,"Identify Process and Thread functions and mark them as PROMELA-Procs");
	// find process functions,like main and thread-functions.
	// For this version of the transformation only main is valid, 
	// but later on threadfunctions might also work. Therefore the
	// a vector is used to store the main function
	vector<SgFunctionDefinition*> procFuncVec=findProcessFunctions(project);
	// mark those function declarations,definitions and main-basic block to enshure proper promela-output
	for (int i=0;i<procFuncVec.size();i++)
	{
		cout <<"\t*marking function for promela-output: "<<procFuncVec[i]->get_declaration()->get_name().getString()<<endl;
		// mark the function body for transformation
		markForPromelaTransformation(procFuncVec[i]);
		// force the first SgBasicBlock to be converted to promela!!!!!
		markForPromelaTransformation(procFuncVec[i]->get_body () );
		//CI (04/16/2007): This is currently not necessary
		if (isSgFile(procFuncVec[i]->get_parent()->get_parent()->get_parent()))
		{
			cout <<"marking file"<<endl;
			markForPromelaTransformation(procFuncVec[i]->get_parent()->get_parent()->get_parent());
		}
	}
	
	
	announceStep(4,3,"Identify functions using global variables and add them to the inlining list");
	vector<SgVariableDeclaration *> globalVarDeclVec;
	vector<SgFunctionDefinition *> functionsToInline;
	std::map<SgNode*,bool> functionToInlineMap;
	
	//TODO: search in each functions for uses of a global variable, if the function contains a use, mark it and continue with the next function
	list<SgNode*> sgGlobalList=NodeQuery::querySubTree(project,V_SgGlobal);	
	for (list<SgNode*>::iterator sgGlobalIt=sgGlobalList.begin();sgGlobalIt!=sgGlobalList.end();sgGlobalIt++)	
	{
		// iterate over this scope without recursing and find all variabledeclarations ...
		SgDeclarationStatementPtrList globalDeclList=isSgGlobal(*sgGlobalIt)->get_declarations ();
		for (SgDeclarationStatementPtrList::iterator it=globalDeclList.begin();it!=globalDeclList.end();it++)
		{
			if (isSgVariableDeclaration(*it))
			{
				cout <<"\t\t*found VarDecl:"<<(*it)->unparseToString()<<endl;
				//TODO:	check if its from a header and if it is used..
				globalVarDeclVec.push_back(isSgVariableDeclaration(*it));
			}
		}		
	}
	
	// remove MPI-Symbols form the globals	
	vector<SgVariableDeclaration *> globalVarDeclVecTmp;
	for (int i=0;i<globalVarDeclVec.size();i++)
	{
		// if the current declaration is an mpi declaration, remove it from the global list (it is from a header, and will be directly supported in spin
		if (isNodeMPI(globalVarDeclVec[i]))
		{
			cout<<"\t\t* found an MPI-Var-Decl, ignoring"<<endl;
		}
		else
		{
			globalVarDeclVecTmp.push_back(globalVarDeclVec[i]);
		}
	}
	globalVarDeclVec=globalVarDeclVecTmp;
	
	// now that we have a list of all global declarations, find their uses
	list<SgNode*>  functionList=NodeQuery::querySubTree(project,V_SgFunctionDefinition);
	for (list<SgNode*>::iterator fIt=functionList.begin();fIt!=functionList.end();fIt++)
	{
		//check if the function is main, .. kind of silly to inline main...
		if (isSgFunctionDefinition(*fIt)->get_declaration()->get_name().getString()=="main") continue;
		// search the function for globals
		list<SgNode*> sgVarRefList=NodeQuery::querySubTree(*fIt,V_SgVarRefExp);	
		for (list<SgNode*>::iterator it=sgVarRefList.begin();sgVarRefList.end()!=it;it++)
		{
			//
			// get the declaration and then if the declaration is of global scope -> it is a global
			if (isSgFunctionParameterList(isSgVarRefExp(*it)->get_symbol () ->get_declaration ()->get_declaration())) continue;
			if (isSgVarRefExp(*it)->get_symbol () ->get_declaration ()->get_declaration()==NULL) // is an initialized name form the function.parameter.list
				continue;
			SgNode * tmp=(*it);
			while(!isSgStatement(tmp) && !isSgGlobal(tmp)) tmp=tmp->get_parent();
//			cout <<"\t\tParentStmt: "<<tmp->unparseToString()<<endl;
//			cout <<"\t\tdebug:"<<(*it)->unparseToString()<<endl;
//			cout <<"\t\tclass:"<<isSgVarRefExp(*it)->get_symbol () ->get_declaration ()->get_declaration()->class_name()<<endl;
			SgVariableDeclaration *decl=isSgVariableDeclaration(isSgVarRefExp(*it)->get_symbol () ->get_declaration ()->get_declaration());
			assert(decl!=NULL);
//			cout <<"\t\t*decl: "<<decl->unparseToString()<<endl;
			if (isSgGlobal(decl->get_parent())) // is a global scope -> global
			{
				// we have a global, add the function to the list and break
				functionsToInline.push_back(isSgFunctionDefinition(*fIt));
				functionToInlineMap[isSgFunctionDefinition(*fIt)]=true;
				cout <<"\t\t*Found use of Global <"<<decl->get_definition()->get_vardefn ()->get_name ().getString()<<"> in "<<isSgFunctionDefinition(*fIt)->get_declaration()->get_name().getString()<<", -> added to inline list"<<endl;
				break;
			}
		}
	}
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step4_3.dot");


	announceStep(4,4,"Identify function called by proc-functions that contain modelchecking critical code");
	// a function that contains modelchecking code is marked as a promela-target after PropagatePromelaTargetTraversal -> propagate promela-targets
	PropagatePromelaTargetTraversal().traverse(project);
	for (list<SgNode*>::iterator fIt=functionList.begin();fIt!=functionList.end();fIt++)
	{
		// if the definition has a promela and is not main
		//check if the function is main, .. kind of silly to inline main...
		if (isSgFunctionDefinition(*fIt)->get_declaration()->get_name().getString()=="main") continue;
		// add it to the list of inline targets..
		if (!functionToInlineMap.count(isSgFunctionDefinition(*fIt)) || !functionToInlineMap[isSgFunctionDefinition(*fIt)])
		{
		functionsToInline.push_back(isSgFunctionDefinition(*fIt));
		functionToInlineMap[isSgFunctionDefinition(*fIt)]=true;
		cout <<"\t\t*"<<isSgFunctionDefinition(*fIt)->get_declaration()->get_name().getString()<<" contains a modelchecking target, -> added to inline list"<<endl;
		}
		else
		{
			cout <<"\t\t*"<<isSgFunctionDefinition(*fIt)->get_declaration()->get_name().getString()<<" contains a modelchecking target, but is already marked for inlining"<<endl;
		}
	}	
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step4_4.dot");
	
	
	
	announceStep(4,5,"Inlining identifyed functions");
	bool doneSomeInlineing=false;
	if (functionsToInline.size())
	{
		// for all thread functions
		for (int procNr=0;procNr<procFuncVec.size();procNr++)
		{
			list<SgNode*> callSiteList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgFunctionCallExp);
			for (list<SgNode*>::iterator callSite=callSiteList.begin();callSite!=callSiteList.end();callSite++)
			{
				SgFunctionDefinition * def;
				// get the definition for that function
				if (isSgFunctionRefExp(isSgFunctionCallExp((*callSite))->get_function()))
				{
					// straight function call
					def=isSgFunctionRefExp(isSgFunctionCallExp((*callSite))->get_function())->get_symbol()->get_declaration()->get_definition();
				}
				else
				{
					cerr <<"ERROR: could not identify function-definition"<<endl;
					assert(false);
				}

				if (functionToInlineMap[def])
				{
					cout <<"\t\t*need to inline function >"<<def->get_declaration()->get_name().getString()<<"<"<<endl;
					bool inliningOK=false;
					inliningOK=doInline(isSgFunctionCallExp(*callSite));
					assert(inliningOK==true);
					doneSomeInlineing|=true;
				}
			}
		}
		if (doneSomeInlineing)
		{
			cout <<"\t\t*Inlineing done->performing ast-consistancy-check"<<endl;
			// now we can remove the definion and declaration of the iline functions
			for (int fNr=0;fNr<functionsToInline.size();fNr++)
			{
				cout <<"\t\t* removing function declaration and definition of "<<functionsToInline[fNr]->get_declaration()->get_name().getString()<<endl;
				LowLevelRewrite::remove(isSgStatement(functionsToInline[fNr]));
				LowLevelRewrite::remove(isSgStatement(functionsToInline[fNr]->get_declaration()));
				
			}
//			AstTests::runAllTests(project);
		}
	}
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step4_5.dot");
//this one failes..	AstTests::runAllTests(project);
	
	announceStep(4,6,"transform for loops to whjile loops");
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
        {
		changeForLoops(procFuncVec[procNr]);
	}
	
	announceStep(4,6,"MPI-Specific: Moving globals into main");
	for(int i=0;i<globalVarDeclVec.size();i++)
	{
		bool used=false;
		// do a nast search if or not the globals are used (->rules aout includesd stuff)
		SgInitializedName * gIniName=globalVarDeclVec[globalVarDeclVec.size()-1-i]->get_definition()->get_vardefn ();
		list<SgNode*> varRefExpList=NodeQuery::querySubTree(procFuncVec[0],V_SgVarRefExp);
		for (list<SgNode*>::iterator it=varRefExpList.begin();it!=varRefExpList.end();it++)
		{
			// check if the initialized name is in the list...
			SgVarRefExp * exp=isSgVarRefExp(*it);
			SgInitializedName * iniName=exp->get_symbol ()->get_declaration () ;
			if (gIniName==iniName)	
			{
				used=true;
				break;
			}
			
		}
		// and if not used->skip
		if (!used) continue;
		
		LowLevelRewrite::remove(isSgStatement(globalVarDeclVec[globalVarDeclVec.size()-1-i]));
		// is allwayzs main
		// add them in reverse order in case they depend on each other
		assert(procFuncVec.size()>0);
		assert(procFuncVec[0]->get_body ());
		procFuncVec[0]->get_body ()->prepend_statement (globalVarDeclVec[globalVarDeclVec.size()-1-i]);
		// and remove it from the old localtion
		
	}

	announceStep(4,7,"Replacing do-while and for-loops which contain model-targets with while-loops");
	//CI (4/19/2007): TODO
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
	{	
		list<SgNode*> loopsToReplace;
		list<SgNode*> tmpList;
		tmpList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgForStatement);
		loopsToReplace.insert(loopsToReplace.end(),tmpList.begin(),tmpList.end());
		
		tmpList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgDoWhileStmt);
		loopsToReplace.insert(loopsToReplace.end(),tmpList.begin(),tmpList.end());
		for (list<SgNode*>::iterator it=loopsToReplace.begin();it!=loopsToReplace.end();it++)
		{
			if (!toConvert(*it)) continue;
			//else
			if (isSgForStatement(*it)) forToWhileTransformation(isSgForStatement(*it));
			if (isSgDoWhileStmt(*it)) doWhileToWhileTransformation(isSgDoWhileStmt(*it));
		}
	}	

	// since the inline does not preserve ast-attributes, we need to regenerate them prom the "copied" pragmas
	announceStep(4,8,"Re-Identify modelchecking targets");
	PromelaMarker().traverse(project, preorder);
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step4_8.dot");
	
	// from this point on, only work on the function of procFuncVec is necessary,since all necessary data has been inlined
	
	announceStep(4,9,"Identify sideffecting-conditionals and move them out of the control-statement, if the statement is transformed to promela");
	// first update the modelchecking information
	PropagatePromelaTargetTraversal().traverse(project);
	list<SgNode*>  conditinalControlStmts;
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
	{
		list<SgNode*> tmpList;
		// IF
		tmpList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgIfStmt);
		conditinalControlStmts.insert(conditinalControlStmts.end(),tmpList.begin(),tmpList.end());
		// WHILE
		tmpList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgWhileStmt);
		conditinalControlStmts.insert(conditinalControlStmts.end(),tmpList.begin(),tmpList.end());
		// SWITCH
		tmpList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgSwitchStatement);
		conditinalControlStmts.insert(conditinalControlStmts.end(),tmpList.begin(),tmpList.end());

		
		//make loop userfriendly:
		
		for (list<SgNode*>::iterator it=conditinalControlStmts.begin();it!=conditinalControlStmts.end();it++)
		{
			bool isWhile=false;
			
			// if the statement is not suppoesed to be unparsed to promela -> skip
			if (!toConvert(*it)) continue;

			SgStatement * conditional=NULL;
			if (isSgIfStmt(*it))
			{
				conditional=isSgIfStmt(*it)->get_conditional ();
			}
			else if (isSgWhileStmt(*it))
			{
				conditional=isSgWhileStmt(*it)->get_condition ();
				isWhile=true;
			}
			else if (isSgSwitchStatement(*it))
			{
				conditional=isSgSwitchStatement(*it)->get_item_selector ();
			}
			// now that we have the SgStatement
			if (isSideEffektingStmt(conditional))
			{
				cout <<"\t\t*need to move >"<<conditional->unparseToString()<<"< outside the control-structure"<<endl;
				if (isWhile)
				{
					// if the loop contains a continue, substitute those with the end-labl and a goto there,
					list<SgNode*> continueStmtList=	NodeQuery::querySubTree(*it,V_SgContinueStmt);
					for (list<SgNode*>::iterator continueIt=continueStmtList.begin();
						continueIt!=continueStmtList.end();
						continueIt++)
						{
							// traverse from this one up to the next loop-parent
							SgNode * tmp=*continueIt;
							while(!isSgWhileStmt(tmp) &&
										!isSgDoWhileStmt(tmp)&&
										!isSgForStatement(tmp)) tmp=tmp->get_parent();
							// if the continue belongs to this while-loop
							if (tmp==*it)
								removeContinues(isSgWhileStmt(*it));
						}
				}
				
				// generate a VariableDeclaration
				SgVariableDeclaration * tempVarDecl=new SgVariableDeclaration(
					Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
					SgName("spinBoolTmp"), new SgTypeBool());
				// get the expression form the statement
				assert(isSgExprStatement(conditional)!=NULL);
				SgExpression * condExpr=isSgExprStatement(conditional)->get_expression ();
				// generate a initial assignment using the existing stmt
				SgInitializer * initializer=new SgAssignInitializer(
					Sg_File_Info::generateDefaultFileInfoForTransformationNode(),condExpr);
				// attach initializer
				tempVarDecl->get_definition()->get_vardefn()->set_initializer (initializer);
				// create a sgvarRefExpr
				SgVarRefExp * ref=new  SgVarRefExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
				new SgVariableSymbol (tempVarDecl->get_definition()->get_vardefn ()));
				// replace the original statement with a SgVarRefExpr
				isSgExprStatement(conditional)->set_expression(ref);
				ref->set_parent(conditional);
				// get the parent scope and set it for the variable
				SgNode * tmp=*it;
				while(!isSgScopeStatement(tmp)) tmp=tmp->get_parent();
				tempVarDecl->get_definition()->get_vardefn()->set_scope(isSgScopeStatement(tmp)->get_scope());
				// and prepend the declaration before the conditional
				LowLevelRewrite::insert(isSgStatement(*it),tempVarDecl,true);
				// and append it to the while-body, which will work together with the inliner..
				if (isWhile)
				{
					
					ref=new  SgVarRefExp (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),
						new SgVariableSymbol (tempVarDecl->get_definition()->get_vardefn ()));
					// copy the other expression
					SgExpression *rhs=isSgExpression(SgTreeCopy().copyAst(condExpr));					
					SgAssignOp * assgnExpr=new SgAssignOp(Sg_File_Info::generateDefaultFileInfoForTransformationNode(),ref,rhs);
					//append at the end of the loop, too
					isSgWhileStmt(*it)->get_body ()->append_statement (
						new		SgExprStatement (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),assgnExpr)
						);
//					assgnExpr->set_parent(isSgWhileStmt(*it)->get_body ());
				}				
			}
			// else do nothing
		}
	}	


	// * 3.3: find all variables in the function,give them a unique name and move them to the bgeinning of the function, this is allowed because this is for C only!!!. This would not work for classes which must be constructed
	announceStep(4,8,"find all variables assign unique name and transform them to \n\tmatch promelas 2-scope-convention");
	string procName;
	vector<SgClassType *>classTypeList;
	// first rename all variables to have unique names...
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
	{
		flattenScopes(isSgFunctionDefinition(procFuncVec[procNr]));
	}
	// output a marked ast
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.reduced.dot");		

	
	
	announceStep(4,9,"final propagation of promela-targets");
	project->unparse();
	PropagatePromelaTargetTraversal().traverse(project);
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.transformations.dot");
	
	cout <<"-------------------------------------------------------------------------------"<<endl
             <<"\tFrom here on the C-Ast will be modified into a Promela/C-AST"<<endl
             <<"-------------------------------------------------------------------------------"<<endl;

	announceStep(6,1,"using the initial promela targets mark the ast");
	// using the sdg traverse the project to unparse to promela
	// the important step is to identify staements which can not be transformed to
	// promela because of PROMELA restrictions and therfore must be kept in C
	// since we have done slicing this is only done for functioncalls and such stuff
	// find the model-intersting nodes`
	sdg=new SystemDependenceGraph();
	sdg->parseProject(project);
	vector<SpinPStructContainer*> pStructContainerArray;
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
	{
		//	list<SgNode*> stmtList=NodeQuery::querySubTree(project,V_SgStatement);
		list<SgNode*> stmtList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgStatement);
		list<SgNode*> markedNodes;
		// find all nodes which have beem tagged for promela-transformation
		for (list<SgNode*>::iterator stmtIt=stmtList.begin();stmtIt!=stmtList.end();stmtIt++)
		{
			if (toConvert(*stmtIt))
			{
				markedNodes.push_back(*stmtIt);
			}
		}
		// once these statements have been identified, track their dependencys
		cout <<"processing stmt's"<<endl;
		for (list<SgNode*>::iterator it=markedNodes.begin();it!=markedNodes.end();it++)
		{
			cout <<"*stmt marked: "<<(*it)->unparseToString()<<endl;
			if (isSgFunctionDefinition(*it)) continue;
			cout <<"  control dependencys:"<<endl;
			DependenceNode* currDepNode=sdg->getNode(*it);
			set <SimpleDirectedGraphNode *> preds=currDepNode->getPredecessors();
			//
			cout <<(*it)->unparseToString()<<" has "<<preds.size()<<" predecessors"<<endl;
			// process all predecessors...
			for (set < SimpleDirectedGraphNode * >::iterator i = preds.begin();i != preds.end(); i++)
			{
				DependenceNode *pred = dynamic_cast < DependenceNode * >(*i);
				// ok this node is what dependency?
				set < DependenceGraph::EdgeType > connectingSet=sdg->edgeType(pred,currDepNode);
				SgNode * predNode=getStatement(pred->getSgNode());
				SgNode * parentPredNode=NULL;
				// since controlstatements like if and while have a statement as an expression in rose and since the expression has all the control dependencys track those, but mark the parent statement,too
				if (isSgWhileStmt(predNode->get_parent())) parentPredNode=predNode->get_parent();
				else if (isSgIfStmt(predNode->get_parent())) parentPredNode=predNode->get_parent();
				// if control, add to the transformation set
				if (connectingSet.count(DependenceGraph::CONTROL))
				{
					cout <<"\tcontrol dependency"<<endl;
					if (toConvert(predNode))
					{
						cout <<"\t>"<<predNode->unparseToString()<<"<already marked for conversion"<<endl;
					}
					else
					{
						markedNodes.push_back(pred->getSgNode());
						cout <<"\tmark >"<<predNode->unparseToString()<<"< for conversion"<<endl;
						markForPromelaTransformation(predNode);
						if (parentPredNode!=NULL)
							markForPromelaTransformation(parentPredNode);
					}
				}
				else if (connectingSet.count(DependenceGraph::DATA))
				{
					cout <<"data dependency presen, but not resolved!"<<endl;
					// this will be done by checkin all controll dependeny nodes and adding those to the promela state vector...
				}
				else
				{
					cout <<"ILLIGAL DEPENDENCY FOUND. Previouse transformations should have removed those!!!"<<endl;
				}
			}
		}
	}
	// now the ast is in its final marked for
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step6_1.dot");

	announceStep(6,2,"Creating PStruct");
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
	{

		/* create astruct that contains the data that is stored in the state vector...*/
		SpinPStructContainer * pStruct=
			new SpinPStructContainer(
				procFuncVec[procNr]->get_declaration()->get_name().getString()
			);
		// add the struct to the AST-for fun->it will be hidden in the promela output pass	
		LowLevelRewrite::insert(
			isSgStatement(procFuncVec[procNr]->get_parent()),
			pStruct->getStructSubTree(),
			true
		);
		
		// put the struct in the vector for later use
		pStructContainerArray.push_back(pStruct);
		// register class type
		classTypeList.push_back(new SgClassType(pStruct->getStructSubTree()));
	}
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step6_2.dot");
	
	announceStep(5,0,"transform MPI-calls to SPIN-M-calls");
	MPISpinReplacement MPIreplacer;
	for (int i=0;i<procFuncVec.size();i++)
	{
//old:	MPIreplacer.replaceIn(procFuncVec[i],pStructContainerArray[i]->getPStructVarDeclaration());
		MPIreplacer.replaceIn(procFuncVec[i],pStructContainerArray[i]);
	}
	project->unparse();
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step5.dot");
	
	announceStep(6,3,"Identifying variables and putting them in the state-vector");
	for (int procNr=0;procNr<procFuncVec.size();procNr++)
	{
		// traverse over each promela-procedure and track wich variables are use.
		//later add those to the state vector. the variables that need to be in 
		// the state vector will be stored in an ast attribute attached to the 
		// function-definition...

		
		// Create the container that will contain the variables for the "promela-export"
		CStateASTAttrib * c_stateAttr=new CStateASTAttrib();
		
		// find all VariableDeclarations
		list<SgNode*> variableDeclarationList=NodeQuery::querySubTree(procFuncVec[procNr],V_SgVariableDeclaration);
		// this set will contain a variable declarations
		set<SgNode*> iniNameList;
		set<SgInitializedName *>cStateVariables;
	
	
		// the list in markedNodes contains all control nodes,
		// now traverse again and check variables
		for (list<SgNode*>::iterator it=
			variableDeclarationList.begin();
			it!=variableDeclarationList.end();
			it++)
		{
			// get the paren scope...
			SgNode * parentScopeTmp=*it;
			// get parents until we find the first scope
			while(!isSgScopeStatement(parentScopeTmp))
			{
				parentScopeTmp=parentScopeTmp->get_parent();
			}
			
			// tmp is now the next scope
			if (toConvert(parentScopeTmp))
			{
				// parent scope will be transforemd to promela -> block might be split up 
				// in mutliple c_blocks, and since varDecls don't carry accross c-blocks->statevec
				SgVariableDeclaration * decl=isSgVariableDeclaration(*it);

				// if the variable is promela-compatible and the parent scope is the scope form the procFuncVec -> no explicit state vector needed, can be keept in promela...
//				if (isPromelaCompatibleVariableType(decl))
//				{
					// ok, mark the variable as unparsable to promela...
//				}
//				else
//				{
					// add it the the state=ast-trribute
					cStateVariables.insert(decl->get_definition()->get_vardefn ());
					cout <<"converting "<<decl->get_definition()->get_vardefn ()->get_name ().getString()<<endl;
					markAsPromelaStateVar(decl);
					c_stateAttr->add(decl->get_definition()->get_vardefn () );
					cout <<"\t\t*added "<< decl->get_definition()->get_vardefn ()->get_name ().getString()<<" to the state-vector"<<endl;
					// remove the old variable-declaration
					LowLevelRewrite::remove(isSgStatement(decl));
					pStructContainerArray[procNr]->addVariable(decl);
//				}				
			}
			else
			{
				// the block is PURE c-> no promelastate will pass across its borders
				// therefore all variable-declaration within can be keept as is...
			}
				

		
			/*CI (4/19/2007) not all variable-uses need to be inc-block style, determine that later
			c_stateAttr->add(isSgInitializedName(*setIt));
			setStateVecVarAttrib(*setIt,new StateVecVarASTAttrib(true));*/
			// now force all nodes that contain a variable that is declared using the c_state stmt to be converted to c!!!!
			markStatementsWithCStateVariables(procFuncVec[procNr],cStateVariables);
			
		}
		// attach the c-state-attribute to the function-defintion
		setCStateAttrib(procFuncVec[procNr],c_stateAttr);
	}
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step6_3.dot");
	
	

	announceStep(6,4,"inserting promela proctype variable to the proc-functions");
	// * 3.4: insert the promela proctype variable to the proc-functions
	for (int i=0;i<procFuncVec.size();i++)
	{
	/*
		// create instance
		SgType * instanceType=classTypeList[i];
		
		Sg_File_Info * instanceFI=Sg_File_Info::generateDefaultFileInfoForTransformationNode();
		SgName instanceName=string("P")+(procFuncVec[i])->get_declaration()->get_name().getString()+"Instance";
		SgStatement * instanceVarDecl=new SgVariableDeclaration(instanceFI,instanceName,instanceType);
		procFuncVec[i]->get_body()->prepend_statement (instanceVarDecl);
		*/
		
		procFuncVec[i]->get_body()->prepend_statement (
			pStructContainerArray[i]->getPStructVarDeclaration()
			);
		// fix scope
		pStructContainerArray[i]->getPStructVarDeclaration()->get_definition()->get_vardefn ()->set_scope(procFuncVec[i]->get_body()->get_scope());

			
		procFuncVec[i]->get_body()->prepend_statement (
			pStructContainerArray[i]->getPStructVarInstanziationDeclaration()
			);
		pStructContainerArray[i]->getPStructVarInstanziationDeclaration()->get_definition()->get_vardefn ()->set_scope(procFuncVec[i]->get_body()->get_scope());
			
/*

		// creat ptr variable
		Sg_File_Info * initializerFI=Sg_File_Info::generateDefaultFileInfoForTransformationNode();
		SgVarRefExp * varRef=new  SgVarRefExp (
		Sg_File_Info::generateDefaultFileInfoForTransformationNode(),new SgVariableSymbol (isSgVariableDeclaration(instanceVarDecl)->get_definition()->get_vardefn ()));
		
		SgAddressOfOp *aof=new SgAddressOfOp (Sg_File_Info::generateDefaultFileInfoForTransformationNode(),isSgExpression(varRef),instanceType);
		SgInitializer * ptrInitializer= new	SgAssignInitializer(initializerFI,aof);
		*/
		/*
		Sg_File_Info * ptrFI=Sg_File_Info::generateDefaultFileInfoForTransformationNode();
//		ptrFI->setCompilerGenerated();
	//	ptrFI->unsetOutputInCodeGeneration ();
		SgType * pointerType=new SgPointerType (instanceType);
		SgName ptrName=string("P")+(procFuncVec[i])->get_declaration()->get_name().getString();
		SgStatement * ptrVarDecl=new SgVariableDeclaration(ptrFI,ptrName,pointerType);
		//SgStatement * ptrVarDecl=new SgVariableDeclaration(ptrFI,ptrName,pointerType,ptrInitializer);
		
		procFuncVec[i]->get_body()->prepend_statement (ptrVarDecl);
		procVariableList.push_back(isSgVariableDeclaration(ptrVarDecl));*/
//		procVariableList.push_back(pStructContainerArray[i]->getPStructVarDeclaration());
	}


	announceStep(6,5,"transform the use of variables in c_code-blocks to PROEMALs-callingconvetion");
	for (int i=0;i<procFuncVec.size();i++)
	{
		refactorSgVarRefExpInCBlocks(procFuncVec[i],pStructContainerArray[i]->getPStructVarDeclaration());
	}
	// since mpi-commands within promela will be transformned in by the promela-mpi-iunclude-> keep the in spin for the moment..., but a direct approach is also thinkable...
	{
	list<SgNode*> stmtList=NodeQuery::querySubTree(procFuncVec[0],V_SgStatement);
	// find all nodes which have beem tagged for promela-transformation
	for (list<SgNode*>::iterator stmtIt=stmtList.begin();stmtIt!=stmtList.end();stmtIt++)
	{
		if (isNodeMPI(*stmtIt) && !toConvert(*stmtIt))
		{
			markForPromelaTransformation(*stmtIt);
		}
	}
	}
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.Step6_5.dot");

	// *3.x finally remove return statements in all promela functions and replace them with a goto to the end of that function body
	announceStep(6,6,"remove all return-statements and replace with goto-statenents");
	for (int i=0;i<procFuncVec.size();i++)
	{
		removeReturnStmts(procFuncVec[i]);
	}

	// 4.x mark AST for unparse, adding astattributes
	announceStep(6,7,"final PROMELA-attribute propagation");
	MarkAST4Unparse finalMarker;
	finalMarker.traverse(project);
	
	// add mpi include and pp-directives
	if (MPIreplacer.getNrOfMPICalls()>0)
	{
		announceStep(6,8,"MPI-touchup. Add the SPIN-M #include mpi-spin.prom directive adn the active MPI_PROC");	
		cout <<"\t\t*found "<<MPIreplacer.getNrOfMPICalls()<<" MPI-calls"<<endl;

		SgGlobal * globalNode=NULL;
		SgNode * tmp=procFuncVec[0];
		while(!isSgGlobal(tmp)) tmp=tmp->get_parent();
		globalNode=isSgGlobal(tmp);

		if (globalNode->get_attachedPreprocessingInfoPtr ()==NULL)
			globalNode->set_attachedPreprocessingInfoPtr(new AttachedPreprocessingInfoType());

		PreprocessingInfo  * spinInclude=new PreprocessingInfo(PreprocessingInfo::CpreprocessorIncludeDeclaration,string("#include \"mpi-spin.prom\""),"user-generated",0,0,0,PreprocessingInfo::before,false,false);
		globalNode->addToAttachedPreprocessingInfo(spinInclude);


		PreprocessingInfo  * activeMPI_PROC=new PreprocessingInfo(PreprocessingInfo::CpreprocessorIncludeDeclaration,string("active MPI_PROC"),"user-generated",0,0,0,PreprocessingInfo::after,false,false);
		globalNode->addToAttachedPreprocessingInfo(activeMPI_PROC);
	}

	cout <<"*--------------------------------*\n\tstep 7->unparse\n*--------------------------------*"<<endl;	
	announceStep(7,0,"unparsing mixed ast");
	// output the dot-grpah
	AstDOTGeneration dotgen;
	dotgen.generateInputFiles(project,AstDOTGeneration::PREORDER);
	// set4: assert that the promela-part is valid
	// Step4: unparse project to C/Promela
//	Unparser_Opt options;
//	ofstream * out=new ofstream("main.pml");
//	CToProMeLaUnparser unparser(out,string("main.pml"),options,0);
//	SgUnparse_Info info;
//	cout <<" Test of unparser. Filename is "<<unparser.getFileName()<<endl;
	debugPromelaAst->generate(string((*(project->get_fileList ()->begin()))->getFileName())+".SPIN.1.dot");
	
	cout <<"unparing to C"<<endl;
	project->unparse();
	for (int i=0;i<procFuncVec.size();i++)
		pStructContainerArray[i]->hideShugar();
	cout << "unparsing to Promela"<<endl;
	unparsePromelaProject(project,NULL,NULL);

//unnparser.
//	unparser.run_unparser();
//	unparseProject(project);
//	unparser.unparseProject(project,info);
	
	return 0;
}
