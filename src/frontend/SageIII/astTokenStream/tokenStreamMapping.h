
#ifndef TOKEN_STREAM_SEQUENCE_MAPPING_HEADER
#define TOKEN_STREAM_SEQUENCE_MAPPING_HEADER

class TokenStreamSequenceToNodeMapping_key
   {
  // The purpose of this class is to support when to share the TokenStreamSequenceToNodeMapping
  // objects across multiple IR nodes of the AST.  Token sequences of IR nodes in the AST that 
  // are the same (excluding leading a trailing tokens subsequences) should share the same 
  // TokenStreamSequenceToNodeMapping objects.

     public:
          SgNode* node;
          int lower_bound, upper_bound;

          TokenStreamSequenceToNodeMapping_key(SgNode* n, int input_lower_bound, int input_upper_bound);
          TokenStreamSequenceToNodeMapping_key(const TokenStreamSequenceToNodeMapping_key & X);

          bool operator==(const TokenStreamSequenceToNodeMapping_key & X) const;
          bool operator<(const TokenStreamSequenceToNodeMapping_key & X) const;
   };


class TokenStreamSequenceToNodeMapping
   {
  // This is the principal data structure used in the token mapping.

  // This class is used to make the token sequence to each IR node (or nodes).
  // It is used as an element in a list to report all mapping of 
  // subsequences to IR nodes in the AST.

     public:
       // To allow sharing of token stream sbsequences across multiple nodes 
       // we need to permit this to be a collection of SgNode's. Likely a 
       // vector would be a good choice since it would preserve order.
       // Pointer to the AST IR node.
          SgNode* node;

       // leading whitespace
          int leading_whitespace_start, leading_whitespace_end;

       // start,end of token subsequence (associated with specified node(s)).
          int token_subsequence_start, token_subsequence_end;

       // trailing whitespace
          int trailing_whitespace_start, trailing_whitespace_end;

       // DQ (12/31/2014): Added to support the middle subsequence of tokens in the SgIfStmt as a special case.
          int else_whitespace_start, else_whitespace_end;

       // Currently some normalized parts of the ROSE AST can share the same 
       // TokenStreamSequenceToNodeMapping data structure.  The best example
       // of this is the case of a variable declaration using multiple variables
       // (e.g. "int a,b,c;").  This will currently be normalized to be three
       // seperate variable declarations (though this will be fixed in the future).
       // Since each of the variable declarations will have the same source 
       // position in the generated AST, the same token sequence will map to 
       // each of the seperate (normalized) variable declarations.  Thus the
       // TokenStreamSequenceToNodeMapping can be shared all of the seperate 
       // variable declarations.  Now that we have a container of SgNodes,
       // this boolean should be true iff the container has more than 1 node.
          bool shared;

          bool constructedInEvaluationOfSynthesizedAttribute;

       // Use a vector as a container for the associated IR nodes for this token sequence when it is shared.
          std::vector<SgNode*> nodeVector;

       // Static date for generating unique keys into the tokenSequencePool
          static size_t tokenStreamSize;

       // A map of unique subsequences (intervals).  An alternative might be a Boost interval map.
       // static map<size_t,TokenStreamSequenceToNodeMapping*> tokenSequencePool;
       // static map<size_t,TokenStreamSequenceToNodeMapping*,TokenStreamSequenceToNodeMapping_key> tokenSequencePool;
          static std::map<TokenStreamSequenceToNodeMapping_key,TokenStreamSequenceToNodeMapping*> tokenSequencePool;

       // Constructor
          TokenStreamSequenceToNodeMapping(SgNode* n, 
               int input_leading_whitespace_start,  int input_leading_whitespace_end, 
               int input_token_subsequence_start,   int input_token_subsequence_end, 
               int input_trailing_whitespace_start, int input_trailing_whitespace_end,
               int input_else_whitespace_start,     int input_else_whitespace_end);

       // Constructor
          TokenStreamSequenceToNodeMapping( const TokenStreamSequenceToNodeMapping & X );

       // Factory interval generator for new intervals (token sequences).
          static TokenStreamSequenceToNodeMapping* createTokenInterval (SgNode* n, 
               int input_leading_whitespace_start, int input_leading_whitespace_end, 
               int input_token_subsequence_start, int input_token_subsequence_end, 
               int input_trailing_whitespace_start, int input_trailing_whitespace_end,
               int input_else_whitespace_start,     int input_else_whitespace_end);

          void display(std::string label) const;
   };


#include "frontierDetection.h"

// DQ (12/4/2014): Added alternative form of detection where to switch 
// between unparsing from the AST and unparsing from the token stream.
#include "simpleFrontierDetection.h"

// DQ (11/8/2015): We need a seperate traversal to recognise from the 
// token stream mapping, what subtrees are a part of macro expansions 
// that are transformations.  These macro eexpansions must be unparsed
// as a single unit (we can't just unparse parts of them from the token 
// stream and parts from the AST; because there representation in the 
// token stream is only as the unexpanded macro).
#include "detectMacroOrIncludeFileExpansions.h"
#include "detectMacroExpansionsToBeUnparsedAsAstTransformations.h"

#endif
