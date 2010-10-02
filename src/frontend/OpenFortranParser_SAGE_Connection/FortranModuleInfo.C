
#include "sage3basic.h"
#include "FortranModuleInfo.h"
#include "boost/filesystem.hpp"

using namespace std;
using std::string;
using std::map;

// DQ (10/1/2010): Used a typedef to simplify the code using the moduleNameAstMap data member.
// map<string, SgModuleStatement*> FortranModuleInfo::moduleNameAstMap;
FortranModuleInfo::ModuleMapType  FortranModuleInfo::moduleNameAstMap;
unsigned                          FortranModuleInfo::nestedSgFile;
SgProject*                        FortranModuleInfo::currentProject;
vector<string>                    FortranModuleInfo::inputDirs;


bool
FortranModuleInfo::isRmodFile() 
  {

    if (nestedSgFile)
       return true;

    return false;

  }



SgProject*
FortranModuleInfo::getCurrentProject() 
  {
     return currentProject;
  }


string
FortranModuleInfo::find_file_from_inputDirs(string basename ) {
   string dir;
   string name;

   int sizeArg = inputDirs.size();

   for (int i = 0; i< sizeArg; i++) {
       dir = inputDirs[i];
       name = dir+"/"+ basename;

       string tmp = name+MOD_FILE_SUFFIX;
       if (boost::filesystem::exists(tmp.c_str())) {
             return name;
       }
    }

   return  basename;
}



void 
FortranModuleInfo::set_inputDirs(SgProject* project) {

  vector<string> args = project->get_originalCommandLineArgumentList();
  string  rmodDir;

  int sizeArgs = args.size();

  for (int i = 0; i< sizeArgs; i++) {
#if 0
      if (args[i] == "-caf:inputdir") {

          rmodDir = args[i+1];

          if (boost::filesystem::exists(rmodDir.c_str())) {
                inputDirs.push_back(rmodDir);
          } else 
                cout << "WARNING: the input directory is not exist : " << rmodDir<< endl;
      }
#else
     if (args[i].find("-I",0)==0) {
           rmodDir = args[i].substr(2);

           if (boost::filesystem::exists(rmodDir.c_str())) {
                inputDirs.push_back(rmodDir);
          } else 
                cout << "WARNING: the input directory does not exist : " << rmodDir<< endl;
     } 
#endif
  }
   
}



void
FortranModuleInfo::setCurrentProject(SgProject* project)
  {
     currentProject = project;
  }



