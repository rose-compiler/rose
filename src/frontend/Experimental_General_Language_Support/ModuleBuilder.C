#include "sage3basic.h"
#include "ModuleBuilder.h"

namespace Rose {

using namespace Rose::Diagnostics;

#if TEMPLATES
template <typename T>
bool ModuleBuilder<T>::isRoseModuleFile() 
#else
bool ModuleBuilder::isRoseModuleFile() 
#endif
  {
     if (nestedSgFile) {
        return true;
     }
     return false;
  }


#if TEMPLATES
template <typename T>
SgProject* ModuleBuilder<T>::getCurrentProject() 
#else
SgProject* ModuleBuilder::getCurrentProject() 
#endif
  {
  // return currentProject;
     return SageInterface::getProject();
  }


#if TEMPLATES
template <typename T>
std::string ModuleBuilder<T>::find_file_from_inputDirs(const std::string & basename)
#else
std::string ModuleBuilder::find_file_from_inputDirs(const std::string & basename)
#endif
  {
      std::string dir;
      std::string name;

      int sizeArg = inputDirs.size();

      for (int i = 0; i< sizeArg; i++) {
         dir = inputDirs[i];
         name = dir+"/"+ basename;

         std::string tmp = name + MOD_FILE_SUFFIX;
         if (boost::filesystem::exists(tmp)) {
            return name;
         }
      }

      return basename;
  }



#if TEMPLATES
template <typename T>
void ModuleBuilder<T>::set_inputDirs(SgProject* project) {
#else
void ModuleBuilder::set_inputDirs(SgProject* project) {
#endif

   std::vector<std::string> args = project->get_originalCommandLineArgumentList();
   std::string  rmodDir;

  // Add path to iso_c_binding.rmod. The intrinsic modules have been placed in the
  // 3rdPartyLibraries because they could be compiler dependent. If placed there we could
  // reasonable have multiple versions at some point.
  //
  std::string intrinsic_mod_path = findRoseSupportPathFromSource("src/3rdPartyLibraries/fortran-parser", "share/rose");
  inputDirs.push_back(intrinsic_mod_path);

  int sizeArgs = args.size();

  for (int i = 0; i< sizeArgs; i++) {
     if (args[i].find("-I",0)==0) {
           rmodDir = args[i].substr(2);
           std::string rmodDir_no_quotes =
                boost::replace_all_copy(rmodDir, "\"", "");

           if (boost::filesystem::exists(rmodDir_no_quotes.c_str())) {
                inputDirs.push_back(rmodDir_no_quotes);
          } else 
             {
               if (Rose::ir_node_mlog[Rose::Diagnostics::DEBUG])
                  {
                     std::cout << "WARNING: the input directory does not exist (rose): " << rmodDir << std::endl;
                  }
             }
     } 
  }
   
}



#if TEMPLATES
template <typename T>
void ModuleBuilder<T>::setCurrentProject(SgProject* project)
#else
void ModuleBuilder::setCurrentProject(SgProject* project)
#endif
  {
     currentProject = project;
  }



#if TEMPLATES
template <typename T>
T* ModuleBuilder<T>::getModule(const std::string &module_name)
#else
SgJovialCompoolStatement* ModuleBuilder::getModule(const std::string &module_name)
#endif
   {
  // A note about the syntax. The typename (or class) qualifier is required to give a hint to the
  // compiler because the iterator has a template parameter. Find returns a pair so second is used to
  // return the value.
     typename ModuleMapType::iterator mapIterator = moduleNameMap.find(module_name);
#if TEMPLATES
     T* module_stmt = (mapIterator != moduleNameMap.end()) ? mapIterator->second : NULL;
#else
     SgJovialCompoolStatement* module_stmt = (mapIterator != moduleNameMap.end()) ? mapIterator->second : NULL;
#endif

  // No need to read the module file if module declaration was stored in the map
     if (module_stmt != NULL)
        {
           return module_stmt;
        }

     std::string nameWithPath = find_file_from_inputDirs(module_name);

     SgSourceFile* newModuleFile = createSgSourceFile(nameWithPath);

     if (newModuleFile == NULL )
        {
           mlog[ERROR] << "ModuleBuilder::getModule: No file found for the module: "<< module_name << std::endl;
           ROSE_ASSERT(false);
           return NULL;
        }
     else
        {
        // Extract the pointer to the SgModule from the SgSourceFile
        //
        // WARNING: For Fortran: Rose_STL_Container<SgNode*> moduleDeclarationList = NodeQuery::querySubTree (newModuleFile,V_ SgModuleStatement);
           Rose_STL_Container<SgNode*> moduleDeclarationList = NodeQuery::querySubTree (newModuleFile,V_SgJovialCompoolStatement);

        // There should only be a single module defined in the associated *.rmod file.
           ROSE_ASSERT(moduleDeclarationList.size() == 1);

           module_stmt = isSgJovialCompoolStatement(moduleDeclarationList[0]);
           ASSERT_not_null(module_stmt);

        // Store the extracted module into the module name map (this is the only location where the map is modified)
           moduleNameMap.insert(ModuleMapType::value_type(module_name, module_stmt));

           return module_stmt;
        }
   }


#if TEMPLATES
template <typename T>
SgSourceFile* ModuleBuilder<T>::createSgSourceFile(const std::string &module_name)
#else
SgSourceFile* ModuleBuilder::createSgSourceFile(const std::string &module_name)
#endif
  {
     int errorCode = 0;
     std::vector<std::string> argv;

  // current directory
     std::string module_filename = boost::algorithm::to_lower_copy(module_name) + MOD_FILE_SUFFIX;

     if (boost::filesystem::exists(module_filename) == false)
        {
           mlog[ERROR] << "Module file filename = " << module_filename << " NOT FOUND (expected to be present) \n";
           ROSE_ASSERT(false);
           //return NULL;
        }

     argv.push_back(SKIP_SYNTAX_CHECK);
     argv.push_back(module_filename);

     nestedSgFile++;

     SgProject* project = getCurrentProject();
     ASSERT_not_null(project);

     SgSourceFile* newFile = isSgSourceFile(determineFileType(argv,errorCode,project));
     ASSERT_not_null(newFile);

  // Run the frontend to process the compool file
     newFile->runFrontend(errorCode);

     if (errorCode != 0)
        {
           mlog[ERROR] << "In ModuleBuilder<T>::createSgSourceFile(): frontend returned 0 \n";
           ROSE_ASSERT(errorCode == 0);
        }

     ASSERT_not_null (newFile);
     ASSERT_not_null (newFile->get_startOfConstruct());
     ROSE_ASSERT (newFile->get_parent() == project);

  // Don't want to unparse or compile a module file
     newFile->set_skipfinalCompileStep(true);
     newFile->set_skip_unparse(true);

     project->set_file(*newFile);

     nestedSgFile--;

     return newFile;
  }


#if TEMPLATES
template <typename T>
void ModuleBuilder<T>::clearMap()
#else
void ModuleBuilder::clearMap()
#endif
  {
     moduleNameMap.clear();
     return;
  }


#if TEMPLATES
template <typename T>
void ModuleBuilder<T>::dumpMap()
#else
void ModuleBuilder::dumpMap()
#endif
  {
#if TEMPLATES
     std::map<std::string,T*>::iterator iter;
#else
     std::map<std::string,SgJovialCompoolStatement*>::iterator iter;
#endif

     std::cout << "Module Statement*  map::" << std::endl;
     for (iter = moduleNameMap.begin(); iter != moduleNameMap.end(); iter++) {
        std::cout <<"FIRST : " << (*iter).first << " SECOND : " << (*iter).second << std::endl;
     }
  }


} // namespace Rose
