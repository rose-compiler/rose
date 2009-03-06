#ifndef WAVE_HELPERS_H
#define WAVE_HELPERS_H

// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include <stack>
#include "linearizeAST.h"
//A list of macro calls
typedef std::list<PreprocessingInfo*> macro_call_list;


typedef std::map<PreprocessingInfo*, macro_call_list > macro_def_call_type;

bool
checkIfNodeMaps(token_type tok, SgNode* node);


bool
tokenHasNoCorrespondingASTLocatedConstruct(token_type tok);

     //build the map of macro defs to macro calls to that def
      //using the global map of filenames to PreprocessingInfo's.
  
macro_def_call_type allCallsToMacro2( std::list<std::string>
internalIncludePathList );


extern bool VERBOSE_MESSAGES_OF_WAVE;
#endif