SgModuleStatement*
FortranModuleInfo::getModule(string modName)
   {
  // DQ (11/12/2008): I am unclear if the conversion of the module name to lowercase 
  // should happen here, it does not appear to be required since at least the tests 
  // codes we have appear to work.

     size_t numberOfModules_before = moduleNameAstMap.size();
  // printf ("In FortranModuleInfo::getModule(%s): numberOfModules_before = %zu \n",modName.c_str(),numberOfModules_before);

  // DQ (10/1/2010): STL Maps should not be used this way (a side-effect is that it adds a null entry to the map).
  // This results in fragle code.  I don't know why, but this breaks in the move to OFP 0.8.2 (likely due to case
  // issues since 0.8.2 eliminates the reduction of all keywords and identifiers to lower case).
  // SgModuleStatement *modStmt = moduleNameAstMap[modName];
  // map<string, SgModuleStatement*>::iterator mapIterator = moduleNameAstMap.find(modName);
     ModuleMapType::iterator mapIterator = moduleNameAstMap.find(modName);
     SgModuleStatement *modStmt = (mapIterator != moduleNameAstMap.end()) ? mapIterator->second : NULL;

  // DQ (10/1/2010): This assert (below) used to fail because STL maps were not being properly handled.
  // Note that it is a little known side-effect of "moduleNameAstMap[modName]" that is will insert an
  // entry into the map.
     size_t numberOfModules_after = moduleNameAstMap.size();
     ROSE_ASSERT(numberOfModules_before == numberOfModules_after);

  // printf ("In FortranModuleInfo::getModule(%s): modStmt = %p \n",modName.c_str(),modStmt);

     if (modStmt != NULL)
        {
          if ( SgProject::get_verbose() > 2 )
               printf ("This module has been previously processed (seen) in this compilation unit. \n");

          return modStmt;
        }

     string nameWithPath = find_file_from_inputDirs(modName);

     if ( SgProject::get_verbose() > 2 )
          printf ("In FortranModuleInfo::getModule(%s): nameWithPath = %s \n",modName.c_str(),nameWithPath.c_str());

  // if (createSgSourceFile(nameWithPath) == NULL )
  // printf ("********* BUILD NEW MODULE FILE IF NOT ALREADY BUILT **************** \n");
     SgSourceFile* newModuleFile = createSgSourceFile(nameWithPath);
  // printf ("********************************************************************* \n");

     if (newModuleFile == NULL )
        {
          cerr << "error: No declaration found for the module: "<<modName << endl;
          return NULL;
        }
       else
        {
       // in createSgSourceFile: insert moduleNameAstMap[modName]

          if ( SgProject::get_verbose() > 2 )
               printf ("In FortranModuleInfo::getModule(%s): createSgSourceFile(nameWithPath) != NULL nameWithPath = %s \n",modName.c_str(),nameWithPath.c_str());
#if 1
       // DQ (10/1/2010): This is a work-around for OFP 0.8.2 failing to call the c_action_end_module_stmt() 
       // rule for an included module (from use statement) that includes another module (again, using 
       // a use statment).  The first module using the use statment will not have its 
       // c_action_end_module_stmt() rule called. See the test_forcing.F90 example test code.

       // Extract the pointer to the SgModule from the SgSourceFile
          Rose_STL_Container<SgNode*> moduleDeclarationList = NodeQuery::querySubTree (newModuleFile,V_SgModuleStatement);

       // There should only be a single module defined in the associated *.rmod file.
          ROSE_ASSERT(moduleDeclarationList.size() == 1);
          modStmt = isSgModuleStatement(moduleDeclarationList[0]);
          ROSE_ASSERT(modStmt != NULL);

       // Insert the extracted module into the moduleNameAstMap (this is the only location where the moduleNameAstMap is modified).
       // moduleNameAstMap.insert(std::pair<string,SgModuleStatement*>(modName,modStmt));
          moduleNameAstMap.insert(ModuleMapType::value_type(modName,modStmt));

#ifdef USE_STMT_DEBUG
          printf ("In FortranModuleInfo::getModule(%s) modStmt = %p: display the moduleNameAstMap \n",modName.c_str(),modStmt);
          dumpMap();
          printf ("DONE: In FortranModuleInfo::getModule(%s) modStmt = %p: display the moduleNameAstMap \n\n",modName.c_str(),modStmt);
#endif
#else
       // ROSE_ASSERT(moduleNameAstMap[modName] != NULL);
       // ROSE_ASSERT(moduleNameAstMap.find(modName) != moduleNameAstMap.end());

       // This is the correct (safer) way to check the existence of an entry 
       // in an STL map (without the side-effect of adding a null entry).
       // return moduleNameAstMap[modName];
          mapIterator = moduleNameAstMap.find(modName);
          modStmt = (mapIterator != moduleNameAstMap.end()) ? mapIterator->second : NULL;
       // ROSE_ASSERT(modStmt != NULL);
#endif
#if 0
       // DQ (10/1/2010): Workaround to OFP failing to call the c_action_end_module_stmt()
          if (mapIterator == moduleNameAstMap.end())
             {
               printf ("Insert newModuleFile = %p modName = %s into moduleNameAstMap \n",newModuleFile,modName.c_str());
            // moduleNameAstMap.insert(std::pair<string,SgModuleStatement*>(modName,newModuleFile));
               moduleNameAstMap.insert(ModuleMapType::pair(modName,newModuleFile));
             }
#endif

          if ( SgProject::get_verbose() > 2 )
               printf ("Leaving FortranModuleInfo::getModule(%s): modStmt = %p \n",modName.c_str(),modStmt);

          return modStmt;
        }
   }


