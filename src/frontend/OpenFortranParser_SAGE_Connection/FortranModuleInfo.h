/*
 *  Read a module file "XX.rmod"
 *  Create a SgFile for the module
 *  Create an entry for the module in the moduleNAmeAstMap
 */

#ifndef __FORTRANMODULEINFO_H_
#define __FORTRANMODULEINFO_H_

//#include "rose.h"
#include "fortran_support.h"

#define  MOD_FILE_SUFFIX   ".rmod"
#define  SKIP_SYNTAX_CHECK "-rose:skip_syntax_check"

using std::vector;
using std::map;
using std::string;

class FortranModuleInfo
  {

     private:
       static SgProject*                      currentProject;
       static map<string, SgModuleStatement*> moduleNameAstMap;
       static unsigned                        nestedSgFile; 
       static vector<string>                  inputDirs   ;

     public:
       static bool                 isRmodFile();
       static void                 setCurrentProject(SgProject*);
       static SgProject*           getCurrentProject();

       static SgModuleStatement*   getModule(string modName);
       static void                 addMapping(string modName,SgModuleStatement* modStmt);

static string find_file_from_inputDirs(string name);

static void set_inputDirs(SgProject* );
 
       FortranModuleInfo(){};

       ~FortranModuleInfo(){};

     private:
       static SgSourceFile*               createSgSourceFile(string modName);
       static void                  clearMap();
       static void                  dumpMap();

  };


#endif /*__FORTRANMODULEINFO_H_*/
