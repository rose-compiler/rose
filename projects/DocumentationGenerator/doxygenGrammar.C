#include <doxygenGrammar.h>

using namespace std;

DxNode::DxNode() : body() 
   {
   }

DxParameter::DxParameter(string _name) : name(_name) 
   {
   }

string
DxNode::unparse() 
   {
     string cmdName = commandName();
     string str;
     if (!cmdName.empty()) 
        {
          str = "\\"+cmdName+" ";
        }
     str.append(body);
     return str;
   }

string
DxText::commandName() 
   {
     return string();
   }

string
DxBrief::commandName() 
   {
     return "brief";
   }

string
DxName::commandName() 
   {
     return "name";
   }

string
DxSeeAlso::commandName() 
   {
     return "sa";
   }

string
DxParameter::commandName() 
   {
     return "param";
   }

string
DxParameter::unparse() 
   {
     return "\\param "+name+" "+body;
   }

string
DxReturn::commandName() 
   {
     return "return";
   }

string
DxDeprecated::commandName() 
   {
     return "deprecated";
   }

string
DxFunctionDeclaration::commandName() 
   {
     return "fn";
   }

string
DxVariableDeclaration::commandName() 
   {
     return "var";
   }

string
DxClassDeclaration::commandName() 
   {
     return "class";
   }
