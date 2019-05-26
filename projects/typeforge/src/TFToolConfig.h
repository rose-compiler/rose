#ifndef TYPEFORGE_TOOLCONFIG_H
#define TYPEFORGE_TOOLCONFIG_H

#include "ToolConfig.hpp"

namespace Typeforge {

namespace TFToolConfig {
 
  extern ToolConfig* config;
  extern std::string fileName;

  void open(std::string name);
  void write();

  void addChangeVarType(std::string handle, std::string name, std::string scope, std::string source, std::string fromType, std::string toType);
  void addChangeVarBaseType(std::string handle, std::string name, std::string scope, std::string source, std::string fromType, std::string toType);

}

}

#endif
