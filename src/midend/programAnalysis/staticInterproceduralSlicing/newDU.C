#include "rose.h"

#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DefUseChain.h>
#include <DirectedGraph.h>
#include "DependenceGraph.h"

#include "SlicingInfo.h"
#include "CreateSlice.h"
#include "ControlFlowGraph.h"
#include "DominatorTree.h"


#include <list>
#include <set>
#include <iostream>
#include "DFAnalysis.h"
#include "DefUseExtension.h"

#define DEBUG 1
using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;
using namespace DUVariableAnalysisExt;

void analyseFunction(SgFunctionDefinition * def,DFAnalysis *defUseAnalysis)
{
	list < SgNode * > varUse=NodeQuery::querySubTree(def,V_SgVarRefExp);
	for (list < SgNode * >::iterator i = varUse.begin();i!=varUse.end();i++)
	{
		SgVarRefExp * use=isSgVarRefExp(*i);
		SgNode * interstingUseParent=DUVariableAnalysisExt::getNextParentInterstingNode(use);
		cout <<"SgVarRefExpr: <"<<use->get_symbol()->get_name ().str()<<"> in: "<<interstingUseParent->unparseToString()<<endl;
		bool eval=true;
		bool checkForIndirect=false;
		if (isPointerType(use) || isComposedType(use))
		{
			cout <<"\t*pointer or composed: possible iDef or iUse"<<endl;
			checkForIndirect=true;
		}
		else if (isMemberVar(use))
		{
			cout <<"\t*is a member"<<endl;
			eval=false;
		}
		else
		{
			cout <<"\t*normal variable"<<endl;
		}
		if (defUseAnalysis->isNodeGlobalVariable(use->get_symbol()->get_declaration()))
		{
			cout <<"var is global"<<endl;
		}
		if (eval)
		{
			//check if def
			if (isDef(use))
			{
				cout <<"\t*DEF"<<endl;
			}
			if (isUse(use))
			{
				cout <<"\t*USE"<<endl;
				// output def for this use
				SgInitializedName *initName=use->get_symbol()->get_declaration();
				// get the def for this use
				SgVarRefExp * def;
				SgNode * interstingDefParent;
				std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
				for (int j=0;j<defPlaces.size();j++)
				{
					def=isSgVarRefExp(defPlaces[j]);
//					cout <<"\t\t*def in: "<<defPlaces[j]->unparseToString()<<endl;
					interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
					if (isSgInitializedName(interstingDefParent))
                                                cout <<"\t\t*def in: "<<isSgInitializedName(interstingDefParent)->get_name ().getString()<<endl;					
					else cout <<"\t\t*def in: "<<interstingDefParent->unparseToString()<<endl;
				}
				
			}
			if (isIDef(use))
			{
				cout<<"\t*IDEF"<<endl;
				// output def for this use
				SgInitializedName *initName=use->get_symbol()->get_declaration();
				// get the def for this use
				SgVarRefExp * def;
				SgNode * interstingDefParent;
				std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
				for (int j=0;j<defPlaces.size();j++)
				{
					def=isSgVarRefExp(defPlaces[j]);
					interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
					if (isSgInitializedName(interstingDefParent))
                                                cout <<"\t\t*def in: "<<isSgInitializedName(interstingDefParent)->get_name ().getString()<<endl;					
					else cout <<"\t\t*def in: "<<interstingDefParent->unparseToString()<<endl;
				}
			}
			if (isIUse(use))
			{
				cout<<"\t*IUSE"<<endl;
				// output def for this use
				SgInitializedName *initName=use->get_symbol()->get_declaration();
				// get the def for this use
				SgVarRefExp * def;
				SgNode * interstingDefParent;
				std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
				for (int j=0;j<defPlaces.size();j++)
				{
					def=isSgVarRefExp(defPlaces[j]);
					interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
					if (isSgInitializedName(interstingDefParent))
                                                cout <<"\t\t*def in: "<<isSgInitializedName(interstingDefParent)->get_name ().getString()<<endl;					
					else cout <<"\t\t*def in: "<<interstingDefParent->unparseToString()<<endl;
				}
				defPlaces=defUseAnalysis->getUseFor(use,initName);
				for (int j=0;j<defPlaces.size();j++)
				{
					def=isSgVarRefExp(defPlaces[j]);
					if (isIDef(def))
					interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
					if (isSgInitializedName(interstingDefParent))
                                                cout <<"\t\t*idef in: "<<isSgInitializedName(interstingDefParent)->get_name ().getString()<<endl;					
					else cout <<"\t\t*idef in: "<<interstingDefParent->unparseToString()<<endl;
				}
			}
//			test(use);
		}
		/*
		if (DUVariableAnalysisExt::isDef(use))
		{
			cout<<"\t*this is a def"<<endl;
		}
		
		if (DUVariableAnalysisExt::isIDef(use))
		{
			cout <<"\t*this is a idef"<<endl;
			// get the defining node...
			SgInitializedName *initName=use->get_symbol()->get_declaration();
			SgVarRefExp * def;
			SgNode * interstingDefParent;
			std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
			for (int j=0;j<defPlaces.size();j++)
			{
				def=isSgVarRefExp(defPlaces[j]);
				cout <<"\t\t*def in: "<<defPlaces[j]->unparseToString()<<endl;
				interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
//				cout <<"\t*next intersting node is: "<<interstingDefParent->unparseToString()<<endl;

			}
		}
		if (DUVariableAnalysisExt::isIUse(use))
		{
			cout<<"\t*this is iuse"<<endl;
			// get the use nodes and return only iDefs
			SgInitializedName *initName=use->get_symbol()->get_declaration();
			// get the def for this use
			SgVarRefExp * def;
			SgNode * interstingDefParent;
			std::vector< SgNode* > defPlaces=defUseAnalysis->getUseFor(use,initName);
			for (int j=0;j<defPlaces.size();j++)
			{
				def=isSgVarRefExp(defPlaces[j]);
				cout <<"\t\t*def in: "<<defPlaces[j]->unparseToString()<<endl;
				interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
	//			cout <<"\t*next intersting node is: "<<interstingDefParent->unparseToString()<<endl;

			}
		}
		if (DUVariableAnalysisExt::isUse(use))
		{
			cout<<"\t*this is use"<<endl;
			// get defs
			SgInitializedName *initName=use->get_symbol()->get_declaration();
			// get the def for this use
			SgVarRefExp * def;
			SgNode * interstingDefParent;
			std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
			for (int j=0;j<defPlaces.size();j++)
			{
				def=isSgVarRefExp(defPlaces[j]);
				cout <<"\t\t*def in: "<<defPlaces[j]->unparseToString()<<endl;
				interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
//				cout <<"\t*next intersting node is: "<<interstingDefParent->unparseToString()<<endl;

			}
			// get idefs
		}*/
	/*
		if (isIDef(use))
		{
			cout <<"idef says YES"<<endl;
		}
		if (isIUse(use))
		{
			cout <<"iuse says yes"<<endl;
		}*/
	/*	if (isUse(use))
		{
		*/
		
		/*}*/
	}
}

