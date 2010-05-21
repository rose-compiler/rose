#ifndef MAP_SCOPES_IN_TOKEN_STREAM_AND_AST
#define MAP_SCOPES_IN_TOKEN_STREAM_AND_AST

#include <functional>
#include "linearizeAST.h"
#include <vector>
#include <boost/tuple/tuple.hpp>

struct separator
   {
     int begin_pos;
     int end_pos;
     std::vector<separator> sub_separators;

     separator(int bp, int ep, std::vector<separator> ss );
     separator(int bp, int ep);
// #ifndef USE_ROSE
  // If we are using ROSE to compile ROSE source code then the Wave support is not present.
     void outputValues(token_container& tokenStream, int counter = 0);
// #endif
     void outputValues(std::vector<SgNode*>& linearizedAST, int counter = 0 );
   };

separator* mapSeparatorsAST(std::vector<SgNode*>& linearizedAST);

// #ifndef USE_ROSE
  // If we are using ROSE to compile ROSE source code then the Wave support is not present.
separator* mapSeparatorsTokenStream(token_container& tokenStream);
// #endif

#endif
