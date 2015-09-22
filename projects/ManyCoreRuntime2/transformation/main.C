/*
  Author: Pei-Hung Lin
  Contact: lin32@llnl.gov

  Date Created       : June, 2014
 
*/  
#include "NASC.h"
using namespace std;
using namespace SageInterface;
using namespace SageBuilder;
using namespace NASC;

class distAttribute : public AstAttribute
{
  public:
  int isVariableHalo;
  int Halo[3];
  SgVariableSymbol* haloSymbol[3];
  Rose_STL_Container<SgVariableSymbol*> varSymbolList;
  distAttribute() : isVariableHalo(0){Halo[0]=0; Halo[1]=0; Halo[2]=0;}
  distAttribute(int x, int y, int z) : isVariableHalo(0){Halo[0]=x; Halo[1]=y; Halo[2]=z;}
  distAttribute(SgVariableSymbol* x, SgVariableSymbol* y, SgVariableSymbol* z) : isVariableHalo(1){haloSymbol[0]=x; haloSymbol[1]=y; haloSymbol[2]=z;}
  private:
};

class ManycoreFrontend : public AstSimpleProcessing
{
  public: 
    static SgPragmaDeclaration* parentPragma;
    ManycoreFrontend() {parentPragma = NULL; }
  protected:
    virtual void visit(SgNode* n);
};

SgPragmaDeclaration* ManycoreFrontend::parentPragma=NULL;

void ManycoreFrontend::visit(SgNode * node)
{
  if (isSgPragmaDeclaration(node) != NULL)
  {
    SgPragmaDeclaration * pragDecl = isSgPragmaDeclaration(node);
    string pragmaString = pragDecl->get_pragma()->get_pragma();
    // Try to parse the following pragma grammar:
    // #pragma omp data_distribution (dist_policy:array_list) halo(array_list: x_pattern, y_pattern, z_pattern)
    size_t begin = pragmaString.find("data_distribution");
    if(begin != string::npos)
    {
      cout << "full pragma:" << pragmaString << endl;    
      size_t firtRPpos = pragmaString.find(")");
      string distString = pragmaString.substr(begin+17,firtRPpos-(begin+17)+1); 
      cout << "dist info:" << distString << endl;   
      string distPolicy = distString.substr(distString.find("(")+1,distString.find(":")); 
      cout << "dist policy:" << distPolicy << endl;  
      distString = distString.substr(distString.find(":")+1);
      while(distString.find(",") != string::npos || distString.find(")") != string::npos)
      {
//        cout << "dist info:" << distString << endl;  
        string varName = distString.substr(0,distString.find("["));
        cout << "name=" << varName << endl;
        distString = distString.substr(distString.find("[")+1); 
        string xLB = distString.substr(0,distString.find(":"));
        cout << "xLB=" << xLB << endl;
        distString = distString.substr(distString.find(":")+1); 
        string xUB = distString.substr(0,distString.find("]"));
        cout << "xUB=" << xUB << endl;
        distString = distString.substr(distString.find("[")+1); 
        string yLB = distString.substr(0,distString.find(":"));
        cout << "yLB=" << yLB << endl;
        distString = distString.substr(distString.find(":")+1); 
        string yUB = distString.substr(0,distString.find("]"));
        cout << "yUB=" << yUB << endl;
        distString = distString.substr(distString.find("[")+1); 
        string zLB = distString.substr(0,distString.find(":"));
        cout << "zLB=" << zLB << endl;
        distString = distString.substr(distString.find(":")+1); 
        string zUB = distString.substr(0,distString.find("]"));
        cout << "zUB=" << zUB << endl;
        if(distString.find(",") == string::npos)
          distString = distString.substr(distString.find(")")+1);
        distString = distString.substr(distString.find(",")+1);
      }
      string haloString = pragmaString.substr(pragmaString.find("halo")+4); 
      cout << "halo info:" << haloString << endl;    
      haloString = haloString.substr(haloString.find("(")+1);

      SgScopeStatement* scope = getScope(node);
      Rose_STL_Container<SgVariableSymbol*> varList;
      while(haloString.find(":") != string::npos)
      {
//        cout << "dist info:" << haloString << endl;  
        string varName = haloString.substr(0,haloString.find("["));
        cout << "name=" << varName << endl;
        SgVariableSymbol* varSymbol = lookupVariableSymbolInParentScopes(varName, scope);
        ROSE_ASSERT(varSymbol);
        varList.push_back(varSymbol);
        haloString = haloString.substr(haloString.find("[")+1); 
        string xLB = haloString.substr(0,haloString.find(":"));
        cout << "xLB=" << xLB << endl;
        haloString = haloString.substr(haloString.find(":")+1); 
        string xUB = haloString.substr(0,haloString.find("]"));
        cout << "xUB=" << xUB << endl;
        haloString = haloString.substr(haloString.find("[")+1); 
        string yLB = haloString.substr(0,haloString.find(":"));
        cout << "yLB=" << yLB << endl;
        haloString = haloString.substr(haloString.find(":")+1); 
        string yUB = haloString.substr(0,haloString.find("]"));
        cout << "yUB=" << yUB << endl;
        haloString = haloString.substr(haloString.find("[")+1); 
        string zLB = haloString.substr(0,haloString.find(":"));
        cout << "zLB=" << zLB << endl;
        haloString = haloString.substr(haloString.find(":")+1); 
        string zUB = haloString.substr(0,haloString.find("]"));
        cout << "zUB=" << zUB << endl;
        if(haloString.find(",") > haloString.find(":"))
        {
          haloString = haloString.substr(haloString.find(":")+1);
          break;
        }
        haloString = haloString.substr(haloString.find(",")+1);
      }
      string xHaloLB = haloString.substr(haloString.find("<")+1, haloString.find(",")-haloString.find("<")-1);
      string xHaloUB = haloString.substr(haloString.find(",")+1, haloString.find(">")-haloString.find(",")-1);
        cout << "HaloXLB=" << xHaloLB <<  " HaloXUB=" << xHaloUB << endl;
      SgVariableSymbol* xSymbol = lookupVariableSymbolInParentScopes(xHaloLB, scope);
      haloString = haloString.substr(haloString.find("]")+2);
      string yHaloLB = haloString.substr(haloString.find("<")+1, haloString.find(",")-haloString.find("<")-1);
      string yHaloUB = haloString.substr(haloString.find(",")+1, haloString.find(">")-haloString.find(",")-1);
        cout << "HaloYLB=" << yHaloLB <<  " HaloYUB=" << yHaloUB << endl;
      SgVariableSymbol* ySymbol = lookupVariableSymbolInParentScopes(yHaloLB, scope);
      haloString = haloString.substr(haloString.find("]")+2);
      string zHaloLB = haloString.substr(haloString.find("<")+1, haloString.find(",")-haloString.find("<")-1);
      string zHaloUB = haloString.substr(haloString.find(",")+1, haloString.find(">")-haloString.find(",")-1);
        cout << "HaloZLB=" << zHaloLB <<  " HaloZUB=" << zHaloUB << endl;
      SgVariableSymbol* zSymbol = lookupVariableSymbolInParentScopes(zHaloLB, scope);
      distAttribute* haloAttribute = new distAttribute(xSymbol, ySymbol, zSymbol);
      haloAttribute->varSymbolList = varList;
      node->addNewAttribute("haloAttribute",haloAttribute);
    }
    
  }
}// end ManycoreFrontend::visit

