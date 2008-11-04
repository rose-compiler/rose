
#include "rose.h"
#include "FortranModuleInfo.h"

using namespace std;
using std::string;
using std::map;

map<string, SgModuleStatement*> FortranModuleInfo::moduleNameAstMap;
unsigned                        FortranModuleInfo::nestedSgFile;
SgProject*                      FortranModuleInfo::currentProject;


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



void
FortranModuleInfo::setCurrentProject(SgProject* project)
  {
     currentProject = project;
  }



SgModuleStatement*
FortranModuleInfo::getModule(string modName)
  {
    SgModuleStatement *modStmt = moduleNameAstMap[modName];

    if (modStmt)
          return modStmt;

    if (createSgSourceFile(modName) == NULL ) {
        cerr << "error: No declaration found for the module: "<<modName << endl;
        return NULL;
    } else // in createSgSourceFile: insert moduleNameAstMap[modName] 
          return moduleNameAstMap[modName];
  }


SgSourceFile*
FortranModuleInfo::createSgSourceFile(string modName)
   {
     int errorCode = 0;
     vector<string> argv ;

     string rmodFileName = modName + MOD_FILE_SUFFIX;

     argv.push_back(SKIP_SYNTAX_CHECK);
     argv.push_back(rmodFileName);

     nestedSgFile++;
     SgProject*  project = getCurrentProject();

     SgSourceFile* newFile = isSgSourceFile(determineFileType(argv,errorCode,project));
       // SgSourceFile* newFile =  new SgSourceFile (argv, errorCode, 0, project);

     ROSE_ASSERT (newFile != NULL);
     ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);

     //Set the project as the  parent 
     newFile->set_parent(project);

     project->set_file(*newFile);

     nestedSgFile--;

     return newFile;

   }



void       
FortranModuleInfo::addMapping(string modName,SgModuleStatement* modNode)
  {
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

