#ifndef CREATE_MAP_TOKENS_TO_NODES
#define CREATE_MAP_TOKENS_TO_NODES

#include "doCompleteMapping.h"

class createMap
   {
     private:
          std::vector<SgNode*>& linearizedAST;

#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
// #ifndef USE_ROSE
       // If we are using ROSE to compile ROSE source code then the Wave support is not present.
       // token_container is a std::vector<token_type>
          token_container& tokenStream;
// #endif
#endif

      // The map is of a SgScopeStatment in the AST to a pair<int,int>.
      //The pairs' int values is relative to the tokenStream which you can
      //get through get_tokenStream(). Both int vaferues refer to an index in the vector
      //of tokens in the internal variable 'tokenStream'. The first int will point
      //you to a token representing a '{' and the second int to a token representing
      //a '}'. These two together is a match block which corresponds to an AST SgScopeStatement.

          std::map<SgNode*,std::pair<int,int> > nodeToTokenMap;
          std::map<std::pair<int,int>, SgNode*> tokenToNodeMap;

       // Will build the Node to token map
          void buildMaps();

       // WARNING! Has a side effect nodeToTokenMap
       // Will map the nodes in the AST to the tokens in the tokenStream
          void internalMatchBetweenASTandTokenStreamSeparator(separator* ast, separator* tokenStream);

     public:

#ifndef ROSE_SKIP_COMPILATION_OF_WAVE
// #ifndef USE_ROSE
       // If we are using ROSE to compile ROSE source code then the Wave support is not present.
          createMap(std::vector<SgNode*>& linAST, token_container& tokStream);

       // get the token stream provided in the constructor
          token_container& get_tokenStream();
// #endif
#endif

       // get the linearized AST provided in the constructor
          std::vector<SgNode*>&               get_linearizedAST();

       // the integer int refers to an index in the tokenStream vector
          std::map<SgNode*,std::pair<int,int> > get_mapFromNodeToToken();
          std::map<std::pair<int,int>,SgNode*> get_mapFromTokenToNode();

          void printNodeToTokenMap();
          void printTokenToNodeMap();
   };

#endif