SgSourceFile*
FortranModuleInfo::createSgSourceFile(string modName)
   {
     int errorCode = 0;
     vector<string> argv;

  // DQ (11/12/2008): Modified to force filename to lower case.
  // printf ("In FortranModuleInfo::createSgSourceFile(): generating a module file %s using module name = %s \n",StringUtility::convertToLowerCase(modName).c_str(),modName.c_str());
  // modName = StringUtility::convertToLowerCase(modName);

  // current directory
     string rmodFileName = modName + MOD_FILE_SUFFIX;

  // printf ("Searching for file rmodFileName = %s \n",rmodFileName.c_str());
     if (boost::filesystem::exists(rmodFileName.c_str()) == false)
        {
          printf ("File rmodFileName = %s NOT FOUND (expected to be present) \n",rmodFileName.c_str());
          return NULL;
        }

     argv.push_back(SKIP_SYNTAX_CHECK);
     argv.push_back(rmodFileName);

     nestedSgFile++;

     if ( SgProject::get_verbose() > 1 )
          printf ("START FortranModuleInfo::createSgSourceFile(%s): nestedSgFile = %d \n",rmodFileName.c_str(),nestedSgFile);

     SgProject*  project = getCurrentProject();

     SgSourceFile* newFile = isSgSourceFile(determineFileType(argv,errorCode,project));
  // SgSourceFile* newFile =  new SgSourceFile (argv, errorCode, 0, project);

     ROSE_ASSERT (newFile != NULL);
     ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);

  // Set the project as the  parent 
     newFile->set_parent(project);

  // DQ (11/12/2008): This rmod file should be explicitly marked to not be compiled.
  // printf ("Marking the new module file to not be compiled \n");
     newFile->set_skipfinalCompileStep(true);
     newFile->set_skip_unparse(true);

     project->set_file(*newFile);

     if ( SgProject::get_verbose() > 1 )
          printf ("END FortranModuleInfo::createSgSourceFile(%s): nestedSgFile = %d \n",rmodFileName.c_str(),nestedSgFile);

     nestedSgFile--;

     return newFile;
   }


#if 0
// DQ (10/1/2010): This support is now better implemented directly in the FortranModuleInfo::getModule() function.
void
FortranModuleInfo::addMapping(string modName,SgModuleStatement* modNode)
   {
     printf ("In FortranModuleInfo::addMapping() modName = %s modNode = %p \n",modName.c_str(),modNode);
     ROSE_ASSERT(modNode != NULL);

     if ( moduleNameAstMap[modName] == NULL ) 
        {
          moduleNameAstMap[modName] = modNode;
        }
       else
        {
          cerr << "Warning: The map entry for " << modName << " is not empty. " << endl;
        }

// #ifdef USE_STMT_DEBUG
     printf ("In FortranModuleInfo::addMapping(%s,%p): display the moduleNameAstMap \n",modName.c_str(),modNode);
     dumpMap();
     printf ("DONE: In FortranModuleInfo::addMapping(%s,%p): display the moduleNameAstMap \n\n",modName.c_str(),modNode);
// #endif
  }
#endif



void
FortranModuleInfo::clearMap()
   {
     moduleNameAstMap.clear();
     return;
   }



void 
FortranModuleInfo::dumpMap()
   {
     map<string,SgModuleStatement*>::iterator iter;

     cout << "Module Statement*  map::" << endl;
     for(iter = moduleNameAstMap.begin(); iter != moduleNameAstMap.end(); iter++)
           cout <<"FIRST : " << (*iter).first << " SECOND : " << (*iter).second << endl;    
   }

