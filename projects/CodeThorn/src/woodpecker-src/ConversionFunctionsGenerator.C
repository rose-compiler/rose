#include "sage3basic.h"
#include "ConversionFunctionsGenerator.h"
#include "SgNodeHelper.h"
#include "CppStdUtilities.h"

using namespace std;

string ConversionFunctionsGenerator::generateCodeForGlobalVarAdressMaps(set<string> vars) {
  string code;
  code+="#include <string>\n";
  code+="#include <map>\n";

  code+="std::map <std::string,int*> mapGlobalVarAddress;\n";
  code+="std::map <int*,std::string> mapAddressGlobalVar;\n";

  code+="void mapGlobalVarInsert(std::string name, int* addr) {\n \
         mapGlobalVarAddress[name]=addr;\n \
         mapAddressGlobalVar[addr]=name;\n \
         }\n";
  code+="void createGlobalVarAddressMapsFP() {\n";
  for(set<string>::iterator i=vars.begin();i!=vars.end();++i) {
    string var=*i;
    string varNameCode="\""+var+"\"";
    string varAddressCode="&"+var;
    code+="mapGlobalVarInsert("+varNameCode+",(int*)"+varAddressCode+");\n";
  } 
  code+="}\n";
  return code;
}


void ConversionFunctionsGenerator::generateFile(SgProject* root, string conversionFunctionsFileName) {
  set<string> varNameSet;
  std::list<SgVariableDeclaration*> globalVarDeclList=SgNodeHelper::listOfGlobalVars(root);
  for(std::list<SgVariableDeclaration*>::iterator i=globalVarDeclList.begin();i!=globalVarDeclList.end();++i) {
    SgInitializedNamePtrList& initNamePtrList=(*i)->get_variables();
    for(SgInitializedNamePtrList::iterator j=initNamePtrList.begin();j!=initNamePtrList.end();++j) {
      SgInitializedName* initName=*j;
      if ( true || isSgArrayType(initName->get_type()) ) {  // optional filter (array variables only)
        SgName varName=initName->get_name();
        string varNameString=varName; // implicit conversion
        varNameSet.insert(varNameString);
      }
    }
  }
  string code=generateCodeForGlobalVarAdressMaps(varNameSet);
  ofstream myfile;
  myfile.open(conversionFunctionsFileName.c_str());
  myfile<<code;
  myfile.close();
}

#if 0
int main() {
  set<string> s;
  s.insert("a");
  s.insert("b");
  s.insert("c");
  ConversionFunctionsGenerator gen;
  cout << gen.generateCodeForGlobalVarAdressMaps(s);
  return 0;
}
#endif