int main( int argc, char * argv[] )
{
  SgProject* project = frontend(argc,argv);
  ManycoreFrontend manycorefrontend;
  manycorefrontend.traverse(project, preorder);
  transformation(project);
//  SgScopeStatement* scope = getFirstGlobalScope(project);
//  buildCenterComputation(scope);
//  buildBoundaryComputation(scope);
  return backend(project);
}


void NASC::transformation(SgProject* project)
{
  Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(project, V_SgPragmaDeclaration);
  typedef Rose_STL_Container<SgNode*>::reverse_iterator pragmaListIterator;
  for (pragmaListIterator listElement=pragmaList.rbegin();listElement!=pragmaList.rend();++listElement)
  {
    SgPragmaDeclaration * decl = isSgPragmaDeclaration(*listElement);
    ROSE_ASSERT(decl != NULL);
    SgStatement* nextStmt = getNextStatement(decl);
    ROSE_ASSERT(nextStmt != NULL);
    AstAttribute* attr = decl->getAttribute("haloAttribute");
    distAttribute* haloAttribute = dynamic_cast<distAttribute*> (attr);
    if(haloAttribute)
    {
      if(haloAttribute->isVariableHalo)
      {
        SgVariableSymbol* xHaloSymbol = haloAttribute->haloSymbol[0];
        SgVariableSymbol* yHaloSymbol = haloAttribute->haloSymbol[1];
        SgVariableSymbol* zHaloSymbol = haloAttribute->haloSymbol[2];
        cout << "X halo:" << haloAttribute->haloSymbol[0]->get_name() << endl;
      }
      else
      {
        int xHalo = haloAttribute->Halo[0];
        int yHalo = haloAttribute->Halo[1];
        int zHalo = haloAttribute->Halo[2];
        cout << "X halo:" << haloAttribute->Halo[0] << endl;
      }
      Rose_STL_Container<SgVariableSymbol*> localList = haloAttribute->varSymbolList;
      cout << "first symbol used for dist is:" << localList.at(0)->get_name() << endl;
cout << "nextStmt:" << nextStmt->sage_class_name() << endl;
      createSubdomainBound(localList.at(0),nextStmt);
    }
  }
}

void NASC::createSubdomainBound(SgVariableSymbol* var, SgStatement* stmt)
{
  SgScopeStatement* scope = getScope(stmt);
  SgVarRefExp* varRef = buildVarRefExp(var);
  SgModifierType* constIntType = buildConstType(buildIntType());
  SgVariableDeclaration* varDeclX = buildVariableDeclaration("size", \
	constIntType, \
	buildAssignInitializer(buildDotExp(varRef,buildFunctionCallExp("get_arraySize",constIntType,buildExprListExp(buildIntVal(0)),scope)),constIntType), \
	scope);
  insertStatementBefore(stmt,varDeclX);
}

void NASC::buildCenterComputation(SgScopeStatement* scope)
{
  SgName funcName = SgName("center");
  SgFunctionDeclaration* funcNonDefDecl = buildNondefiningFunctionDeclaration(funcName, buildVoidType(),buildFunctionParameterList(), scope, NULL);
  SgFunctionDeclaration* funcDefDecl = buildDefiningFunctionDeclaration(funcName, buildVoidType(),buildFunctionParameterList(), scope, NULL, false, funcNonDefDecl, NULL);
  appendStatement(funcDefDecl,scope);
  return;
}

void NASC::buildBoundaryComputation(SgScopeStatement* scope)
{
  SgName funcName = SgName("boundary");
  SgFunctionDeclaration* funcNonDefDecl = buildNondefiningFunctionDeclaration(funcName, buildVoidType(),buildFunctionParameterList(), scope, NULL);
  SgFunctionDeclaration* funcDefDecl = buildDefiningFunctionDeclaration(funcName, buildVoidType(),buildFunctionParameterList(), scope, NULL, false, funcNonDefDecl, NULL);
  appendStatement(funcDefDecl,scope);
  return;
}
