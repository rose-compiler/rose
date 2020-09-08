/*
 *  Read a module file "XX.rmod"
 *  Create a SgFile for the module
 *  Create an entry for the module in the moduleNAmeAstMap
 */

#ifndef __FORTRANMODULEINFO_H_
#define __FORTRANMODULEINFO_H_

// #include "rose.h"
// #include "fortran_support.h"

#define  SKIP_SYNTAX_CHECK "-rose:skip_syntax_check"

// DQ (10/11/2010): Never use using declarations in a header file since
// they apply to the whole translation unit and have global effects.
// using std::vector;
// using std::map;
// using std::string;

class FortranModuleInfo
  {
     private:
       static SgProject*                      currentProject;

    // DQ (10/1/2010): Added a typedef to simplify code using the moduleNameAstMap data member.
    // static map<string, SgModuleStatement*> moduleNameAstMap;
       typedef std::map<std::string, SgModuleStatement*> ModuleMapType;
       static ModuleMapType moduleNameAstMap;

       static unsigned int             nestedSgFile; 
       static std::vector<std::string> inputDirs   ;

     public:
       static bool                 isRmodFile();
       static void                 setCurrentProject(SgProject*);
       static SgProject*           getCurrentProject();

       static SgModuleStatement*   getModule(std::string modName);
       static void                 addMapping(std::string modName,SgModuleStatement* modStmt);

       static std::string module_file_suffix() { return std::string(".rmod"); }
       static std::string find_file_from_inputDirs(std::string name);

       static void set_inputDirs(SgProject* );
 
       FortranModuleInfo() {};
      ~FortranModuleInfo() {};

     private:
       static SgSourceFile*  createSgSourceFile(std::string modName);
       static void           clearMap();
       static void           dumpMap();

  };


#endif /*__FORTRANMODULEINFO_H_*/