int main(int argc, char *argv[])
{
	std::string filename;

        SgProject *project = frontend(argc, argv);
	// Create the global def-use analysis
	DFAnalysis *defUseAnalysis=new DefUseAnalysis(project);
	if (defUseAnalysis->run(false)==1)
	{
		std::cerr<<"newDU:: DFAnalysis failed!  defUseAnalysis->run()==0"<<endl;
		exit(0);
	}
	std::vector<InterproceduralInfo*> ip;

        list < SgNode * >functionDeclarations = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);

        for (list < SgNode * >::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++)
        {
                DataDependenceGraph *ddg;
                InterproceduralInfo *ipi;

                SgFunctionDeclaration *fD = isSgFunctionDeclaration(*i);

                // SGFunctionDefinition * fDef;
                ROSE_ASSERT(fD != NULL);

                // CI (01/08/2007): A missing function definition is an indicator to a 
                // 
                // 
                // librarycall. 
                // * An other possibility would be a programmer-mistake, which we
                // don't treat at this point.  // I assume librarycall
                if (fD->get_definition() == NULL)
                {
                }
                else
                {
                        cout <<"--------------------------------------------------------------"<<endl;
                        analyseFunction(fD->get_definition(),defUseAnalysis);

                }   
        }
	return 0;
}
