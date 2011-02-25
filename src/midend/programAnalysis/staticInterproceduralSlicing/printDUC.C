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
#include "DefUseExtension.h"
#include "EDefUse.h"
//nclude "stl_pair.h"

#define DEBUG 1
using namespace DominatorTreesAndDominanceFrontiers;
using namespace std;
using namespace DUVariableAnalysisExt;

void printNode(std::ofstream &f,SgInitializedName * name,SgNode* interestingNode,set<SgNode*> * printedSet)
{
  // if the node is in the printed set, skip
  if (printedSet->count(interestingNode)) return;
  printedSet->insert(interestingNode);
  char ptrStr[100];
  sprintf(ptrStr,"%p",interestingNode);
  f <<"\""<<ptrStr<<"\" [label=\"Var:";
    f<<isSgInitializedName(name)->get_name ().getString();
//  f<<"\\n in interesting node: "<<interestingNode->unparseToString();
//  f<<"\\n of type: "<<interestingNode->class_name  ();
  SgNode * tmp=interestingNode;
  while(!isSgStatement(tmp))tmp=tmp->get_parent();
  f<<"\\n in Statement: "<<tmp->unparseToString()<<"\"];";
}

void printEdge(std::ofstream & f,SgNode * source,SgNode*dest,int type,set<pair<SgNode*,SgNode*> > * printedSet)
{
  char defPtrStr[100],usePtrStr[100];
  sprintf(usePtrStr,"%p",dest);
  sprintf(defPtrStr,"%p",source);
  if (printedSet->count(pair<SgNode*,SgNode*>(source,dest)))
    return;
  printedSet->insert(pair<SgNode*,SgNode*>(source,dest));
  // don't print self edges
  if (type)
    f<<"\""<<defPtrStr<<"\" -> \""<<usePtrStr<<"\"[label=\"DefUse\",color=green];"<<endl;
   
  else if (dest!=source)
    f<<"\""<<defPtrStr<<"\" -> \""<<usePtrStr<<"\"[label=\"Use\",color=blue];"<<endl;
}

void analyseFunction(SgFunctionDefinition * def,EDefUse *defUseAnalysis,std::ofstream &f)
{
        char defPtrStr[100],usePtrStr[100];;
        set<SgNode*> *printedNodeSet=new set<SgNode*>();
        set<pair<SgNode*,SgNode*> > *printedDataEdgeSet=new set<pair<SgNode*,SgNode*> >();

        list < SgNode * > varUse=NodeQuery::querySubTree(def,V_SgVarRefExp);
        for (list < SgNode * >::iterator i = varUse.begin();i!=varUse.end();i++)
        {
                SgVarRefExp * use=isSgVarRefExp(*i);
                SgNode * interstingUseParent=DUVariableAnalysisExt::getNextParentInterstingNode(use);
                // output def for this use
                SgInitializedName *initName=use->get_symbol()->get_declaration();
                // get the def for this use
                SgVarRefExp * def;
                SgNode * interstingDefParent;
                std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
                for (int j=0;j<defPlaces.size();j++)
                {
                        interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
      printNode(f,initName,interstingUseParent,printedNodeSet);
      printNode(f,initName,interstingDefParent,printedNodeSet);
      printEdge(f,interstingDefParent,interstingUseParent,1,printedDataEdgeSet);
                }
  }
        for (list < SgNode * >::iterator i = varUse.begin();i!=varUse.end();i++)
        {
                SgVarRefExp * use=isSgVarRefExp(*i);
                SgNode * interstingUseParent=DUVariableAnalysisExt::getNextParentInterstingNode(use);
                // output def for this use
                SgInitializedName *initName=use->get_symbol()->get_declaration();
                // get the def for this use
                SgVarRefExp * def;
                SgNode * interstingDefParent;
                std::vector< SgNode* > defPlaces=defUseAnalysis->getDefFor(use,initName);
                defPlaces=defUseAnalysis->getUseFor(use,initName);
                for (int j=0;j<defPlaces.size();j++)
                {
      interstingDefParent=DUVariableAnalysisExt::getNextParentInterstingNode(defPlaces[j]);
      printNode(f,initName,interstingUseParent,printedNodeSet);
      printNode(f,initName,interstingDefParent,printedNodeSet);
      printEdge(f,interstingUseParent,interstingDefParent,0,printedDataEdgeSet);
                }
  }
                //                      test(use);
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
//                              cout <<"\t*next intersting node is: "<<interstingDefParent->unparseToString()<<endl;

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
        //                      cout <<"\t*next intersting node is: "<<interstingDefParent->unparseToString()<<endl;

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
//                              cout <<"\t*next intersting node is: "<<interstingDefParent->unparseToString()<<endl;

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
        /*      if (isUse(use))
                {
                */
                
                /*}*/
}

int main(int argc, char *argv[])
{
        std::string filename;

    SgProject *project = frontend(argc, argv);
                // Create the global def-use analysis
                EDefUse *defUseAnalysis=new EDefUse(project);
                if (defUseAnalysis->run(false)==0)
                {
                        std::cerr<<"EDefUse failed!"<<endl;
                }
                std::vector<InterproceduralInfo*> ip;

    list < SgNode * >functionDeclarations = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
        
    for (list < SgNode * >::iterator i = functionDeclarations.begin(); i != functionDeclarations.end(); i++)
    {
                filename=(*(*project->get_fileList()).begin())->get_sourceFileNameWithoutPath ()+"."+isSgFunctionDeclaration(*i)->get_name ().getString()+".duc.dot";
                //std::ofstream f("duCHAIN.dot");
                std::ofstream f(filename.c_str());
                f << "digraph \"GDUCHAIN\" {" << std::endl;
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
                                        analyseFunction(fD->get_definition(),defUseAnalysis,f);

        }   
                f << "}" << std::endl;
    f.close();
    }
                return 0;
}
