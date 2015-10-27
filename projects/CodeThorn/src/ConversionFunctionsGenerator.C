#include "ConversionFunctionsGenerator.h"

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
  code+="void createGlobalVarAddressMaps() {\n";
  for(set<string>::iterator i=vars.begin();i!=vars.end();++i) {
    string var=*i;
    string varNameCode="\""+var+"\"";
    string varAddressCode="&"+var;
    code+="mapGlobalVarInsert("+varNameCode+",(int*)"+varAddressCode+");\n";
  } 
  code+="}\n";
  return code;
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
