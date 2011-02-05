#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include "rose.h"
#include "fortran_error_handler.h"
#include "token.h"
#include "fortran_support.h"
#include "FortranParserState.h"

#define FEH_DEBUG 0

/**
 *  handling error occured due to failed assertion
 * this error handler only works during parser phase since it uses
 * Token_t to know the line number

 * TODO: we need to make this function more general
 **/
void 
fortran_error_handler(int signum)
{
  // get the current filename 
  std::string sFilename = getCurrentFilename();
  if (sFilename.size()==0) {
     fprintf(stderr, "ERROR while parsing the source code\n");
  } else {
    
    SgScopeStatement* scope = astScopeStack.front();
    SgStatement* lastStatement = scope;
    SgStatementPtrList statementList = scope->generateStatementList();
    if (statementList.empty() == false)
       {
         lastStatement = statementList.back();
       }
    int lineNumberOfLastStatement = (astScopeStack.empty() == false) ? lastStatement->get_file_info()->get_line() : 0;
    // get the latest token parsed
    if (lineNumberOfLastStatement > 0)
      std::cerr <<"FATAL ERROR in file "<<sFilename<<":"<<lineNumberOfLastStatement<<std::endl;
    else
     std::cerr <<"FATAL ERROR while parsing "<<sFilename<<std::endl;
  }
  fflush(NULL); // flush all stdio
  fortran_error_handler_end();
  exit(-1);
}

/**
 * ending the interception of SIGABRT and SIGSEGV
 */
void fortran_error_handler_end()
{
  struct sigaction errorAction;

  errorAction.sa_handler = SIG_DFL;
  sigemptyset(&errorAction.sa_mask);
  errorAction.sa_flags = 0;
  if (sigaction(SIGABRT, &errorAction, NULL) == -1) {
    fprintf( stderr,"ERROR: unable to unset sigaction for SIGABRT \n" ); 
  } else {

    errorAction.sa_handler = SIG_DFL;
    sigemptyset(&errorAction.sa_mask);
    errorAction.sa_flags = 0;
    if (sigaction(SIGSEGV, &errorAction, NULL) == -1) {
      fprintf( stderr,"ERROR: unable to unset sigaction for SIGSEGV\n" ); 
    }
  }

#if FEH_DEBUG
  fprintf(stdout, "FEH: handler has been unmasked\n");
#endif
}


/**
 * starting the interception of SIGABRT and SIGSEGV
 * Laksono 2009.12.14: I think we need to set the priority of action handler
 */
void fortran_error_handler_begin()
{
  struct sigaction errorAction;

  errorAction.sa_handler = fortran_error_handler;
  sigemptyset(&errorAction.sa_mask);
  errorAction.sa_flags = 0;
  if (sigaction(SIGABRT, &errorAction, NULL) == -1) {
    fprintf( stderr,"ERROR: unable to set sigaction for SIGABRT \n" ); 
  } else {

    errorAction.sa_handler = fortran_error_handler;
    sigemptyset(&errorAction.sa_mask);
    errorAction.sa_flags = 0;
    if (sigaction(SIGSEGV, &errorAction, NULL) == -1) {
      fprintf( stderr,"ERROR: unable to set sigaction for SIGSEGV\n" ); 
    } 
  }

#if FEH_DEBUG
  fprintf(stdout, "FEH: handler has been masked\n");
#endif
}
