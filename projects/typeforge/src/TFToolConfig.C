#include "TFToolConfig.h"
#include <iostream>

namespace Typeforge {

namespace TFToolConfig {

using namespace std;

ToolConfig* config = nullptr;
string fileName = "";

void open(string name){
  fileName = name;
  if(config == nullptr){
    try{
      config = new ToolConfig(name);
    }catch(...){
      config = new ToolConfig();
    }
    config->setToolID("typeforge");
  }
}

void write(){
  if(config == nullptr) return;
  if(fileName != ""){
    config->saveConfig(fileName);
  }
  fileName = "";
  delete config;
  config = nullptr;
}

void addChangeVarType(string handle, string name, string scope, string source, string fromType, string toType){
  if(config != nullptr) config->addReplaceVarType(handle, name, scope, source, fromType, toType);
}

void addChangeVarBaseType(string handle, string name, string scope, string source, string fromType, string toType){
  if(config != nullptr) config->addReplaceVarBaseType(handle, name, scope, source, fromType, toType);
}

}

}

