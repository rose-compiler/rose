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


class FortranModuleInfo
  {

     private:
       static SgProject*                      currentProject;
       static std::map<std::string, SgModuleStatement*> moduleNameAstMap;
       static unsigned                        nestedSgFile; 

     public:
       static bool                 isRmodFile();
       static void                 setCurrentProject(SgProject*);
       static SgProject*           getCurrentProject();

       static SgModuleStatement*   getModule(std::string modName);
       static void                 addMapping(std::string modName,SgModuleStatement* modStmt);
 
       FortranModuleInfo(){};

       ~FortranModuleInfo(){};

     private:
       static SgSourceFile*               createSgSourceFile(std::string modName);
       static void                  clearMap();
       static void                  dumpMap();

  };


#endif /*__FORTRANMODULEINFO_H_*/
