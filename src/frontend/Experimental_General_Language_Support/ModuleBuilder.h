#ifndef ROSE_MODULE_BUILDER_H_
#define ROSE_MODULE_BUILDER_H_

#include <map>

#define TEMPLATES 0

class SgProject;
class SgSourceFile;

// Need std=c++11
//
#ifndef nullptr
#define nullptr NULL
#endif

namespace Rose {

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Load a Fortran module file or Jovial compool file

/*
 *  Read a module file "*.{rmod,rcmp}"
 *  Create a SgFile for the module
 *  Create an entry for the module in the moduleNameMap
 */

#define SKIP_SYNTAX_CHECK "-rose:skip_syntax_check"

class ModuleBuilder
{
   typedef std::map<std::string, SgSourceFile*> ModuleMapType;

public:
   ModuleBuilder() : nestedSgFile(0), currentProject(nullptr)
    {
    }

   bool areLoadingModuleFile() { return (nestedSgFile > 0 ? true : false); }

   void setCurrentProject(SgProject* project);
   SgProject* getCurrentProject() { return SageInterface::getProject(); }

   void loadModule(const std::string &module_name, std::vector<std::string> &import_names, SgGlobal* file_scope);
   SgSourceFile* getModule(const std::string &module_name);
   void addMapping(const std::string &module_name, SgSourceFile* module_stmt);

   std::string getModuleFileSuffix() {return std::string(".rcmp");}
   std::string find_file_from_inputDirs(const std::string &name);

   void setInputDirs(SgProject*);
 
 private:
   unsigned int nestedSgFile;
   SgProject* currentProject;

   std::vector<std::string> inputDirs;
   ModuleMapType moduleNameMap;

   SgSourceFile* createSgSourceFile(const std::string &module_name);

   void clearMap() { moduleNameMap.clear(); }
   void dumpMap();
};


class ModuleBuilderFactory
{
 public:
  static ModuleBuilder& get_compool_builder()
    {
      static ModuleBuilder compool_builder;
      return compool_builder;
    }
};


} // namespace Rose

#endif /* ROSE_MODULE_BUILDER_H_ */
