// Header file to support attaching preprocessingInfo objects
// to AST nodes

#ifndef _ATTACH_PREPROCESSING_INFO_H_
#define _ATTACH_PREPROCESSING_INFO_H_

#include "attachPreprocessingInfoTraversal.h"


// DQ (4/5/2006): Andreas has removed this code!

// void printOutComments ( SgLocatedNode* locatedNode );

// This function is defined in preproc.C and used to collect the attributes
// extern ROSEAttributesList *getPreprocessorDirectives(const char *fileName);
// extern ROSEAttributesList *getPreprocessorDirectives(const char *fileName);
// ROSEAttributesList *getPreprocessorDirectives(const char *fileName);
// ROSEAttributesList *getPreprocessorDirectives( std::string fileName );
// ROSEAttributesList *getPreprocessorDirectives( std::string fileName, LexTokenStreamTypePointer & input_token_stream_pointer );
ROSEAttributesList *getPreprocessorDirectives( std::string fileName );

void attachPreprocessingInfo(SgSourceFile *sageFile);

// DQ (11/30/2008): Part of refactoring of code specific to Wave.
void attachPreprocessingInfoUsingWave(SgSourceFile *sageFile);

#if 0
// DQ (12/16/2008): comment out while I debug the non-wave support.

// AS (012006) Added the function
//     void attachPreprocessingInfo(SgFile *sageFile,  std::map<string,ROSEAttributesList*>*);
// to support reintroductions of macros and fetching of preprocessor directive using Wave. 
// This function does not in itself rely on Wave, but simply takes the same arguement as 
// 'attachPreprocessingInfo(SgFile *sageFile)', but adds an argument std::map<string,ROSEAttributesList*>* 
// which is a map of a pair (filename,list of attributes in that file). The two functions will perform 
// the same tasks in all ways but the way they find the preprocessor directives. In addition to that
// the mechanism here opens up for reintroduction of unexpanded macros into the AST without
// changing the implementation at all. This relies on some external mechanism, that be the 
// Wave preprocessor or some other, to find the preprocessor directives and the unexpanded 
// macros.
void attachPreprocessingInfo(SgSourceFile *sageFile,  std::map<std::string,ROSEAttributesList*>*);
#endif

#endif

// EOF
