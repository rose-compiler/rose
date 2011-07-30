// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 17,6 MB to 7,2MB
#include "sage3basic.h"
#include "DFAnalysis.h"
#include "DefUseAnalysis.h"
#include "EDefUse.h"
using namespace std;
EDefUse::EDefUse(SgProject * proj)
{
  internalDefUse=new DefUseAnalysis(proj);
}


void EDefUse::printDefUse()
{

	cout<< "defs:\n";

	internalDefUse->printDefMap();

	cout<< "uses:\n";

  	internalDefUse->printUseMap();

}

int EDefUse::run(bool debug)
{
  internalDefUse->run(debug);
  return 0; // JJW 10-17-2007 does not appear to ever be used
}
std::vector < SgNode* > EDefUse::getDefFor(SgNode* node, SgInitializedName* initName)
{
  std::vector < SgNode* > retVec,
                          defVec=
  internalDefUse->getDefFor(node,initName);
  for (unsigned int i=0;i<defVec.size();i++)
  {
    SgNode * defNode=defVec[i];
    if (isSgFunctionCallExp(defNode))
    {
      SgFunctionCallExp *call=isSgFunctionCallExp(defNode);
      Rose_STL_Container< SgNode * > varRefList=NodeQuery::querySubTree(call,V_SgVarRefExp);
      bool foundDefinition=false;
      for (Rose_STL_Container< SgNode * >::iterator varRefIt=varRefList.begin();
           varRefIt!=varRefList.end();
           varRefIt++)
      {
        // check if this var ref is the defined variable
        SgVarRefExp * varRef=isSgVarRefExp(*varRefIt);
        if (varRef->get_symbol()->get_declaration ()!=initName)
        {
          // wrong variable, discard
          continue;
        }
        SgNode * hiker=varRef;
        while(!isSgExprListExp(hiker->get_parent())) hiker=hiker->get_parent();
        // hiker is the element just beneath the exprListExp
        if (!isSgFunctionCallExp(hiker->get_parent()->get_parent()) && hiker->get_parent()->get_parent()!=call)
        {
          // wrong function or not the right list
          continue;
        }
        // this is the correct var ref
        foundDefinition|=true;
        retVec.push_back(hiker);
      }
    }
    else
    {
      retVec.push_back(defNode);
    }
  }
  return retVec;
}
std::vector < SgNode* > EDefUse::getUseFor(SgNode* node, SgInitializedName* initName)
{
  return internalDefUse->getUseFor(node,initName);
}
bool EDefUse::isNodeGlobalVariable(SgInitializedName* node)
{
  return internalDefUse->isNodeGlobalVariable(node);
}
std::vector < std::pair < SgInitializedName* , SgNode* > > EDefUse::getDefMultiMapFor(SgNode* node)
{
  return internalDefUse->getDefMultiMapFor(node);
}
