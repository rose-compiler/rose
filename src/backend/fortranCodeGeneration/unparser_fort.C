/* unparser_fort.C
 * Contains the implementation of the constructors, destructor, formatting functions,
 * and fucntions that unparse directives.
 */

#ifdef HAVE_CONFIG_H
// This avoids requiring the user to use config.h and follows 
// the automake manual request that we use <> instead of ""
#include <config.h>
#endif

#include "rose.h"
#include "unparser_fort.h"


//-----------------------------------------------------------------------------------
//  UnparserFort::UnparserFort
//  
//  Constructor that takes a SgFile*, iostream*, ROSEAttributesList*, Unparser_Opt,
//  and int. All other fields are set to NULL or 0.
//-----------------------------------------------------------------------------------
UnparserFort::UnparserFort(ostream* nos, char* fname, Unparser_Opt nopt, 
			   int nline, UnparseFormatHelp* h, 
			   UnparseDelegate* r)
  : opt(nopt), cur(nos, h), repl(r)
{
  ROSE_ASSERT(nos != NULL);
  strcpy(currentOutputFileName, fname);
  cur_index        = 0;
  line_to_unparse  = nline;
}

//-----------------------------------------------------------------------------------
//  UnparserFort::~UnparserFort
//
//  Destructor
//----------------------------------------------------------------------------------- 
UnparserFort::~UnparserFort()
{
}

//-----------------------------------------------------------------------------------
//  UnparserFort::unparseFile
//
//-----------------------------------------------------------------------------------
void
UnparserFort::unparseFile(SgFile* file, SgUnparse_Info& info)
{
  // Turn ON the error checking which triggers if the default
  // SgUnparse_Info constructor is called
  SgUnparse_Info::forceDefaultConstructorToTriggerError = true;

  SgScopeStatement* globalScope = (SgScopeStatement*) (&(file->root()));
  ROSE_ASSERT(globalScope != NULL);

  //std::cout << "*** Unparsing as F90 ***\n" << endl;
  unparseStatement(globalScope, info);
  cur.flush();
  
  // Turn OFF the error checking which triggers if the default
  // SgUnparse_Info constructor is called
  SgUnparse_Info::forceDefaultConstructorToTriggerError = false;
}


//-----------------------------------------------------------------------------------
//  char* Unparser::getCurOutFileName() 
//
//  get the filename from the Sage File Object
//-----------------------------------------------------------------------------------
char*
UnparserFort::getCurOutFileName()
{
  return currentOutputFileName;
}  

UnparseFormatFort& 
UnparserFort::get_output_stream()
{
  return cur;
}


//-----------------------------------------------------------------------------------
//  UnparserFort::isStmtFromFile
//
//-----------------------------------------------------------------------------------
bool
UnparserFort::isStmtFromFile(SgStatement* stmt, char* sourceFilename)
{
  ROSE_ASSERT(stmt != NULL);

  // Since Fortran has an official include mechanism, we generally
  // shouldn't have to worry about untangling files included via
  // preprocessor macros.  However, that would mean life is fair...

  bool stmtInFile = false;

  if (opt.get_unparse_includes_opt() == true) {
    // If we are to unparse all included files into the source file
    // this this is ALWAYS true
    stmtInFile = true;
  }
  else {
    // Compare the file names from the file info object in each
    // statement.  Also, the statement is considered to be in the file
    // if it is part of a transformation added to the AST.
    char* stmtFilenm = ROSE::getFileName(stmt);
    ROSE_ASSERT(stmtFilenm != NULL);
    
    if (strcmp(stmtFilenm, sourceFilename) == 0 ) {
      // filename match: easy
      stmtInFile = true;
    }
    else if (isPartOfTransformation(stmt)) {
      // stmt is part of a transformation
      stmtInFile = true;
    }
  }
  
  return stmtInFile;
}
