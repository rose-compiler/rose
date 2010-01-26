// DQ (8/19/2004): Moved from ROSE/src/midend/astRewriteMechanism/rewrite.h
// Added global function for getting the string associated 
// with an enum (which is defined in global scope)
// string getVariantName ( VariantT v );
#ifndef __sagesupport
#define __sagesupport
//#include "setup.h"
// DQ (7/7/2005): Temp location while I implement this function
int buildAstMergeCommandFile ( SgProject* project );

int AstMergeSupport ( SgProject* project );

// Temporary function to be later put into Sg_FileInfo
StringUtility::FileNameLocation get_location    ( Sg_File_Info* X );
StringUtility::FileNameLibrary  get_library     ( Sg_File_Info* X );
std::string                     get_libraryName ( Sg_File_Info* X );
StringUtility::OSType           get_OS_type      ();
int                             get_distanceFromSourceDirectory ( Sg_File_Info* X );
// tps (01/26/2010) : SgFile* determineFileType redefined in sageInterface.h !!!!
//SgFile*                         determineFileType ( vector<string> argv, int nextErrorCode, SgProject* project );
#endif


