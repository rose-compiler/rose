
#include "sage3basic.h"
#include "FortranModuleInfo.h"
#include "boost/filesystem.hpp"

using namespace std;
using std::string;
using std::map;

map<string, SgModuleStatement*> FortranModuleInfo::moduleNameAstMap;
unsigned                        FortranModuleInfo::nestedSgFile;
SgProject*                      FortranModuleInfo::currentProject;
vector<string>                      FortranModuleInfo::inputDirs;


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

    SgModuleStatement *modStmt = moduleNameAstMap[modName];

    if (modStmt)
          return modStmt;

    string nameWithPath = find_file_from_inputDirs(modName);

    if (createSgSourceFile(nameWithPath) == NULL ) {
        cerr << "error: No declaration found for the module: "<<modName << endl;
        return NULL;
    } else // in createSgSourceFile: insert moduleNameAstMap[modName] 
          return moduleNameAstMap[modName];
  }


SgSourceFile*
FortranModuleInfo::createSgSourceFile(string modName)
   {
     int errorCode = 0;
     vector<string> argv;

  // DQ (11/12/2008): Modified to force filename to lower case.
  // printf ("In FortranModuleInfo::createSgSourceFile(): generating a module file %s using module name = %s \n",StringUtility::convertToLowerCase(modName).c_str(),modName.c_str());
 //     modName = StringUtility::convertToLowerCase(modName);

 //current directory
     string rmodFileName = modName + MOD_FILE_SUFFIX;

    if (boost::filesystem::exists(rmodFileName.c_str()) == false) {
        return NULL;
    }

     argv.push_back(SKIP_SYNTAX_CHECK);
     argv.push_back(rmodFileName);

     nestedSgFile++;
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

     nestedSgFile--;

     return newFile;
   }



void       
FortranModuleInfo::addMapping(string modName,SgModuleStatement* modNode)
  {
  // printf ("In FortranModuleInfo::addMapping() modName = %s \n",modName.c_str());

     if ( moduleNameAstMap[modName] == NULL ) 
            moduleNameAstMap[modName] = modNode;
     else
            cerr << "Warning: The map entry for " << modName 
                 << " is not empty. " << endl;
     
#ifdef USE_STMT_DEBUG
     dumpMap();
#endif
  }




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

