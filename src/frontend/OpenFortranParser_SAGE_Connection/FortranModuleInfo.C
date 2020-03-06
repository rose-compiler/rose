
#include "sage3basic.h"
#include "FortranModuleInfo.h"
#include "boost/filesystem.hpp"
#include "boost/algorithm/string/replace.hpp"


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

  // Add path to iso_c_binding.rmod. The intrinsic modules have been placed in the
  // 3rdPartyLibraries because they could be compiler dependent. If placed there we could
  // reasonable have multiple versions at some point.
  //
  std::string intrinsic_mod_path = findRoseSupportPathFromSource("src/3rdPartyLibraries/fortran-parser", "share/rose");
  inputDirs.push_back(intrinsic_mod_path);

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
           std::string rmodDir_no_quotes =
                boost::replace_all_copy(rmodDir, "\"", "");

           if (boost::filesystem::exists(rmodDir_no_quotes.c_str())) {
                inputDirs.push_back(rmodDir_no_quotes);
          } else 
             {
            // DQ (3/15/2017): Fixed to use mlog message logging.
               if (Rose::ir_node_mlog[Rose::Diagnostics::DEBUG])
                  {
                    cout << "WARNING: the input directory does not exist (rose): " << rmodDir<< endl;
                  }
             }
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

#if 0
     printf ("In FortranModuleInfo::getModule(%s): numberOfModules_before = %" PRIuPTR " \n",modName.c_str(),numberOfModules_before);
#endif

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

#if 0
     printf ("In FortranModuleInfo::getModule(%s): modStmt = %p \n",modName.c_str(),modStmt);
#endif

     if (modStmt != NULL)
        {
          if (SgProject::get_verbose() > 1)
               printf ("This module has been previously processed (seen) in this compilation unit. \n");

          return modStmt;
        }

     string nameWithPath = find_file_from_inputDirs(modName);

     if (SgProject::get_verbose() > 1)
          printf ("In FortranModuleInfo::getModule(%s): nameWithPath = %s \n",modName.c_str(),nameWithPath.c_str());

#if 0
     printf ("********* BUILD NEW MODULE FILE IF NOT ALREADY BUILT **************** \n");
#endif

     SgSourceFile* newModuleFile = createSgSourceFile(nameWithPath);

#if 0
     printf ("********************************************************************* \n");
#endif

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     generateDOT ( *newModuleFile );

  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(newModuleFile,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

     if (newModuleFile == NULL )
        {
          cerr << "error: No declaration found for the module: "<<modName << endl;
          return NULL;
        }
       else
        {
       // in createSgSourceFile: insert moduleNameAstMap[modName]

          if (SgProject::get_verbose() > 1)
               printf ("In FortranModuleInfo::getModule(%s): createSgSourceFile(nameWithPath) != NULL nameWithPath = %s \n",modName.c_str(),nameWithPath.c_str());
#if 1
       // DQ (10/1/2010): This is a work-around for OFP 0.8.2 failing to call the c_action_end_module_stmt() 
       // rule for an included module (from use statement) that includes another module (again, using 
       // a use statment).  The first module using the use statment will not have its 
       // c_action_end_module_stmt() rule called. See the test_forcing.F90 example test code.

       // Extract the pointer to the SgModule from the SgSourceFile
          Rose_STL_Container<SgNode*> moduleDeclarationList = NodeQuery::querySubTree (newModuleFile,V_SgModuleStatement);

       // There should only be a single module defined in the associated *.rmod file.
          if (moduleDeclarationList.size() != 1)
             {
               printf ("Error: moduleDeclarationList.size() = %" PRIuPTR " \n",moduleDeclarationList.size());
             }
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

#error "DEAD CODE!"

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
#if 0
     printf ("In FortranModuleInfo::createSgSourceFile(): generating a module file %s using module name = %s \n",StringUtility::convertToLowerCase(modName).c_str(),modName.c_str());
#endif
  // modName = StringUtility::convertToLowerCase(modName);

  // current directory
     string rmodFileName = modName + MOD_FILE_SUFFIX;

#if 0
     printf ("In FortranModuleInfo::createSgSourceFile(): Searching for file rmodFileName = %s \n",rmodFileName.c_str());
     printf ("In FortranModuleInfo::createSgSourceFile(): boost::filesystem::exists(rmodFileName.c_str()) = %s \n",boost::filesystem::exists(rmodFileName.c_str()) ? "true" : "false");
#endif

     if (boost::filesystem::exists(rmodFileName.c_str()) == false)
        {
          printf ("File rmodFileName = %s NOT FOUND (expected to be present) \n",rmodFileName.c_str());
          return NULL;
        }

     argv.push_back(SKIP_SYNTAX_CHECK);
     argv.push_back(rmodFileName);

     nestedSgFile++;

     if (SgProject::get_verbose() > 1)
          printf ("START FortranModuleInfo::createSgSourceFile(%s): nestedSgFile = %d \n",rmodFileName.c_str(),nestedSgFile);

     SgProject*  project = getCurrentProject();

     SgSourceFile* newFile = isSgSourceFile(determineFileType(argv,errorCode,project));
  // SgSourceFile* newFile =  new SgSourceFile (argv, errorCode, 0, project);
     ROSE_ASSERT(newFile != NULL);

#if 0
     printf ("In FortranModuleInfo::createSgSourceFile(): Calling the fronend explicitly! \n");
#endif

   // DQ (6/13/2013): Since we seperated the construction of the SgFile IR nodes from the invocation of the frontend, we have to call the frontend explicitly.
     newFile->runFrontend(errorCode);

  // DQ (6/13/2013): At least report that the error code is not checked, this is just something that I noticed but don't want to modify just now.
  // I guess that the point is that it should compile since the module compiled previously, but it should still be enforced to be zero.
     if (errorCode != 0)
        {
          printf ("In FortranModuleInfo::createSgSourceFile(): errorCode != 0 is not checked \n");
          ROSE_ASSERT(errorCode == 0);
        }

     ROSE_ASSERT (newFile != NULL);
     ROSE_ASSERT (newFile->get_startOfConstruct() != NULL);

  // Set the project as the  parent 
     newFile->set_parent(project);

  // DQ (11/12/2008): This rmod file should be explicitly marked to not be compiled.
  // printf ("Marking the new module file to not be compiled \n");
     newFile->set_skipfinalCompileStep(true);
     newFile->set_skip_unparse(true);

     project->set_file(*newFile);

     if (SgProject::get_verbose() > 1)
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

