#include "TFToolConfig.h"
#include <iostream>
using namespace std;

ToolConfig* TFToolConfig::config = nullptr;
string TFToolConfig::fileName = "";

void TFToolConfig::open(string name){
  TFToolConfig::fileName = name;
  if(TFToolConfig::config == nullptr){
    try{
      TFToolConfig::config = new ToolConfig(name);
    }catch(...){
      TFToolConfig::config = new ToolConfig();
    }
    TFToolConfig::config->setToolID("typeforge");
  }
}

void TFToolConfig::write(){
  if(TFToolConfig::config == nullptr) return;
  if(TFToolConfig::fileName != ""){
    TFToolConfig::config->saveConfig(TFToolConfig::fileName);
  }
  TFToolConfig::fileName = "";
  delete TFToolConfig::config;
  TFToolConfig::config = nullptr;
}

void TFToolConfig::addChangeVarType(string handle, string name, string scope, string source, string fromType, string toType){
  if(TFToolConfig::config != nullptr) TFToolConfig::config->addReplaceVarType(handle, name, scope, source, fromType, toType);
}

void TFToolConfig::addChangeVarBaseType(string handle, string name, string scope, string source, string fromType, string toType){
  if(TFToolConfig::config != nullptr) TFToolConfig::config->addReplaceVarBaseType(handle, name, scope, source, fromType, toType);
}

