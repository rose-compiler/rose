/*
// This file supports the new parse tree support in ROSE.
// specifically this is a "Concrete Syntax Augmented AST"
// Because the ROSE IR is close to that of the C/C++/Fortran 
// grammar the parse tree can be derived from the token stream
// and the AST.  The principal representation of the CSA AST
// is a map using the IR nodes of the AST as keys into the map
// and the map elements being a data structure (TokenStreamSequenceToNodeMapping)
// containing three pairs of indexes representing the subsequence 
// of tokens for the leading tokens (often white space), the token
// subsequence for the AST IR node (including its subtree), and
// the trailing token subsequence (often white space).

// So where the AST might be:
//        SgWhileStmt
//        /        \
// SgStatement  SgStatement
// (predicate)    (body)
//
// The associated parse tree would be:
//
//              SgWhileStmt
//        /  /      \      \      \
// "while" "(" SgStatement  ")" SgStatement
//               (predicate)      (body)
//
// (so much for ASCI art).
//

// We have a number of ways that we expect could be a problem for this 
// token stream mapping (possible failure modes):
//   1) Toky() macro to write code (not working yet)
//   2) Token pasting operator ## (WORKS)
//   3) Use equivalent of generated binary as a test for generate source code 
//      that is equivalent to the input file up to the use of new lines and other 
//      white space (THIS IS NOT A GREAT TEST (unless the filename of the generated 
//      code is made the same)).
//   4) Use multiple variable names in the same variable declaration (FIXED).
//
// Each of these are being addressed before moving this code into ROSE,
// merging it with the Wave support, and modifying the unparser to 
// use the token stream support.
*/

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
#include "sage3basic.h"

#include "FileUtility.h"
#include "general_token_defs.h"


// This does not appear to exist for my version of boost.
// #include <boost/icl/interval_map.hpp>


// DQ (10/9/2013): Required mods:
//    1) The edges of subtress need to be trimmed back to avoid overlap.
//       Also all overlap should be detected.
//    2) The sharing should cause multiple IR nodes to be associated with 
//       a token susbsequence data structure.


#define DEBUG_TOKEN_OUTPUT 0
#define DEBUG_EVALUATE_INHERITATE_ATTRIBUTE  0
#define DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE 0
#define ERROR_CHECKING 0

using namespace std;
using namespace Rose;

// namespace for token ID values.
using namespace ROSE_token_ids;


#include "tokenStreamMapping.h"

// DQ (1/26/2015): Added support to determine max source position extents on subtrees.
#include "maxExtents.h"


TokenStreamSequenceToNodeMapping_key::TokenStreamSequenceToNodeMapping_key(SgNode* n, int input_lower_bound, int input_upper_bound)
   {
     node = n;
     lower_bound = input_lower_bound;
     upper_bound = input_upper_bound;
   }

TokenStreamSequenceToNodeMapping_key::TokenStreamSequenceToNodeMapping_key(const TokenStreamSequenceToNodeMapping_key & X)
   {
     node = X.node;
     lower_bound = X.lower_bound;
     upper_bound = X.upper_bound;
   }

bool
TokenStreamSequenceToNodeMapping_key::operator== (const TokenStreamSequenceToNodeMapping_key & X) const
   {
  // Allow matching to happen in the same child list (of the same parent).
  // bool result = (X.node == node) && (X.lower_bound == lower_bound) && (X.upper_bound == upper_bound);
     bool result = ((X.node == node) || (node->get_parent() == X.node->get_parent())) && (X.lower_bound == lower_bound) && (X.upper_bound == upper_bound);

#if 0
     printf ("In TokenStreamSequenceToNodeMapping_key::operator==(X): \n");
     printf ("   --- X.node = %p = %s X.lower_bound = %d X.upper_bound = %d \n",X.node,X.node->class_name().c_str(),X.lower_bound,X.upper_bound);
     printf ("   --- node   = %p = %s X.lower_bound = %d X.upper_bound = %d \n",node,node->class_name().c_str(),lower_bound,upper_bound);
     printf ("   --- result = %s \n",result ? "true" : "false");
#endif

     return result;
   }

bool
TokenStreamSequenceToNodeMapping_key::operator< (const TokenStreamSequenceToNodeMapping_key & X) const
   {
     bool result = false;
     if (lower_bound < X.lower_bound)
        {
          result = true;
        }
       else
        {
          if ( (lower_bound == X.lower_bound) && (upper_bound < X.upper_bound) )
             {
               result = true;
             }
            else
             {
               if ( (lower_bound == X.lower_bound) && (upper_bound == X.upper_bound) )
                  {
#if 0
                    printf ("lower and upper bounds are the same \n");
#endif
                 // This is the test that makes the same range different for a same interval expressed 
                 // at different levels in the AST (and the same if the IR nodes are siblings).
                    if (node->get_parent() == X.node->get_parent())
                       {
#if 0
                         printf ("parent nodes are the SAME \n");
#endif
                         result = false;
                       }
                      else
                       {
#if 0
                         printf ("parent nodes are DIFFERENT \n");
#endif
                         result = true;
                       }
                  }
#if 0
               if (node == X.node)
                  {
                    result = false;
                  }
                 else
                  {
#if 1
                    if (node->get_parent() == X.node->get_parent())
                       {
                         result = true;
                       }
#endif                     
                  }
               ROSE_ASSERT(node != X.node);
#endif
             }
        }

#if 0
     printf ("In TokenStreamSequenceToNodeMapping_key::operator<(X): \n");
     printf ("   --- X.node = %p = %s X.lower_bound = %d X.upper_bound = %d \n",X.node,X.node->class_name().c_str(),X.lower_bound,X.upper_bound);
     printf ("   --- node   = %p = %s X.lower_bound = %d X.upper_bound = %d \n",node,node->class_name().c_str(),lower_bound,upper_bound);
     printf ("   --- result = %s \n",result ? "true" : "false");
#endif

#if 0
     printf ("In TokenStreamSequenceToNodeMapping_key::operator<(): node = %p = %s X.node = %p = %s \n",node,node->class_name().c_str(),X.node,X.node->class_name().c_str());
     printf ("   --- upper_bound = %d X.upper_bound = %d \n",upper_bound,X.upper_bound);
     printf ("   --- lower_bound = %d X.lower_bound = %d \n",lower_bound,X.lower_bound);
     printf ("   --- result = %s \n",result ? "true" : "false");
#endif

     return result;
   }




// Declaration of space for static data
size_t TokenStreamSequenceToNodeMapping::tokenStreamSize = 0;
// map<size_t,TokenStreamSequenceToNodeMapping*> TokenStreamSequenceToNodeMapping::tokenSequencePool;
// map<size_t,TokenStreamSequenceToNodeMapping*,TokenStreamSequenceToNodeMapping_key> TokenStreamSequenceToNodeMapping::tokenSequencePool;
map<TokenStreamSequenceToNodeMapping_key,TokenStreamSequenceToNodeMapping*> TokenStreamSequenceToNodeMapping::tokenSequencePool;


TokenStreamSequenceToNodeMapping::TokenStreamSequenceToNodeMapping(
     SgNode* n, 
     int input_leading_whitespace_start,  int input_leading_whitespace_end, 
     int input_token_subsequence_start,   int input_token_subsequence_end, 
     int input_trailing_whitespace_start, int input_trailing_whitespace_end,
     int input_else_whitespace_start,     int input_else_whitespace_end)
   : node(n),
     leading_whitespace_start (input_leading_whitespace_start), 
     leading_whitespace_end   (input_leading_whitespace_end), 
     token_subsequence_start  (input_token_subsequence_start), 
     token_subsequence_end    (input_token_subsequence_end), 
     trailing_whitespace_start(input_trailing_whitespace_start), 
     trailing_whitespace_end  (input_trailing_whitespace_end),
     else_whitespace_start    (input_else_whitespace_start), 
     else_whitespace_end      (input_else_whitespace_end),
     shared(false)
   {
     constructedInEvaluationOfSynthesizedAttribute = false;
   }


TokenStreamSequenceToNodeMapping::TokenStreamSequenceToNodeMapping( const TokenStreamSequenceToNodeMapping & X)
   {
     node                      = X.node;
     leading_whitespace_start  = X.leading_whitespace_start;
     leading_whitespace_end    = X.leading_whitespace_end;
     token_subsequence_start   = X.token_subsequence_start;
     token_subsequence_end     = X.token_subsequence_end;
     trailing_whitespace_start = X.trailing_whitespace_start;
     trailing_whitespace_end   = X.trailing_whitespace_end;
     else_whitespace_start     = X.else_whitespace_start;
     else_whitespace_end       = X.else_whitespace_end;
     shared                    = X.shared;

     constructedInEvaluationOfSynthesizedAttribute = X.constructedInEvaluationOfSynthesizedAttribute;
   }

void
TokenStreamSequenceToNodeMapping::display(string label) const
   {
     printf ("TokenStreamSequenceToNodeMapping::display(%s) \n",label.c_str());

  // DQ (9/28/2018): Adding assertion.
     ROSE_ASSERT(node != NULL);

     printf ("   node = %p = %s \n",node,node->class_name().c_str());
     printf ("   shared = %s \n",shared ? "true" : "false");
     printf ("   leading_whitespace (%d,%d) token_subsequence (%d,%d) trailing_whitespace (%d,%d) else_whitespace (%d,%d) \n",
          leading_whitespace_start,leading_whitespace_end,
          token_subsequence_start,token_subsequence_end,
          trailing_whitespace_start,trailing_whitespace_end,
          else_whitespace_start,else_whitespace_end);
   }


TokenStreamSequenceToNodeMapping*
TokenStreamSequenceToNodeMapping::createTokenInterval (SgNode* n, int input_leading_whitespace_start, int input_leading_whitespace_end, int input_token_subsequence_start, int input_token_subsequence_end, int input_trailing_whitespace_start, int input_trailing_whitespace_end, int input_else_whitespace_start, int input_else_whitespace_end)
   {
  // The token interval is unique and using it we define an interval tree (of tokens) on the AST to seperate the token stream over the AST IR nodes.
  // This function defines a set which used a unique key for any possible interval.

     ROSE_ASSERT(tokenStreamSize > 0);

  // Generate the key from the node and token subsequence interval.
     TokenStreamSequenceToNodeMapping_key key(n,input_token_subsequence_start,input_token_subsequence_end);

     TokenStreamSequenceToNodeMapping* newTokenSequence = NULL;
  // map<size_t,TokenStreamSequenceToNodeMapping*>::iterator iter = tokenSequencePool.find(key);
     map<TokenStreamSequenceToNodeMapping_key,TokenStreamSequenceToNodeMapping*>::iterator iter = tokenSequencePool.find(key);

     if (iter != tokenSequencePool.end())
        {
       // This branch will permit sharing of a previously built TokenStreamSequenceToNodeMapping.
          newTokenSequence = iter->second;
          ROSE_ASSERT(newTokenSequence != NULL);
#if 0
          printf ("TokenStreamSequenceToNodeMapping::createTokenInterval(): Reuse an existing TokenStreamSequenceToNodeMapping newTokenSequence = %p = %s \n",newTokenSequence,newTokenSequence->node->class_name().c_str());
          printf ("   --- n = %p = %s \n",n,n->class_name().c_str());
          printf ("   --- newTokenSequence->token_subsequence_start = %d newTokenSequence->token_subsequence_end = %d \n",newTokenSequence->token_subsequence_start,newTokenSequence->token_subsequence_end);
#endif
          newTokenSequence->shared = true;

          ROSE_ASSERT(newTokenSequence->shared == true);

       // Add the first node to the list.
          newTokenSequence->nodeVector.push_back(n);
#if 0
          printf ("   --- newTokenSequence->nodeVector.size() = %zu \n",newTokenSequence->nodeVector.size());
#endif
        }
       else
        {
       // This branch will force a new TokenStreamSequenceToNodeMapping to be built.
           newTokenSequence = new TokenStreamSequenceToNodeMapping(n,input_leading_whitespace_start,input_leading_whitespace_end,input_token_subsequence_start,input_token_subsequence_end,input_trailing_whitespace_start,input_trailing_whitespace_end,input_else_whitespace_start,input_else_whitespace_end);
          ROSE_ASSERT(newTokenSequence != NULL);
#if 0
          printf ("TokenStreamSequenceToNodeMapping::createTokenInterval(): Building a new TokenStreamSequenceToNodeMapping newTokenSequence = %p = %s \n",newTokenSequence,newTokenSequence->node->class_name().c_str());
          printf ("   --- n = %p = %s \n",n,n->class_name().c_str());
          printf ("   --- newTokenSequence->token_subsequence_start = %d newTokenSequence->token_subsequence_end = %d \n",newTokenSequence->token_subsequence_start,newTokenSequence->token_subsequence_end);
#endif
          newTokenSequence->shared = false;

          ROSE_ASSERT(newTokenSequence->shared == false);

       // Add the input SgNode* to the list of IR nodes sharing this TokenStreamSequenceToNodeMapping.
          newTokenSequence->nodeVector.push_back(n);

       // tokenSequencePool.insert(pair<size_t,TokenStreamSequenceToNodeMapping*>(key,newTokenSequence));
          tokenSequencePool.insert(pair<TokenStreamSequenceToNodeMapping_key,TokenStreamSequenceToNodeMapping*>(key,newTokenSequence));
        }

     return newTokenSequence;
   }


// DQ (11/25/2018): This name appears to collide silently at link time with any ROSE tools that uses the same name.
// This is important to fix ASAP.  For now I will verify the behavior further by changing the name in the ROSE tools 
// that I am building.

// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class InheritedAttribute // : AstInheritedAttribute
   {
     public:
       // Store a reference to the token stream (sublist?).
       // vector<stream_element*> & tokenStream;

       // Same a reference to the associated source file so that we can get the filename to compare against.
          SgSourceFile* sourceFile;

       // Detect when to stop processing deeper into the AST.
          bool processChildNodes;

          int start_of_token_sequence;
          int end_of_token_sequence;

       // Specific constructors are required
       // InheritedAttribute(LexTokenStreamType* ts); // : tokenStream(ts), processChildNodes(true) {};
       // InheritedAttribute(vector<stream_element*> & tokenList);
          InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end,bool processed);

          InheritedAttribute ( const InheritedAttribute & X ); // : processChildNodes(X.processChildNodes) {};
   };

// InheritedAttribute::InheritedAttribute(vector<stream_element*> & ts) : tokenStream(ts), processChildNodes(true) 
InheritedAttribute::InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end, bool processed) 
   : sourceFile(input_sourceFile),
     processChildNodes(processed)
   {
#if 0
     printf ("In InheritedAttribute constructor: start = %d end = %d \n",start,end);
#endif

     start_of_token_sequence = start;
     end_of_token_sequence   = end;
   }

InheritedAttribute::InheritedAttribute ( const InheritedAttribute & X ) 
   : sourceFile(X.sourceFile),
     processChildNodes(X.processChildNodes)
   {
     start_of_token_sequence = X.start_of_token_sequence;
     end_of_token_sequence   = X.end_of_token_sequence;
   }


// DQ (11/25/2018): This name appears to collide silently at link time with any ROSE tools that uses the same name.
// This is important to fix ASAP.  For now I will verify the behavior further by changing the name in the ROSE tools 
// that I am building.

class SynthesizedAttribute
   {
     public:
         SgNode* node;

         SynthesizedAttribute();
         SynthesizedAttribute(SgNode* n);

         SynthesizedAttribute(const SynthesizedAttribute & X);
   };

SynthesizedAttribute::SynthesizedAttribute()
   {
#if 0
     printf ("In SynthesizedAttribute(): default constructor \n");
#endif
     node = NULL;
   }

SynthesizedAttribute::SynthesizedAttribute(SgNode* n)
   {
     ROSE_ASSERT(n != NULL);
#if 0
     printf ("In SynthesizedAttribute(SgNode* n): n = %p = %s \n",n,n->class_name().c_str());
#endif
     node = n;
   }

SynthesizedAttribute::SynthesizedAttribute(const SynthesizedAttribute & X)
   {
#if 0
     printf ("In SynthesizedAttribute(const SynthesizedAttribute & X): copy constructor: X.node = %p = %s \n",X.node,(X.node != NULL) ? X.node->class_name().c_str() : "null");
#endif
     node = X.node;
   }

   
// We need this to be a SgTopDownBottomUpProcessing traversal.
// class TokenMappingTraversal : public AstTopDownProcessing<InheritedAttribute>
class TokenMappingTraversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
          vector<stream_element*> & tokenStream;

       // Graph functions that write DOT file nodes, any children (and edges from the node to the children) to the output file.
          void graph ( SgNode* node );

       // This is the map of subsequences of the token sequence to the ROSE AST IR nodes.
       // map<SgNode*,pair<int,int> > tokenStreamSequenceMap;
          map<SgNode*,TokenStreamSequenceToNodeMapping*> tokenStreamSequenceMap;

       // We need an ordered sequence to check between the current and last element.
       // vector<pair<SgNode*,pair<int,int> > > tokenStreamSequenceVector;
          vector<TokenStreamSequenceToNodeMapping*> tokenStreamSequenceVector;

       // DQ (11/20/2015): Provide a statement to use as a key in the token sequence map to get representative whitespace.
       // This is required to format transformed statements in scopes (especially required if all statements are transformed
       // as a part of a larger transformation of the file.  The representative statements white space is used to format
       // the code unparsed from the AST.
          map<SgScopeStatement*,SgStatement*> representativeWhitespaceStatementMap;

       // DQ (12/11/2015): Adding capability for token sequence to update the source position info in IR nodes.
          bool useTokenSequenceToImproveSourcePositionInfo;

       // DQ (12/11/2015): Adding capability for token sequence to update the source position info in IR nodes.
          TokenMappingTraversal(vector<stream_element*> & tokenStream, bool input_useTokenSequenceToImproveSourcePositionInfo);

       // virtual function must be defined
          InheritedAttribute evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute);

       // virtual function must be defined
          SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n,InheritedAttribute inheritedAttribute,SubTreeSynthesizedAttributes synthesizedAttributeList );

       // Check for unassigned tokens that are not white space.
          void outputTokenStreamSequenceMap();

       // Output a subsequence of the tokenStream.
          string generateTokenSubsequence( int start, int end);

          void consistancyCheck();

       // DQ (12/15/2014): refactoring code for reuse.
          void trimLeadingWhiteSpaceFromLeft  (TokenStreamSequenceToNodeMapping* mappingInfo, int original_start_of_token_subsequence);
          void trimTrailingWhiteSpaceFromRight(TokenStreamSequenceToNodeMapping* mappingInfo, int original_end_of_token_subsequence);

       // DQ (12/31/2014): Compute the location of the tokens associate with the else syntax.
          void discoverElseSyntax(TokenStreamSequenceToNodeMapping* if_statement_mappingInfo, TokenStreamSequenceToNodeMapping* true_body_mappingInfo, TokenStreamSequenceToNodeMapping* false_body_mappingInfo);
   };


void
TokenMappingTraversal::consistancyCheck()
   {
     static int counter = 0;

#if ERROR_CHECKING
     ROSE_ASSERT(tokenStreamSequenceMap.size() == tokenStreamSequenceVector.size());

     for (size_t i = 0; i < tokenStreamSequenceVector.size(); i++)
        {
          ROSE_ASSERT(tokenStreamSequenceVector[i] != NULL);
          ROSE_ASSERT(tokenStreamSequenceVector[i]->node != NULL);
          if (tokenStreamSequenceMap.find(tokenStreamSequenceVector[i]->node) == tokenStreamSequenceMap.end())
             {
               printf ("Error: cannot find: tokenStreamSequenceVector[i=%" PRIuPTR "] = %p node = %p = %s \n",i,tokenStreamSequenceVector[i],tokenStreamSequenceVector[i]->node,tokenStreamSequenceVector[i]->node->class_name().c_str());
               tokenStreamSequenceVector[i]->node->get_file_info()->display("error: can't find node in tokenStreamSequenceMap: debug");

               printf ("Error: TokenStreamSequenceToNodeMapping* element in tokenStreamSequenceVector not found in tokenStreamSequenceMap \n");
               ROSE_ASSERT(false);
             }
        }
#endif

  // The map is the more useful data structure longer term, but we need the tokenStreamSequenceVector 
  // to build the initial sequence (though a better approach might not need the tokenStreamSequenceVector
  // data strcuture and could maybe use the tokenStreamSequenceMap exclusively.

  // DQ (12/3/2014): Make this an error message (failes for amr/Coarsen_particles.cc).
  // ROSE_ASSERT(tokenStreamSequenceMap.size() == tokenStreamSequenceVector.size());
     if (tokenStreamSequenceMap.size() != tokenStreamSequenceVector.size())
        {
#if 0
          printf ("ERROR: TokenMappingTraversal::consistancyCheck(): tokenStreamSequenceMap.size() != tokenStreamSequenceVector.size() \n");
          printf ("   --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
          printf ("   --- tokenStreamSequenceVector.size() = %zu \n",tokenStreamSequenceVector.size());
#else
          if (tokenStreamSequenceMap.size() < tokenStreamSequenceVector.size())
             {
            // DQ (1/3/2014): Supress too much output from this warning.  I think this is caused by the tokenization not handeling line continuation characters "\".
               if (counter % 1000 == 0)
                  {
                    printf ("ERROR: TokenMappingTraversal::consistancyCheck(): tokenStreamSequenceMap.size() < tokenStreamSequenceVector.size() \n");
                    printf ("   --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
                    printf ("   --- tokenStreamSequenceVector.size() = %zu \n",tokenStreamSequenceVector.size());
                  }
               counter++;
             }
#endif
        }

  // DQ (1/6/2015): Test the tokenStreamSequenceMap to make sure there are no NULL entries.
     map<SgNode*,TokenStreamSequenceToNodeMapping*>::iterator i = tokenStreamSequenceMap.begin();
     while (i != tokenStreamSequenceMap.end())
        {
          ROSE_ASSERT(i->first != NULL);
          ROSE_ASSERT(i->second != NULL);

          i++;
        }
   }


class Graph_TokenMappingTraversal : public AstSimpleProcessing
   {
     public:
       // File for output for generated graph.
          static std::ofstream file;

       // The map is stored so that we can lookup the token subsequence information using the SgNode pointer as a key.
          map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap;

       // The vector is stored so that we can build the list of nodes with edges (edges
       // are missing the the token information, which might be better to support there).
          vector<stream_element*> & tokenList;

          Graph_TokenMappingTraversal(vector<stream_element*> & input_tokenList, map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenMap);

          void visit(SgNode* n);

       // static void graph_ast_and_token_stream(SgSourceFile* file, vector<stream_element*> & tokenList);
          static void graph_ast_and_token_stream(SgSourceFile* file, vector<stream_element*> & tokenList, map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap);

       // Map the toke_id to a string.
          static string getTokenIdString (int i);
   };

// Need to define space for static data member.
std::ofstream Graph_TokenMappingTraversal::file;


Graph_TokenMappingTraversal::Graph_TokenMappingTraversal( vector<stream_element*> & input_tokenList, map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenMap )
   : tokenStreamSequenceMap(tokenMap),
     tokenList(input_tokenList)
   {
   }


string
Graph_TokenMappingTraversal::getTokenIdString (int i)
   {
     string s;
     switch (i)
        {
          case C_CXX_SYNTAX:             s = "syntax";                 break;
          case C_CXX_WHITESPACE:         s = "whitespace";             break;
          case C_CXX_PRAGMA:             s = "pragma";                 break;
          case C_CXX_IDENTIFIER:         s = "identifier";             break;
          case C_CXX_PREPROCESSING_INFO: s = "CPP PREPROCESSING INFO"; break;

          default:
             {
               if (i >= C_CXX_ASM && i <= C_CXX_WHILE)
                  {
                    s = "keyword";
                  }
                 else
                  {
                    printf ("Error: not clear what this token is: i = %d \n",i);
                    ROSE_ASSERT(false);
                  }
             }
        }

     return s;
   }


void
Graph_TokenMappingTraversal::visit(SgNode* n)
   {
     if (n != NULL)
        {
       // Add a node to the graph
          string node_name = n->class_name();

          string label = "";

       // DQ (12/21/2014): Output names of associated declarations.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
          if (functionDeclaration != NULL)
             {
               label += string("\n name = ") + functionDeclaration->get_name().str();
             }

       // DQ (12/21/2014): Output names of associated declarations.
          SgClassDeclaration* classDeclaration = isSgClassDeclaration(n);
          if (classDeclaration != NULL)
             {
               label += string("\n name = ") + classDeclaration->get_name().str();
             }

       // This could be a seperate subgraph...if it were seperated from this AST traversal into a seperate AST traversal.
       // Check if we have strored token information about this AST IR node.
          if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
             {
               TokenStreamSequenceToNodeMapping* mapping = tokenStreamSequenceMap[n];
               ROSE_ASSERT(mapping != NULL);

               ROSE_ASSERT(tokenList.empty() == false);

            // label += "YYY";
               Sg_File_Info* start_pos = mapping->node->get_startOfConstruct();
               Sg_File_Info* end_pos   = mapping->node->get_endOfConstruct();

               if (end_pos == NULL)
                  {
#if 0
                    printf ("WARNING: end_pos == NULL: n = %p = %s \n",n,n->class_name().c_str());
#endif
                 // We might want to handle this better (with a properly initialized ending Sg_File_Info for a SgFile).
                    end_pos = start_pos;
                  }

               label += "\\ncompiler generated = " + string(start_pos->isCompilerGenerated() ? "true" : "false");

            // Output the mapping so that we can easily see where the same mapping is being used for multiple IR nodes.
               label += "\\ninterval map entry = " + StringUtility::numberToString(mapping);

            // Output if this is a shared token sequence across more than one IR node.
               label += (mapping->shared == true) ? "\\nshared == true" : "\\nshared == false";

            // Report if this was built as part of the evaluation of the synthesized attribute 
            // (representing fillin of the token sequence more accurately computed using the 
            // evaluateInheritedAttribute() function which uses source position information).
               label += (mapping->constructedInEvaluationOfSynthesizedAttribute == true) ? "\\nconstructedInEvaluationOfSynthesizedAttribute == true" : "\\nconstructedInEvaluationOfSynthesizedAttribute == false";

            // printf ("   --- node = %p = %s: start (line=%d:column=%d) end(line=%d,column=%d) \n",
            //      mappingInfo->node,mappingInfo->node->class_name().c_str(),start_pos->get_physical_line(),
            //      start_pos->get_col(),end_pos->get_physical_line(),end_pos->get_col());
               label += "\\nnode pos ((line=" + StringUtility::numberToString(start_pos->get_physical_line()) + ":column=" + StringUtility::numberToString(start_pos->get_col()) + ")"
                                   ",(line=" + StringUtility::numberToString(end_pos->get_physical_line())   + ",column=" + StringUtility::numberToString(end_pos->get_col()) + ")) ";

               int leading_whitespace_start  = mapping->leading_whitespace_start;
               int leading_whitespace_end    = mapping->leading_whitespace_end;
               int token_subsequence_start   = mapping->token_subsequence_start;
               int token_subsequence_end     = mapping->token_subsequence_end;
               int trailing_whitespace_start = mapping->trailing_whitespace_start;
               int trailing_whitespace_end   = mapping->trailing_whitespace_end;
               int else_whitespace_start     = mapping->else_whitespace_start;
               int else_whitespace_end       = mapping->else_whitespace_end;
#if 0
               printf ("   --- leading_whitespace tokens (%d:%d,%d:%d) token_subsequence (%d:%d,%d:%d) trailing_whitespace tokens (%d:%d,%d:%d) \n",
                    leading_whitespace_start  < 0 ? -1 : tokenList[leading_whitespace_start]->beginning_fpi.line_num, 
                    leading_whitespace_start  < 0 ? -1 : tokenList[leading_whitespace_start]->beginning_fpi.column_num,
                    leading_whitespace_end    < 0 ? -1 : tokenList[leading_whitespace_end]->ending_fpi.line_num, 
                    leading_whitespace_end    < 0 ? -1 : tokenList[leading_whitespace_end]->ending_fpi.column_num,
                    token_subsequence_start   < 0 ? -1 : tokenList[token_subsequence_start]->beginning_fpi.line_num, 
                    token_subsequence_start   < 0 ? -1 : tokenList[token_subsequence_start]->beginning_fpi.column_num,
                    token_subsequence_end     < 0 ? -1 : tokenList[token_subsequence_end]->ending_fpi.line_num, 
                    token_subsequence_end     < 0 ? -1 : tokenList[token_subsequence_end]->ending_fpi.column_num,
                    trailing_whitespace_start < 0 ? -1 : tokenList[trailing_whitespace_start]->beginning_fpi.line_num, 
                    trailing_whitespace_start < 0 ? -1 : tokenList[trailing_whitespace_start]->beginning_fpi.column_num,
                    trailing_whitespace_end   < 0 ? -1 : tokenList[trailing_whitespace_end]->ending_fpi.line_num,
                    trailing_whitespace_end   < 0 ? -1 : tokenList[trailing_whitespace_end]->ending_fpi.column_num);
#endif
#if 1
               printf ("\nGraph_TokenMappingTraversal::visit(): n = %p = %s \n",n,n->class_name().c_str());
               printf ("   --- leading_whitespace_start  = %d leading_whitespace_end  = %d \n",leading_whitespace_start,leading_whitespace_end);
               printf ("   --- token_subsequence_start   = %d token_subsequence_end   = %d \n",token_subsequence_start,token_subsequence_end);
               printf ("   --- trailing_whitespace_start = %d trailing_whitespace_end = %d \n",trailing_whitespace_start,trailing_whitespace_end);
               printf ("   --- else_whitespace_start     = %d else_whitespace_end     = %d \n",else_whitespace_start,else_whitespace_end);
#endif
               int leading_whitespace_start_line    = leading_whitespace_start  < 0 ? -1 : tokenList[leading_whitespace_start]->beginning_fpi.line_num; 
               int leading_whitespace_start_column  = leading_whitespace_start  < 0 ? -1 : tokenList[leading_whitespace_start]->beginning_fpi.column_num;
               int leading_whitespace_end_line      = leading_whitespace_end    < 0 ? -1 : tokenList[leading_whitespace_end]->ending_fpi.line_num; 
               int leading_whitespace_end_column    = leading_whitespace_end    < 0 ? -1 : tokenList[leading_whitespace_end]->ending_fpi.column_num;
               int token_subsequence_start_line     = token_subsequence_start   < 0 ? -1 : tokenList[token_subsequence_start]->beginning_fpi.line_num; 
               int token_subsequence_start_column   = token_subsequence_start   < 0 ? -1 : tokenList[token_subsequence_start]->beginning_fpi.column_num;
               int token_subsequence_end_line       = token_subsequence_end     < 0 ? -1 : tokenList[token_subsequence_end]->ending_fpi.line_num; 
               int token_subsequence_end_column     = token_subsequence_end     < 0 ? -1 : tokenList[token_subsequence_end]->ending_fpi.column_num;

            // DQ (12/23/2018): Added assertion to catch issue in token based unparsing.
               ROSE_ASSERT(tokenList[trailing_whitespace_start] != NULL);
#if 1
               printf ("tokenList.size() = %zu trailing_whitespace_start = %d \n",tokenList.size(),trailing_whitespace_start);
#endif
               ROSE_ASSERT(trailing_whitespace_start < (int)tokenList.size());
               int trailing_whitespace_start_line   = trailing_whitespace_start < 0 ? -1 : tokenList[trailing_whitespace_start]->beginning_fpi.line_num; 
               int trailing_whitespace_start_column = trailing_whitespace_start < 0 ? -1 : tokenList[trailing_whitespace_start]->beginning_fpi.column_num;

            // DQ (12/23/2018): Added assertion to catch issue in token based unparsing.
               ROSE_ASSERT(tokenList[trailing_whitespace_end] != NULL);
               ROSE_ASSERT(trailing_whitespace_end < (int)tokenList.size());

               int trailing_whitespace_end_line     = trailing_whitespace_end   < 0 ? -1 : tokenList[trailing_whitespace_end]->ending_fpi.line_num;
               int trailing_whitespace_end_column   = trailing_whitespace_end   < 0 ? -1 : tokenList[trailing_whitespace_end]->ending_fpi.column_num;

            // DQ (3/25/2017): Clang reports these as unused variables.
            // int else_whitespace_start_line       = else_whitespace_start < 0 ? -1 : tokenList[else_whitespace_start]->beginning_fpi.line_num; 
            // int else_whitespace_start_column     = else_whitespace_start < 0 ? -1 : tokenList[else_whitespace_start]->beginning_fpi.column_num;
            // int else_whitespace_end_line         = else_whitespace_end   < 0 ? -1 : tokenList[else_whitespace_end]->ending_fpi.line_num;
            // int else_whitespace_end_column       = else_whitespace_end   < 0 ? -1 : tokenList[else_whitespace_end]->ending_fpi.column_num;

               label += "\\n leading_whitespace token #'s (" + StringUtility::numberToString(leading_whitespace_start) + "," + StringUtility::numberToString(leading_whitespace_end) +
                        ") pos (" + StringUtility::numberToString(leading_whitespace_start_line)  + ":" + StringUtility::numberToString(leading_whitespace_start_column) + "," + 
                                    StringUtility::numberToString(leading_whitespace_end_line)    + ":" + StringUtility::numberToString(leading_whitespace_end_column) + ")";

               label += "\\n token_subsequence token #'s (" + StringUtility::numberToString(token_subsequence_start) + "," + StringUtility::numberToString(token_subsequence_end) +
                        ") pos (" + StringUtility::numberToString(token_subsequence_start_line)   + ":" + StringUtility::numberToString(token_subsequence_start_column) + "," + 
                                    StringUtility::numberToString(token_subsequence_end_line)     + ":" + StringUtility::numberToString(token_subsequence_end_column) + ")";

            // label += "\\n trailing_whitespace tokens (" + StringUtility::numberToString(trailing_whitespace_start_line) + ":" + StringUtility::numberToString(trailing_whitespace_start_column) + "," + 
            //                                               StringUtility::numberToString(trailing_whitespace_end_line)   + ":" + StringUtility::numberToString(trailing_whitespace_end_column) + ")";
               label += "\\n trailing_whitespace token #'s (" + StringUtility::numberToString(trailing_whitespace_start) + "," + StringUtility::numberToString(trailing_whitespace_end) +
                        ") pos (" + StringUtility::numberToString(trailing_whitespace_start_line)  + ":" + StringUtility::numberToString(trailing_whitespace_start_column) + "," + 
                                    StringUtility::numberToString(trailing_whitespace_end_line)    + ":" + StringUtility::numberToString(trailing_whitespace_end_column) + ")";

#if 1
            // More information in the graph
               if (leading_whitespace_start >= 0)
                  {
                    string edge_name = "leading_whitespace:start";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[leading_whitespace_start]) << "\"[label=\"" << edge_name << "\" color=\"cyan\" weight=1];" << endl;
                  }

               if (leading_whitespace_end >= 0)
                  {
                    string edge_name = "leading_whitespace:end";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[leading_whitespace_end]) << "\"[label=\"" << edge_name << "\" color=\"cyan3\" weight=1];" << endl;
                  }
#endif
            // Most important information in the graph
               if (token_subsequence_start >= 0)
                  {
                    string edge_name = "token_subsequence:start";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[token_subsequence_start]) << "\"[label=\"" << edge_name << "\" color=\"goldenrod\" weight=1];" << endl;
                  }

            // Most important information in the graph
               if (token_subsequence_end >= 0)
                  {
                    string edge_name = "token_subsequence:end";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[token_subsequence_end]) << "\"[label=\"" << edge_name << "\" color=\"goldenrod3\" weight=1];" << endl;
                  }
#if 1
            // More information in the graph
               if (trailing_whitespace_start >= 0)
                  {
                    string edge_name = "trailing_whitespace:start";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[trailing_whitespace_start]) << "\"[label=\"" << edge_name << "\" color=\"purple\" weight=1];" << endl;
                  }

               if (trailing_whitespace_end >= 0)
                  {
                    string edge_name = "trailing_whitespace:end";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[trailing_whitespace_end]) << "\"[label=\"" << edge_name << "\" color=\"purple3\" weight=1];" << endl;
                  }
#endif
#if 1
            // More information in the graph
               if (else_whitespace_start >= 0)
                  {
                    string edge_name = "else_whitespace:start";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[else_whitespace_start]) << "\"[label=\"" << edge_name << "\" color=\"red\" weight=1];" << endl;
                  }

               if (else_whitespace_end >= 0)
                  {
                    string edge_name = "else_whitespace:end";
                    file << "\"" << StringUtility::numberToString(n) << "\" -> \"" << StringUtility::numberToString(tokenList[else_whitespace_end]) << "\"[label=\"" << edge_name << "\" color=\"red3\" weight=1];" << endl;
                  }
#endif
             }
            else
             {
            // If this is a SgInitializedName IR node then output the name in the graph node's label.
               SgInitializedName* initializedName = isSgInitializedName(n);
               if (initializedName != NULL)
                  {
                    label += "\\nname = " + initializedName->get_name().getString();
                  }

               Sg_File_Info* file_info_start = n->get_startOfConstruct();
               Sg_File_Info* file_info_end   = n->get_endOfConstruct();

               ROSE_ASSERT(file_info_start != NULL);
               ROSE_ASSERT(file_info_end   != NULL);

               label += "\\nno token info";
               label += "\\nfile=" + StringUtility::stripPathFromFileName(file_info_start->get_physical_filename()) + 
                        "\\n("  + StringUtility::numberToString(file_info_start->get_physical_line()) + "," + StringUtility::numberToString(file_info_start->get_col()) + ")" +
                        " to (" + StringUtility::numberToString(file_info_end->get_physical_line())   + "," + StringUtility::numberToString(file_info_end->get_col()) + ")";
             }

       // DQ (10/29/2013): We need to avoid having the dot file include fron-end specific IR nodes since it makes it too large.
          ROSE_ASSERT(n->get_file_info() != NULL);
          bool currentNodeIsFrontendSpecific = n->get_file_info()->isFrontendSpecific();
          if (currentNodeIsFrontendSpecific == false)
             {
               file << "\"" << StringUtility::numberToString(n) << "\"[" << "label=\"" << node_name << "\\n" << StringUtility::numberToString(n) << label << "\"];" << endl;
             }

       // Note that there are times when the parent is not the same as the AST parent generated from a traversal (but there are usually subtle errors).

       // Add an edge
          if (n->get_parent() != NULL)
             {
            // DQ (10/29/2013): We need to avoid having the dot file include front-end specific IR nodes since it makes it too large.
               if (n->get_parent()->get_file_info() == NULL)
                  {
#if 0
                    printf ("Error: In Graph_TokenMappingTraversal::visit(): n->get_parent()->get_file_info() == NULL: n = %p = %s parent = %p = %s \n",
                         n,n->class_name().c_str(),n->get_parent(),n->get_parent()->class_name().c_str());
#endif
                  }
            // ROSE_ASSERT(n->get_parent()->get_file_info() != NULL);
               
               bool parentNodeIsFrontendSpecific = n->get_parent()->get_file_info() != NULL ? n->get_parent()->get_file_info()->isFrontendSpecific() : true;
               if (currentNodeIsFrontendSpecific == false && parentNodeIsFrontendSpecific == false)
                  {
                    ROSE_ASSERT(n != NULL);
                    ROSE_ASSERT(n->get_parent() != NULL);

                 // DQ (9/11/2018): This node has no children and it is an error to call the get_childIndex() function for that IR node.
                    if (isSgHeaderFileBody(n->get_parent()) == NULL)
                       {
                         size_t child_index = n->get_parent()->get_childIndex(n);
#if 0
                         printf ("In Graph_TokenMappingTraversal::visit(): child_index = %zu \n",child_index);
                         printf ("In Graph_TokenMappingTraversal::visit(): n->get_parent()->get_traversalSuccessorNamesContainer().size() = %zu \n",n->get_parent()->get_traversalSuccessorNamesContainer().size());
#endif
                      // DQ (1/4/2015): Handle strange case (demonstrated by tests/nonsmoke/functional/roseTests/astInterfaceTests/inputmoveDeclarationToInnermostScope_test2015_11.C).
                      // string edge_name   = n->get_parent()->get_traversalSuccessorNamesContainer()[child_index];
                         bool name_available = (child_index < n->get_parent()->get_traversalSuccessorNamesContainer().size());
                         string edge_name   = name_available ? n->get_parent()->get_traversalSuccessorNamesContainer()[child_index] : "unknown edge name";

                         if (name_available == false)
                            {
#if 0
                              printf ("Warning: child not found in parent list of children: for n = %p = %s and parent = %p = %s \n",n,n->class_name().c_str(),n->get_parent(),n->get_parent()->class_name().c_str());
#endif
                            }

                         file << "\"" << StringUtility::numberToString(n->get_parent()) << "\" -> \"" << StringUtility::numberToString(n) << "\"[label=\"" << edge_name << "\" color=\"black\" weight=1];" << endl;
                       }
                  }
             }
        }
   }



void
Graph_TokenMappingTraversal::graph_ast_and_token_stream( SgSourceFile* source_file, vector<stream_element*> & tokenList, map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap )
   {
  // DQ (10/6/2013): Build a dot graph of the AST and token stream and the mapping between them.

  // Build filename...
     string filename = "tokenMappingToAST";

     string dot_header = filename;
     filename += ".dot";

#if 1
     printf ("In graph_ast_and_token_stream(): filename = %s \n",filename.c_str());
#endif

  // Open file...(file is declared in the EDG_ROSE_Graph namespace).
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
     file << "digraph \"" << dot_header << "\" {" << endl;


#if 0
// Subgraph example...
subgraph cluster_0 {
style=filled;
color=lightgrey;
node [style=filled,color=white];
a0 -> a1 -> a2 -> a3;
label = "process #1";
}

subgraph cluster_1 {
node [style=filled];
b0 -> b1 -> b2 -> b3;
label = "process #2";
color=blue
}
start -> a0;
start -> b0;
a1 -> b3;
b2 -> a3;
a3 -> a0;
a3 -> end;
b3 -> end;

start [shape=Mdiamond];
end [shape=Msquare];
#endif

     Graph_TokenMappingTraversal traversal(tokenList,tokenStreamSequenceMap);

  // This could be a seperate subgraph...
     traversal.traverse(source_file,preorder);

  // This could be a seperate subgraph...
     for (size_t i = 0; i < tokenList.size(); i++)
        {
#if 0
          printf ("tokenList[i=%" PRIuPTR "] = %p \n",i,tokenList[i]);
#endif
          string token_name = "token #";
          token_name += StringUtility::numberToString(i) + "\\n";

          bool blankString = (tokenList[i]->p_tok_elem->token_lexeme.empty() == false) ? true : false;
          for (size_t j = 0; j < tokenList[i]->p_tok_elem->token_lexeme.length(); j++)
             {
               if (tokenList[i]->p_tok_elem->token_lexeme[j] != ' ')
                  {
                    blankString = false;
                  }
             }

       // if (tokenList[i]->p_tok_elem->token_lexeme == " ")
          if (blankString == true)
             {
            // Record that this token is a string of blanks and the size of the string.
               token_name += "whitespace:blank:" + StringUtility::numberToString(tokenList[i]->p_tok_elem->token_lexeme.length());
             }
            else
             {
            // node_name += escapeString(tokenList[i]->p_tok_elem->token_lexeme);
               token_name += escapeString(escapeString(tokenList[i]->p_tok_elem->token_lexeme));
             }

       // I want to but the name of the type of the token here later, (e.g keyword, identifier, syntax, whitespace, etc.)
          string label = "\\ntoken type = ";
          label += StringUtility::numberToString(tokenList[i]->p_tok_elem->token_id) + " = " + getTokenIdString(tokenList[i]->p_tok_elem->token_id);

          file << "\"" << StringUtility::numberToString(tokenList[i]) << "\"[" << "label=\"" << token_name << "\\n" << StringUtility::numberToString(tokenList[i]) << label << "\"];" << endl;

          if (i > 0)
             {
               string token_edge_name = "next";
               file << "\"" << StringUtility::numberToString(tokenList[i-1]) << "\" -> \"" << StringUtility::numberToString(tokenList[i]) << "\"[label=\"" << token_edge_name << "\" color=\"red\" weight=1];" << endl;
             }
        }

  // Close off the DOT file.
     file << endl;
     file << "} " << endl;
     file.close();
   }


void
TokenMappingTraversal::trimLeadingWhiteSpaceFromLeft(TokenStreamSequenceToNodeMapping* mappingInfo, int original_start_of_token_subsequence )
   {
  // Search backward through the token sequence to find the first non-whitespace token.

     int leading_whitespace_end   = mappingInfo->token_subsequence_start - 1;
     int leading_whitespace_start = leading_whitespace_end;

#if 0
     printf ("In trimLeadingWhiteSpaceFromLeft(): before loop: leading_whitespace_start            = %d \n",leading_whitespace_start);
     printf ("In trimLeadingWhiteSpaceFromLeft(): before loop: leading_whitespace_end              = %d \n",leading_whitespace_end);
     printf ("In trimLeadingWhiteSpaceFromLeft(): before loop: original_start_of_token_subsequence = %d \n",original_start_of_token_subsequence);
#endif

  // DQ (1/2/2015): There is no leading white space at the start of the token sequence (by definition).
  // This case happends for the trivial case of SgGlobal.
  // if (original_start_of_token_subsequence == 0)
     if (leading_whitespace_start < 0)
        {
#if 0
          printf ("Note: In trimLeadingWhiteSpaceFromLeft(): leading_whitespace_end < 0: returning without modification to mappingInfo \n");
#endif
          return;
        }
     ROSE_ASSERT(leading_whitespace_start >= 0);

  // DQ (12/5/2016): Eliminate warning that we want to consider an error: -Wsign-compare
  // ROSE_ASSERT(leading_whitespace_start < tokenStream.size());
     ROSE_ASSERT((size_t)leading_whitespace_start < tokenStream.size());

  // DQ (12/26/2014): Modify to only adjust the white space if there exists some whitespace to start with.
     if ( tokenStream[leading_whitespace_start]->p_tok_elem->token_id == C_CXX_WHITESPACE ||
          tokenStream[leading_whitespace_start]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO )
        {
       // while (leading_whitespace_start > original_start_of_token_subsequence && tokenStream[leading_whitespace_start-1]->p_tok_elem->token_id == C_CXX_WHITESPACE)
          while ( leading_whitespace_start > original_start_of_token_subsequence &&
                  ( tokenStream[leading_whitespace_start-1]->p_tok_elem->token_id == C_CXX_WHITESPACE ||
                    tokenStream[leading_whitespace_start-1]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO) )
             {
#if 0
               printf ("in loop: leading_whitespace_start = %d \n",leading_whitespace_start);
#endif
               ROSE_ASSERT(leading_whitespace_start > original_start_of_token_subsequence);

               leading_whitespace_start--;
             }

#if 0
          printf ("In trimLeadingWhiteSpaceFromLeft(): after loop: leading_whitespace_start = %d \n",leading_whitespace_start);
#endif

       // If the positions are out of bounds then we don't have any leading whitespace.
          if (leading_whitespace_start < original_start_of_token_subsequence && leading_whitespace_end < original_start_of_token_subsequence)
             {
               leading_whitespace_start = -1;
               leading_whitespace_end   = -1;
             }
        }
       else
        {
#if 0
          printf ("In trimLeadingWhiteSpaceFromLeft(): no initial whitespace detected to start the  loop (reset to not define whitespace) \n");
#endif
          leading_whitespace_start = -1;
          leading_whitespace_end   = -1;
        }

     mappingInfo->leading_whitespace_start = leading_whitespace_start;
     mappingInfo->leading_whitespace_end   = leading_whitespace_end;

#if 0
     printf ("In trimLeadingWhiteSpaceFromLeft(): (adjusted) mappingInfo->leading_whitespace_start = %d \n",mappingInfo->leading_whitespace_start);
     printf ("In trimLeadingWhiteSpaceFromLeft(): (adjusted) mappingInfo->leading_whitespace_end   = %d \n",mappingInfo->leading_whitespace_end);
#endif

     ROSE_ASSERT(mappingInfo->leading_whitespace_start <= mappingInfo->leading_whitespace_end);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void
TokenMappingTraversal::trimTrailingWhiteSpaceFromRight(TokenStreamSequenceToNodeMapping* mappingInfo, int original_end_of_token_subsequence )
   {
  // Search forward through the token sequence to find the first non-whitespace token.

     int trailing_whitespace_start = mappingInfo->token_subsequence_end + 1;
     int trailing_whitespace_end   = trailing_whitespace_start;

#if 0
     printf ("In trimTrailingWhiteSpaceFromRight(): before loop: trailing_whitespace_start            = %d \n",trailing_whitespace_start);
     printf ("In trimTrailingWhiteSpaceFromRight(): before loop: trailing_whitespace_end              = %d \n",trailing_whitespace_end);
     printf ("In trimTrailingWhiteSpaceFromRight(): before loop: original_end_of_token_subsequence    = %d \n",original_end_of_token_subsequence);
#endif

  // DQ (1/2/2015): There is no trailing white space at the end of the token sequence (by definition).
  // This case happends for the trivial case of SgGlobal.
  // if (original_end_of_token_subsequence == tokenStream.size()-1)

  // DQ (12/5/2016): Eliminate warning that we want to consider an error: -Wsign-compare
  // if (trailing_whitespace_end > tokenStream.size()-1)
     if ((size_t)trailing_whitespace_end >= tokenStream.size())
        {
#if 0
          printf ("Note: In trimTrailingWhiteSpaceFromRight(): trailing_whitespace_end > tokenStream.size()-1: returning without modification to mappingInfo \n");
#endif
          return;
        }
     ROSE_ASSERT(trailing_whitespace_end >= 0);

  // DQ (12/5/2016): Eliminate warning that we want to consider an error: -Wsign-compare
  // ROSE_ASSERT(trailing_whitespace_end < tokenStream.size());
     ROSE_ASSERT((size_t)trailing_whitespace_end < tokenStream.size());

  // DQ (12/26/2014): Modify to only adjust the white space if there exists some whitespace to start with.
     if ( tokenStream[trailing_whitespace_end]->p_tok_elem->token_id == C_CXX_WHITESPACE ||
          tokenStream[trailing_whitespace_end]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO )
        {
       // while (leading_whitespace_start > original_start_of_token_subsequence && tokenStream[leading_whitespace_start-1]->p_tok_elem->token_id == C_CXX_WHITESPACE)
          while ( trailing_whitespace_end < original_end_of_token_subsequence &&
                  ( tokenStream[trailing_whitespace_end+1]->p_tok_elem->token_id == C_CXX_WHITESPACE ||
                    tokenStream[trailing_whitespace_end+1]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO) )
             {
#if 0
               printf ("in loop: trailing_whitespace_end = %d \n",trailing_whitespace_end);
#endif
               ROSE_ASSERT(trailing_whitespace_end < original_end_of_token_subsequence);

               trailing_whitespace_end++;
             }

#if 0
          printf ("In trimtrailingWhiteSpaceFromRight(): after loop: trailing_whitespace_end = %d \n",trailing_whitespace_end);
#endif

       // If the positions are out of bounds then we don't have any leading whitespace.
          if (trailing_whitespace_end > original_end_of_token_subsequence && trailing_whitespace_start > original_end_of_token_subsequence)
             {
               trailing_whitespace_start = -1;
               trailing_whitespace_end   = -1;
             }
        }
       else
        {
#if 0
          printf ("In trimTrailingWhiteSpaceFromRight(): no initial whitespace detected to start the  loop (reset to not define whitespace) \n");
#endif
          trailing_whitespace_start = -1;
          trailing_whitespace_end   = -1;
        }

     mappingInfo->trailing_whitespace_start = trailing_whitespace_start;
     mappingInfo->trailing_whitespace_end   = trailing_whitespace_end;

#if 0
     printf ("In trimTrailingWhiteSpaceFromRight(): (adjusted) mappingInfo->trailing_whitespace_start = %d \n",mappingInfo->trailing_whitespace_start);
     printf ("In trimTrailingWhiteSpaceFromRight(): (adjusted) mappingInfo->trailing_whitespace_end   = %d \n",mappingInfo->trailing_whitespace_end);
#endif

     ROSE_ASSERT(mappingInfo->trailing_whitespace_end >= mappingInfo->trailing_whitespace_start);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


void 
TokenMappingTraversal::discoverElseSyntax(TokenStreamSequenceToNodeMapping* if_statement_mappingInfo, TokenStreamSequenceToNodeMapping* true_body_mappingInfo, TokenStreamSequenceToNodeMapping* false_body_mappingInfo)
   {
  // Search forward through the token sequence to find the first non-whitespace token.

     int else_whitespace_start = true_body_mappingInfo->token_subsequence_end + 1;
     int else_whitespace_end   = else_whitespace_start;

     int original_end_of_token_subsequence = false_body_mappingInfo->token_subsequence_start + 0;

#if 0
     printf ("In discoverElseSyntax(): before loop: else_whitespace_start             = %d \n",else_whitespace_start);
     printf ("In discoverElseSyntax(): before loop: else_whitespace_end               = %d \n",else_whitespace_end);
     printf ("In discoverElseSyntax(): before loop: original_end_of_token_subsequence = %d \n",original_end_of_token_subsequence);
#endif

  // DQ (12/26/2014): Modify to only adjust the white space if there exists some whitespace to start with.
     if ( tokenStream[else_whitespace_end]->p_tok_elem->token_id == C_CXX_WHITESPACE ||
          tokenStream[else_whitespace_end]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO )
        {
       // while (leading_whitespace_start > original_start_of_token_subsequence && tokenStream[leading_whitespace_start-1]->p_tok_elem->token_id == C_CXX_WHITESPACE)
          while ( else_whitespace_end < original_end_of_token_subsequence &&
                  ( tokenStream[else_whitespace_end]->p_tok_elem->token_id == C_CXX_WHITESPACE ||
                    tokenStream[else_whitespace_end]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO) )
             {
#if 0
               printf ("in loop: else_whitespace_end = %d \n",else_whitespace_end);
#endif
               ROSE_ASSERT(else_whitespace_end < original_end_of_token_subsequence);

               else_whitespace_end++;
             }

#if 0
          printf ("In discoverElseSyntax(): after loop: else_whitespace_end = %d \n",else_whitespace_end);
#endif

       // If the positions are out of bounds then we don't have any leading whitespace.
          if (else_whitespace_end > original_end_of_token_subsequence && else_whitespace_start > original_end_of_token_subsequence)
             {
               else_whitespace_start = -1;
               else_whitespace_end   = -1;
             }
        }
       else
        {
#if 0
          printf ("In discoverElseSyntax(): no initial whitespace detected to start the  loop (reset to not define whitespace) \n");
#endif
          else_whitespace_start = -1;
          else_whitespace_end   = -1;
        }

  // Set the else_whitespace_start to the end (since "else" is a single token.
     else_whitespace_start = else_whitespace_end;

     if_statement_mappingInfo->else_whitespace_start = else_whitespace_start;
     if_statement_mappingInfo->else_whitespace_end   = else_whitespace_end;

#if 0
     printf ("In discoverElseSyntax(): (adjusted) if_statement_mappingInfo->else_whitespace_start = %d \n",if_statement_mappingInfo->else_whitespace_start);
     printf ("In discoverElseSyntax(): (adjusted) if_statement_mappingInfo->else_whitespace_end   = %d \n",if_statement_mappingInfo->else_whitespace_end);
#endif

     ROSE_ASSERT(if_statement_mappingInfo->else_whitespace_end == if_statement_mappingInfo->else_whitespace_start);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }



SynthesizedAttribute
TokenMappingTraversal::evaluateSynthesizedAttribute ( SgNode* n, InheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes )
   {
  // This traversal step computes the leading an trailing edges of each node in the child list for the current SgNode.
  // It also builds token subsequence mappings for any interval of child IR nodes of the AST for which they were not 
  // computed in the evaluateInheritedAttribute() function (on the way down in the AST traversal).

     int original_start_of_token_subsequence = inheritedAttribute.start_of_token_sequence;
     int original_end_of_token_subsequence   = inheritedAttribute.end_of_token_sequence;

#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
     printf ("\nIn evaluateSynthesizedAttribute(): n = %p = %s childAttributes.size() = %zu (start=%d,end=%d) \n",n,n->class_name().c_str(),childAttributes.size(),original_start_of_token_subsequence,original_end_of_token_subsequence);
     if (isSgClassDeclaration(n) != NULL)
        {
          printf ("   --- class name = %s \n",isSgClassDeclaration(n)->get_name().str());
        }
     if (isSgFunctionDeclaration(n) != NULL)
        {
          printf ("   --- function name = %s \n",isSgFunctionDeclaration(n)->get_name().str());
        }
     printf ("   --- original_start_of_token_subsequence = %d original_end_of_token_subsequence = %d \n",original_start_of_token_subsequence,original_end_of_token_subsequence);
     printf ("   --- inheritedAttribute.processChildNodes = %s \n",inheritedAttribute.processChildNodes ? "true" : "false");
#endif

  // DQ (10/14/2013): Added consistancy test.
     consistancyCheck();

#if 0
  // DQ (11/30/2013): This is not the correct way to handle children of a SgBasicBlock.
     SgBasicBlock* block = isSgBasicBlock(n);
     if (block != NULL)
        {
       // We want the children to not include the "{" and "}" of the SgBasicBlock.
          original_start_of_token_subsequence++;
          original_end_of_token_subsequence--;
#if 0
          printf ("In evaluateSynthesizedAttribute(): Reset in the case of a SgBasicBlock: original_start_of_token_subsequence = %d original_end_of_token_subsequence = %d \n",original_start_of_token_subsequence,original_end_of_token_subsequence);
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // DQ (12/15/2014): We need to handle the case of even a single child as well 
  // (so that we get the leading and trailing white space correct).
  // if (childAttributes.size() > 1)
     if (childAttributes.size() > 0)
        {
       // Where the number of children are greater than 1, then we have to compute the token subsequence that appears between the children.

          if (isSgStatement(n) != NULL)
             {
            // This is a statement with multiple children.
#if 0
               isSgStatement(n)->get_startOfConstruct()->display("In evaluateSynthesizedAttribute(): debug");
#endif
            // Note: because some nodes traversed in the AST are NULL, we have to accumulate the non-null children 
            // and process the cases where there are 2 or more of them.
               vector<TokenStreamSequenceToNodeMapping*> tokenToNodeVector;

            // Save the index entries of child IR nodes that didn't have an asociat3d token sequence.
            // vector<SgNode*> nodesWithoutTokenMappings;
               vector<size_t> childrenWithoutTokenMappings;

#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
               printf ("In evaluateSynthesizedAttribute(): children: \n");
#endif

            // DQ (12/22/2014): Record the last child that contains token mapping information (e.g. template 
            // instantations and normalized template declarations will be excluded).
               int firstChildWithTokenMapping = -1;
               int lastChildWithTokenMapping  = -1;

            // DQ (1/24/2015): Handle the case of the null for init and null test statements in "for ( ; ; )".
               SgForStatement* forStatement = isSgForStatement(n);
               if (forStatement != NULL)
                  {
                 // TokenStreamSequenceToNodeMapping* for_mappingInfo = tokenStreamSequenceMap[n];
                 // if (for_mappingInfo != NULL)
                    if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                       {
                         TokenStreamSequenceToNodeMapping* for_mappingInfo      = tokenStreamSequenceMap[n];
                      // TokenStreamSequenceToNodeMapping* for_init_mappingInfo = tokenStreamSequenceMap[childAttributes[SgForStatement_for_init_stmt].node];
                      // TokenStreamSequenceToNodeMapping* for_test_mappingInfo = tokenStreamSequenceMap[childAttributes[SgForStatement_test].node];
#if 0
                         printf ("Found SgForStatement with mapping in the token stream \n");
                         printf ("   --- tokenStreamSequenceMap.find(childAttributes[SgForStatement_test].node) != tokenStreamSequenceMap.end() = %s \n",
                              tokenStreamSequenceMap.find(childAttributes[SgForStatement_test].node) != tokenStreamSequenceMap.end() ? "true" : "false");
                         printf ("   --- tokenStreamSequenceMap.find(childAttributes[SgForStatement_for_init_stmt].node) != tokenStreamSequenceMap.end() = %s \n",
                              tokenStreamSequenceMap.find(childAttributes[SgForStatement_for_init_stmt].node) != tokenStreamSequenceMap.end() ? "true" : "false");
#endif
                         ROSE_ASSERT(for_mappingInfo != NULL);
                      // ROSE_ASSERT(for_init_mappingInfo != NULL);
                      // ROSE_ASSERT(for_test_mappingInfo != NULL);
                      // if (for_test_mappingInfo != NULL)
                      // if (tokenStreamSequenceMap.find(childAttributes[SgForStatement_test].node) != tokenStreamSequenceMap.end())
                         if ( ( tokenStreamSequenceMap.find(childAttributes[SgForStatement_test].node) != tokenStreamSequenceMap.end() ) &&
                              ( tokenStreamSequenceMap.find(childAttributes[SgForStatement_for_init_stmt].node) != tokenStreamSequenceMap.end() ) )
                            {
                              TokenStreamSequenceToNodeMapping* for_init_mappingInfo = tokenStreamSequenceMap[childAttributes[SgForStatement_for_init_stmt].node];
                              TokenStreamSequenceToNodeMapping* for_test_mappingInfo = tokenStreamSequenceMap[childAttributes[SgForStatement_test].node];

                              SgForInitStatement* previous_for_init_statement = isSgForInitStatement(for_init_mappingInfo->node);
                              SgNullStatement* null_statement = isSgNullStatement(for_test_mappingInfo->node);
#if 0
                              printf ("previous_for_init_statement = %p \n",previous_for_init_statement);
                              printf ("null_statement              = %p \n",null_statement);
#endif
                              if (previous_for_init_statement != NULL && null_statement != NULL)
                                 {
                                // This is at least after the for_init_statement, and is a better position to start the direct search within the token stream.
                                // This will also avoid the test statement being confused as being shared with the token stream subsequence of the for_init_statement.
#if 0
                                   printf ("INITIAL RESET: the for_test_mappingInfo->token_subsequence = (%d,%d) to (%d,%d) \n",
                                        for_test_mappingInfo->token_subsequence_start,for_test_mappingInfo->token_subsequence_end,
                                        for_init_mappingInfo->token_subsequence_end+1,for_init_mappingInfo->token_subsequence_end+1);
#endif
                                   for_test_mappingInfo->token_subsequence_start = for_init_mappingInfo->token_subsequence_end+1;
                                   for_test_mappingInfo->token_subsequence_end   = for_test_mappingInfo->token_subsequence_start;

                                   int index      = for_test_mappingInfo->token_subsequence_start;
                                   int upperBound = for_mappingInfo->token_subsequence_end;

                                   ROSE_ASSERT(index >= 0);
                                   ROSE_ASSERT(upperBound >= 0);

                                   while (tokenStream[index]->p_tok_elem->token_lexeme != ";" && index < upperBound)
                                      {
                                        index++;
                                      }
                                   ROSE_ASSERT(tokenStream[index]->p_tok_elem->token_lexeme == ";");

                                   for_test_mappingInfo->token_subsequence_start = index;
                                   for_test_mappingInfo->token_subsequence_end   = index;
#if 0
                                   printf ("LATER RESET: the for_test_mappingInfo->token_subsequence = (%d,%d) to (%d,%d) \n",
                                        for_test_mappingInfo->token_subsequence_start,for_test_mappingInfo->token_subsequence_end,
                                        for_init_mappingInfo->token_subsequence_end+1,for_init_mappingInfo->token_subsequence_end+1);
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }

                      // DQ (1/27/2015): Adjust the the for loop increment expression mapping relative to the end of the for loop test to handle
                      // the case where this may be a prefix operator++() and compiler generated and thus not mapped accurately to the token start.
                         if ( ( tokenStreamSequenceMap.find(childAttributes[SgForStatement_test].node) != tokenStreamSequenceMap.end() ) &&
                              ( tokenStreamSequenceMap.find(childAttributes[SgForStatement_increment].node) != tokenStreamSequenceMap.end() ) )
                            {
#if 0
                              printf ("$$$$$$$$$$ Detected for loop increment mapping (might be nodified here) \n");
#endif
                              TokenStreamSequenceToNodeMapping* for_test_mappingInfo      = tokenStreamSequenceMap[childAttributes[SgForStatement_test].node];
                              TokenStreamSequenceToNodeMapping* for_increment_mappingInfo = tokenStreamSequenceMap[childAttributes[SgForStatement_increment].node];

                              ROSE_ASSERT(for_test_mappingInfo != NULL);
                              ROSE_ASSERT(for_increment_mappingInfo != NULL);

                              int test_end = for_test_mappingInfo->token_subsequence_end;
                              int increment_start = for_increment_mappingInfo->token_subsequence_start;

                           // DQ (3/25/2017): Clang reports these as unused variables.
                           // int increment_end = for_increment_mappingInfo->token_subsequence_start;
#if 0
                              printf ("test_end = %d increment_start = %d increment_end = %d \n",test_end,increment_start,increment_end);
#endif
                              int better_start_of_token_subsequence = test_end + 1;

                              while ( better_start_of_token_subsequence < increment_start && 
                                      ( tokenStream[better_start_of_token_subsequence]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO ||
                                        tokenStream[better_start_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                 {
                                   better_start_of_token_subsequence++;
                                 }
#if 0
                              printf ("better_start_of_token_subsequence = %d \n",better_start_of_token_subsequence);
#endif
                              for_increment_mappingInfo->token_subsequence_start = better_start_of_token_subsequence;
#if 0
                              printf ("RESET: for_increment_mappingInfo->token_subsequence_start = %d \n",for_increment_mappingInfo->token_subsequence_start);
#endif
                            }
                       }
                  }

            // DQ (1/27/2015): Test for and handle the case of extra parenthisis around the conditional in a while statement (e.g. "while ( (i=5) ) { }").
            // See example test code: test2015_111.C.
               SgWhileStmt* whileStatement = isSgWhileStmt(n);
               if (whileStatement != NULL)
                  {
                    if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                       {
                         TokenStreamSequenceToNodeMapping* while_mappingInfo      = tokenStreamSequenceMap[n];
                         if ( ( tokenStreamSequenceMap.find(childAttributes[SgWhileStmt_condition].node) != tokenStreamSequenceMap.end() ) &&
                              ( tokenStreamSequenceMap.find(childAttributes[SgWhileStmt_body].node) != tokenStreamSequenceMap.end() ) )
                            {
#if 0
                              printf ("$$$$$$$$$$ Detected while loop condition mapping (might be nodified here) \n");
#endif
                              TokenStreamSequenceToNodeMapping* while_condition_mappingInfo = tokenStreamSequenceMap[childAttributes[SgWhileStmt_condition].node];
                              TokenStreamSequenceToNodeMapping* while_body_mappingInfo      = tokenStreamSequenceMap[childAttributes[SgWhileStmt_body].node];

                              int while_token_index_start     = while_mappingInfo->token_subsequence_start;
                              int while_condition_index_start = while_condition_mappingInfo->token_subsequence_start;
                              int while_condition_index_end   = while_condition_mappingInfo->token_subsequence_end;
                              int while_body_index_start      = while_body_mappingInfo->token_subsequence_start;
#if 0
                              printf ("while_token_index_start     = %d \n",while_token_index_start);
                              printf ("while_condition_index_start = %d \n",while_condition_index_start);
                              printf ("while_condition_index_end   = %d \n",while_condition_index_end);
                              printf ("while_body_index_start      = %d \n",while_body_index_start);
#endif
                           // Start at the while_condition_index_start and iterate back to find the first "(" after the "while token" at while_token_index_start.
                              int better_start_of_condition_token_subsequence = while_token_index_start + 1;

                              bool skipped_first = false;
                              while ( better_start_of_condition_token_subsequence < while_condition_index_start && 
                                      ( tokenStream[better_start_of_condition_token_subsequence]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO ||
                                        tokenStream[better_start_of_condition_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                 {
                                   better_start_of_condition_token_subsequence++;
                                // if (mappingInfo->leading_whitespace_start != -1 && tokenStream[mappingInfo->leading_whitespace_start]->p_tok_elem->token_lexeme == "{")
                                   if (skipped_first == false && tokenStream[better_start_of_condition_token_subsequence]->p_tok_elem->token_lexeme == "(")
                                      {
                                        better_start_of_condition_token_subsequence++;
                                        skipped_first = true;
                                      }
                                 }
#if 0
                              printf ("better_start_of_condition_token_subsequence = %d \n",better_start_of_condition_token_subsequence);
#endif

                           // Start at the while_body_index_start and iterate back to find the first ")".
                           // Can't use while_body_index_start - 1, because there might not be whitespace before 
                           // the "{" (see test2015_113.C), unless we test directly for a ")" as we do below.
                              int better_end_of_condition_token_subsequence = while_body_index_start - 1;

                           // Make allowence for case of while statement with body that is not a SgBasicBlock (see test2015_90.C).
                              bool skipped_last = false;
                              if (tokenStream[better_end_of_condition_token_subsequence]->p_tok_elem->token_lexeme == ")" )
                                 {
                                   skipped_last = true;
                                   better_end_of_condition_token_subsequence--;
                                 }

                              while ( better_end_of_condition_token_subsequence > while_condition_index_end && 
                                      ( tokenStream[better_end_of_condition_token_subsequence]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO ||
                                        tokenStream[better_end_of_condition_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                     // tokenStream[better_end_of_condition_token_subsequence]->p_tok_elem->token_lexeme == "{" ) )
                                 {
                                   better_end_of_condition_token_subsequence--;
                                   if (skipped_last == false && tokenStream[better_end_of_condition_token_subsequence]->p_tok_elem->token_lexeme == ")")
                                      {
                                        better_end_of_condition_token_subsequence--;
                                        skipped_last = true;
                                      }
                                 }
#if 0
                              printf ("better_end_of_condition_token_subsequence   = %d \n",better_end_of_condition_token_subsequence);
#endif
                              while_condition_mappingInfo->token_subsequence_start = better_start_of_condition_token_subsequence;
                              while_condition_mappingInfo->token_subsequence_end   = better_end_of_condition_token_subsequence;
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                       }
                  }

            // DQ (1/27/2015): Adding special support since the function prototype does not have any children that will generate mappings (except itself).
            // So the tokenToNodeVector will be empty.
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
               if (functionDeclaration != NULL)
                  {
                 // DQ (12/13/2015): I think this applies to all function declarations, not just non-defining and 
                 // certainly not just the first non-defining declaration.
                 // if (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())
                    if (true) // functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())
                       {
#if 0
                         printf ("Detected function prototype: \n");
                         functionDeclaration->get_startOfConstruct()->display("start of functionDeclaration: debug");
                         functionDeclaration->get_endOfConstruct()->display("end of functionDeclaration: debug");
#endif
                         if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                            {
                              TokenStreamSequenceToNodeMapping* function_protytype_mappingInfo = tokenStreamSequenceMap[n];
                              ROSE_ASSERT(function_protytype_mappingInfo != NULL);

                           // DQ (12/13/2015): This is required code and demonstrated in tests/nonsmoke/functional/roseTests/astTokenStreamTests/input_test2015_01.c
                           // DQ (12/12/2015): I am hoping we can avoid this fixup (we want to have as few as possible).
                           // Also this might not be required now that we have fixed the source position information in 
                           // the AST for secondary declarations (using the declaration_range where available).

                              int function_protype_start = function_protytype_mappingInfo->token_subsequence_start;
                              int function_protype_end   = function_protytype_mappingInfo->token_subsequence_end;
#if 0
                              printf ("   --- function_protype_start = %d function_protype_end = %d \n",function_protype_start,function_protype_end);
#endif
                              ROSE_ASSERT(function_protype_start >= 0);

                           // DQ (12/13/2015): This is not so simple, we have now fixed a bug in the source positon in the starting position of 
                           // secondary declarations (which is the proper fix for the start of the function prototypes).  This while loop handles
                           // the case of CPP directives in the way of identifying the start of the function prototype in the token sequence.

                           // DQ (12/13/2015): This is not the correct fix, restoring this to the original setting.
                           // DQ (12/12/2015): This is the cause of getting the wrong token for the start of the function (skips over the return type).
                           // int better_start_of_function_prototype_token_subsequence = function_protype_start;
                           // int better_start_of_function_prototype_token_subsequence = function_protype_start + 1;
                              int better_start_of_function_prototype_token_subsequence = function_protype_start;
                              SgScopeStatement* scope = isSgScopeStatement(functionDeclaration->get_parent());

                           // DQ (12/13/2015): Some scopes have an associated token such as "{" that we need to skip over,
                           // and some do not (e.g. SgGlobal).  Only three specific kinds of scope have this property.
                              if (isSgBasicBlock(scope) != NULL || 
                                  isSgNamespaceDefinitionStatement(scope) != NULL ||
                                  isSgClassDefinition(scope) != NULL)
                                 {
                                   better_start_of_function_prototype_token_subsequence++;
                                 }

                              while ( better_start_of_function_prototype_token_subsequence < function_protype_end && 
                                      ( tokenStream[better_start_of_function_prototype_token_subsequence]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO ||
                                        tokenStream[better_start_of_function_prototype_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                 {
                                   better_start_of_function_prototype_token_subsequence++;
                                 }
#if 0
                              printf ("better_start_of_function_prototype_token_subsequence = %d \n",better_start_of_function_prototype_token_subsequence);
#endif
                              function_protytype_mappingInfo->token_subsequence_start = better_start_of_function_prototype_token_subsequence;
#if 0
                              printf ("RESET: function_protytype_mappingInfo->token_subsequence_start = %d end = %d \n",function_protytype_mappingInfo->token_subsequence_start,function_protytype_mappingInfo->token_subsequence_end);
#endif
                            }
                       }
                      else
                       {
                      // DQ (12/13/2015): I think the code above needs to address the case of any other non-defining declarations.
                         if (functionDeclaration != functionDeclaration->get_firstNondefiningDeclaration())
                            {
                              printf ("WARNING: additional nondefining functions not the first non-defining declaration may have to also be reset! \n");
                            }
                       }
                  }

               for (size_t i = 0; i < childAttributes.size(); i++)
                  {
                 // ROSE_ASSERT(childAttributes[i].node != NULL);

                    string child_name = n->get_traversalSuccessorNamesContainer()[i];

#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                    printf ("   --- In evaluateSynthesizedAttribute(): child_name = %s child node = %p = %s \n",
                         child_name.c_str(),childAttributes[i].node,(childAttributes[i].node != NULL) ? childAttributes[i].node->class_name().c_str() : "null");
#endif
                 // DQ (10/14/2013): Added consistancy test.
                    consistancyCheck();

#if 0
                 // DQ (1/6/2015): Adding assertions to eliminate possible null entries in the tokenStreamSequenceMap.
                 // ROSE_ASSERT(childAttributes[i].node != NULL);
                    if (childAttributes[i].node == NULL)
                       {
#if 0
                         printf ("WARNING: tokenStreamSequenceMap will contain a NULL key entry: n = %p = %s childAttributes[i=%zu].node == NULL \n",n,n->class_name().c_str(),i);
#else
                      // DQ (1/7/2015): I want to control the output of this warning, and look into it later.
                         static int counter = 0;
                         if (counter % 10000 == 0)
                            {
                              printf ("WARNING: tokenStreamSequenceMap will contain a NULL key entry: n = %p = %s childAttributes[i=%zu].node == NULL \n",n,n->class_name().c_str(),i);
                            }
#endif
                         counter++;
                       }
#endif
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                    printf ("   --- tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end() = %s \n",
                         tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end() ? "true" : "false");
#endif
                 // Look up these children in the tokenStreamSequenceMap
                 // if (tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end())
                 // if (childAttributes[i].node != NULL && tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end())
                    if (tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end())
                       {
                         TokenStreamSequenceToNodeMapping* mappingInfo = tokenStreamSequenceMap[childAttributes[i].node];

                         ROSE_ASSERT(mappingInfo != NULL);

#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                         printf ("       --- Found mapping information \n");
                         printf ("       --- mappingInfo->token_subsequence_start = %d end = %d \n",mappingInfo->token_subsequence_start,mappingInfo->token_subsequence_end);
#endif

                      // This is used to know where the compute the leading white space token information (in cases where the leading children do not have a token mapping).
                         if (firstChildWithTokenMapping < 0)
                            {
                              firstChildWithTokenMapping = (int)i;
                            }

                      // This is used to know where the compute the trailing white space token information (in cases where the trailing children do not have a token mapping).
                         lastChildWithTokenMapping = (int)i;

                      // DQ (1/6/2015): Adding assertion.
                         ROSE_ASSERT(mappingInfo != NULL);

                         TokenStreamSequenceToNodeMapping* mappingInfo_to_add = mappingInfo;

                         vector<TokenStreamSequenceToNodeMapping*> tokenToNodeEntriesToRemove;

#define DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS 0

                      // DQ (10/14/2013): We need to detect cases where a sibling token subsequence range is nested in another 
                      // token subsequence range of another sibling. These will likely be adjacent IR nodes (see test2013_87.c).
                         for (size_t j = 0; j < tokenToNodeVector.size(); j++)
                            {
                           // Check to see if this is the superset of any existing subsequence range.
                              TokenStreamSequenceToNodeMapping* previous_mappingInfo = tokenToNodeVector[j];
                              ROSE_ASSERT(previous_mappingInfo != NULL);

                              int current_token_sequence_start  = mappingInfo->token_subsequence_start;
                              int current_token_sequence_end    = mappingInfo->token_subsequence_end;
                              int previous_token_sequence_start = previous_mappingInfo->token_subsequence_start;
                              int previous_token_sequence_end   = previous_mappingInfo->token_subsequence_end;

                              ROSE_ASSERT(current_token_sequence_start  >= 0 && current_token_sequence_end  >= 0);
                              ROSE_ASSERT(previous_token_sequence_start >= 0 && previous_token_sequence_end >= 0);
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                              printf ("Checking for nested subsequences: current_token_sequence (%d,%d) previous_token_sequence (%d,%d) \n",current_token_sequence_start,current_token_sequence_end,previous_token_sequence_start,previous_token_sequence_end);
#endif
                           // We want to only detect proper nesting (not equality).
                           // Equality is represented via sharing, while nested subsets will cause the nested subsequence data structure to be removed.
                           // Note: test2013_90.c demonstrates where the nesting is not perfect (an edge is shared). So this existing implementation 
                           // has to detect that case.
                           // if ( (current_token_sequence_start <= previous_token_sequence_start) && (current_token_sequence_end >= previous_token_sequence_end) )
                           // if ( (current_token_sequence_start < previous_token_sequence_start) && (current_token_sequence_end > previous_token_sequence_end) )
                              if ( ( (current_token_sequence_start  < previous_token_sequence_start) && (current_token_sequence_end  > previous_token_sequence_end) ) ||
                                   ( (current_token_sequence_start == previous_token_sequence_start) && (current_token_sequence_end  > previous_token_sequence_end) ) ||
                                   ( (current_token_sequence_start  < previous_token_sequence_start) && (current_token_sequence_end == previous_token_sequence_end) ) )
                                 {
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("previous_mappingInfo = %p mappingInfo = %p \n",previous_mappingInfo,mappingInfo);
                                   printf ("Found properly nested subsequence: previous_mappingInfo->node = %p = %s IS NESTED IN mappingInfo->node = %p = %s \n",
                                        previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str(),
                                        mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   SgStatement* statement = isSgStatement(childAttributes[i].node);
                                   if (statement != NULL)
                                      {
                                        statement->get_startOfConstruct()->display("Found properly nested subsequence: startOfConstruct: debug");
                                        statement->get_endOfConstruct()  ->display("Found properly nested subsequence: endOfConstruct: debug");
                                      }
#endif
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("Checking for nested subsequences: current_token_sequence (%d,%d) previous_token_sequence (%d,%d) \n",current_token_sequence_start,current_token_sequence_end,previous_token_sequence_start,previous_token_sequence_end);
#endif
                                // Remove the inner class from the tokenToNodeVector, and the IR node to token subsequence map
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("Remove the inner class from the tokenToNodeVector, and the IR node to token subsequence map \n");
#endif
                                   tokenToNodeEntriesToRemove.push_back(previous_mappingInfo);

                                // Mark the outer token sequense as being shared across multiple IR nodes
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("Mark the outer token sequense as being shared across multiple IR nodes \n");
#endif
                                // Later I think we can't assert this (if there is more then two levels of nesting amoungst child token subsequences.
                                   if (mappingInfo->shared == true)
                                      {
                                        printf ("WARNING: detected recursive case of handling nested token sequences \n");
                                        printf ("WARNING: detected recursive case: current_token_sequence (%d,%d) previous_token_sequence (%d,%d) \n",current_token_sequence_start,current_token_sequence_end,previous_token_sequence_start,previous_token_sequence_end);
                                      }
                                // ROSE_ASSERT(mappingInfo->shared == false);

                                // Nested token sequences remove the data structure of the inner nested token sequence 
                                // (see test2013_87.c), so it is not the same as shared across multiple nodes (as in test2013_81.c).
                                // mappingInfo->shared = true;

                                // Add the associated IR nodes to the token sequence's node vector.
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("Add the associated IR nodes to the token sequence's node vector \n");
#endif
                                   mappingInfo->nodeVector.push_back(previous_mappingInfo->node);

                                // Save the mappingInfo for all children as this IR node in the AST.
                                // tokenToNodeVector.push_back(mappingInfo);
                                   mappingInfo_to_add = mappingInfo;
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
                                // if ( (current_token_sequence_start > previous_token_sequence_start) && (current_token_sequence_end < previous_token_sequence_end) )
                                   if ( ( (current_token_sequence_start  > previous_token_sequence_start) && (current_token_sequence_end  < previous_token_sequence_end) ) ||
                                         ( (current_token_sequence_start == previous_token_sequence_start) && (current_token_sequence_end  < previous_token_sequence_end) ) ||
                                         ( (current_token_sequence_start  > previous_token_sequence_start) && (current_token_sequence_end == previous_token_sequence_end) ) )
                                      {
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                        printf ("previous_mappingInfo = %p mappingInfo = %p \n",previous_mappingInfo,mappingInfo);
                                        printf ("Found properly nested subsequence: previous_mappingInfo->node = %p = %s IS A SUPER SET OF mappingInfo->node = %p = %s \n",
                                             previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str(),
                                             mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                        printf ("BEFORE ERASE: tokenStreamSequenceMap.size() = %" PRIuPTR " tokenStreamSequenceVector.size() = %" PRIuPTR " \n",tokenStreamSequenceMap.size(),tokenStreamSequenceVector.size());
#endif
                                     // Remove the associated token stream subsequence from the map.
                                     // ROSE_ASSERT(tokenStreamSequenceMap.find(mappingInfo->node) != tokenStreamSequenceMap.end());
                                     // k1 = tokenStreamSequenceMap.find(mappingInfo->node);
                                        if (tokenStreamSequenceMap.find(mappingInfo->node) != tokenStreamSequenceMap.end())
                                           {
                                          // tokenStreamSequenceMap.erase(tokenStreamSequenceMap.find(mappingInfo->node));
                                             tokenStreamSequenceMap.erase(tokenStreamSequenceMap.find(mappingInfo->node));
                                           }
                                          else
                                           {
                                             printf ("mappingInfo = %p node = %p = %s NOT FOUND in tokenStreamSequenceMap \n",mappingInfo,mappingInfo->node,mappingInfo->node->class_name().c_str());
                                           }

                                        vector<TokenStreamSequenceToNodeMapping*>::iterator k2 = find(tokenStreamSequenceVector.begin(),tokenStreamSequenceVector.end(),mappingInfo);
                                     // ROSE_ASSERT(k2 != tokenStreamSequenceVector.end());
                                        if (k2 != tokenStreamSequenceVector.end())
                                           {
                                             tokenStreamSequenceVector.erase(k2);
                                           }
                                          else
                                           {
                                             printf ("mappingInfo = %p node = %p = %s NOT FOUND in tokenStreamSequenceVector \n",mappingInfo,mappingInfo->node,mappingInfo->node->class_name().c_str());
                                           }
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                        printf ("AFTER ERASE: tokenStreamSequenceMap.size() = %" PRIuPTR " tokenStreamSequenceVector.size() = %" PRIuPTR " \n",tokenStreamSequenceMap.size(),tokenStreamSequenceVector.size());
#endif
                                     // DQ (10/14/2013): Added consistancy test.
                                        consistancyCheck();
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                     else
                                      {
                                     // This is not any kind of nested subsequence.
#if 0
                                     // DQ (1/13/2015): Detect multiple references to the same token subsequence (happens for multiple or nested statements in macros).
                                     // See inputmoveDeclarationToInnermostScope_test2015_44.C for an example.  This location does not work for this test, since the 
                                     // target statements that nest are in a deeper level of the AST.  So I think we need a seperate traversal to address this.

                                         if ( (current_token_sequence_start == previous_token_sequence_start) && (current_token_sequence_end == previous_token_sequence_end) )
                                           {
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                             printf ("previous_mappingInfo = %p mappingInfo = %p \n",previous_mappingInfo,mappingInfo);
                                             printf ("Found matching token subsequence: previous_mappingInfo->node = %p = %s IS A MATCH OF mappingInfo->node = %p = %s \n",
                                                  previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str(),
                                                  mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif

#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
#endif
                                      }
                                 }
                            }

                         ROSE_ASSERT(mappingInfo_to_add != NULL);
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                         printf ("BEFORE erase: tokenToNodeVector.size() = %" PRIuPTR " \n",tokenToNodeVector.size());
#endif
                      // DQ (10/14/2013): Added consistancy test.
                         consistancyCheck();
#if 0
                      // DQ (11/30/2013): Added exit as part of testing.
                         if (isSgBasicBlock(n) != NULL)
                            {
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
                            }
#endif
                      // Remove the entries that we have detected to be nested inside of other sibling IR node subsequences.
                      // tokenToNodeVector.erase(tokenToNodeEntriesToRemove.begin(),tokenToNodeEntriesToRemove.end());
                         for (size_t index = 0; index < tokenToNodeEntriesToRemove.size(); index++)
                            {
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                              printf ("tokenToNodeEntriesToRemove[index=%" PRIuPTR "] = %p node = %p = %s \n",index,tokenToNodeEntriesToRemove[index],tokenToNodeEntriesToRemove[index]->node,tokenToNodeEntriesToRemove[index]->node->class_name().c_str());
#endif
                              vector<TokenStreamSequenceToNodeMapping*>::iterator k1,k2;
                              k1 = find(tokenToNodeVector.begin(),tokenToNodeVector.end(),tokenToNodeEntriesToRemove[index]);
                              if (k1 != tokenToNodeVector.end())
                                 {
                                   tokenToNodeVector.erase(k1);
                                 }

                              ROSE_ASSERT(tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end());
                              ROSE_ASSERT(tokenToNodeEntriesToRemove[index]->node != NULL);
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                              printf ("BEFORE ERASE: tokenStreamSequenceMap.size() = %" PRIuPTR " tokenStreamSequenceVector.size() = %" PRIuPTR " \n",tokenStreamSequenceMap.size(),tokenStreamSequenceVector.size());
#endif
                           // tokenStreamSequenceMap.erase(tokenToNodeEntriesToRemove[index].node);
                           // tokenStreamSequenceMap.erase(tokenStreamSequenceMap.find(tokenToNodeEntriesToRemove[index]->node));
                              if (tokenStreamSequenceMap.find(tokenToNodeEntriesToRemove[index]->node) != tokenStreamSequenceMap.end())
                                 {
                                   tokenStreamSequenceMap.erase(tokenStreamSequenceMap.find(tokenToNodeEntriesToRemove[index]->node));
                                 }
                                else
                                 {
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("tokenToNodeEntriesToRemove[index=%" PRIuPTR "] = %p node = %p = %s NOT FOUND in tokenStreamSequenceMap \n",
                                        index,tokenToNodeEntriesToRemove[index],tokenToNodeEntriesToRemove[index]->node,tokenToNodeEntriesToRemove[index]->node->class_name().c_str());
#endif
                                 }

                              k2 = find(tokenStreamSequenceVector.begin(),tokenStreamSequenceVector.end(),tokenToNodeEntriesToRemove[index]);
                              if (k2 != tokenStreamSequenceVector.end())
                                 {
                                   tokenStreamSequenceVector.erase(k2);
                                 }
                                else
                                 {
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                                   printf ("tokenToNodeEntriesToRemove[index=%" PRIuPTR "] = %p node = %p = %s NOT FOUND in tokenStreamSequenceVector \n",
                                        index,tokenToNodeEntriesToRemove[index],tokenToNodeEntriesToRemove[index]->node,tokenToNodeEntriesToRemove[index]->node->class_name().c_str());
#endif
                                 }
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                              printf ("AFTER ERASE: tokenStreamSequenceMap.size() = %" PRIuPTR " tokenStreamSequenceVector.size() = %" PRIuPTR " \n",tokenStreamSequenceMap.size(),tokenStreamSequenceVector.size());
#endif
                            }
#if DEBUG_TOKEN_SHARING_BETWEEN_STATEMENTS
                         printf ("AFTER erase: tokenToNodeVector.size() = %" PRIuPTR " \n",tokenToNodeVector.size());
#endif
                      // DQ (10/14/2013): Added consistancy test.
                         consistancyCheck();
#if 0
                         vector<TokenStreamSequenceToNodeMapping*>::iterator k = tokenToNodeEntriesToRemove.begin();
                         while (k != tokenToNodeEntriesToRemove.end())
                            {
#if 0
                           // printf ("Deleting *k = %p node = %p = %s \n",*k,(*k)->node,(*k)->node->class_name().c_str());
                           // printf ("Deleting *k = %p node = %p \n",*k,(*k)->node);
                              printf ("Deleting *k = %p \n",*k);
#endif
                           // delete *k;
                           // *k = NULL;
                              k++;
                            }
#endif
                      // Save the mappingInfo for all children as this IR node in the AST.
                      // tokenToNodeVector.push_back(mappingInfo);
                         tokenToNodeVector.push_back(mappingInfo_to_add);
#if 0
                         if (tokenToNodeVector.size() > 1)
                            {
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
                            }
#endif
                      // DQ (12/11/2015): Check the child node's source position information against the token sequence mapping.
#if 1
                      // DQ (12/11/2015): Debugging code.
                      // ROSE_ASSERT(useTokenSequenceToImproveSourcePositionInfo == true);

                         if (useTokenSequenceToImproveSourcePositionInfo == true)
                            {
                              Sg_File_Info* start = (childAttributes[i].node != NULL) ? childAttributes[i].node->get_startOfConstruct() : NULL;
                              Sg_File_Info* end   = (childAttributes[i].node != NULL) ? childAttributes[i].node->get_endOfConstruct()   : NULL;

                              ROSE_ASSERT(start != NULL);
                              ROSE_ASSERT(end != NULL);
#if 1
#if 0
                              printf ("Check consistency of IR node source position with token sequence mapping: child_name = %s child node = %p = %s \n",
                                   child_name.c_str(),childAttributes[i].node,(childAttributes[i].node != NULL) ? childAttributes[i].node->class_name().c_str() : "null");
                              printf ("   --- start: line = %d col = %d end: line = %d col = %d \n",start->get_line(),start->get_col(),end->get_line(),end->get_col());
#endif

                              ROSE_ASSERT(tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end());
                              TokenStreamSequenceToNodeMapping* current_mappingInfo = tokenStreamSequenceMap[n];
                              ROSE_ASSERT(current_mappingInfo != NULL);

                           // DQ (3/25/2017): Clang reports these as unused variables.
                           // int current_token_sequence_start  = current_mappingInfo->token_subsequence_start;
                           // int current_token_sequence_end    = current_mappingInfo->token_subsequence_end;
#if 0
                              printf ("   --- n = %p = %s current_token_sequence (%d,%d) \n",n,n->class_name().c_str(),current_token_sequence_start,current_token_sequence_end);
#endif
                           // ROSE_ASSERT(childAttributes[i].node != NULL);
                              if (childAttributes[i].node != NULL)
                                 {
                                // ROSE_ASSERT(tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end());
                                   if (tokenStreamSequenceMap.find(childAttributes[i].node) != tokenStreamSequenceMap.end())
                                      {
                                        TokenStreamSequenceToNodeMapping* local_mappingInfo = tokenStreamSequenceMap[childAttributes[i].node];
                                        ROSE_ASSERT(local_mappingInfo != NULL);

                                     // Output the starting and ending token index information
                                        int local_token_sequence_start  = local_mappingInfo->token_subsequence_start;
                                        int local_token_sequence_end    = local_mappingInfo->token_subsequence_end;
#if 0
                                        printf ("   --- child info %p = %s : local_token_sequence (%d,%d) \n",
                                             childAttributes[i].node,childAttributes[i].node->class_name().c_str(),local_token_sequence_start,local_token_sequence_end);
#endif
                                     // Output the starting and ending line and column information
                                        int start_line = tokenStream[local_token_sequence_start]->beginning_fpi.line_num;
                                        int start_col  = tokenStream[local_token_sequence_start]->beginning_fpi.column_num;
                                        int end_line   = tokenStream[local_token_sequence_end]  ->ending_fpi.line_num;
                                        int end_col    = tokenStream[local_token_sequence_end]  ->ending_fpi.column_num;
#if 0
                                        printf ("   --- child info %p = %s : line and column info: start (%d,%d) end(%d,%d) \n",
                                             childAttributes[i].node,childAttributes[i].node->class_name().c_str(),start_line,start_col,end_line,end_col);
#endif
                                        if (start->get_line() != start_line || start->get_col() != start_col)
                                           {
                                             printf ("FIXUP: IR node %p = %s : Need to fixup STARTING source position (%d,%d) to match token stream: line and column info: start (%d,%d) \n",
                                                  childAttributes[i].node,childAttributes[i].node->class_name().c_str(),start->get_line(),start->get_col(),start_line,start_col);

                                          // Defer the corrections on the starting position until I find an example requiring this fixup.
                                          // childAttributes[i].node->get_startOfConstruct()->set_line(start_line);
                                          // childAttributes[i].node->get_startOfConstruct()->set_col(start_col);
                                             if (start->get_line() == start_line)
                                                {
                                               // ROSE_ASSERT(end->get_col() < end_col);
                                                  if (start->get_col() > start_col)
                                                     {
                                                    // This is the most I would like to be off by (and is a typical correction because many declarations omit the position of the trailing ";").
                                                    // Where macros are expaned for initializers in a variable declaration the colum number can be significantly off.
                                                    // ROSE_ASSERT(end_col - end->get_col() <= 1);

                                                    // DQ (12/12/2015): If the column positions are close then go ahead and fix the column position 
                                                    // in the AST, but not if they are beyond some distance.
                                                    // ROSE_ASSERT(end_col - end->get_col() <= 22);
                                                       if ( start->get_col() - start_col < 22)
                                                          {
                                                         // childAttributes[i].node->get_endOfConstruct()->set_line(end_line);
                                                            childAttributes[i].node->get_startOfConstruct()->set_col(start_col);
                                                          }
                                                         else
                                                          {
                                                            printf ("   --- WARNING: column numbers of IR node source position and token sequence don't match well enough (correction to large) \n");
                                                            printf ("   --- --- IR node %p = %s : Need to fixup STARTING source position in IR (%d,%d) to match token stream's line and column info: start (%d,%d) \n",
                                                                 childAttributes[i].node,childAttributes[i].node->class_name().c_str(),start->get_line(),start->get_col(),start_line,start_col);
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // Examples failing this test are: "#pragma pack(1)" which does not compute the ending column 
                                                    // position correctly because EDG normalizes the pragma's string to be "pack ( 1 )".  
                                                    // See tests/nonsmoke/functional/CompileTests/C_tests/YardenPragmaPackExample.c for an example.

                                                       printf ("   --- WARNING: column numbers of IR node source position and token sequence don't match well enough (correction is the wrong sign) \n");
                                                       printf ("   --- --- IR node %p = %s : Need to fixup STARTING source position in IR (%d,%d) to match token stream's line and column info: start (%d,%d) \n",
                                                            childAttributes[i].node,childAttributes[i].node->class_name().c_str(),start->get_line(),start->get_col(),start_line,start_col);
                                                     }
                                                }
                                               else
                                                {
                                                  printf ("   --- WARNING: line numbers of IR node source position and token sequence don't match (we want the line numbers to match exactly) \n");
                                                  printf ("   --- --- IR node %p = %s : Need to fixup STARTING source position in IR (%d,%d) to match token stream's line and column info: start (%d,%d) \n",
                                                     childAttributes[i].node,childAttributes[i].node->class_name().c_str(),start->get_line(),start->get_col(),start_line,start_col);
                                                }
                                           }

                                        if (end->get_line() != end_line || end->get_col() != end_col)
                                           {
                                             printf ("FIXUP: IR node %p = %s : Need to fixup ENDING source position (%d,%d) to match token stream: line and column info: start (%d,%d) \n",
                                                  childAttributes[i].node,childAttributes[i].node->class_name().c_str(),end->get_line(),end->get_col(),end_line,end_col);

                                          // DQ (12/12/2015): Make sure that the line number is at least correct.
                                          // ROSE_ASSERT(end->get_line() == end_line);
                                          // ROSE_ASSERT(end->get_line() <= end_line);
                                             if (end->get_line() == end_line)
                                                {
                                               // ROSE_ASSERT(end->get_col() < end_col);
                                                  if (end->get_col() < end_col)
                                                     {
                                                    // This is the most I would like to be off by (and is a typical correction because many declarations omit the position of the trailing ";").
                                                    // Where macros are expaned for initializers in a variable declaration the colum number can be significantly off.
                                                    // ROSE_ASSERT(end_col - end->get_col() <= 1);

                                                    // DQ (12/12/2015): If the column positions are close then go ahead and fix the column position 
                                                    // in the AST, but not if they are beyond some distance.
                                                    // ROSE_ASSERT(end_col - end->get_col() <= 22);
                                                       if (end_col - end->get_col() < 22)
                                                          {
                                                         // childAttributes[i].node->get_endOfConstruct()->set_line(end_line);
                                                            childAttributes[i].node->get_endOfConstruct()->set_col(end_col);
                                                          }
                                                         else
                                                          {
                                                            printf ("   --- WARNING: column numbers of IR node source position and token sequence don't match well enough (correction to large) \n");
                                                            printf ("   --- --- IR node %p = %s : Need to fixup ENDING source position in IR (%d,%d) to match token stream's line and column info: start (%d,%d) \n",
                                                                 childAttributes[i].node,childAttributes[i].node->class_name().c_str(),end->get_line(),end->get_col(),end_line,end_col);
                                                          }
                                                     }
                                                    else
                                                     {
                                                    // Examples failing this test are: "#pragma pack(1)" which does not compute the ending column 
                                                    // position correctly because EDG normalizes the pragma's string to be "pack ( 1 )".  
                                                    // See tests/nonsmoke/functional/CompileTests/C_tests/YardenPragmaPackExample.c for an example.

                                                       printf ("   --- WARNING: column numbers of IR node source position and token sequence don't match well enough (correction is the wrong sign) \n");
                                                       printf ("   --- --- IR node %p = %s : Need to fixup ENDING source position in IR (%d,%d) to match token stream's line and column info: start (%d,%d) \n",
                                                            childAttributes[i].node,childAttributes[i].node->class_name().c_str(),end->get_line(),end->get_col(),end_line,end_col);
                                                     }
                                                }
                                               else
                                                {
                                                  printf ("   --- WARNING: line numbers of IR node source position and token sequence don't match (we want the line numbers to match exactly) \n");
                                                  printf ("   --- --- IR node %p = %s : Need to fixup ENDING source position in IR (%d,%d) to match token stream's line and column info: start (%d,%d) \n",
                                                     childAttributes[i].node,childAttributes[i].node->class_name().c_str(),end->get_line(),end->get_col(),end_line,end_col);
                                                }
                                           }
                                      }
                                     else
                                      {
#if 1
                                        printf ("No mapping to token stream available for child = %p = %s \n",childAttributes[i].node,childAttributes[i].node->class_name().c_str());
#endif
                                      }
                                 }

#endif
#if 0
#if 0
                              int updated_end_line = previous_for_init_statement->get_endOfConstruct()->get_line();
                              int updated_end_col  = previous_for_init_statement->get_endOfConstruct()->get_col();
#else
                              int updated_end_line = tokenStream[index]->ending_fpi.line_num;
                              int updated_end_col  = tokenStream[index]->ending_fpi.column_num;
#endif
#if 1
                              printf ("Update the SgForInitStatement (%p) source position: updated_end_line = %d updated_end_col = %d \n",previous_for_init_statement,updated_end_line,updated_end_col);
#endif
#if 0
                              previous_for_init_statement->get_endOfConstruct()->set_line(updated_end_line);
                              previous_for_init_statement->get_endOfConstruct()->set_col(updated_end_col);
#endif
#endif
                            }
#endif
                       }
                      else
                       {
                      // We need to build a TokenStreamSequenceToNodeMapping for this case (but we currently do this afterward)..

#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                         string nodeName = childAttributes[i].node != NULL ? childAttributes[i].node->class_name() : "null";
                         printf ("      --- No mapping has been found at childAttributes[i].node = %p = %s \n",childAttributes[i].node,nodeName.c_str());
#endif
                      // We need to ignore NULL pointers.
                      // nodesWithoutTokenMappings.push_back(childAttributes[i].node);
                      // childrenWithoutTokenMappings.push_back(i);
                         if (childAttributes[i].node != NULL)
                            {
                           // Also make sure this IR node is associated with the current file.
                              SgStatement* statement = isSgStatement(childAttributes[i].node);
                              if (statement != NULL)
                                 {
                                   Sg_File_Info* start_pos = statement->get_startOfConstruct();
                                   ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);

                                // Note that this is implemented internally to use the physical file information (not logical file info).
                                   bool process_node = (start_pos->isSameFile(inheritedAttribute.sourceFile));
#if 0
                                   printf ("      --- process_node (is same file test) = %s \n",process_node ? "true" : "false");
#endif
                                   process_node = (process_node == true) && (inheritedAttribute.processChildNodes == true);
#if 0
                                   printf ("      --- process_node (inheritedAttribute.processChildNodes test) = %s \n",process_node ? "true" : "false");
#endif
#if 1
                                // DQ (1/4/2014): commented out to test with using token based unparsing.

                                // DQ (12/22/2014): If this is part of an EDG normalization of template function definitions in template classes then it should not be processed.
                                   SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(statement);
                                   if (functionDeclaration != NULL)
                                      {
                                        process_node = (process_node == true) && (functionDeclaration->isNormalizedTemplateFunction() == false);
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                                        printf ("      --- process_node = %s functionDeclaration->isNormalizedTemplateFunction() = %s \n",
                                             process_node ? "true" : "false",functionDeclaration->isNormalizedTemplateFunction() ? "true" : "false");
#endif
                                      }
#endif
                                   if (process_node == true)
                                      {
                                        childrenWithoutTokenMappings.push_back(i);
                                      }
                                 }
                            }
                       }

                 // DQ (10/14/2013): Added consistancy test.
                    consistancyCheck();
                  }
#if 0
            // List the IR nodes that have an identified token subsequence mapping (after removing nexted subsequence mappings).
               printf ("$$$$$$$$$$$$ List the IR nodes that have an identified token subsequence mappings (tokenToNodeVector.size() = %zu): n = %p = %s \n",
                    tokenToNodeVector.size(),n,n->class_name().c_str());
               for (size_t j = 0; j < tokenToNodeVector.size(); j++)
                  {
                    printf ("   --- tokenToNodeVector[j=%" PRIuPTR "] = %p = %s \n",j,tokenToNodeVector[j]->node,tokenToNodeVector[j]->node->class_name().c_str());
                 // printf ("   --- is first child with token mapping = %s \n",(firstChildWithTokenMapping == j) ? "true" : "false");
                 // printf ("   --- is last child with token mapping  = %s \n",(lastChildWithTokenMapping == j) ? "true" : "false");
                    printf ("   --- tokenToNodeVector[j=%zu]->leading_whitespace_start  = %d end = %d \n",j,tokenToNodeVector[j]->leading_whitespace_start,tokenToNodeVector[j]->leading_whitespace_end);
                    printf ("   --- tokenToNodeVector[j=%zu]->token_subsequence_start   = %d end = %d \n",j,tokenToNodeVector[j]->token_subsequence_start,tokenToNodeVector[j]->token_subsequence_end);
                    printf ("   --- tokenToNodeVector[j=%zu]->trailing_whitespace_start = %d end = %d \n",j,tokenToNodeVector[j]->trailing_whitespace_start,tokenToNodeVector[j]->trailing_whitespace_end);
                  }
#endif

#define DEBUG_MACRO_HANDLING 0

            // DQ (1/3/2014): We need to handle macro expansions that are characterized by having the same start and end source positions (but could also be a single token statement, e.g. ";").
            // Unfortunately, it can also be a token for a variable reference expression and thus we have to handle this case explicitly.
#if DEBUG_MACRO_HANDLING
               printf ("^^^^^^^^^^^ Looking for macro expansions within token subsequence mappings: \n");
            // printf ("   --- tokenToNodeVector[j=%zu]->token_subsequence_start   = %d end = %d \n",j,tokenToNodeVector[j]->token_subsequence_start,tokenToNodeVector[j]->token_subsequence_end);
#endif
               for (size_t j = 0; j < tokenToNodeVector.size(); j++)
                  {
#if DEBUG_MACRO_HANDLING
                    printf ("   --- tokenToNodeVector[j=%zu]->token_subsequence_start   = %d end = %d \n",j,tokenToNodeVector[j]->token_subsequence_start,tokenToNodeVector[j]->token_subsequence_end);
#endif
                 // This can be true for the case of a ";" (SgNullExpression in a SgExprStatement) as well as for where macros are used.
                    if (tokenToNodeVector[j]->token_subsequence_start == tokenToNodeVector[j]->token_subsequence_end)
                       {
#if DEBUG_MACRO_HANDLING
                         printf ("Detected possible macro expansion (statement without proper ending position) \n");
                         printf ("   --- tokenToNodeVector[j=%zu] = %p = %s \n",j,tokenToNodeVector[j]->node,tokenToNodeVector[j]->node->class_name().c_str());
                         printf ("   --- tokenToNodeVector[j=%zu]->leading_whitespace_start  = %d end = %d \n",j,tokenToNodeVector[j]->leading_whitespace_start,tokenToNodeVector[j]->leading_whitespace_end);
                         printf ("   --- tokenToNodeVector[j=%zu]->token_subsequence_start   = %d end = %d \n",j,tokenToNodeVector[j]->token_subsequence_start,tokenToNodeVector[j]->token_subsequence_end);
                         printf ("   --- tokenToNodeVector[j=%zu]->trailing_whitespace_start = %d end = %d \n",j,tokenToNodeVector[j]->trailing_whitespace_start,tokenToNodeVector[j]->trailing_whitespace_end);
                         printf ("   --- original_end_of_token_subsequence = %d \n",original_end_of_token_subsequence);
#endif

                      // DQ (1/22/2015): Added realization that there are a few more single character statements than I realized previously (these can look like macro expansions).
                      // Note: there are a few statements that are a single character and can be tripped up by this test (startOfConstruct() == endOfConstruct()).
                      // Examples are: ";" and single character value expressions that are interpreted as SgExprStatement IR nodes (e.g. in "if(0)").

                      // SgVarRefExp* varRefExp = isSgVarRefExp(tokenToNodeVector[j]->node);
                      // if (varRefExp == NULL)
                         SgLocatedNode* tmp_locatedNode = isSgLocatedNode(tokenToNodeVector[j]->node);
                         bool processAsMacroExpansion = tmp_locatedNode != NULL && (*(tmp_locatedNode->get_startOfConstruct()) == *(tmp_locatedNode->get_endOfConstruct()));
#if DEBUG_MACRO_HANDLING
                         printf ("   --- processAsMacroExpansion = %s \n",processAsMacroExpansion ? "true" : "false");
#endif
                      // DQ (1/22/2015): We need to account for statements that have surrounding syntax, and can be a single character statement.
                      // SgIfStmt* ifStatement = isSgIfStmt(n);
                      // if (ifStatement != NULL && tokenToNodeVector[j]->node == ifStatement->get_conditional())
                      // DQ (12/26/2014): Adding support for fixing the leading white space of conditionals statements (in C++ most conditional expressions are actually statements).
                         SgWhileStmt*       whileStatement   = isSgWhileStmt(n);
                         SgDoWhileStmt*     doWhileStatement = isSgDoWhileStmt(n);
                         SgSwitchStatement* switchStatement  = isSgSwitchStatement(n);
                         SgIfStmt*          ifStatement      = isSgIfStmt(n);
                         SgForStatement*    forStatement     = isSgForStatement(n);
                         if (whileStatement != NULL || switchStatement != NULL || ifStatement != NULL || doWhileStatement != NULL || forStatement != NULL)
                            {
                              ROSE_ASSERT(tmp_locatedNode != NULL);
                              SgStatement* conditionStatement = isSgStatement(tmp_locatedNode);
                              if (conditionStatement != NULL)
                                 {
                                   if ( (whileStatement   != NULL && conditionStatement == whileStatement->get_condition()) ||
                                        (doWhileStatement != NULL && conditionStatement == doWhileStatement->get_condition()) ||
                                        (switchStatement  != NULL && conditionStatement == switchStatement->get_item_selector()) ||
                                        (forStatement     != NULL && conditionStatement == forStatement->get_test()) ||
                                        (ifStatement      != NULL && conditionStatement == ifStatement->get_conditional()) )
                                      {
#if 0
                                        printf ("$$$$$$$$$$$$ Handle special case of single character SgStatement (condition) nested in n = %p = %s \n",n,n->class_name().c_str());
#endif
                                     // Test inputmove*_test2015_74.C demonstrates this problem where the test in "if(0)" is a 1 token statement.
                                     // In this case we have to skip over the ")" as well or disqualify an attempt at a better evaluation.
                                        processAsMacroExpansion = false;
                                      }
                                 }
                            }

                         if (processAsMacroExpansion == true)
                            {
                           // Set the ending position to be either the next statement (if it exists) or the value of original_end_of_token_subsequence.
                              if (j < tokenToNodeVector.size()-1)
                                 {
                                // tokenToNodeVector[j]->token_subsequence_end = tokenToNodeVector[j+1]->token_subsequence_start - 1;
                                   if (tokenToNodeVector[j]->token_subsequence_start == tokenToNodeVector[j+1]->token_subsequence_start)
                                      {
                                     // This is the case of a shared toke subsequence for a macro (representing multiple statements).
                                     // Don't change the tokenToNodeVector[j]->token_subsequence_end.
                                      }
                                     else
                                      {
                                        tokenToNodeVector[j]->token_subsequence_end = tokenToNodeVector[j+1]->token_subsequence_start - 1;

                                     // DQ (1/13/2015): I think we can enforce this.
                                        ROSE_ASSERT(tokenToNodeVector[j]->token_subsequence_end >= tokenToNodeVector[j]->token_subsequence_start);
                                      }
                                 }
                                else
                                 {
                                // DQ (1/13/2015): Adjust this to not be at the same token as the end of the scope (should be whitespace, or CPP, or "else").
                                // tokenToNodeVector[j]->token_subsequence_end = original_end_of_token_subsequence;
                                   tokenToNodeVector[j]->token_subsequence_end = original_end_of_token_subsequence - 1;
                                 }
#if DEBUG_MACRO_HANDLING
                              printf ("Reset ending position: initial value: tokenToNodeVector[j=%zu]->token_subsequence_start = %d end = %d \n",
                                   j,tokenToNodeVector[j]->token_subsequence_start,tokenToNodeVector[j]->token_subsequence_end);
#endif
                              int better_end_of_token_subsequence = tokenToNodeVector[j]->token_subsequence_end;

#if DEBUG_MACRO_HANDLING
                              printf ("BEFORE WHILE: tokenToNodeVector[j]->token_subsequence_start = %d better_end_of_token_subsequence = %d \n",tokenToNodeVector[j]->token_subsequence_start,better_end_of_token_subsequence);
#endif
                           // DQ (1/3/2015): Allow for the "else" keyword to be skipped over in triming tokens from the start of the current statement.
                           // while ( better_end_of_token_subsequence > tokenToNodeVector[j]->token_subsequence_start && 
                           //         ( tokenStream[better_end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_ELSE ||
                           //           tokenStream[better_end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO ||
                           //           tokenStream[better_end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                           // while ( better_end_of_token_subsequence > tokenToNodeVector[j]->token_subsequence_start && 
                              while ( better_end_of_token_subsequence > tokenToNodeVector[j]->token_subsequence_start && 
                                      ( tokenStream[better_end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_ELSE ||
                                        tokenStream[better_end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_PREPROCESSING_INFO ||
                                        tokenStream[better_end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                 {
#if DEBUG_MACRO_HANDLING
                                   printf ("IN WHILE: tokenToNodeVector[j]->token_subsequence_start = %d better_end_of_token_subsequence = %d \n",tokenToNodeVector[j]->token_subsequence_start,better_end_of_token_subsequence);
#endif
                                   better_end_of_token_subsequence--;
                                 }

#if DEBUG_MACRO_HANDLING
                              printf ("AFTER WHILE: tokenToNodeVector[j]->token_subsequence_start = %d better_end_of_token_subsequence = %d \n",tokenToNodeVector[j]->token_subsequence_start,better_end_of_token_subsequence);
#endif
                              tokenToNodeVector[j]->token_subsequence_end = better_end_of_token_subsequence;
#if DEBUG_MACRO_HANDLING
                              printf ("Reset ending position: final value: tokenToNodeVector[j=%zu]->token_subsequence_start = %d end = %d \n",
                                   j,tokenToNodeVector[j]->token_subsequence_start,tokenToNodeVector[j]->token_subsequence_end);
#endif
                            }
                           else
                            {
#if DEBUG_MACRO_HANDLING
                              printf ("Detected possible macro expansion: but it is just a SgVarRefExp = %p = %s \n",tmp_locatedNode,tmp_locatedNode->class_name().c_str());
#endif
                            }
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }

            // ROSE_ASSERT(firstChildWithTokenMapping <= lastChildWithTokenMapping);
#if 0
               printf ("tokenToNodeVector.size() = %" PRIuPTR " childrenWithoutTokenMappings.size() = %" PRIuPTR " \n",tokenToNodeVector.size(),childrenWithoutTokenMappings.size());
               printf ("   --- firstChildWithTokenMapping = %d \n",firstChildWithTokenMapping);
               printf ("   --- lastChildWithTokenMapping  = %d \n",lastChildWithTokenMapping);
#endif
               if (tokenToNodeVector.size() > 0)
                  {
                 // We have to process the elements of the tokenToNodeVector.

                 // There should be at least one child with token information.
                    ROSE_ASSERT(lastChildWithTokenMapping >= 0);
#if 0
                    printf ("In evaluateSynthesizedAttribute(): inheritedAttribute.processChildNodes = %s start_of_token_sequence = %d end_of_token_sequence = %d \n",
                            inheritedAttribute.processChildNodes ? "true" : "false",inheritedAttribute.start_of_token_sequence,inheritedAttribute.end_of_token_sequence);
#endif
                    ROSE_ASSERT(tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end());
                    int current_node_token_subsequence_start   = -1;
                    int current_node_token_subsequence_end     = -1;

                 // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
                 // int last_node_token_subsequence_start = -1; // current_node_token_subsequence_start;
                 // int last_node_token_subsequence_end   = -1; // current_node_token_subsequence_end;

                    if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                       {
                         TokenStreamSequenceToNodeMapping* current_node_mappingInfo = tokenStreamSequenceMap[n];
                         current_node_token_subsequence_start   = current_node_mappingInfo->token_subsequence_start;
                         current_node_token_subsequence_end     = current_node_mappingInfo->token_subsequence_end;
#if 0
                         printf ("CURRENT NODE: tokens: current_node_token_subsequence_start = %d current_node_token_subsequence_end = %d \n",current_node_token_subsequence_start,current_node_token_subsequence_end);

                         printf ("   --- current node: token string = -->|");
                         for (int i = current_node_token_subsequence_start; i <= current_node_token_subsequence_end; i++)
                            {
                              printf ("%s",tokenStream[i]->p_tok_elem->token_lexeme.c_str());
                            }
                         printf ("|<--\n");
#endif
                       }

#define DEBUG_LEADING_AND_TRAILING_WHITESPACE 0

                 // DQ (1/21/2015): Added to support to control resetting of previous_mapping trailing token sequence if we was explicitly specified to not be set in the previous iteration.
                 // bool previous_fixupDarkTokenSubsequencesForLeadingWhitespace  = false;
                    bool previous_fixupDarkTokenSubsequencesForTrailingWhitespace = false;

                    for (size_t i = 0; i < tokenToNodeVector.size(); i++)
                       {
#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                         printf ("In evaluateSynthesizedAttribute(): tokenToNodeVector[%" PRIuPTR "] = %p \n",i,tokenToNodeVector[i]);
#endif
                         TokenStreamSequenceToNodeMapping* mappingInfo = tokenToNodeVector[i];
#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                         printf ("   --- node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                         Sg_File_Info* start_pos = mappingInfo->node->get_startOfConstruct();
                         Sg_File_Info* end_pos   = mappingInfo->node->get_endOfConstruct();
                         printf ("   --- node = %p = %s: start (line=%d:column=%d) end(line=%d,column=%d) \n",mappingInfo->node,mappingInfo->node->class_name().c_str(),start_pos->get_physical_line(),start_pos->get_col(),end_pos->get_physical_line(),end_pos->get_col());
                         printf ("START MAPPING i=%d: mappingInfo->token_subsequence_start = %d mappingInfo->token_subsequence_end = %d \n",i,mappingInfo->token_subsequence_start,mappingInfo->token_subsequence_end);
#endif
                      // int leading_whitespace_start  = mappingInfo->leading_whitespace_start;
                      // int leading_whitespace_end    = mappingInfo->leading_whitespace_end;
                         int token_subsequence_start   = mappingInfo->token_subsequence_start;
                         int token_subsequence_end     = mappingInfo->token_subsequence_end;
                      // int trailing_whitespace_start = mappingInfo->trailing_whitespace_start;
                      // int trailing_whitespace_end   = mappingInfo->trailing_whitespace_end;
#if 0
                         printf ("   --- child node: token string = -->|");
                         for (int j = token_subsequence_start; j <= token_subsequence_end; j++)
                            {
                              printf ("%s",tokenStream[j]->p_tok_elem->token_lexeme.c_str());
                            }
                         printf ("|<--\n");
#endif

#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                      // printf ("   --- TOKENS: leading_whitespace tokens (%d,%d) token_subsequence (%d,%d) trailing_whitespace tokens (%d,%d) \n",
                      //    leading_whitespace_start,leading_whitespace_end,token_subsequence_start,token_subsequence_end,trailing_whitespace_start,trailing_whitespace_end);
                         printf ("   --- TOKENS: leading_whitespace tokens (N/A,N/A) token_subsequence (%d,%d) trailing_whitespace tokens (N/A,N/A) \n",token_subsequence_start,token_subsequence_end);
#endif
#if 0
                         if (leading_whitespace_start < 0 || leading_whitespace_start > leading_whitespace_end)
                            {
                           // This is a case to update (fix).
                              printf ("Error: leading_whitespace_start < 0 || leading_whitespace_start > leading_whitespace_end \n");
                            }

                      // This should always be true.
                         ROSE_ASSERT(token_subsequence_start <= token_subsequence_end);

                         if (trailing_whitespace_start < 0 || trailing_whitespace_start > trailing_whitespace_end)
                            {
                           // This is a case to update (fix).
                              printf ("Error: trailing_whitespace_start < 0 || trailing_whitespace_start > trailing_whitespace_end \n");
                            }
#endif
                      // We can't enforce this if we abandon the computation of leading whitespace in the 
                      // evaluation of the inherited attributes (on the way down in the traversal of the AST).
                      // ROSE_ASSERT(leading_whitespace_start >= 0);
                      // ROSE_ASSERT(leading_whitespace_end   >= 0);

                         ROSE_ASSERT(token_subsequence_start >= 0);
                         ROSE_ASSERT(token_subsequence_end   >= 0);

                      // ROSE_ASSERT(trailing_whitespace_start >= 0);
                      // ROSE_ASSERT(trailing_whitespace_end   >= 0);
#if 0
                         printf ("   --- leading_whitespace tokens (%d:%d,%d:%d) token_subsequence (%d:%d,%d:%d) trailing_whitespace tokens (%d:%d,%d:%d) \n",
                                 leading_whitespace_start  < 0 ? -1 : tokenStream[leading_whitespace_start]->beginning_fpi.line_num, 
                                 leading_whitespace_start  < 0 ? -1 : tokenStream[leading_whitespace_start]->beginning_fpi.column_num,
                                 leading_whitespace_end    < 0 ? -1 : tokenStream[leading_whitespace_end]->ending_fpi.line_num, 
                                 leading_whitespace_end    < 0 ? -1 : tokenStream[leading_whitespace_end]->ending_fpi.column_num,
                                 token_subsequence_start   < 0 ? -1 : tokenStream[token_subsequence_start]->beginning_fpi.line_num, 
                                 token_subsequence_start   < 0 ? -1 : tokenStream[token_subsequence_start]->beginning_fpi.column_num,
                                 token_subsequence_end     < 0 ? -1 : tokenStream[token_subsequence_end]->ending_fpi.line_num, 
                                 token_subsequence_end     < 0 ? -1 : tokenStream[token_subsequence_end]->ending_fpi.column_num,
                                 trailing_whitespace_start < 0 ? -1 : tokenStream[trailing_whitespace_start]->beginning_fpi.line_num, 
                                 trailing_whitespace_start < 0 ? -1 : tokenStream[trailing_whitespace_start]->beginning_fpi.column_num,
                                 trailing_whitespace_end   < 0 ? -1 : tokenStream[trailing_whitespace_end]->ending_fpi.line_num,
                                 trailing_whitespace_end   < 0 ? -1 : tokenStream[trailing_whitespace_end]->ending_fpi.column_num);
#endif
#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                         printf ("   --- token_subsequence (%d:%d,%d:%d) \n",
                                 token_subsequence_start   < 0 ? -1 : tokenStream[token_subsequence_start]->beginning_fpi.line_num, 
                                 token_subsequence_start   < 0 ? -1 : tokenStream[token_subsequence_start]->beginning_fpi.column_num,
                                 token_subsequence_end     < 0 ? -1 : tokenStream[token_subsequence_end]->ending_fpi.line_num, 
                                 token_subsequence_end     < 0 ? -1 : tokenStream[token_subsequence_end]->ending_fpi.column_num);
#endif
                      // Handle the left edge of the AST subtree: modify the edged (leading whitespace).

                         ROSE_ASSERT(firstChildWithTokenMapping <= lastChildWithTokenMapping);
#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                         printf ("firstChildWithTokenMapping = %d i = %zu \n",firstChildWithTokenMapping,i);
#endif
#if DEBUG_LEADING_AND_TRAILING_WHITESPACE
                      // DQ (1/3/2014): We need to set initial values for the left and right edges.  This is critical for macros that are
                      // expanded to be statements without accurate source position endings (though the starting source position is available).
                         printf ("##### current_node_token_subsequence_start = %d \n",current_node_token_subsequence_start);
                         printf ("   --- is left edge  = %s \n",(i == 0) ? "true" : "false");
                         printf ("   --- is right edge = %s \n",(i == tokenToNodeVector.size()-1) ? "true" : "false");
#endif

#if 0
                      // DQ (1/27/2015): Debugging case of test2015_110.C (prefix operator++() used in for loop increment expression).
                         if (isSgForStatement(n) != NULL && mappingInfo->node == isSgForStatement(n)->get_increment() )
                            {
                              printf ("Exiting as a test! (detected for loop increment expression) \n");
                              ROSE_ASSERT(false);
                            }
#endif

#if 1
                      // DQ (1/2/2015): New more general mechanism, we now want to uniformally make sure that the leading and trailing whitespace does not include syntax.
                         trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
                         trimTrailingWhiteSpaceFromRight(mappingInfo,original_end_of_token_subsequence);

                         ROSE_ASSERT(mappingInfo->leading_whitespace_start  <= mappingInfo->leading_whitespace_end);
                         ROSE_ASSERT(mappingInfo->trailing_whitespace_start <= mappingInfo->trailing_whitespace_end);
                         ROSE_ASSERT(mappingInfo->else_whitespace_start     <= mappingInfo->else_whitespace_end);

                      // DQ (1/11/2014): Record the dark tokens so that we can add them the the white space range.
                      // Dark tokens are defines as those tokens between the previous mappings trailing whitespace 
                      // end and the token sequence start for the current IR node.

#define DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE 0
#define DEBUG_DARK_TOKEN_FIXUP 0

                         ROSE_ASSERT(mappingInfo->node != NULL);

                      // DQ (1/17/2015): I think the logic here is backward!
                      // bool fixupDarkTokenSubsequences = false;
                         bool fixupDarkTokenSubsequencesForLeadingWhitespace  = true;
                         bool fixupDarkTokenSubsequencesForTrailingWhitespace = true;
#if 0
                         fixupDarkTokenSubsequencesForLeadingWhitespace = false;
#endif

                      // This is required to support the dark token sequence support for trailing white space.
                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
                         if (functionDeclaration != NULL && mappingInfo->node == functionDeclaration->get_definition() )
                            {
                              fixupDarkTokenSubsequencesForLeadingWhitespace  = false;
                            }

                         SgSwitchStatement* switchStatement  = isSgSwitchStatement(n);
                         SgWhileStmt*       whileStatement   = isSgWhileStmt(n);
                         SgForStatement*    forStatement     = isSgForStatement(n);
                         SgIfStmt*          ifStatement      = isSgIfStmt(n);
                         SgDoWhileStmt*     doWhileStatement = isSgDoWhileStmt(n);

                         if (forStatement != NULL && mappingInfo->node == forStatement->get_loop_body() )
                            {
                           // fixupDarkTokenSubsequences = true;
                              fixupDarkTokenSubsequencesForTrailingWhitespace = false;
                            }

                      // DQ (1/20/2015): Adding support for the trailing whitespace of the "true" branch of an "if" statement.
                         if (ifStatement != NULL && (mappingInfo->node == ifStatement->get_true_body()) )
                            {
                              fixupDarkTokenSubsequencesForTrailingWhitespace  = false;
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }

                      // This is required to support the dark token sequence support for leading white space.
                      // The problem here is that these statements have syntax that would have to be identified 
                      // so that we would not inlcude it in the leading whitespace.  For now it would be simpler 
                      // to avoid processing these cases, however it could be a problem if a dark token subsequence 
                      // were embedded just right.
                      // if (ifStatement != NULL && mappingInfo->node == ifStatement->get_true_body() )
                         if (ifStatement != NULL && (mappingInfo->node == ifStatement->get_true_body() || mappingInfo->node == ifStatement->get_false_body()) )
                            {
                              fixupDarkTokenSubsequencesForLeadingWhitespace  = false;
                            }

                      // if (forStatement != NULL && mappingInfo->node == forStatement->get_for_init_stmt() )
                         if ( (forStatement     != NULL && mappingInfo->node == forStatement->get_for_init_stmt()) ||
                              (whileStatement   != NULL && mappingInfo->node == whileStatement->get_condition()) ||
                              (switchStatement  != NULL && mappingInfo->node == switchStatement->get_item_selector()) ||
                              (doWhileStatement != NULL && mappingInfo->node == doWhileStatement->get_condition()) )
                            {
                              fixupDarkTokenSubsequencesForLeadingWhitespace = false;
                            }
#if 1
                      // DQ (1/25/2015): Test disabling this for symetry with SgIfStmt.
                      // DQ (1/21/2015): Turn this off to account for syntax between the condition and the body of a while statement.
                      // This might be required for SgIfStmt and other compound statements as well.
                      // if ( (whileStatement != NULL && mappingInfo->node == whileStatement->get_condition()) )
                         if ( (whileStatement   != NULL && mappingInfo->node == whileStatement->get_condition()) ||
                              (switchStatement  != NULL && mappingInfo->node == switchStatement->get_item_selector()) ||
                              (doWhileStatement != NULL && mappingInfo->node == doWhileStatement->get_condition()) )
                            {
                              fixupDarkTokenSubsequencesForTrailingWhitespace = false;
                            }
#endif
                      // DQ (1/22/2015): Turn off processing of the dark tokens in a SgCaseOptionStmt until we can eliminate the 
                      // compiler generated SgBasicBlock used as the body.  Note that we are currenty forcing the generation of 
                      // the body from the AST since there is no mapping from the token stream to the compiler-generated basic 
                      // block body (if it is compiler generated).
                         if ( isSgCaseOptionStmt(mappingInfo->node) != NULL )
                            {
#if DEBUG_DARK_TOKEN_FIXUP
                              printf ("disable dark token processing for the trailing while space of a SgCaseOptionStmt = %p \n",mappingInfo->node);
#endif
                              fixupDarkTokenSubsequencesForTrailingWhitespace = false;

                           // DQ (1/22/2015): test2015_93.C demonstrates that we need to also turn off the processing of the leading white space as well.
                              fixupDarkTokenSubsequencesForLeadingWhitespace  = false;
                            }

                      // These statements have syntax that seperate the main construct from the construct's associated body (namely the ")" closing parenthesis).
                         if ( (switchStatement != NULL && mappingInfo->node == switchStatement->get_body()) ||
                              (whileStatement  != NULL && mappingInfo->node == whileStatement->get_body())  ||
                              (forStatement    != NULL && mappingInfo->node == forStatement->get_loop_body()) )
                            {
                              fixupDarkTokenSubsequencesForLeadingWhitespace  = false;
                            }

#if DEBUG_DARK_TOKEN_FIXUP
                         printf ("fixupDarkTokenSubsequencesForTrailingWhitespace = %s n = %p = %s mappingInfo->node = %p = %s \n",fixupDarkTokenSubsequencesForTrailingWhitespace ? "true" : "false",n,n->class_name().c_str(),mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                         printf ("fixupDarkTokenSubsequencesForLeadingWhitespace  = %s n = %p = %s mappingInfo->node = %p = %s \n",fixupDarkTokenSubsequencesForLeadingWhitespace ? "true" : "false",n,n->class_name().c_str(),mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif

                         if (fixupDarkTokenSubsequencesForLeadingWhitespace == true)
                            {
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                              printf ("fixupDarkTokenSubsequencesForLeadingWhitespace == true: i = %zu tokenToNodeVector.size() = %zu \n",i,tokenToNodeVector.size());
#endif
                           // DQ (1/17/2015): Fixup the leading white space subsequence to include the dark token subsequences.
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                              printf ("mappingInfo->leading_whitespace_start  = %d end = %d \n",mappingInfo->leading_whitespace_start,mappingInfo->leading_whitespace_end);
                              printf ("mappingInfo->token_subsequence_start   = %d end = %d \n",mappingInfo->token_subsequence_start,mappingInfo->token_subsequence_end);
                              printf ("mappingInfo->trailing_whitespace_start = %d end = %d \n",mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
#endif
                              if (i == 0)
                                 {
                                // DQ (1/17/2015): Adding support for tests/nonsmoke/functional/roseTests/astInterface/*_test2015_47.C
                                   TokenStreamSequenceToNodeMapping* previous_mappingInfo = NULL;
                                   if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                                      {
                                        previous_mappingInfo = tokenStreamSequenceMap[n];
                                      }

                                   if (previous_mappingInfo != NULL)
                                      {
                                        int previous_mappingInfo_leading_whitespace_end = previous_mappingInfo->token_subsequence_start + 1;
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                                        int current_mappingInfo_leading_whitespace_start = mappingInfo->trailing_whitespace_end;
                                        printf ("   --- previous_mappingInfo_leading_whitespace_end = %d \n",previous_mappingInfo_leading_whitespace_end);
                                        printf ("   --- current_mappingInfo_leading_whitespace_start = %d \n",current_mappingInfo_leading_whitespace_start);
#endif
                                        ROSE_ASSERT(previous_mappingInfo->token_subsequence_start >= 0);

                                     // if (mappingInfo->leading_whitespace_start != -1)
                                        if (mappingInfo->leading_whitespace_start != -1 && mappingInfo->leading_whitespace_start > previous_mappingInfo_leading_whitespace_end)
                                           {
                                             mappingInfo->leading_whitespace_start = previous_mappingInfo_leading_whitespace_end;
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                                             printf ("   --- reset mappingInfo->leading_whitespace_start to %d \n",previous_mappingInfo_leading_whitespace_end);
#endif
                                           }
                                      }
#if 0
                                   if (isSgBasicBlock(n) != NULL)
                                      {
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
                                      }
#endif
                                 }
                                else
                                 {
                                   TokenStreamSequenceToNodeMapping* previous_mappingInfo = tokenToNodeVector[i-1];
                                   ROSE_ASSERT(previous_mappingInfo != NULL);

                                // DQ (1/28/2015): Added assertion.
                                   int temp_i = i;
                                   if (previous_mappingInfo->node == mappingInfo->node)
                                      {
                                     // This is likely a shared token sequence and we need to go back one more.
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE || 0
                                        printf ("WARNING: (in leading whitespace computation): previous_mappingInfo->node == mappingInfo->node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
                                        printf ("   --- This is likely a shared token sequence and we need to go back one more to define the previous_mappingInfo = %p node = %p \n",previous_mappingInfo,previous_mappingInfo->node);
#endif
                                        while (temp_i >= 1 && previous_mappingInfo->node == mappingInfo->node)
                                           {
                                             previous_mappingInfo = tokenToNodeVector[temp_i-1];
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE || 0
                                             printf ("In loop looking for different node: temp_i = %d previous_mappingInfo = %p node = %p = %s \n",
                                                  temp_i,previous_mappingInfo,previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str());
#endif
                                             temp_i--;

                                             ROSE_ASSERT(previous_mappingInfo != NULL);
                                           }
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE || 0
                                        printf ("   --- temp_i = %d \n",temp_i);
#endif
                                        if (temp_i == 0 && previous_mappingInfo->node == mappingInfo->node)
                                           {
#if 0
                                             printf ("WARNING: (in leading whitespace computation): Need logic to look at the outer scope! \n");
                                          // ROSE_ASSERT(false);
#endif
                                           }
                                      }

                                // DQ (1/28/2015): Added assertion.
                                   if (previous_mappingInfo->node == mappingInfo->node)
                                      {
#if 0
                                        printf ("WARNING: previous_mappingInfo->node == mappingInfo->node: I think this should not happen! i = %d temp_i = %d \n",i,temp_i);
#endif
                                      }
                                // ROSE_ASSERT(previous_mappingInfo->node != mappingInfo->node);

                                   if (previous_mappingInfo != NULL)
                                      {
                                     // int previous_mappingInfo_leading_whitespace_end = previous_mappingInfo->token_subsequence_start + 1;
                                        int previous_mappingInfo_leading_whitespace_end = previous_mappingInfo->token_subsequence_end + 1;
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                                        int current_mappingInfo_leading_whitespace_start = mappingInfo->trailing_whitespace_end;
                                        printf ("   --- previous_mappingInfo_leading_whitespace_end = %d \n",previous_mappingInfo_leading_whitespace_end);
                                        printf ("   --- current_mappingInfo_leading_whitespace_start = %d \n",current_mappingInfo_leading_whitespace_start);
#endif
                                        ROSE_ASSERT(previous_mappingInfo->token_subsequence_start >= 0);

                                     // if (mappingInfo->leading_whitespace_start != -1)
                                        if (mappingInfo->leading_whitespace_start != -1 && mappingInfo->leading_whitespace_start > previous_mappingInfo_leading_whitespace_end)
                                           {
                                             mappingInfo->leading_whitespace_start = previous_mappingInfo_leading_whitespace_end;
#if DEBUG_DARK_TOKEN_FIXUP_FOR_LEADING_WHITESPACE
                                             printf ("   --- reset mappingInfo->leading_whitespace_start to %d \n",previous_mappingInfo_leading_whitespace_end);
#endif
                                          }
                                     }
                                 }
                            }

                      // if (fixupDarkTokenSubsequences == false)
                         if (fixupDarkTokenSubsequencesForTrailingWhitespace == true)
                            {
                           // Fixup the trailing white space subsequence to include the dark token subsequences.
                              if (i == 0)
                           // if (i == tokenToNodeVector.size()-1)
                                 {
                                // Dark tokens are defined for the leading token sequence, but we want to work out and debug 
                                // the case of the trailing dark token sequence first.
                                   if (tokenToNodeVector.size() == 1)
                                      {
                                        TokenStreamSequenceToNodeMapping* previous_mappingInfo = NULL;
                                        if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                                           {
                                             previous_mappingInfo = tokenStreamSequenceMap[n];
                                           }

                                        if (previous_mappingInfo != NULL)
                                           {
                                          // In this case their is only a single child so the first child has a trailing token sequence to fixup.
#if DEBUG_DARK_TOKEN_FIXUP
                                          // printf ("Dark tokens fixup: i == 0: Fixup the trailing token sequence for the singleton child token sequence \n");
                                             printf ("Dark tokens fixup: i == tokenToNodeVector.size()-1: i = %zu \n",i);
                                             printf ("   --- previous node = %p = %s \n",previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str());
                                             printf ("   --- node          = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
                                          // int previous_mappingInfo_trailing_whitespace_end = previous_mappingInfo->trailing_whitespace_end;
                                          // int current_mappingInfo_leading_whitespace_start = mappingInfo->leading_whitespace_start;
                                             int previous_mappingInfo_trailing_whitespace_end = previous_mappingInfo->token_subsequence_end;
                                             int current_mappingInfo_leading_whitespace_start = mappingInfo->trailing_whitespace_end;
#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("   --- previous_mappingInfo_trailing_whitespace_end = %d \n",previous_mappingInfo_trailing_whitespace_end);
                                             printf ("   --- current_mappingInfo_leading_whitespace_start = %d \n",current_mappingInfo_leading_whitespace_start);
#endif
                                             ROSE_ASSERT(previous_mappingInfo_trailing_whitespace_end >= 0);
                                          // ROSE_ASSERT(current_mappingInfo_leading_whitespace_start >= 0);
                                             if (current_mappingInfo_leading_whitespace_start < 0)
                                                {
#if DEBUG_DARK_TOKEN_FIXUP
                                                  printf ("Skip this case where current_mappingInfo_leading_whitespace_start < 0 \n");
#endif
                                                }
                                               else
                                                {
                                               // if (current_mappingInfo_leading_whitespace_start + 1 > previous_mappingInfo_trailing_whitespace_end)
                                                  if (current_mappingInfo_leading_whitespace_start + 1 < previous_mappingInfo_trailing_whitespace_end)
                                                     {
#if DEBUG_DARK_TOKEN_FIXUP
                                                       printf ("Dark tokens fixup: Reset test 1 the mappingInfo->trailing_whitespace_end from %d to %d \n",mappingInfo->trailing_whitespace_end,previous_mappingInfo_trailing_whitespace_end);
#endif
                                                    // DQ (1/23/2015): inputmove*_test2015_84.C demonstrates that this must be previous_mappingInfo_trailing_whitespace_end - 1.
                                                    // mappingInfo->trailing_whitespace_end = previous_mappingInfo_trailing_whitespace_end - 1;
                                                    // mappingInfo->trailing_whitespace_end = previous_mappingInfo_trailing_whitespace_end;
                                                       mappingInfo->trailing_whitespace_end = previous_mappingInfo_trailing_whitespace_end - 1;
#if 0
                                                       printf ("Exiting as a test! \n");
                                                       ROSE_ASSERT(false);
#endif
                                                     }
                                                }
                                           }
                                      }
                                     else
                                      {
                                     // This is the case of a first child which other children so here we would fixup the leading token 
                                     // sequence to match the trail token sequence of the previous IR node.
                                      }
                                 }
                                else
                                 {
                                // This branch can be refactored with the previous branch.
                                // This is the first case to fixup.
                                   TokenStreamSequenceToNodeMapping* previous_mappingInfo = tokenToNodeVector[i-1];
                                   ROSE_ASSERT(previous_mappingInfo != NULL);

                                // DQ (1/28/2015): Added assertion.
                                   int temp_i = i;
                                   if (previous_mappingInfo->node == mappingInfo->node)
                                      {
                                     // This is likely a shared token sequence and we need to go back one more.
#if DEBUG_DARK_TOKEN_FIXUP
                                        printf ("WARNING: (in trailing whitespace computation): previous_mappingInfo->node == mappingInfo->node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
                                        printf ("   --- This is likely a shared token sequence and we need to go back one more to define the previous_mappingInfo = %p node = %p \n",previous_mappingInfo,previous_mappingInfo->node);
#endif
                                        while (temp_i >= 1 && previous_mappingInfo->node == mappingInfo->node)
                                           {
                                             previous_mappingInfo = tokenToNodeVector[temp_i-1];
#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("In loop looking for different node: temp_i = %d previous_mappingInfo = %p node = %p \n",temp_i,previous_mappingInfo,previous_mappingInfo->node);
#endif
                                             temp_i--;

                                             ROSE_ASSERT(previous_mappingInfo != NULL);
                                           }
#if DEBUG_DARK_TOKEN_FIXUP
                                        printf ("   --- temp_i = %d \n",temp_i);
#endif
                                        if (temp_i == 0 && previous_mappingInfo->node == mappingInfo->node)
                                           {
#if 0
                                             printf ("WARNING: (in trailing whitespace computation): Need logic to look at the outer scope! \n");
                                          // ROSE_ASSERT(false);
#endif
                                           }
                                      }

                                   ROSE_ASSERT(previous_mappingInfo != NULL);
#if DEBUG_DARK_TOKEN_FIXUP
                                   printf ("Dark tokens fixup: i != 0: i = %d \n",i);
                                   printf ("   --- previous node = %p = %s \n",previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str());
                                   printf ("   --- node          = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
                                   printf ("   --- previous_mappingInfo = %p mappingInfo = %p \n",previous_mappingInfo,mappingInfo);
#endif
                                // DQ (1/28/2015): Added assertion.
                                   if (previous_mappingInfo->node == mappingInfo->node)
                                      {
#if 0
                                        printf ("WARNING: previous_mappingInfo->node == mappingInfo->node: I think this should not happen! i = %d temp_i = %d \n",i,temp_i);
#endif
                                      }
                                // ROSE_ASSERT(previous_mappingInfo->node != mappingInfo->node);

                                // int previous_mappingInfo_trailing_whitespace_end = previous_mappingInfo->trailing_whitespace_end;
                                   int previous_mappingInfo_trailing_whitespace_end = previous_mappingInfo->trailing_whitespace_end;
                                // int current_mappingInfo_leading_whitespace_start = mappingInfo->leading_whitespace_start;
                                // int current_mappingInfo_leading_whitespace_start = mappingInfo->trailing_whitespace_end;
                                   int current_mappingInfo_token_subsequence_start  = mappingInfo->token_subsequence_start;
#if DEBUG_DARK_TOKEN_FIXUP
                                   printf ("   --- (preliminary values): previous_mappingInfo_trailing_whitespace_end = %d \n",previous_mappingInfo_trailing_whitespace_end);
                                   printf ("   --- (preliminary values): current_mappingInfo_token_subsequence_start  = %d \n",current_mappingInfo_token_subsequence_start);
#endif
                                // DQ (1/20/2015): Adding support to compute the trailing whitespace of a true block of a SgIfStmt (accounting for the "else" syntax).
                                   if (ifStatement != NULL)
                                      {
                                        if (previous_mappingInfo->node == ifStatement->get_true_body() && mappingInfo->node == ifStatement->get_false_body())
                                           {
                                          // Reset this to a value that will skip the reset of the previous_mappingInfo->trailing_whitespace_end below.
                                          // current_mappingInfo_token_subsequence_start = mappingInfo->else_whitespace_start;
                                          // current_mappingInfo_token_subsequence_start = previous_mappingInfo_trailing_whitespace_end;
                                             current_mappingInfo_token_subsequence_start = -1;

#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("Need to account for the else between the true and false statements of a SgIfStmt node (reset current_mappingInfo_token_subsequence_start = %d) \n",current_mappingInfo_token_subsequence_start);
#endif
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }

                                // Check for there being now token between the end of the previous mapping the the start of the current
                                // mapping (if so then there can be no dark subsequence of tokens), because there are no tokens.
                                // This is relevant to test2015_92.C
                                   if (previous_mappingInfo->token_subsequence_end + 1 == mappingInfo->token_subsequence_start)
                                      {
#if DEBUG_DARK_TOKEN_FIXUP
                                        printf ("No dark subsequence of tokens is possible: set current_mappingInfo_token_subsequence_start = -1 so that we skip dark token subsequence processing \n");
#endif
                                        current_mappingInfo_token_subsequence_start = -1;
                                      }

#if DEBUG_DARK_TOKEN_FIXUP
                                   printf ("   --- (final values): previous_mappingInfo_trailing_whitespace_end = %d \n",previous_mappingInfo_trailing_whitespace_end);
                                   printf ("   --- (final values): current_mappingInfo_token_subsequence_start  = %d \n",current_mappingInfo_token_subsequence_start);
#endif
                                // DQ (1/22/2015): Since we set the previous_mappingInfo->trailing_whitespace_end to current_mappingInfo_token_subsequence_start, 
                                // that is the value we should test (and skip if less then zero).
                                // if (current_mappingInfo_leading_whitespace_start < 0)
                                // if (previous_mappingInfo_trailing_whitespace_end < 0)
                                   if (current_mappingInfo_token_subsequence_start < 0)
                                      {
#if DEBUG_DARK_TOKEN_FIXUP
                                        printf ("Skip this case where previous_mappingInfo_trailing_whitespace_end < 0 \n");
#endif
                                      }
                                     else
                                      {
                                     // Test for dark token subsequece
                                        if (previous_mappingInfo_trailing_whitespace_end >= 0)
                                           {
                                             if (current_mappingInfo_token_subsequence_start > previous_mappingInfo_trailing_whitespace_end + 1)
                                                {
#if DEBUG_DARK_TOKEN_FIXUP
                                               // printf ("Found valid dark token sequence (%d -> %d) \n",current_mappingInfo_token_subsequence_start,previous_mappingInfo_trailing_whitespace_end);
                                                  printf ("Found valid dark token sequence (%d -> %d) \n",previous_mappingInfo_trailing_whitespace_end,current_mappingInfo_token_subsequence_start);
#endif
                                                }
                                           }
                                          else
                                           {
#if 0
                                             printf ("previous_mappingInfo_trailing_whitespace_end not setup! \n");
#endif
                                           }

                                     // if (current_mappingInfo_leading_whitespace_start + 1 > previous_mappingInfo_trailing_whitespace_end)
                                        if (current_mappingInfo_token_subsequence_start > previous_mappingInfo_trailing_whitespace_end + 1)
                                           {
#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("Dark tokens fixup: Reset the previous_mappingInfo->trailing_whitespace_end from %d to %d \n",mappingInfo->trailing_whitespace_end,previous_mappingInfo_trailing_whitespace_end);
#endif
                                          // If the previous value of fixupDarkTokenSubsequencesForTrailingWhitespace was false then skip resetting the previous_mappingInfo->trailing_whitespace_end.
                                          // previous_mappingInfo->trailing_whitespace_end = current_mappingInfo_token_subsequence_start;
#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("previous_fixupDarkTokenSubsequencesForTrailingWhitespace = %s \n",previous_fixupDarkTokenSubsequencesForTrailingWhitespace ? "true " : "false");
#endif
                                             if (previous_fixupDarkTokenSubsequencesForTrailingWhitespace == true)
                                                {
                                               // mappingInfo->trailing_whitespace_end = previous_mappingInfo_trailing_whitespace_end - 1;
                                               // mappingInfo->trailing_whitespace_end = previous_mappingInfo_trailing_whitespace_end;

                                               // DQ (1/22/2015): This code is demonstrated in inputmove*_test2015_91.C
                                               // previous_mappingInfo->trailing_whitespace_end = current_mappingInfo_token_subsequence_start;
                                                  previous_mappingInfo->trailing_whitespace_end = current_mappingInfo_token_subsequence_start - 1;
                                                  if (previous_mappingInfo->trailing_whitespace_start < 0)
                                                     {
#if DEBUG_DARK_TOKEN_FIXUP
                                                       printf ("Since previous_mappingInfo->trailing_whitespace_start was not set, reset it to previous_mappingInfo->token_subsequence_end + 1 = %d \n",previous_mappingInfo->token_subsequence_end+1);
#endif
                                                       ROSE_ASSERT(previous_mappingInfo->token_subsequence_end > 0);
                                                       previous_mappingInfo->trailing_whitespace_start = previous_mappingInfo->token_subsequence_end + 1;
                                                     }

                                               // DQ (1/22/2015): Since the leading whitespace for the current mappingInfo was not set it to that of the previous_mappingInfo.
                                                  if (mappingInfo->leading_whitespace_start != previous_mappingInfo->trailing_whitespace_start && mappingInfo->leading_whitespace_end != previous_mappingInfo->trailing_whitespace_end)
                                                     {
#if DEBUG_DARK_TOKEN_FIXUP
                                                       printf ("Set the leading whitespace for mappingInfo to the trailing whitespace of the previous_mappingInfo \n");
#endif
                                                       mappingInfo->leading_whitespace_start = previous_mappingInfo->trailing_whitespace_start;
                                                       mappingInfo->leading_whitespace_end   = previous_mappingInfo->trailing_whitespace_end;
                                                     }
                                                }
                                               else
                                                {
#if DEBUG_DARK_TOKEN_FIXUP
                                                  printf ("Skipped reset of previous_mappingInfo->trailing_whitespace_end = %d \n",previous_mappingInfo->trailing_whitespace_end);
#endif
                                                }
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }

                                // DQ (1/22/2015): I think this might be at the wrong level of nesting (should be outside of the i != 0 branch).
                                // printf ("NOTE: Possible wrong level of nesting: I think this might be at the wrong level of nesting (should be outside of the i != 0 branch) \n");

                                // This is the same case as i == 0.
                                // Handle the trailing space.
                                   if (i == tokenToNodeVector.size()-1)
                                      {
                                        TokenStreamSequenceToNodeMapping* previous_mappingInfo = NULL;
                                        if (tokenStreamSequenceMap.find(n) != tokenStreamSequenceMap.end())
                                           {
                                             previous_mappingInfo = tokenStreamSequenceMap[n];
                                           }

                                        ROSE_ASSERT(previous_mappingInfo != NULL);
                                        if (previous_mappingInfo != NULL)
                                           {
#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("Dark tokens fixup: i == tokenToNodeVector.size()-1: i = %d \n",i);
                                             printf ("   --- previous node = %p = %s \n",previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str());
                                             printf ("   --- node          = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
                                          // int previous_mappingInfo_trailing_whitespace_end = previous_mappingInfo->trailing_whitespace_end;
                                          // int current_mappingInfo_leading_whitespace_start = mappingInfo->trailing_whitespace_end;
                                             int previous_mappingInfo_token_subsequence_end   = previous_mappingInfo->token_subsequence_end;
                                             int current_mappingInfo_leading_whitespace_start = mappingInfo->trailing_whitespace_end;
#if DEBUG_DARK_TOKEN_FIXUP
                                             printf ("   --- previous_mappingInfo_token_subsequence_end = %d \n",previous_mappingInfo_token_subsequence_end);
                                             printf ("   --- current_mappingInfo_leading_whitespace_start = %d \n",current_mappingInfo_leading_whitespace_start);
#endif
#if DEBUG_DARK_TOKEN_FIXUP
                                          // printf ("Dark tokens fixup: Reset test 2 the mappingInfo->trailing_whitespace_end from %d to %d \n",mappingInfo->trailing_whitespace_end,previous_mappingInfo_token_subsequence_end);
#endif
                                             ROSE_ASSERT(previous_mappingInfo_token_subsequence_end >= 0);
                                          // ROSE_ASSERT(current_mappingInfo_leading_whitespace_start >= 0);
                                             if (current_mappingInfo_leading_whitespace_start < 0)
                                                {
#if DEBUG_DARK_TOKEN_FIXUP
                                                  printf ("Skip this case where current_mappingInfo_leading_whitespace_start < 0 \n");
#endif
                                                }
                                               else
                                                {
                                               // if (current_mappingInfo_leading_whitespace_start + 1 > previous_mappingInfo_trailing_whitespace_end)
                                                  if (current_mappingInfo_leading_whitespace_start + 1 < previous_mappingInfo_token_subsequence_end)
                                                     {
                                                    // DQ (1/12/2015): If this is a node with syntax, then we want the token just before, else we want the value of previous_mappingInfo_token_subsequence_end
                                                    // int update_value = previous_mappingInfo_token_subsequence_end - 1;
                                                       int update_value = 0;
                                                    // if (isSgGlobal(mappingInfo->node) != NULL)
                                                       if (isSgGlobal(n) != NULL)
                                                          {
                                                         // The end of the global scope does not have syntax to mark it.
                                                            update_value = previous_mappingInfo_token_subsequence_end;
                                                          }
                                                         else
                                                          {
                                                            update_value = previous_mappingInfo_token_subsequence_end - 1;
                                                          }
#if DEBUG_DARK_TOKEN_FIXUP
                                                    // printf ("Dark tokens fixup: Reset test 2 the mappingInfo->trailing_whitespace_end from %d to %d \n",mappingInfo->trailing_whitespace_end,previous_mappingInfo_token_subsequence_end);
                                                       printf ("Dark tokens fixup: Reset test 2 the mappingInfo->trailing_whitespace_end from %d to %d \n",mappingInfo->trailing_whitespace_end,update_value);
#endif
                                                    // mappingInfo->trailing_whitespace_end = previous_mappingInfo_trailing_whitespace_end - 1;
                                                       mappingInfo->trailing_whitespace_end = update_value;
#if 0
                                                       printf ("Exiting as a test! \n");
                                                       ROSE_ASSERT(false);
#endif
                                                     }
                                                }
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }

                                 }
#if DEBUG_DARK_TOKEN_FIXUP
                         printf ("END MAPPING i=%d: mappingInfo->token_subsequence_start = %d mappingInfo->token_subsequence_end = %d \n",i,mappingInfo->token_subsequence_start,mappingInfo->token_subsequence_end);
#endif
                           // end of body for if fixupDarkTokenSubsequences == false
                            }

                      // DQ (1/21/2015): Added to support to control resetting of previous_mapping trailing token sequence if we was explicitly specified to not be set in the previous iteration.
                      // bool previous_fixupDarkTokenSubsequencesForLeadingWhitespace  = false;
                         previous_fixupDarkTokenSubsequencesForTrailingWhitespace = fixupDarkTokenSubsequencesForTrailingWhitespace;

                      // DQ (1/20/2015): Adding support for the trailing whitespace of the "true" branch of an "if" statement.
                         if (ifStatement != NULL && (mappingInfo->node == ifStatement->get_true_body()) )
                            {
#if 0
                              printf ("!!!!!!!!!!!!!!!!!!!!!!! DETECTED TRUE BLOCK OF IF STATEMENT !!!!!!!!!!!!!!!!!!!!!!! \n");
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }

#else
                      // DQ (1/2/2015): START OF COMMENTED OUT BLOCK: We want to make sure that leading a trailing white space does not include syntax.

#error "DEAD CODE!"

                      // if (i == firstChildWithTokenMapping)
                         if (i == 0)
                            {
                           // Use the current node's lower bound as the base of the leading whitespace subsequence.
                              mappingInfo->leading_whitespace_start = current_node_token_subsequence_start;
                              mappingInfo->leading_whitespace_end   = mappingInfo->token_subsequence_start - 1;
#if 0
                              printf ("   --- mappingInfo->leading_whitespace_start = %d \n",mappingInfo->leading_whitespace_start);
                              printf ("   --- mappingInfo->leading_whitespace_end   = %d \n",mappingInfo->leading_whitespace_end);
#endif
                           // I think that if this fails then both of these need to be set to the default value: -1.
                              if (mappingInfo->leading_whitespace_start > mappingInfo->leading_whitespace_end)
                                 {
#if 0
                                   printf ("Warning: i = %zu: mappingInfo->leading_whitespace_start=%d > mappingInfo->leading_whitespace_end=%d: (reset) \n",i,mappingInfo->leading_whitespace_start,mappingInfo->leading_whitespace_end);
#endif
                                   mappingInfo->leading_whitespace_start = -1;
                                   mappingInfo->leading_whitespace_end   = -1;
                                 }

                              ROSE_ASSERT(mappingInfo->leading_whitespace_start <= mappingInfo->leading_whitespace_end);
#if 0
                              printf ("Note: i = %zu: mappingInfo->leading_whitespace_start=%d > mappingInfo->leading_whitespace_end=%d: (reset) \n",i,mappingInfo->leading_whitespace_start,mappingInfo->leading_whitespace_end);
#endif
                           // DQ (12/1/2013): Added support for the SgClassDefinition, not that the input range is adjusted in the evaluation of the inherited 
                           // attribute so that the case of a SgClassDefinition matches that of the SgBasicBlock (and any other "{" and "}" pairing).
                           // So this should apply to the SgEnumeclaration as well (though that has not internal scope, associated definition, as I recall).
                           // DQ (11/30/2013): Added to support trimming of the token stream for the first and last children to exclude the trailing "{" token.
                           // This likely also happens for the SgClassDefinition and maybe a few other IR nodes which inclose statements in "{" and "}".
                           // I don't think we have any choice bu to do token stream searching to resolve this problem unless we can do it unconditionally 
                           // based on existence of the SgBasicBlock and SgClassDefinition (which might be possible).
                           // if (isSgBasicBlock(n) != NULL)
                              if (isSgBasicBlock(n) != NULL || isSgClassDefinition(n) != NULL)
                                 {

                                // This is a search of the token stream that we would like to avoid.
                                // If this is the first statement of set of statements nested in a "{" and "}" then we have to trim the edges of the first first statement.
                                   if (mappingInfo->leading_whitespace_start != -1 && tokenStream[mappingInfo->leading_whitespace_start]->p_tok_elem->token_lexeme == "{")
                                      {
#if 0
                                        printf ("Adjust the mappingInfo->leading_whitespace_start to avoid the { token \n");
#endif
                                     // DQ (11/30/2013): This case is added to support test2012_166.c.
                                     // mappingInfo->leading_whitespace_start++;
                                        if (mappingInfo->leading_whitespace_start < mappingInfo->leading_whitespace_end)
                                           {
                                             mappingInfo->leading_whitespace_start++;
                                           }
                                          else
                                           {

                                          // This case is added to support test2012_166.c.
                                             printf ("WARNING: adjustment skipped because it would fails assertion: mappingInfo->leading_whitespace_start <= mappingInfo->leading_whitespace_end \n");
                                           }
#if 0
                                        printf ("   --- after adjustment: mappingInfo->leading_whitespace_start = %d \n",mappingInfo->leading_whitespace_start);
                                        printf ("   --- after adjustment: mappingInfo->leading_whitespace_end   = %d \n",mappingInfo->leading_whitespace_end);
#endif
#if 0
                                        n->get_file_info()->display("after adjustment: mappingInfo->leading_whitespace_start");
#endif
                                        ROSE_ASSERT(mappingInfo->leading_whitespace_start <= mappingInfo->leading_whitespace_end);
                                      }
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }



                           // DQ (12/15/2014): A SgFunctionDefinition is nested in a SgFunctionDefinition and we need to compute the 
                           // leading white space to NOT include the function header (return type, function name, and function parameters).
                           // This is only required for the leading white space.

                           // SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(n);
                           // if (functionDefinition != NULL)
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
                              if (functionDeclaration != NULL)
                                 {
                                   ROSE_ASSERT(mappingInfo->node != NULL);
                                   SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(mappingInfo->node);
                                   if (functionDefinition != NULL)
                                      {
#if 0
                                        printf ("$$$$$$$$$$$$ Handle special case of SgFunctionDefinition nested in SgFunctionDeclaration \n");
#endif
#if 0
                                        printf ("   --- (before adjustement) mappingInfo->leading_whitespace_start = %d \n",mappingInfo->leading_whitespace_start);
                                        printf ("   --- (before adjustement) mappingInfo->leading_whitespace_end   = %d \n",mappingInfo->leading_whitespace_end);
#endif
                                        trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
                                      }
                                 }


                           // DQ (12/15/2014): Adding support for fixing the leading white space of the SgForInitStatement.
                              SgForStatement* forStatement = isSgForStatement(n);
                              if (forStatement != NULL)
                                 {
                                   ROSE_ASSERT(mappingInfo->node != NULL);
                                   SgForInitStatement* forInitStatement = isSgForInitStatement(mappingInfo->node);
                                   if (forInitStatement != NULL)
                                      {
#if 0
                                        printf ("$$$$$$$$$$$$ Handle special case of SgForInitStatement nested in SgForStatement \n");
#endif
                                        trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
#if 0
                                   SgStatement* body = isSgStatement(mappingInfo->node);

                                   if (body != NULL && forStatement->get_loop_body() == body)
                                      {
#if 0
                                        printf ("$$$$$$$$$$$$ Handle special case of body nested in SgForStatement \n");
#endif
                                        trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
#endif
                                 }


                           // DQ (12/26/2014): Adding support for fixing the leading white space of conditionals statements (in C++ most conditional expressions are actually statements).
                              SgWhileStmt*       whileStatement  = isSgWhileStmt(n);
                              SgSwitchStatement* switchStatement = isSgSwitchStatement(n);
                              SgIfStmt*          ifStatement     = isSgIfStmt(n);
                              if (whileStatement != NULL || switchStatement != NULL || ifStatement != NULL)
                                 {
                                   ROSE_ASSERT(mappingInfo->node != NULL);
                                   SgStatement* conditionStatement = isSgStatement(mappingInfo->node);
                                   if (conditionStatement != NULL)
                                      {
                                        if ( (whileStatement  != NULL && conditionStatement == whileStatement->get_condition()) ||
                                             (switchStatement != NULL && conditionStatement == switchStatement->get_item_selector()) ||
                                             (ifStatement     != NULL && conditionStatement == ifStatement->get_conditional()) )
                                           {
#if 0
                                             printf ("$$$$$$$$$$$$ Handle special case of SgStatement (condition) nested in n = %p = %s \n",n,n->class_name().c_str());
#endif
                                             trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);

                                             trimTrailingWhiteSpaceFromRight(mappingInfo,original_end_of_token_subsequence);
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }
                                 }


                           // DQ (12/15/2014): Adding support for fixing the leading white space of the SgForInitStatement.
                              SgStatement* statement = isSgStatement(n);
                              if (statement != NULL)
                                 {
                                   ROSE_ASSERT(mappingInfo->node != NULL);
                                   SgBasicBlock* basicBlock = isSgBasicBlock(mappingInfo->node);
                                   if (basicBlock != NULL)
                                      {
#if 0
                                        printf ("$$$$$$$$$$$$ Handle special case of SgBasicBlock nested in SgStatement \n");
#endif
                                        trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }


                           // DQ (12/18/2014): Adding support for fixing the white space starting position in the first statement after a SgNamespaceDefinitionStatement.
                              SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(n);
                              if (namespaceDefinition != NULL)
                                 {
                                // The first token past the namespace definition's "{" should be excluded from the whitespace of the first statement.
                                   SgStatement* statement = isSgStatement(mappingInfo->node);
                                   if (statement != NULL)
                                      {
#if 0
                                        printf ("$$$$$$$$$$$$ Handle special case of leading whitespace of first statment in SgNamespaceDefinitionStatement \n");
#endif
                                        trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }
                           else
                            {
                           // Handle the left edge of a non-left-most child in the synthisized attribute list.


                              if (mappingInfo->shared == false)
                                 {
                                // DQ (10/16/2013): I think this is an error.
                                // mappingInfo->leading_whitespace_start = last_node_token_subsequence_start;
                                // mappingInfo->leading_whitespace_end   = mappingInfo->token_subsequence_start - 1;
                                   mappingInfo->leading_whitespace_start = last_node_token_subsequence_end + 1;
                                   ROSE_ASSERT(last_node_token_subsequence_end >= 0);
                                   mappingInfo->leading_whitespace_end   = mappingInfo->token_subsequence_start - 1;

                                // I think that if this fails then both of these need to be set to the default value: -1.
                                   if (mappingInfo->leading_whitespace_start > mappingInfo->leading_whitespace_end)
                                      {
#if 0
                                        printf ("Warning: i = %zu: mappingInfo->leading_whitespace_start=%d > mappingInfo->leading_whitespace_end=%d: (reset) \n",i,mappingInfo->leading_whitespace_start,mappingInfo->leading_whitespace_end);
#endif
                                        mappingInfo->leading_whitespace_start = -1;
                                        mappingInfo->leading_whitespace_end   = -1;
                                      }
                                   ROSE_ASSERT(mappingInfo->leading_whitespace_start <= mappingInfo->leading_whitespace_end);
#if 0
                                   printf ("Setting leading token subsequence to (%d,%d) \n",mappingInfo->leading_whitespace_start,mappingInfo->leading_whitespace_end);
#endif


                                // DQ (12/15/2014): Adding support for fixing the leading white space of the SgForInitStatement.
                                   SgStatement* statement = isSgStatement(n);
                                   if (statement != NULL)
                                      {
                                        ROSE_ASSERT(mappingInfo->node != NULL);

                                        SgBasicBlock* basicBlock = isSgBasicBlock(mappingInfo->node);
                                        if (basicBlock != NULL)
                                           {
#if 0
                                             printf ("$$$$$$$$$$$$ Handle special case of SgBasicBlock nested in SgStatement (not the first child) \n");
#endif
                                             trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }


                                     // DQ (12/15/2014): Adding support for fixing the leading white space of the SgForStatement body when it is not a SgBasicBlock.
                                        SgForStatement* forStatement = isSgForStatement(n);
                                        if (basicBlock == NULL && forStatement != NULL)
                                           {
                                             ROSE_ASSERT(mappingInfo->node != NULL);
                                             SgStatement* body = isSgStatement(mappingInfo->node);
                                             if (body != NULL && forStatement->get_loop_body() == body)
                                                {
#if 0
                                                  printf ("$$$$$$$$$$$$ Handle special case of body nested in SgForStatement \n");
#endif
                                                  trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                                  printf ("Exiting as a test! \n");
                                                  ROSE_ASSERT(false);
#endif
                                                }
                                           }

#if 0
                                     // DQ (12/31/2014): Fixup the else syntax when processing the SgIfStmt rather than when processing it as a child of it's parent SgStatement.

                                     // DQ (12/31/2014): Fixup the location of the else syntax between the true and false bodies.
                                        SgIfStmt* ifStatement = isSgIfStmt(mappingInfo->node);
                                        if (ifStatement != NULL && ifStatement->get_false_body() != NULL)
                                           {
#if 1
                                             printf ("$$$$$$$$$$$$ Handle special case of else syntax in SgIfStmt in SgStatement (not the first child) \n");
#endif
                                             discoverElseSyntax(true_body_mappingInfo,false_body_mappingInfo);

#if 1
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
#endif
                                      }


                                // DQ (12/31/2014): Handle the "else" syntax within the SgIfStmt.
                                   SgIfStmt* ifStatement = isSgIfStmt(n);
                                   if (ifStatement != NULL)
                                      {
                                        ROSE_ASSERT(mappingInfo->node != NULL);

                                        SgStatement* true_body_statement = isSgStatement(mappingInfo->node);
                                        if (true_body_statement == ifStatement->get_true_body() && ifStatement->get_false_body() != NULL)
                                           {
#if 0
                                             printf ("$$$$$$$$$$$$ Handle special case of SgIfStmt = %p and the true body position \n",ifStatement);
#endif
                                          // trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
                                             trimTrailingWhiteSpaceFromRight(mappingInfo,original_end_of_token_subsequence);
#if 0
                                             printf ("Exiting as a test! (Handle special case of SgIfStmt = %p and the else syntax position) \n",ifStatement);
                                             ROSE_ASSERT(false);
#endif
                                           }


                                        SgStatement* false_body_statement = isSgStatement(mappingInfo->node);
                                        if (false_body_statement == ifStatement->get_false_body())
                                           {
#if 0
                                             printf ("$$$$$$$$$$$$ Handle special case of SgIfStmt = %p and the false body position \n",ifStatement);
#endif
                                             trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
                                          // trimTrailingWhiteSpaceFromRight(mappingInfo,original_end_of_token_subsequence);
#if 0
                                             printf ("Exiting as a test! (Handle special case of SgIfStmt = %p and the else syntax position) \n",ifStatement);
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }
#if 0
                                // DQ (12/31/2014): This is never called because mappingInfo->node can not be both a SgBasicBlock AND a SgCaseOptionStmt or SgDefaultOptionStmt IR node!
                                // DQ (12/27/2014): Fix the leading whitespace.
                                   SgBasicBlock* basicBlock = isSgBasicBlock(mappingInfo->node);
                                   if (basicBlock != NULL)
                                      {
                                        ROSE_ASSERT(mappingInfo->node != NULL);

                                     // These are not set correctly in EDG.
                                        SgCaseOptionStmt* caseOptionStatement       = isSgCaseOptionStmt(mappingInfo->node);
                                        SgDefaultOptionStmt* defaultOptionStatement = isSgDefaultOptionStmt(mappingInfo->node);
                                        if (caseOptionStatement != NULL || defaultOptionStatement != NULL)
                                           {
#if 1
                                             printf ("$$$$$$$$$$$$ Handle special case of SgCaseOptionStmt = %p or SgDefaultOptionStmt = %p nested in SgBasicBlock \n",caseOptionStatement,defaultOptionStatement);
#endif
                                             trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
                                             trimTrailingWhiteSpaceFromRight(mappingInfo,original_end_of_token_subsequence);

                                             printf ("Exiting as a test! (never called because mappingInfo->node can not be both a SgBasicBlock AND a SgCaseOptionStmt or SgDefaultOptionStmt IR node!) \n");
                                             ROSE_ASSERT(false);
                                           }
                                      }
#endif
#if 0
                                // DQ (12/27/2014): Adding better mapping of constructs to token stream.
                                   SgSwitchStatement* switchStatement = isSgSwitchStatement(n);
                                   if (switchStatement != NULL)
                                      {
                                        ROSE_ASSERT(mappingInfo->node != NULL);
                                        SgBasicBlock* body = isSgBasicBlock(mappingInfo->node);
                                        if (body != NULL)
                                           {
                                             if (body == switchStatement->get_body())
                                                {
#if 0
                                                  printf ("$$$$$$$$$$$$ Handle special case of SgBasicBlock nested in SgSwitchStatement \n");
#endif
                                               // trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
#if 0
                                                  printf ("Exiting as a test! \n");
                                                  ROSE_ASSERT(false);
#endif
                                                }
                                           }
                                      }
#endif
                                 }
                                else
                                 {
#if 0
                                   printf ("Case of shared mappingInfo data structure not handled (leading) mappingInfo->node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
                                   printf ("   --- last_node_token_subsequence_start = %d last_node_token_subsequence_end = %d \n",last_node_token_subsequence_start,last_node_token_subsequence_end);
#endif
                                // DQ (10/16/2013): Check if this is the first node in the nodeVector (list of nodes sharing 
                                // this token subsequence data structure), so that we can set the leading token sequence.
                                   size_t index = 0;


                                   bool currentlySetToDefaultValues = (mappingInfo->leading_whitespace_start == -1) && (mappingInfo->leading_whitespace_end == -1);
#if 0
                                   printf ("   --- mappingInfo->nodeVector[index] = %p = %s \n",mappingInfo->nodeVector[index],mappingInfo->nodeVector[index]->class_name().c_str());
                                   printf ("   --- mappingInfo->node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
                                   printf ("   --- currentlySetToDefaultValues = %s \n",currentlySetToDefaultValues ? "true" : "false");
#endif
                                   if (currentlySetToDefaultValues == true && mappingInfo->nodeVector[index] == mappingInfo->node)
                                      {
#if 0
                                        printf ("   --- Found first node of shared TokenStreamSequenceToNodeMapping \n");
#endif
                                        mappingInfo->leading_whitespace_start = last_node_token_subsequence_end + 1;
                                        ROSE_ASSERT(last_node_token_subsequence_end >= 0);
                                        mappingInfo->leading_whitespace_end   = mappingInfo->token_subsequence_start - 1;

                                     // If this happends then we need to set the leading whitespace position to the default (-1).
                                     // ROSE_ASSERT (mappingInfo->leading_whitespace_start <= mappingInfo->leading_whitespace_end);
                                        if (mappingInfo->leading_whitespace_start > mappingInfo->leading_whitespace_end)
                                           {
                                             mappingInfo->leading_whitespace_start = -1;
                                             mappingInfo->leading_whitespace_end   = -1;
                                           }
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }


                         ROSE_ASSERT(lastChildWithTokenMapping >= 0);
                      // ROSE_ASSERT(lastChildWithTokenMapping == (tokenToNodeVector.size()-1));
#if 0
                         printf ("lastChildWithTokenMapping = %d i = %zu tokenToNodeVector.size() = %zu \n",lastChildWithTokenMapping,i,tokenToNodeVector.size());
#endif
                      // Handle the right edge of the AST subtree: modify the edged (trailing whitespace).
                      // if (i == (tokenToNodeVector.size()-1))
                      // if (i == lastChildWithTokenMapping)
                         if (i == (tokenToNodeVector.size()-1))
                            {
                           // Use the current node's lower bound as the base of the leading whitespace subsequence.
                              mappingInfo->trailing_whitespace_start = mappingInfo->token_subsequence_end + 1;
                              mappingInfo->trailing_whitespace_end   = current_node_token_subsequence_end;

                           // I think that if this fails then both of these need to be set to the default value: -1.
                              if (mappingInfo->trailing_whitespace_start > mappingInfo->trailing_whitespace_end)
                                 {
#if 0
                                   printf ("Warning: i = %zu: mappingInfo->trailing_whitespace_start=%d > mappingInfo->trailing_whitespace_end=%d: (reset) \n",i,mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
#endif
                                   mappingInfo->trailing_whitespace_start = -1;
                                   mappingInfo->trailing_whitespace_end   = -1;
                                 }
                              ROSE_ASSERT(mappingInfo->trailing_whitespace_start <= mappingInfo->trailing_whitespace_end);

                           // printf ("In evaluateSynthesizedAttribute(): tokenToNodeVector.size() = %zu tokenStream.size() = %zu \n",tokenToNodeVector.size(),tokenStream.size());
                              ROSE_ASSERT(mappingInfo->trailing_whitespace_end == -1 || mappingInfo->trailing_whitespace_end < (int) tokenStream.size());
#if 0
                              printf ("mappingInfo->trailing_whitespace_start = %d mappingInfo->trailing_whitespace_end = %d \n",mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
#endif
                           // DQ (12/1/2013): Added support for the SgClassDefinition.
                           // DQ (11/30/2013): Added to support trimming of the token stream for the first and last children to exclude the trailing "}" token.
                           // if (isSgBasicBlock(n) != NULL)
                              if (isSgBasicBlock(n) != NULL || isSgClassDefinition(n) != NULL)
                                 {
                                // This is a search of the token stream that we would like to avoid.
                                // If this is the first statement of set of statements nested in a "{" and "}" then we have to trim the edges of the first first statement.
                                   if (mappingInfo->trailing_whitespace_end != -1 && tokenStream[mappingInfo->trailing_whitespace_end]->p_tok_elem->token_lexeme == "}")
                                      {
#if 0
                                        printf ("Adjust the mappingInfo->trailing_whitespace_end to avoid the } token \n");
#endif
                                     // DQ (11/30/2013): This case is added to support test2012_166.c.
                                     // mappingInfo->trailing_whitespace_end--;
                                        if (mappingInfo->trailing_whitespace_start < mappingInfo->trailing_whitespace_end)
                                           {
                                             mappingInfo->trailing_whitespace_end--;
                                           }
                                          else
                                           {

                                          // DQ (11/30/2013): This case is added to support test2012_166.c.
                                             printf ("WARNING: adjustment skipped because it would fails assertion: mappingInfo->trailing_whitespace_start <= mappingInfo->trailing_whitespace_end \n");
                                           }
#if 0
                                        printf ("   --- after adjustment: mappingInfo->trailing_whitespace_end = %d \n",mappingInfo->trailing_whitespace_end);
#endif
                                        ROSE_ASSERT(mappingInfo->trailing_whitespace_start <= mappingInfo->trailing_whitespace_end);
#if 0
                                        printf ("Exiting as a test! \n");
                                        ROSE_ASSERT(false);
#endif
                                      }
                                 }
                            }
                           else
                            {
                           // Handle the right edge of a non-rightmost child in the synthisized attribute list.

                              if (mappingInfo->shared == false)
                                 {
                                   ROSE_ASSERT(i+1 < tokenToNodeVector.size());

                                   TokenStreamSequenceToNodeMapping* next_mappingInfo = tokenToNodeVector[i+1];
                                   ROSE_ASSERT(next_mappingInfo != NULL);
#if 0
                                   printf ("i = %zu next_mappingInfo->token_subsequence_start = %d next_mappingInfo->token_subsequence_end = %d \n",i,next_mappingInfo->token_subsequence_start,next_mappingInfo->token_subsequence_end);
#endif
                                // DQ (1/2/2015): Handle the special case of "else" syntax between the true and false bodies of the SgIfStmt.
                                   bool skipUpdate = false;
                                   printf ("#################### Fix mistaken reset of trailing whitespace end position for the case of the true body of a SgIfStmt! ##################### \n");
                                   SgIfStmt* ifStatement = isSgIfStmt(n);
                                   if (ifStatement != NULL)
                                      {
                                     // if (mappingInfo->node == isSgIfStmt(n)->get_true_body())
                                        if (ifStatement != NULL && (mappingInfo->node == isSgIfStmt(n)->get_true_body() || mappingInfo->node == isSgIfStmt(n)->get_conditional()))
                                           {
#if 0
                                             printf ("Surpress fixup of trailing whitespace end position for this case! \n");
#endif
                                             skipUpdate = true;
#if 0
                                             printf ("Exiting as a test! \n");
                                             ROSE_ASSERT(false);
#endif
                                           }
                                      }


                                // mappingInfo->trailing_whitespace_start = mappingInfo->token_subsequence_end + 1;
                                // mappingInfo->trailing_whitespace_end   = next_mappingInfo->token_subsequence_start - 1;
                                   if (skipUpdate == false)
                                      {
                                        mappingInfo->trailing_whitespace_start = mappingInfo->token_subsequence_end + 1;
                                        mappingInfo->trailing_whitespace_end   = next_mappingInfo->token_subsequence_start - 1;
                                      }
                                     else
                                      {
                                        printf ("Skipping trailing whitespace update for n = %p = %s mappingInfo->trailing_whitespace_start = %d mappingInfo->trailing_whitespace_end = %d \n",
                                             n,n->class_name().c_str(),mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
                                      }


                                // I think that if this fails then both of these need to be set to the default value: -1.
                                   if (mappingInfo->trailing_whitespace_start > mappingInfo->trailing_whitespace_end)
                                      {
#if 0
                                        printf ("Warning: i = %zu: mappingInfo->trailing_whitespace_start=%d > mappingInfo->trailing_whitespace_end=%d: (reset) \n",i,mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
#endif
                                        mappingInfo->trailing_whitespace_start = -1;
                                        mappingInfo->trailing_whitespace_end   = -1;
#if 0
                                        printf ("AFTER RESET: i = %zu: mappingInfo->trailing_whitespace_start=%d > mappingInfo->trailing_whitespace_end=%d: (reset) \n",i,mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
#endif
                                      }
                                   ROSE_ASSERT(mappingInfo->trailing_whitespace_start <= mappingInfo->trailing_whitespace_end);


                                // printf ("In evaluateSynthesizedAttribute(): tokenToNodeVector.size() = %zu tokenStream.size() = %zu \n",tokenToNodeVector.size(),tokenStream.size());
                                   ROSE_ASSERT(next_mappingInfo->token_subsequence_start == -1 || next_mappingInfo->token_subsequence_start < (int)tokenStream.size());

                                   ROSE_ASSERT(mappingInfo->trailing_whitespace_end == -1 || mappingInfo->trailing_whitespace_end < (int)tokenStream.size());
                                 }
                                else
                                 {
#if 0
                                   printf ("Case of shared mappingInfo data structure not handled (trailing) mappingInfo->node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
                                 }
                            }
#error "DEAD CODE!"
                      // DQ (1/2/2015): END OF COMMENTED OUT BLOCK: We want to make sure that leading a trailing white space does not include syntax.
#endif

// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                         printf ("   --- TOKENS: AFTER RESET: leading_whitespace tokens (%d,%d) token_subsequence (%d,%d) trailing_whitespace tokens (%d,%d) \n",
                            mappingInfo->leading_whitespace_start,mappingInfo->leading_whitespace_end,mappingInfo->token_subsequence_start,mappingInfo->token_subsequence_end,mappingInfo->trailing_whitespace_start,mappingInfo->trailing_whitespace_end);
#endif

                      // DQ (12/8/2016): This is commented out as part of eliminating warnings we want to have be errors: [-Werror=unused-but-set-variable.
                      // last_node_token_subsequence_start = token_subsequence_start;
                      // last_node_token_subsequence_end   = token_subsequence_end;

#if 0
                      // DQ (1/27/2015): Debugging case of test2015_110.C (prefix operator++() used in for loop increment expression).
                         if (isSgForStatement(n) != NULL && mappingInfo->node == isSgForStatement(n)->get_increment() )
                            {
                              printf ("Exiting as a test! (detected for loop increment expression) \n");
                              ROSE_ASSERT(false);
                            }
#endif
                       }

                 // Find the intervals of indexes into the child array of IR nodes that don't have associated token subsequences already defined.
                 // Then build a token mapping to represent the token sequence for that interval of IR nodes.  Note that this step fills in the 
                 // mappings of token stream to IR nodes (or sets of IR nodes) where they could not be computed base on the source position used 
                 // in the evaluateInheritedAttribute() function (run previous to this evaluateSynthesizedAttribute() function at this point in 
                 // the AST traversal).
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                    printf ("In evaluateSynthesizedAttribute(): process childrenWithoutTokenMappings: current_node_token_subsequence_start = %d current_node_token_subsequence_end = %d \n",
                         current_node_token_subsequence_start,current_node_token_subsequence_end);
                    printf ("   --- childrenWithoutTokenMappings.size() = %zu \n",childrenWithoutTokenMappings.size());
#endif
                 // for (size_t i = 0; i < childrenWithoutTokenMappings.size(); i++)
                    size_t i = 0;
                    while (i < childrenWithoutTokenMappings.size())
                       {
#if 0
                         printf ("childrenWithoutTokenMappings: i = %zu \n",i);
#endif
// #if 0
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                         printf ("   --- In evaluateSynthesizedAttribute(): childrenWithoutTokenMappings[%zu] = %zu = %p = %s \n",
                              i,childrenWithoutTokenMappings[i],childAttributes[childrenWithoutTokenMappings[i]].node,
                              childAttributes[childrenWithoutTokenMappings[i]].node->class_name().c_str());
#endif
#if 0
                         isSgStatement(childAttributes[childrenWithoutTokenMappings[i]].node)->get_startOfConstruct()->display("In evaluateSynthesizedAttribute(): childAttributes[childrenWithoutTokenMappings[i]].node: debug");
#endif
                         size_t starting_NodeSequenceWithoutTokenMapping = childrenWithoutTokenMappings[i];
                         size_t ending_NodeSequenceWithoutTokenMapping   = starting_NodeSequenceWithoutTokenMapping;

                      // for (size_t j = i+1; j < childrenWithoutTokenMappings.size(); j++)
                         bool endOfSequence = false;
                      // size_t j = i+1;
                         size_t j = 1;
#if 0
                         printf ("childrenWithoutTokenMappings.size() = %zu \n",childrenWithoutTokenMappings.size());
                         printf ("endOfSequence = %s \n",endOfSequence ? "true" : "false");
                         printf ("childAttributes.size() = %zu \n",childAttributes.size());
                         printf ("starting_NodeSequenceWithoutTokenMapping = %zu j = %zu \n",starting_NodeSequenceWithoutTokenMapping,j);
                         printf ("childAttributes[starting_NodeSequenceWithoutTokenMapping+j].node = %p \n",childAttributes[starting_NodeSequenceWithoutTokenMapping+j].node);
#endif
                      // while (endOfSequence == false && j < childrenWithoutTokenMappings.size())
                         while ( (endOfSequence == false) && 
                                 (starting_NodeSequenceWithoutTokenMapping+j < childAttributes.size()) && 
                                 (childAttributes[starting_NodeSequenceWithoutTokenMapping+j].node != NULL) &&
                                 (tokenStreamSequenceMap.find(childAttributes[starting_NodeSequenceWithoutTokenMapping+j].node) != tokenStreamSequenceMap.end()) )
                            {
#if 0
                              printf ("find the end of the interval of sibling IR nodes: j = %zu \n",j);
#endif
#if 0
                              printf ("childrenWithoutTokenMappings[j=%zu] = %zu ending_NodeSequenceWithoutTokenMapping = %zu \n",j,childrenWithoutTokenMappings[j],ending_NodeSequenceWithoutTokenMapping);
#endif
                           // Is this the next child IR node in a sequence.
                              if (childrenWithoutTokenMappings[j] == ending_NodeSequenceWithoutTokenMapping+1)
                                 {
                                // Does this IR node have an associated token subsequence mapping.
                                   SgNode* childNode = childAttributes[childrenWithoutTokenMappings[i]].node;
                                   ROSE_ASSERT(childNode != NULL);

                                   if (tokenStreamSequenceMap.find(childNode) != tokenStreamSequenceMap.end())
                                      {
                                     // This is the end of the sequence of IR nodes withouth a token mapping.
                                        endOfSequence = true;
                                      }
                                     else
                                      {
                                     // The end of the sequence of IR nodes that is without a token sequence is at least one longer...
                                        ending_NodeSequenceWithoutTokenMapping++;
                                        endOfSequence = false;
                                      }

                                   if (ending_NodeSequenceWithoutTokenMapping < childrenWithoutTokenMappings.size())
                                      {
                                      }
                                 }

                              j++;
                            }

                      // i++;
                      // i = ending_NodeSequenceWithoutTokenMapping + 1;
                      // i = j;
                         i += j;

                      // At this point we have identified a subsequence of children that don't have an associated token sequence.
#if 0
                         printf ("i = %zu starting_NodeSequenceWithoutTokenMapping = %zu ending_NodeSequenceWithoutTokenMapping = %zu \n",i,starting_NodeSequenceWithoutTokenMapping,ending_NodeSequenceWithoutTokenMapping);
#endif
                         ROSE_ASSERT(starting_NodeSequenceWithoutTokenMapping <= ending_NodeSequenceWithoutTokenMapping);

                      // Build a new TokenStreamSequenceToNodeMapping.
                         SgNode* starting_node = childAttributes[starting_NodeSequenceWithoutTokenMapping].node;
                      // SgNode* ending_node   = childAttributes[ending_NodeSequenceWithoutTokenMapping].node;

                         int leading_whitespace_start   = -1;
                         int leading_whitespace_end     = -1;

                         int start_of_token_subsequence = -1;
                         int end_of_token_subsequence   = -1;

                         int trailing_whitespace_start  = -1;
                         int trailing_whitespace_end    = -1;

                         int else_whitespace_start      = -1;
                         int else_whitespace_end        = -1;

                      // Find the left and right edges of the token sequences.
                         if (starting_NodeSequenceWithoutTokenMapping == 0)
                            {
                           // Get the source position or the start of the token stream from the current node.
                              start_of_token_subsequence = current_node_token_subsequence_start;

                           // DQ (10/29/2013): If this is a SgBasicBlock, then the first token is a "{" and so the first token of this statement must be after that token.
                              SgBasicBlock* basicBlock = isSgBasicBlock(n);
                              if (basicBlock != NULL)
                                 {
                                   printf ("Reset the start_of_token_subsequence where the parent is a SgBasicBlock: case of first statement of translation unit \n");
                                   start_of_token_subsequence++;
                                 }
#if 0
                              printf ("This left (leading) edge might not be computed correctly (not the same as the partent's left (leading) edge) \n");
                              ROSE_ASSERT(false);
#endif
                            }
                           else
                            {
                           // Find the token sequence on the left.
                              ROSE_ASSERT(starting_NodeSequenceWithoutTokenMapping > 0);

                              SgNode* left_edge_node = childAttributes[starting_NodeSequenceWithoutTokenMapping-1].node;

                              size_t temp_starting_NodeSequenceWithoutTokenMapping = starting_NodeSequenceWithoutTokenMapping-1;
#if 0
                              printf ("Find the token sequence on the left: starting_NodeSequenceWithoutTokenMapping = %zu \n",starting_NodeSequenceWithoutTokenMapping);
                              if (childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node != NULL)
                                 {
                                   printf ("   --- childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node = %p = %s \n",
                                        childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node,
                                        childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node->class_name().c_str());
                                 }
                                else
                                 {
                                   printf ("   --- childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node = %p \n",childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node);
                                 }
                              if (temp_starting_NodeSequenceWithoutTokenMapping >= 0)
                                 {
                                   printf ("   --- tokenStreamSequenceMap.find(childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node) != tokenStreamSequenceMap.end()) = %s \n",
                                        (tokenStreamSequenceMap.find(childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node) != tokenStreamSequenceMap.end()) ? "true" : "false");
                                 }
#endif

                              bool done = false;

                              while ( (done == false) && 
                                   // DQ (3/25/2017): Eliminate Clang warning: warning: comparison of unsigned expression >= 0 is always true [-Wtautological-compare]
                                   // (temp_starting_NodeSequenceWithoutTokenMapping >= 0) &&
                                      ( (childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node == NULL) ||
                                        (tokenStreamSequenceMap.find(childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node) == tokenStreamSequenceMap.end()) ) )
                                 {
#if 0
                                   printf ("   *** Searching for the left edge: temp_starting_NodeSequenceWithoutTokenMapping = %zu \n",temp_starting_NodeSequenceWithoutTokenMapping);
                                   printf ("   --- (start of loop): done = %s \n",done ? "true" : "false");
#endif
                                   if ( (childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node != NULL) && 
                                        (tokenStreamSequenceMap.find(childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node) != tokenStreamSequenceMap.end()) )
                                      {
#if 0
                                        printf ("Found a valid IR node with existing token sequence: temp_starting_NodeSequenceWithoutTokenMapping = %zu \n",temp_starting_NodeSequenceWithoutTokenMapping);
#endif
                                        done = true;
                                      }

                                   if (temp_starting_NodeSequenceWithoutTokenMapping == 0)
                                      {
#if 0
                                        printf ("   --- set loop to end: done = %s \n",done ? "true" : "false");
#endif
                                        done = true;
                                      }
                                     else
                                      {
                                        temp_starting_NodeSequenceWithoutTokenMapping--;
                                      }

                                // DQ (3/25/2017): Eliminate Clang warning: warning: comparison of unsigned expression >= 0 is always true [-Wtautological-compare]
                                // if (temp_starting_NodeSequenceWithoutTokenMapping >= 0)
                                //    {
                                        left_edge_node = childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node;
#if 0
                                        printf ("   --- temp_starting_NodeSequenceWithoutTokenMapping = %zu \n",temp_starting_NodeSequenceWithoutTokenMapping);
                                        printf ("   --- done = %s \n",done ? "true" : "false");
                                        if (childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node != NULL)
                                           {
                                             printf ("   --- --- childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node = %p = %s \n",
                                                  childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node,
                                                  childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node->class_name().c_str());
                                           }
                                          else
                                           {
                                             printf ("   --- --- childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node = %p \n",childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node);
                                           }
                                        if (temp_starting_NodeSequenceWithoutTokenMapping >= 0)
                                           {
                                             printf ("   --- --- tokenStreamSequenceMap.find(childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node) != tokenStreamSequenceMap.end()) = %s \n",
                                                  (tokenStreamSequenceMap.find(childAttributes[temp_starting_NodeSequenceWithoutTokenMapping].node) != tokenStreamSequenceMap.end()) ? "true" : "false");
                                           }
#endif
                                  //  }
#if 0
                                   printf ("   --- (end of loop): done = %s \n",done ? "true" : "false");
#endif
                                 }

                           // SgNode* left_edge_node = childAttributes[starting_NodeSequenceWithoutTokenMapping-1].node;
                              ROSE_ASSERT(left_edge_node != NULL);

                              if (tokenStreamSequenceMap.find(left_edge_node) != tokenStreamSequenceMap.end())
                                 {
                                   TokenStreamSequenceToNodeMapping* mappingInfo = tokenStreamSequenceMap[left_edge_node];
                                   ROSE_ASSERT(mappingInfo != NULL);
#if 0
                                   printf ("Found a mapping for left_edge_node = %p = %s \n",left_edge_node,left_edge_node->class_name().c_str());
#endif
#if 0
                                   isSgStatement(left_edge_node)->get_startOfConstruct()->display("In evaluateSynthesizedAttribute(): left_edge_node: debug");
#endif

                                   if (mappingInfo->token_subsequence_end == original_end_of_token_subsequence)
                                      {
                                        start_of_token_subsequence = -1;
                                      }
                                     else
                                      {
                                        start_of_token_subsequence = mappingInfo->token_subsequence_end + 1;
                                      }
#if 0
                                   printf ("start_of_token_subsequence = %d original_end_of_token_subsequence = %d \n",start_of_token_subsequence,original_end_of_token_subsequence);
#endif
                                   ROSE_ASSERT(start_of_token_subsequence <= original_end_of_token_subsequence);
                                 }
                                else
                                 {
                                // A better solution is to iterate back in the list of children to find a better 
                                // match than just setting the start of this subtrees token list to the start of 
                                // the token list from the parent.
                                   start_of_token_subsequence = original_start_of_token_subsequence;
#if 0
                                   printf ("Could not find a mapping for left_edge_node = %p = %s \n",left_edge_node,left_edge_node->class_name().c_str());
#endif
                                // DQ (10/29/2013): If this is a SgBasicBlock, then the first token is a "{" and so the first token of this statement must be after that token.
                                   SgBasicBlock* basicBlock = isSgBasicBlock(n);
                                   if (basicBlock != NULL)
                                      {
                                        printf ("Reset the start_of_token_subsequence where the parent is a SgBasicBlock: case of first statement of block \n");
                                        start_of_token_subsequence++;
                                      }
                                 }
                            }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                         printf ("Computed: start_of_token_subsequence = %d \n",start_of_token_subsequence);
#endif
                      // DQ (10/13/2013): I think that we can assert this.
                         ROSE_ASSERT(current_node_token_subsequence_end == original_end_of_token_subsequence);

                         if (ending_NodeSequenceWithoutTokenMapping+1 == childAttributes.size())
                            {
                           // If we are at the end of the token sequence, then the start_of_token_subsequence 
                           // was set to -1, if so then set the end_of_token_subsequence to be consistant.
                           // if (start_of_token_subsequence == -1)
                           // if (mappingInfo->token_subsequence_end == original_end_of_token_subsequence)
                              if (start_of_token_subsequence == -1)
                                 {
                                   end_of_token_subsequence = -1;
                                 }
                                else
                                 {
                                   end_of_token_subsequence = current_node_token_subsequence_end;
                                // end_of_token_subsequence = original_end_of_token_subsequence;

                                // DQ (10/29/2013): If this is a SgBasicBlock, then the last token is a "}" and so the last token of this statement must be before that token.
                                   SgBasicBlock* basicBlock = isSgBasicBlock(n);
                                   if (basicBlock != NULL)
                                      {
                                        printf ("Reset the end_of_token_subsequence where the parent is a SgBasicBlock: case of last statement \n");
                                        end_of_token_subsequence--;
                                      }
                                 }
#if 0
                              printf ("This right (trailing) edge might not be computed correctly (not the same as the partent's right (trailing) edge) \n");
                              ROSE_ASSERT(false);
#endif
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                              printf ("end_of_token_subsequence = %d tokenStream.size() = %zu \n",end_of_token_subsequence,tokenStream.size());
#endif
                           // Note: we can't compare signed to unsigned (else it is always false).
                           // ROSE_ASSERT(start_of_token_subsequence >= 0);
                              if (end_of_token_subsequence >= 0)
                                 {
                                    ROSE_ASSERT(end_of_token_subsequence < (int)tokenStream.size());
                                 }
                            }
                           else
                            {
                           // Find the token sequence on the left.
                              SgNode* trailing_edge_node = childAttributes[ending_NodeSequenceWithoutTokenMapping+1].node;
#if 0
                              printf ("Check if this trailing_edge_node can be used to map token sequences \n");
#endif
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                              printf ("ending_NodeSequenceWithoutTokenMapping = %zu childAttributes.size() = %zu trailing_edge_node = %p \n",ending_NodeSequenceWithoutTokenMapping,childAttributes.size(),trailing_edge_node);
#endif
                           // Where the next child is associated with a NULL pointer (e.g. SgFunctionDeclaration can have a NULL pointer to the Python specific decorator list).
                              int index = 1;
                              while ( (trailing_edge_node == NULL) && (ending_NodeSequenceWithoutTokenMapping + index) < childAttributes.size())
                                 {
                                   trailing_edge_node = childAttributes[ending_NodeSequenceWithoutTokenMapping+index].node;
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                                   printf ("index = %d trailing_edge_node = %p \n",index,trailing_edge_node);
#endif
                                   index++;
                                 }

#if 0
                              printf ("Before processing trailing_edge_node = %p start_of_token_subsequence = %d end_of_token_subsequence = %d \n",trailing_edge_node,start_of_token_subsequence,end_of_token_subsequence);
#endif
                           // DQ (10/13/2013): test2012_20.c demonstrates a case where (trailing_edge_node == NULL).
                           // ROSE_ASSERT(trailing_edge_node != NULL);
                              if (trailing_edge_node != NULL)
                                 {
                                   if (tokenStreamSequenceMap.find(trailing_edge_node) != tokenStreamSequenceMap.end())
                                      {
                                        TokenStreamSequenceToNodeMapping* mappingInfo = tokenStreamSequenceMap[trailing_edge_node];
                                        ROSE_ASSERT(mappingInfo != NULL);

                                     // end_of_token_subsequence = mappingInfo->token_subsequence_start - 1;
                                        if (start_of_token_subsequence == -1)
                                           {
                                             end_of_token_subsequence = -1;
                                           }
                                          else
                                           {
#if 0
                                             printf ("mappingInfo->token_subsequence_start = %d original_start_of_token_subsequence = %d \n",mappingInfo->token_subsequence_start,original_start_of_token_subsequence);
#endif
                                          // if (mappingInfo->token_subsequence_start == original_start_of_token_subsequence)
                                          // if (mappingInfo->token_subsequence_start >= original_start_of_token_subsequence)
                                             if (mappingInfo->token_subsequence_start <= original_start_of_token_subsequence)
                                                {
                                                  ROSE_ASSERT(mappingInfo->token_subsequence_start == original_start_of_token_subsequence);
                                                  end_of_token_subsequence = original_start_of_token_subsequence;

                                               // DQ (10/29/2013): If this is a SgBasicBlock, then the last token is a "}" and so the last token of this statement must be before that token.
                                                  SgBasicBlock* basicBlock = isSgBasicBlock(n);
                                                  if (basicBlock != NULL)
                                                     {
                                                       printf ("Reset the end_of_token_subsequence where the parent is a SgBasicBlock: using identified token sequence for this node, but (mappingInfo->token_subsequence_start <= original_start_of_token_subsequence) \n");
                                                       end_of_token_subsequence--;
                                                     }
                                                }
                                               else
                                                {
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                                                  printf ("mappingInfo->token_subsequence_start = %d original_start_of_token_subsequence = %d \n",mappingInfo->token_subsequence_start,original_start_of_token_subsequence);
#endif
                                               // ROSE_ASSERT(mappingInfo->token_subsequence_start < original_start_of_token_subsequence);
                                                  ROSE_ASSERT(mappingInfo->token_subsequence_start > original_start_of_token_subsequence);
                                                  end_of_token_subsequence = mappingInfo->token_subsequence_start - 1;
                                                }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                                             printf ("end_of_token_subsequence = %d original_start_of_token_subsequence = %d \n",end_of_token_subsequence,original_start_of_token_subsequence);
#endif
                                             ROSE_ASSERT(end_of_token_subsequence >= original_start_of_token_subsequence);
                                           }
                                   
                                     // ROSE_ASSERT(end_of_token_subsequence >= original_start_of_token_subsequence);
                                      }
                                     else
                                      {
                                     // end_of_token_subsequence = original_end_of_token_subsequence;
                                        if (start_of_token_subsequence == -1)
                                           {
                                             end_of_token_subsequence = -1;
                                           }
                                          else
                                           {
                                             end_of_token_subsequence = original_end_of_token_subsequence;

                                          // DQ (10/29/2013): If this is a SgBasicBlock, then the last token is a "}" and so the last token of this statement must be before that token.
                                             SgBasicBlock* basicBlock = isSgBasicBlock(n);
                                             if (basicBlock != NULL)
                                                {
                                                  printf ("Reset the end_of_token_subsequence where the parent is a SgBasicBlock: no token sequence for this node \n");
                                                  end_of_token_subsequence--;
                                                }
                                           }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                                        printf ("Didn't find a token sequence for this node: trailing_edge_node = %p = %s \n",trailing_edge_node,trailing_edge_node->class_name().c_str());
                                        printf ("   --- start_of_token_subsequence = %d end_of_token_subsequence = %d \n",start_of_token_subsequence,end_of_token_subsequence);
#endif
                                      }
                                 }
                                else
                                 {
                                // Rare case (see test2013_85.c).

                                // end_of_token_subsequence = original_end_of_token_subsequence;
                                   if (start_of_token_subsequence == -1)
                                      {
                                        end_of_token_subsequence = -1;
                                      }
                                     else
                                      {
                                        end_of_token_subsequence = original_end_of_token_subsequence;

                                     // DQ (10/29/2013): If this is a SgBasicBlock, then the last token is a "}" and so the last token of this statement must be before that token.
                                        SgBasicBlock* basicBlock = isSgBasicBlock(n);
                                        if (basicBlock != NULL)
                                           {
                                             printf ("Reset the end_of_token_subsequence where the parent is a SgBasicBlock: Rare case (see test2013_85.c) \n");
                                             end_of_token_subsequence--;
                                           }
                                      }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                                   printf ("Didn't find a valid trailing_edge_node = %p \n",trailing_edge_node);
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }

                           // Note: we can't compare signed to unsigned (else it is always false).
                           // ROSE_ASSERT(end_of_token_subsequence < tokenStream.size());
                              if (end_of_token_subsequence >= 0)
                                 {
                                   ROSE_ASSERT(end_of_token_subsequence < (long)tokenStream.size());
                                 }
                            }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
                         printf ("start_of_token_subsequence = %d end_of_token_subsequence = %d \n",start_of_token_subsequence,end_of_token_subsequence);
#endif
                      // Error checking for consistancy.
                         if (start_of_token_subsequence == -1)
                            {
                              ROSE_ASSERT(end_of_token_subsequence == -1);
                            }

                      // Error checking for consistancy.
                         if (end_of_token_subsequence == -1)
                            {
                              ROSE_ASSERT(start_of_token_subsequence == -1);
                            }

                      // Note: we can't compare signed to unsigned (else it is always false).
                      // ROSE_ASSERT(end_of_token_subsequence < tokenStream.size());
                         if (end_of_token_subsequence >= 0)
                            {
                           // DQ (12/6/2016): Eliminate warning that we want to consider an error: -Wsign-compare
                              ROSE_ASSERT((size_t)end_of_token_subsequence < tokenStream.size());
                            }

                         if (start_of_token_subsequence >= 0)
                            {
                           // Trim the white space from the leading edge (and assign it to the leading_whitespace_start,leading_whitespace_end values).
                              leading_whitespace_start = start_of_token_subsequence;
                              leading_whitespace_end   = leading_whitespace_start;

                              ROSE_ASSERT(leading_whitespace_start >= 0);

                           // DQ (12/6/2016): Eliminate warning that we want to consider an error: -Wsign-compare
                              ROSE_ASSERT((size_t)leading_whitespace_start < tokenStream.size());

                              ROSE_ASSERT(tokenStream[leading_whitespace_start] != NULL);
                              if (tokenStream[leading_whitespace_start]->p_tok_elem != NULL)
                                 {
                                   ROSE_ASSERT(tokenStream[leading_whitespace_start]->p_tok_elem != NULL);
#if 0
                                   printf ("tokenStream[leading_whitespace_start]->p_tok_elem->token_lexeme = %s \n",tokenStream[leading_whitespace_start]->p_tok_elem->token_lexeme.c_str());
#endif
                                   if (tokenStream[leading_whitespace_start]->p_tok_elem->token_id == C_CXX_WHITESPACE)
                                      {
#if 0
                                        printf ("original_end_of_token_subsequence = %d \n",original_end_of_token_subsequence);
#endif
                                     // Increment the token subsequence at least once since the current position is C_CXX_WHITESPACE.
                                        if (leading_whitespace_end < original_end_of_token_subsequence)
                                             start_of_token_subsequence++;

                                     // DQ (10/29/2013): Allow for the "else" keyword to be skipped over in triming tokens from the start of the current statement.
                                     // while (leading_whitespace_end < original_end_of_token_subsequence && tokenStream[leading_whitespace_end+1]->p_tok_elem->token_id == C_CXX_WHITESPACE)
                                        while ( leading_whitespace_end < original_end_of_token_subsequence && 
                                                ( tokenStream[leading_whitespace_end+1]->p_tok_elem->token_id == C_CXX_ELSE ||
                                                  tokenStream[leading_whitespace_end+1]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                           {
#if 0
                                             printf ("start_of_token_subsequence = %d leading_whitespace_end = %d \n",start_of_token_subsequence,leading_whitespace_end);
#endif
                                             leading_whitespace_end++;

                                          // Increment the token subsequence for the mail token sequence specification.
                                             start_of_token_subsequence++;
                                           }
                                      }
                                     else
                                      {
                                     // Mark this as an empty subsequence.
                                        leading_whitespace_start = -1;
                                        leading_whitespace_end   = -1;
                                      }
                                 }
                                else
                                 {
                                   printf ("Case of CPP directive or comment as token not handled (in adjustment of leading_whitespace_end) \n");
                                 }
                            }
                         
                      // Note: we can't compare signed to unsigned (else it is always false).
                      // ROSE_ASSERT(end_of_token_subsequence < tokenStream.size());
                         if (end_of_token_subsequence >= 0)
                            {
                           // DQ (12/6/2016): Eliminate warning that we want to consider an error: -Wsign-compare
                               ROSE_ASSERT((size_t)end_of_token_subsequence < tokenStream.size());
                            }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                         printf ("leading_whitespace_start = %d leading_whitespace_end = %d \n",leading_whitespace_start,leading_whitespace_end);
#endif
                         if (end_of_token_subsequence >= 0)
                            {
                           // Trim the white space from the trailing edge (and assign it to the trailing_whitespace_start,trailing_whitespace_end values).
                           // trailing_whitespace_start = end_of_token_subsequence;
                           // trailing_whitespace_end   = trailing_whitespace_start;
                              trailing_whitespace_end   = end_of_token_subsequence;
                              trailing_whitespace_start = trailing_whitespace_end;
// #if 0
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                              printf ("start_of_token_subsequence = %d end_of_token_subsequence = %d \n",start_of_token_subsequence,end_of_token_subsequence);
#endif
                           // DQ (12/6/2016): Eliminate warning that we want to consider an error: -Wsign-compare
                              ROSE_ASSERT((size_t)trailing_whitespace_end < tokenStream.size());

                              ROSE_ASSERT(tokenStream[trailing_whitespace_end] != NULL);
                              if (tokenStream[trailing_whitespace_end]->p_tok_elem != NULL)
                                 {
                                   ROSE_ASSERT(tokenStream[trailing_whitespace_end]->p_tok_elem != NULL);
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                                   printf ("tokenStream[trailing_whitespace_end]->p_tok_elem->token_lexeme = %s \n",tokenStream[trailing_whitespace_end]->p_tok_elem->token_lexeme.c_str());
#endif
                                   if (tokenStream[trailing_whitespace_end]->p_tok_elem->token_id == C_CXX_WHITESPACE)
                                      {
                                     // Back off at least once since the current position is C_CXX_WHITESPACE.
                                        end_of_token_subsequence--;

                                     // while (tokenStream[trailing_whitespace_start-1]->p_tok_elem->token_id == C_CXX_WHITESPACE)
                                        while (trailing_whitespace_start > original_start_of_token_subsequence && tokenStream[trailing_whitespace_start-1]->p_tok_elem->token_id == C_CXX_WHITESPACE)
                                           {
                                             ROSE_ASSERT(trailing_whitespace_start > original_start_of_token_subsequence);

                                             trailing_whitespace_start--;

                                          // Back off of the token subsequence for the mail token sequence specification.
                                             end_of_token_subsequence--;
                                           }
                                      }
                                     else
                                      {
                                     // Mark this as an empty subsequence.
                                        trailing_whitespace_start = -1;
                                        trailing_whitespace_end   = -1;
                                      }
                                 }
                                else
                                 {
                                   printf ("Case of CPP directive or comment as token not handled (in adjustment of trailing_whitespace_start) \n");
                                 }
                            }

                         if (start_of_token_subsequence > end_of_token_subsequence)
                            {
                              start_of_token_subsequence = -1;
                              end_of_token_subsequence   = -1;
                            }
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                         printf ("trailing_whitespace_start = %d trailing_whitespace_end = %d \n",trailing_whitespace_start,trailing_whitespace_end);
#endif
// #if 1
#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE || 0
                         printf ("Calling createTokenInterval(): leading_whitespace_start   = %d leading_whitespace_end   = %d \n",leading_whitespace_start,leading_whitespace_end);
                         printf ("Calling createTokenInterval(): start_of_token_subsequence = %d end_of_token_subsequence = %d \n",start_of_token_subsequence,end_of_token_subsequence);
                         printf ("Calling createTokenInterval(): trailing_whitespace_start  = %d trailing_whitespace_end  = %d \n",trailing_whitespace_start,trailing_whitespace_end);
                         printf ("Calling createTokenInterval(): else_whitespace_start      = %d else_whitespace_end      = %d \n",else_whitespace_start,else_whitespace_end);
#endif
                         size_t sizeBeforeNewTokenStreamSequenceToNodeMapping = tokenStreamSequenceVector.size();

                      // In this case we should know that this is a new TokenStreamSequenceToNodeMapping, so maybe we should call new directly.
                         TokenStreamSequenceToNodeMapping* element = 
                              TokenStreamSequenceToNodeMapping::createTokenInterval(starting_node,
                                   leading_whitespace_start,leading_whitespace_end,
                                   start_of_token_subsequence,end_of_token_subsequence,
                                   trailing_whitespace_start,trailing_whitespace_end,
                                   else_whitespace_start,else_whitespace_end);

                         element->constructedInEvaluationOfSynthesizedAttribute = true;

                      // Add to vector (so that we can be the last element).  Note that we might be able to just lookup 
                      // the element that we need instead of using the last element in the vector.
                         tokenStreamSequenceVector.push_back(element);

                      // Add to the map so that we have the final desired data structure (to attach to the SgSourceFile).
                         tokenStreamSequenceMap[starting_node] = element;
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                         size_t sizeAfterNewTokenStreamSequenceToNodeMapping = tokenStreamSequenceVector.size();

                      // Make sure that this has been added to the collections.
                         ROSE_ASSERT(sizeAfterNewTokenStreamSequenceToNodeMapping > sizeBeforeNewTokenStreamSequenceToNodeMapping);

                      // DQ (12/3/2014): Make this an error message (failes for amr/Coarsen_particles.cc).
                      // ROSE_ASSERT(sizeAfterNewTokenStreamSequenceToNodeMapping == tokenStreamSequenceMap.size());
                         if (tokenStreamSequenceMap.size() != sizeAfterNewTokenStreamSequenceToNodeMapping)
                            {
                              printf ("ERROR: TokenMappingTraversal::createTokenInterval(): tokenStreamSequenceMap.size() != tokenStreamSequenceVector.size() \n");
                              printf ("   --- tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
                              printf ("   --- sizeAfterNewTokenStreamSequenceToNodeMapping = %zu \n",sizeAfterNewTokenStreamSequenceToNodeMapping);
                            }
#if 0
                         printf ("starting_NodeSequenceWithoutTokenMapping = %d ending_NodeSequenceWithoutTokenMapping = %d \n",starting_NodeSequenceWithoutTokenMapping,ending_NodeSequenceWithoutTokenMapping);
#endif
                      // for (int k = starting_NodeSequenceWithoutTokenMapping; k < ending_NodeSequenceWithoutTokenMapping; k++)
                      // for (int k = starting_NodeSequenceWithoutTokenMapping; k <= ending_NodeSequenceWithoutTokenMapping; k++)

                      // DQ (12/6/2016): Eliminate warning that we want to consider an error: -Wsign-compare
                         for (size_t k = starting_NodeSequenceWithoutTokenMapping; k < ending_NodeSequenceWithoutTokenMapping; k++)
                            {
                           // Mark this shared and add the associated IR nodes sharing this token sequence. 
                              element->shared = true;
#if 0
                              printf ("Mark as shared and add node childAttributes[k=%d].node = %p = %s \n",k,childAttributes[k].node,childAttributes[k].node->class_name().c_str());
#endif
                              element->nodeVector.push_back(childAttributes[k].node);
#if 0
                              printf ("element->nodeVector.size() = %zu \n",element->nodeVector.size());
#endif
                              ROSE_ASSERT(element->nodeVector.size() == 2 + (k - starting_NodeSequenceWithoutTokenMapping));

                              printf ("This location is never reached! \n");
                              ROSE_ASSERT(false);
                            }
#if 0
                         printf ("******************** End of loop body for childrenWithoutTokenMappings (i = %d) ******************** \n",i);
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }

#if 0
                    printf ("In evaluateSynthesizedAttribute(): DONE processing childrenWithoutTokenMappings: current_node_token_subsequence_start = %d current_node_token_subsequence_end = %d \n",
                         current_node_token_subsequence_start,current_node_token_subsequence_end);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                 // Now with the token subsequences known, we need to unify any redundant subsequences.
                    for (size_t i = 0; i < tokenToNodeVector.size(); i++)
                       {
#if 0
                         printf ("In evaluateSynthesizedAttribute(): tokenToNodeVector[%zu] = %p \n",i,tokenToNodeVector[i]);
#endif
#if 0
                         TokenStreamSequenceToNodeMapping* mappingInfo = tokenToNodeVector[i];
                         printf ("   --- node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
                      // Need to define intervals and detect redundant intervals (based on token_subsequence_start and token_subsequence_end, and not the leading and trailing intervals).
                       }

#if 0
                    printf ("In evaluateSynthesizedAttribute(): need to handle collections of statements n = %p = %s \n",n,(n != NULL) ? n->class_name().c_str() : "null");
                    ROSE_ASSERT(false);
#endif
#if 0
                    if (tokenToNodeVector.size() > 2)
                       {
                         printf ("In evaluateSynthesizedAttribute(): need to handle collections of statements n = %p = %s \n",n,(n != NULL) ? n->class_name().c_str() : "null");
                         ROSE_ASSERT(false);
                       }
#endif
                  }
#if 0
               if (tokenToNodeVector.size() == 1)
                  {
                    printf ("In evaluateSynthesizedAttribute(): need to handle single statements n = %p = %s \n",n,(n != NULL) ? n->class_name().c_str() : "null");
                    ROSE_ASSERT(false);
                  }
#endif
#if 0
               printf ("In evaluateSynthesizedAttribute(): need to handle statements n = %p = %s \n",n,(n != NULL) ? n->class_name().c_str() : "null");
               ROSE_ASSERT(false);
#endif

            // DQ (12/31/2014): Handle the "else" syntax within the SgIfStmt.
               SgIfStmt* ifStatement = isSgIfStmt(n);
               if (ifStatement != NULL)
                  {
                 // trimLeadingWhiteSpaceFromLeft(mappingInfo,original_start_of_token_subsequence);
                 // trimTrailingWhiteSpaceFromRight(mappingInfo,original_end_of_token_subsequence);

                    for (size_t i = 0; i < tokenToNodeVector.size(); i++)
                       {
#if 0
                         printf ("In evaluateSynthesizedAttribute(): processing else sysntax for SgIfStmt: tokenToNodeVector[%zu] = %p \n",i,tokenToNodeVector[i]);
#endif
                         TokenStreamSequenceToNodeMapping* mappingInfo = tokenToNodeVector[i];
#if 0
                         printf ("   --- node = %p = %s \n",mappingInfo->node,mappingInfo->node->class_name().c_str());
#endif
                         ROSE_ASSERT(mappingInfo->node != NULL);

                         SgStatement* false_body_statement = isSgStatement(mappingInfo->node);
                         if (false_body_statement == ifStatement->get_false_body())
                            {
#if 0
                              printf ("$$$$$$$$$$$$ Handle special case of SgIfStmt = %p and the else syntax position \n",ifStatement);
#endif
                           // DQ (1/18/2015): We can't always assume this with the current normalization/denormalization support for SgBasicBlocks in SgIfStmt true and false branches.
                           // ROSE_ASSERT(i > 0);
                              if (i > 0)
                                 {
                                   TokenStreamSequenceToNodeMapping* previous_mappingInfo = tokenToNodeVector[i-1];
                                   ROSE_ASSERT(previous_mappingInfo != NULL);
#if 0
                                   printf ("   --- previous node = %p = %s \n",previous_mappingInfo->node,previous_mappingInfo->node->class_name().c_str());
#endif
                                   ROSE_ASSERT(previous_mappingInfo->node != NULL);

                                // Note that if there is a false body, then there must be a true body (so this should be easy to find as the previous child node).
                                   SgStatement* true_body_statement = isSgStatement(previous_mappingInfo->node);
                                   if (true_body_statement == ifStatement->get_true_body())
                                      {
#if 0
                                        printf ("Found true body: processing else syntax between true and false bodies \n");
#endif
                                        TokenStreamSequenceToNodeMapping* if_statement_mappingInfo  = tokenStreamSequenceMap[ifStatement];
                                        TokenStreamSequenceToNodeMapping* true_body_mappingInfo     = tokenStreamSequenceMap[true_body_statement];
                                        TokenStreamSequenceToNodeMapping* false_body_mappingInfo    = tokenStreamSequenceMap[false_body_statement];

                                        ROSE_ASSERT(if_statement_mappingInfo != NULL);
                                        ROSE_ASSERT(true_body_mappingInfo    != NULL);
                                        ROSE_ASSERT(false_body_mappingInfo   != NULL);

                                        discoverElseSyntax(if_statement_mappingInfo,true_body_mappingInfo,false_body_mappingInfo);
                                      }
#if 0
                                   printf ("Exiting as a test! (Handle special case of SgIfStmt = %p and the else syntax position) \n",ifStatement);
                                   ROSE_ASSERT(false);
#endif
                                 }
                                else
                                 {
#if 0
                                   printf ("In evaluateSynthesizedAttribute(): support for else syntax: false body in SgIfStmt does not have a previous TokenStreamSequenceToNodeMapping: ignoring this case \n");
                                   printf ("   --- false_body_statement = %p \n");
#endif
                                 }
                            }
                       }
                  }
             }
            else
             {
            // Only supporting statements in intial work, we are ignoring everything else.
#if 0
               printf ("In evaluateSynthesizedAttribute(): ignoring non-statements n = %p = %s \n",n,(n != NULL) ? n->class_name().c_str() : "null");
#endif
             }
        }

#if DEBUG_EVALUATE_SYNTHESIZED_ATTRIBUTE
     printf ("Leaving evaluateSynthesizedAttribute(): building SynthesizedAttribute(n): n = %p = %s childAttributes.size() = %zu \n",n,(n != NULL) ? n->class_name().c_str() : "null",childAttributes.size());
  // printf ("Leaving evaluateSynthesizedAttribute(): building SynthesizedAttribute(start_of_token_subsequence=%d,end_of_token_subsequence=%d,processed=%s): n = %p = %s \n",
  //      start_of_token_subsequence,end_of_token_subsequence,processed ? "true" : "false",n,n->class_name().c_str());
#endif

#if 0
  // DQ (1/21/2015): Debugging inputmover*_test2015_90.C
     if (isSgWhileStmt(n) != NULL)
        {
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
        }
#endif
#if 0
  // DQ (1/21/2015): Debugging inputmover*_test2015_90.C
     if (isSgFunctionDeclaration(n) != NULL)
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
          if (functionDeclaration == functionDeclaration->get_firstNondefiningDeclaration())
             {
               printf ("Exiting as a test! (found function prototype) \n");
               ROSE_ASSERT(false);
             }
        }
#endif
#if 0
     SgForStatement* forStatement = isSgForStatement(n->get_parent());
     if (forStatement != NULL)
        {
          if (n == forStatement->get_increment())
             {
            // evaluateForLoopIncrementExpression = true;
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }
        }
#endif

  // DQ (10/14/2013): Added consistancy test.
     consistancyCheck();

     return SynthesizedAttribute(n);
   }


InheritedAttribute
TokenMappingTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute)
   {
  // These are the bounds that we will increment and decrement to trim the size of the token subsequence of leading and trailing white space.
     int start_of_token_subsequence = inheritedAttribute.start_of_token_sequence;
     int end_of_token_subsequence   = inheritedAttribute.end_of_token_sequence;

  // Save the upper and lower bounds
     int original_start_of_token_subsequence = start_of_token_subsequence;
     int original_end_of_token_subsequence   = end_of_token_subsequence;

     bool processed = inheritedAttribute.processChildNodes;

#if 0
     printf ("In evaluateInheritedAttribute(): n = %p = %s original_start_of_token_subsequence = %d original_end_of_token_subsequence = %d processed = %s \n",
          n,n->class_name().c_str(),original_start_of_token_subsequence,original_end_of_token_subsequence,processed ? "true" : "false");
#endif

#if 0
     printf ("Rose::tokenSubsequenceMap.size() = %zu \n",Rose::tokenSubsequenceMap.size());
#endif

  // ROSE_ASSERT(include_sourceFile->get_tokenSubsequenceMap().find(include_sourceFile->get_globalScope()) != include_sourceFile->get_tokenSubsequenceMap().end());

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
     printf ("In evaluateInheritedAttribute(): n = %p = %s original_start_of_token_subsequence = %d original_end_of_token_subsequence = %d processed = %s \n",
          n,n->class_name().c_str(),original_start_of_token_subsequence,original_end_of_token_subsequence,processed ? "true" : "false");
     if (isSgClassDeclaration(n) != NULL)
        {
          printf ("   --- class name = %s \n",isSgClassDeclaration(n)->get_name().str());
        }
     if (isSgFunctionDeclaration(n) != NULL)
        {
          printf ("   --- function name = %s \n",isSgFunctionDeclaration(n)->get_name().str());
        }
     printf ("   --- start_of_token_subsequence          = %d end_of_token_subsequence          = %d \n",start_of_token_subsequence,end_of_token_subsequence);
     printf ("   --- original_start_of_token_subsequence = %d original_end_of_token_subsequence = %d \n",original_start_of_token_subsequence,original_end_of_token_subsequence);
#endif

  // DQ (10/14/2013): Added consistancy test.
     consistancyCheck();

     if (processed == false)
        {
       // Set to clear default values.
          start_of_token_subsequence = -1;
          end_of_token_subsequence   = -1;
        }

  // if (isSgFile(n) != NULL)
     if (isSgFile(n) != NULL || isSgGlobal(n) != NULL)
        {
       // This is where the token stream is attached, and by definition the whole token sequence represents the file.
       // The subsequence iterators are set above and should not be changed.

          int leading_whitespace_start  = -1;
          int leading_whitespace_end    = -1;

       // int token_subsequence_start   = start_of_token_subsequence;
       // int token_subsequence_end     = end_of_token_subsequence;
          int token_subsequence_start   = 0;
          int token_subsequence_end     = ((int)tokenStream.size()) - 1;
#if 0
          printf ("In evaluateInheritedAttribute(): case of SgFile or SgGlobal: token_subsequence_start = %d token_subsequence_end = %d \n",token_subsequence_start,token_subsequence_end);
#endif
          int trailing_whitespace_start = -1;
          int trailing_whitespace_end   = -1;

          int else_whitespace_start     = -1;
          int else_whitespace_end       = -1;

          start_of_token_subsequence = token_subsequence_start;
          end_of_token_subsequence   = token_subsequence_end;
#if 0
          printf ("case of SgFile or SgGlobal: token_subsequence_start = %d trailing_whitespace_end = %d \n",token_subsequence_start,trailing_whitespace_end);
#endif

       // Generate a unique TokenStreamSequenceToNodeMapping for each interval defined by (start_of_token_subsequence,end_of_token_subsequence).
       // TokenStreamSequenceToNodeMapping* element = new TokenStreamSequenceToNodeMapping(n,leading_whitespace_start,leading_whitespace_end,start_of_token_subsequence,end_of_token_subsequence,trailing_whitespace_start,trailing_whitespace_end);
          TokenStreamSequenceToNodeMapping* element = 
               TokenStreamSequenceToNodeMapping::createTokenInterval(n,
                    leading_whitespace_start,leading_whitespace_end,
                    start_of_token_subsequence,end_of_token_subsequence,
                    trailing_whitespace_start,trailing_whitespace_end,
                    else_whitespace_start,else_whitespace_end);

       // Add to vector (so that we can be the last element).  Note that we might be able to just lookup 
       // the element that we need instead of using the last element in the vector.
          tokenStreamSequenceVector.push_back(element);

       // Add to the map so that we have the final desired data structure (to attach to the SgSourceFile).
          tokenStreamSequenceMap[n] = element;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif

#if 0
       // DQ (9/25/2018): Check on the handling of the SgSourceFile (debugging header file unparsing with token stream unparsing).
          if (isSgSourceFile(n) != NULL)
             {
               printf ("Output the value of the element for the SgSourceFile \n");
               element->display("SgSourceFile element");

               SgSourceFile* sourceFile = isSgSourceFile(n);
               ROSE_ASSERT(sourceFile != NULL);

               printf ("sourceFile = %s \n",sourceFile->getFileName().c_str());

#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
#endif
        }
       else
        {
       // SgStatement* statement = isSgStatement(n);
       // SgStatement* locatedNode = isSgStatement(n);
          SgLocatedNode* locatedNode = isSgLocatedNode(n);
       // if (locatedNode != NULL)
          if ( (locatedNode != NULL) && (isSgInitializedName(n) == NULL) )
             {
               Sg_File_Info* start_pos = locatedNode->get_startOfConstruct();
               Sg_File_Info* end_pos   = locatedNode->get_endOfConstruct();
#if 0
            // DQ (12/14/2014): This is part of a bug fix where the ending position does not include the trailing ";" in EDG.
               SgForStatement* parent_is_forStatement = isSgForStatement(locatedNode->get_parent());
               if (parent_is_forStatement != NULL)
                  {
                    if (locatedNode == parent_is_forStatement->get_test())
                       {
                         printf ("Found a node where the end position is not properly represented in EDG \n");
                         ROSE_ASSERT(false);
                       }
                  }
#endif
#if 0
               printf ("start_pos->isFrontendSpecific() = %s \n",start_pos->isFrontendSpecific() ? "true" : "false");
#endif

            // DQ (8/1/2018): This fails for the combination of token based unparsing and unparse 
            // headers option (-rose:unparse_tokens -rose:unparseHeaderFiles).
               ROSE_ASSERT(start_pos != NULL);
               if (end_pos == NULL)
                  {
                    printf ("Error: end_pos == NULL: locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
                    start_pos->display("start_pos");
                    SgIncludeDirectiveStatement* includeDirectiveStatement = isSgIncludeDirectiveStatement(locatedNode);
                    if (includeDirectiveStatement != NULL)
                       {
                         printf ("includeDirectiveStatement: string = %p \n",includeDirectiveStatement->get_headerFileBody());
                       }
                  }
               ROSE_ASSERT(end_pos   != NULL);

               if (start_pos->isFrontendSpecific() == false)
                  {
                    ROSE_ASSERT(end_pos->isFrontendSpecific() == false);
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                    printf ("   --- locatedNode = %p = %s: start (line=%d:column=%d) end(line=%d,column=%d) start_of_token_subsequence = %d end_of_token_subsequence = %d \n",
                         n,n->class_name().c_str(),start_pos->get_physical_line(),start_pos->get_col(),end_pos->get_physical_line(),end_pos->get_col(),
                         start_of_token_subsequence,end_of_token_subsequence);
                    printf ("   --- locatedNode = %p = %s: start_pos->isCompilerGenerated() = %s inheritedAttribute.processChildNodes = %s \n",
                         n,n->class_name().c_str(),start_pos->isCompilerGenerated() ? "true" : "false", inheritedAttribute.processChildNodes ? "true" : "false");
                    printf ("   --- start_pos->isSourcePositionUnavailableInFrontend() = %s \n",start_pos->isSourcePositionUnavailableInFrontend() ? "true" : "false");

                 // DQ (12/21/2014): Debugging code.
                    if (start_pos->isCompilerGenerated() == false && start_pos->isSourcePositionUnavailableInFrontend() == false && start_pos->get_physical_line() == 0 && end_pos->get_physical_line() == 0)
                       {
                         printf ("   --- SageInterface::is_Fortran_language() = %s \n",SageInterface::is_Fortran_language() ? "true" : "false");
                         start_pos->display("get_physical_line() == 0 : debug");
                         end_pos->display("get_physical_line() == 0 : debug");
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
#endif

                 // int starting_line   = start_pos->get_line();
                    int starting_line   = start_pos->get_physical_line();
                    int starting_column = start_pos->get_col();
                 // int ending_line     = end_pos->get_line();
                    int ending_line     = end_pos->get_physical_line();
                    int ending_column   = end_pos->get_col();

                 // DQ (1/26/2015): Added as part of support for more general token mapping of subtrees with valid source position hiding behind compiler generated nodes (that have no source position).
                    bool subtreeHasValidSourcePosition = false;

                 // DQ (1/26/2015): Handle the case of compiler generated IR nodes and their possible mapping
                 // to the token stream (e.g. because they hide non-compiler generated subtrees).
#if 0
                    if ( start_pos->isCompilerGenerated() == true && 
                         isSgFunctionParameterList(n) == NULL && isSgTemplateInstantiationDecl(n) == NULL && isSgTemplateInstantiationDefn(n) == NULL && 
                         isSgTemplateInstantiationMemberFunctionDecl(n) == NULL && isSgCtorInitializerList(n) == NULL)
#else
                 // DQ (1/26/2015): Include the evaluation of the for loop increment expression (even if it is hiding behind a compiler generated IR node).
                    bool evaluateForLoopIncrementExpression = false;
                    SgForStatement* forStatement = isSgForStatement(n->get_parent());
                    if (forStatement != NULL)
                       {
                         if (n == forStatement->get_increment())
                            {
                              evaluateForLoopIncrementExpression = true;
                            }
                       }
#if 0
                    printf ("evaluateForLoopIncrementExpression = %s \n",evaluateForLoopIncrementExpression ? "true" : "false");
#endif
                 // Although I want to include the for loop increment expression, most are just SgExprStatement.
                 // if ( start_pos->isCompilerGenerated() == true && isSgExprStatement(n) != NULL)
                 // if ( start_pos->isCompilerGenerated() == true && (isSgExprStatement(n) != NULL || evaluateForLoopIncrementExpression == true) )
                    if ( start_pos->isCompilerGenerated() == true && (isSgExprStatement(n) != NULL || evaluateForLoopIncrementExpression == true) && (inheritedAttribute.processChildNodes == true) )
#endif
                       {
#if 0
                         printf ("Detected a compiler generated IR node: n = %p = %s \n",n,n->class_name().c_str());
                         printf ("   --- n->get_parent() = %p = %s \n",n->get_parent(),n->get_parent()->class_name().c_str());
                         printf ("Calling MaxSourceExtents::computeMaxSourceExtents(n=%p) \n",n);
#endif
                      // These are passed by reference to computeMaxSourceExtents().
                         int computed_start_line   = 0;
                         int computed_start_column = 0;
                         int computed_end_line     = 0; 
                         int computed_end_column   = 0;

                         MaxSourceExtents::computeMaxSourceExtents(inheritedAttribute.sourceFile,n,computed_start_line,computed_start_column,computed_end_line,computed_end_column);
                         if (computed_start_line == computed_end_line && computed_start_column == computed_end_column)
                            {
                           // DQ (1/26/2015): I think we would have to apply this to non-compiler generated statements to detect macros expansions (might be expensive).
                              printf ("Detected macro after evaluate of sub-tree source positions: computed_start_line = %d computed_start_column = %d computed_end_line = %d computed_end_column = %d \n",
                                   computed_start_line,computed_start_column,computed_end_line,computed_end_column);
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                           else
                            {
#if 0
                              printf ("Subtree is not a macro: computed_start_line = %d computed_start_column = %d computed_end_line = %d computed_end_column = %d \n",
                                   computed_start_line,computed_start_column,computed_end_line,computed_end_column);
#endif
                              subtreeHasValidSourcePosition = true;
                            }

                         starting_line   = computed_start_line;
                         starting_column = computed_start_column;
                         ending_line     = computed_end_line;
                         ending_column   = computed_end_column;
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }

                 // bool process_node = (start_pos->isCompilerGenerated() == false) && (isSgGlobal(n) == NULL) && (inheritedAttribute.processChildNodes == true);
                 // bool process_node = (start_pos->isCompilerGenerated() == false) && (inheritedAttribute.processChildNodes == true);
                    bool process_node = (start_pos->isCompilerGenerated() == false || subtreeHasValidSourcePosition == true) && (inheritedAttribute.processChildNodes == true);

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE || 0
                    printf ("   --- inheritedAttribute.processChildNodes = %s \n",inheritedAttribute.processChildNodes ? "true" : "false");
                    printf ("   --- (initial value 1) process_node = %s \n",process_node ? "true" : "false");
#endif
                    ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);
                 // process_node = (process_node == true) && (start_pos->isSameFile(inheritedAttribute.sourceFile));
                    process_node = (process_node == true) && (start_pos->isSameFile(inheritedAttribute.sourceFile) || subtreeHasValidSourcePosition == true);
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE || 0
                    printf ("   --- (initial value 2) process_node = %s \n",process_node ? "true" : "false");
                    printf ("   --- start_pos->isSameFile(inheritedAttribute.sourceFile) = %s \n",start_pos->isSameFile(inheritedAttribute.sourceFile) ? "true" : "false");
#endif
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE || 0
                    printf ("   --- start_pos->isCompilerGenerated() = %s \n",start_pos->isCompilerGenerated() ? "true" : "false");
                 // printf ("isSgGlobal(n) == NULL = %s \n",(isSgGlobal(n) == NULL) ? "true" : "false");
                    printf ("   --- (after reset): inheritedAttribute.processChildNodes = %s \n",inheritedAttribute.processChildNodes ? "true" : "false");
#endif
                 // DQ (10/30/2013): Not clear if this should be (starting_column < ending_column) or (starting_column <= ending_column).
                 // (Yes, this fixes empty statement (SgExprStatement with SgNullExpression) handling).
                 // It makes a difference for null statements (empty statements), but these are caught in the synthesized attributes as 
                 // childrenWithoutTokenMappings and processes as a special case.
                 // The global scope start and end positions are both set to 0, so it does not make since
                 // to process it except via the default (which is to attach the whole token sequence).
                 // process_node = (process_node == true) && ( (starting_line < ending_line) || ( (starting_line == ending_line) && (starting_column < ending_column) ) );
                    process_node = (process_node == true) && ( (starting_line < ending_line) || ( (starting_line == ending_line) && (starting_column <= ending_column) ) );
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE || 0
                    printf ("   --- (final value) process_node = %s \n",process_node ? "true" : "false");
                    printf ("starting_line = %d ending_line = %d starting_column = %d ending_column = %d process_node = %s \n",starting_line,ending_line,starting_column,ending_column,process_node ? "true" : "false");
#endif

                 // DQ (1/26/2015): This appears to be triggered by a SgNullStatement.
                    if (subtreeHasValidSourcePosition == true && process_node == false)
                       {
#if 0
                         printf ("WARNING: This does not make sense: (subtreeHasValidSourcePosition == true && process_node == false): n = %p = %s \n",n,n->class_name().c_str());
#endif
                      // printf ("ERROR: This does not make sense: (subtreeHasValidSourcePosition == true && process_node == false) \n");
                      // ROSE_ASSERT(false);
                       }
#if 1
                 // DQ (1/4/2014): commented out to test with using token based unparsing.

                 // DQ (12/22/2014): If this is part of an EDG normalization of template function definitions in template classes then it should not be processed.
                    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
                    if (functionDeclaration != NULL)
                       {
                         process_node = (process_node == true) && (functionDeclaration->isNormalizedTemplateFunction() == false);

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                         printf ("process_node = %s functionDeclaration->isNormalizedTemplateFunction() = %s \n",
                              process_node ? "true" : "false",functionDeclaration->isNormalizedTemplateFunction() ? "true" : "false");
#endif
                       }
#endif

                 // DQ (1/24/2015): Adding specialized for the case of "for ( ; ; )" (see test2015_97.C).
                    bool isNullForInitStatement = false;

                    if (process_node == true)
                       {
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                         printf ("In AST:         starting_line              = %d ending_line              = %d \n",starting_line,ending_line);
                         printf ("In tokenStream: start_of_token_subsequence = %d end_of_token_subsequence = %d \n",start_of_token_subsequence,end_of_token_subsequence);
                         printf ("BEFORE BEGIN LOOP: tokenStream[start_of_token_subsequence = %d]->beginning_fpi.line_num   = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
                         printf ("BEFORE BEGIN LOOP: tokenStream[start_of_token_subsequence = %d]->beginning_fpi.column_num = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.column_num);
                         printf ("BEFORE BEGIN LOOP: tokenStream[start_of_token_subsequence = %d]->p_tok_elem->token_lexeme = %s \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
#endif
                      // DQ (1/24/2015): Adding specialized for the case of "for ( ; ; )" (see test2015_97.C).
                      // This is done prior to the more general handling of special processing because it is 
                      // more than just narrowing the established bounds.
                         SgForInitStatement* forInitStatement = isSgForInitStatement(n);
                         if (forInitStatement != NULL)
                            {
#if 0
                              printf ("Detected forInitStatement \n");
#endif
                              ROSE_ASSERT(forInitStatement->get_init_stmt().empty() == false);
                              bool isNullStatement = isSgNullStatement(forInitStatement->get_init_stmt()[0]);
                              if (isNullStatement == true)
                                 {
                                   isNullForInitStatement = true;
#if 0
                                   printf ("Detected SgNullStatement in SgForInitStatement \n");
                                // ROSE_ASSERT(false);
#endif
                                 }
                            }

                      // DQ (1/24/2015): Handle the seperate case of a SgNullStatement in a SgForInitStatement.
                         if (isNullForInitStatement == true)
                            {
                           // while ( (*start_of_token_subsequence)->beginning_fpi.line_num < starting_line && start_of_token_subsequence != end_of_token_subsequence)
                           // while ( tokenStream[start_of_token_subsequence]->beginning_fpi.line_num < starting_line && start_of_token_subsequence <= end_of_token_subsequence)
                           // while ( (tokenStream[start_of_token_subsequence]->beginning_fpi.line_num < starting_line || 
                           //           (tokenStream[start_of_token_subsequence]->beginning_fpi.line_num == starting_line && tokenStream[start_of_token_subsequence]->beginning_fpi.column_num < starting_column))
                           //         && start_of_token_subsequence < end_of_token_subsequence)
                              while(tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != ";" && start_of_token_subsequence < end_of_token_subsequence)
                                 {
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE && 1
                                   printf ("TOP OF BEGIN LOOP (isNullForInitStatement == true): tokenStream[start_of_token_subsequence = %d]->beginning_fpi.line_num = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
#endif
                                   start_of_token_subsequence++;
                                   ROSE_ASSERT(start_of_token_subsequence <= end_of_token_subsequence);
// #if 0
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE && 1
                                   printf ("BOTTOM OF BEGIN LOOP (isNullForInitStatement == true): tokenStream[start_of_token_subsequence = %d]->beginning_fpi.line_num = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
#endif
                                 }

                              end_of_token_subsequence = start_of_token_subsequence;
                            }
                           else
                            {
                           // while ( (*start_of_token_subsequence)->beginning_fpi.line_num < starting_line && start_of_token_subsequence != end_of_token_subsequence)
                           // while ( tokenStream[start_of_token_subsequence]->beginning_fpi.line_num < starting_line && start_of_token_subsequence <= end_of_token_subsequence)
                              while ( (tokenStream[start_of_token_subsequence]->beginning_fpi.line_num < starting_line || 
                                        (tokenStream[start_of_token_subsequence]->beginning_fpi.line_num == starting_line && tokenStream[start_of_token_subsequence]->beginning_fpi.column_num < starting_column))
                                      && start_of_token_subsequence < end_of_token_subsequence)
                                 {
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE && 1
                                   printf ("TOP OF BEGIN LOOP: tokenStream[start_of_token_subsequence = %d]->beginning_fpi.line_num = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
#endif
                                   start_of_token_subsequence++;
                                   ROSE_ASSERT(start_of_token_subsequence <= end_of_token_subsequence);
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE && 1
                                   printf ("BOTTOM OF BEGIN LOOP: tokenStream[start_of_token_subsequence = %d]->beginning_fpi.line_num = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
#endif
                                 }
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                              printf ("AFTER BEGIN LOOP: tokenStream[start_of_token_subsequence = %d]->beginning_fpi.line_num = %d \n",start_of_token_subsequence,tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
#endif
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                              printf ("BEFORE END LOOP: tokenStream[end_of_token_subsequence = %d]->ending_fpi.line_num      = %d \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->ending_fpi.line_num);
                              printf ("BEFORE END LOOP: tokenStream[end_of_token_subsequence = %d]->ending_fpi.column_num    = %d \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->ending_fpi.column_num);
                              printf ("BEFORE END LOOP: tokenStream[end_of_token_subsequence = %d]->p_tok_elem->token_lexeme = %s \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
#endif
#if 0
                              int ending_token_line_number   = tokenStream[end_of_token_subsequence]->ending_fpi.line_num;
                              int ending_token_column_number = tokenStream[end_of_token_subsequence]->ending_fpi.column_num;
                              printf ("ending_token_line_number   = %d ending_line   = %d \n",ending_token_line_number,ending_line);
                              printf ("ending_token_column_number = %d ending_column = %d \n",ending_token_column_number,ending_column);
#endif
                           // while (tokenStream[end_of_token_subsequence]->ending_fpi.line_num > ending_line && end_of_token_subsequence >= start_of_token_subsequence && end_of_token_subsequence > 0)
                              while ( (tokenStream[end_of_token_subsequence]->ending_fpi.line_num > ending_line ||
                                        (tokenStream[end_of_token_subsequence]->ending_fpi.line_num == ending_line && tokenStream[end_of_token_subsequence]->ending_fpi.column_num > ending_column))
                                     && end_of_token_subsequence > start_of_token_subsequence && end_of_token_subsequence > 0)
                                 {
#if 0
                                   printf ("TOP OF END LOOP: tokenStream[end_of_token_subsequence = %d]->ending_fpi.line_num = %d \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->ending_fpi.line_num);
#endif
                                   end_of_token_subsequence--;
                                   ROSE_ASSERT(end_of_token_subsequence >= 0);
#if 0
                                   printf ("BOTTOM OF END LOOP: tokenStream[end_of_token_subsequence = %d]->ending_fpi.line_num = %d \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->ending_fpi.line_num);
#endif
                                 }

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                              printf ("AFTER END LOOP: tokenStream[end_of_token_subsequence = %d]->ending_fpi.line_num      = %d \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->ending_fpi.line_num);
                              printf ("AFTER END LOOP: tokenStream[end_of_token_subsequence = %d]->ending_fpi.column_num    = %d \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->ending_fpi.column_num);
                              printf ("AFTER END LOOP: tokenStream[end_of_token_subsequence = %d]->p_tok_elem->token_lexeme = %s \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
#endif
                            }

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                         printf ("Before special case adjustments in evaluateInheritedAttribute(): building InheritedAttribute(start_of_token_subsequence=%d,end_of_token_subsequence=%d,processed=%s): n = %p = %s \n",
                              start_of_token_subsequence,end_of_token_subsequence,processed ? "true" : "false",n,n->class_name().c_str());
#endif
#if 0
                         if (isNullForInitStatement == true)
                            {
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
                            }
#endif
                      // Fixup any mistakes in the processing, usually cased by bad source position information from EDG.
                      // Specific cases are:
                      //    1) Function prototypes (secondary declarations) have a source position that is typcally the 
                      //       start and end of the function name (missign the leading type and trailing function parameter list).
                      //    2) Variable declarations (missing the token for the trailing ';').
                      // These subsequences need to be fixed up on the way down (I think).

                      // DQ (1/24/2015): Add support for subexpressions containing "NULL" macro.
                         SgWhileStmt* parent_whileStatement = isSgWhileStmt(n->get_parent());
                         SgIfStmt* parent_ifStatement       = isSgIfStmt(n->get_parent());
                      // if (parent_whileStatement != NULL)
                         if (parent_whileStatement != NULL || parent_ifStatement)
                            {
                              SgStatement* s = isSgStatement(n);
                           // if (s != NULL && s == parent_whileStatement->get_condition())
                              if ( (s != NULL && parent_whileStatement != NULL && s == parent_whileStatement->get_condition()) ||
                                   (s != NULL && parent_ifStatement    != NULL && s == parent_ifStatement->get_conditional() ) )
                                 {
                                // Check if this needs to be extended to include macro tokens as part of a nested expression.
#if 0
                                   printf ("Handling NULL condition: start_of_token_subsequence = %d end_of_token_subsequence = %d \n",start_of_token_subsequence,end_of_token_subsequence);
                                   printf ("   --- tokenStream[end_of_token_subsequence = %d]->p_tok_elem->token_lexeme = %s \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
#endif
                                // DQ (1/25/2015): This is a more general interpretation for any macro name.  But it id only better, 
                                // not robust for more general macro recognition, we will need boost::wave to do a better job later.
                                // if (tokenStream[end_of_token_subsequence+1]->p_tok_elem->token_lexeme == "NULL")
                                   if (tokenStream[end_of_token_subsequence+1]->p_tok_elem->token_id == ROSE_token_ids::C_CXX_IDENTIFIER)
                                      {
                                        end_of_token_subsequence++;
                                      }
#if 0
                                   printf ("   --- After adjustment: tokenStream[end_of_token_subsequence = %d]->p_tok_elem->token_lexeme = %s \n",end_of_token_subsequence,tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
#endif
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }

                         SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
                         if (functionDeclaration != NULL)
                            {
                              if (functionDeclaration->get_definingDeclaration() != functionDeclaration)
                                 {
                                // Note: this needs to be moved to the evaluateSynthesizedAttribute function so that it can use the base 
                                // and bound of the adjacent sibling nodes to bound the expansion (for both leading and trailing directions).
#if 0
                                   printf ("NOTE: move fixup of start_of_token_subsequence and end_of_token_subsequence to evaluateSynthesizedAttribute() (SgFunctionDeclaration) \n");
#endif
                                // Then this is a secondary declaration (function prototype).
#if 0
                                   printf ("BEFORE LOOP: SgFunctionDeclaration: Adjusting the start of the token subsequence to find the leading ';' or '}': token = %s \n",tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                                   printf ("   --- original_start_of_token_subsequence = %d \n",original_start_of_token_subsequence);
#endif
                                // while ( (start_of_token_subsequence > original_start_of_token_subsequence) && ( (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != ";") && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "}") ) )
                                   if ( (start_of_token_subsequence > original_start_of_token_subsequence) && ( (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != ";") && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "}") ) )
                                      {
                                        while ( (start_of_token_subsequence > original_start_of_token_subsequence) && ( (tokenStream[start_of_token_subsequence-1]->p_tok_elem->token_lexeme != ";") && (tokenStream[start_of_token_subsequence-1]->p_tok_elem->token_lexeme != "}") ) )
                                           {
                                             start_of_token_subsequence--;
#if 0
                                             printf ("In LOOP: Adjusting the start of the token subsequence (backward) to find the leading ';' or '}': token = %s start_of_token_subsequence = %d \n",
                                                  tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                           }
#if 0
                                        printf ("BEFORE MOVE OFF: start_of_token_subsequence = %d original_end_of_token_subsequence = %d \n",start_of_token_subsequence,original_end_of_token_subsequence);
#endif
                                     // Move off of any possible non-whitespace token from an adjacent statement.
                                        if ( (start_of_token_subsequence < original_end_of_token_subsequence) && ( (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme == ";") || (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme == "}") ) )
                                           {
#if 0
                                             printf ("Move to the next token \n");
#endif
                                             start_of_token_subsequence++;
                                           }
#if 0
                                        printf ("start_of_token_subsequence = %d original_end_of_token_subsequence = %d \n",start_of_token_subsequence,original_end_of_token_subsequence);
#endif
                                        ROSE_ASSERT(tokenStream[start_of_token_subsequence+1] != NULL);
                                        ROSE_ASSERT(tokenStream[start_of_token_subsequence+1]->p_tok_elem != NULL);
#if 0
                                        printf ("tokenStream[start_of_token_subsequence+1]->p_tok_elem->token_id = %d \n",tokenStream[start_of_token_subsequence+1]->p_tok_elem->token_id);
                                        printf ("tokenStream[start_of_token_subsequence = %d]->p_tok_elem->token_id = %s \n",start_of_token_subsequence,Graph_TokenMappingTraversal::getTokenIdString(tokenStream[start_of_token_subsequence]->p_tok_elem->token_id).c_str());
                                        printf ("tokenStream[start_of_token_subsequence+1 = %d]->p_tok_elem->token_id = %s \n",start_of_token_subsequence+1,Graph_TokenMappingTraversal::getTokenIdString(tokenStream[start_of_token_subsequence+1]->p_tok_elem->token_id).c_str());
#endif
                                     // while ( (start_of_token_subsequence < original_end_of_token_subsequence) && ( (tokenStream[start_of_token_subsequence+1]->p_tok_elem->token_lexeme != "_") && (tokenStream[start_of_token_subsequence+1]->p_tok_elem->token_lexeme != "\n") ) )
                                        while ( (start_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE) )
                                           {
                                             start_of_token_subsequence++;
#if 0
                                             printf ("In LOOP: Adjusting the start of the token subsequence (forward) to find the leading ';' or '}': token = %s start_of_token_subsequence = %d \n",
                                                  tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                           }
                                      }

#if 0
                                   printf ("BEFORE LOOP: SgFunctionDeclaration: Adjusting the end of the token subsequence to find the trailing ';' token = %s \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                                   printf ("   --- original_end_of_token_subsequence = %d \n",original_end_of_token_subsequence);
#endif
                                // while ( (end_of_token_subsequence <= original_end_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != ";") )
                                   while ( (end_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != ";") )
                                      {
                                        end_of_token_subsequence++;
#if 0
                                        printf ("In LOOP: Adjusting the end of the token subsequence to find the trailing ';' token = %s end_of_token_subsequence = %d \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),end_of_token_subsequence);
#endif
                                      }
                                 }
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }

                         SgClassDefinition* classDefinition = isSgClassDefinition(n);
                         if (classDefinition != NULL)
                            {
#if 0
                           // printf ("Found case of SgBasicBlock: block = %p \n",block);
                              printf ("Found case of SgClassDefinition: classDefinition = %p \n",classDefinition);
#endif
#if 0
                              printf ("BEFORE LOOP: SgBasicBlock: Adjusting the start of the token subsequence to find the leading '{' token = %s \n",tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("BEFORE LOOP: SgBasicBlock: Adjusting the end of the token subsequence to find the trailing ';' token  = %s \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("   --- original_start_of_token_subsequence = %d start_of_token_subsequence = %d \n",original_start_of_token_subsequence,start_of_token_subsequence);
                              printf ("   --- original_end_of_token_subsequence = %d end_of_token_subsequence = %d \n",original_end_of_token_subsequence,end_of_token_subsequence);
#endif
                           // while ( (start_of_token_subsequence < original_start_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE || tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme == "{") )
                              int saved_start_of_token_subsequence = start_of_token_subsequence;
                              int saved_end_of_token_subsequence   = end_of_token_subsequence;
                              while ( (start_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "{") )
                                 {
                                   start_of_token_subsequence++;
#if 0
                                   printf ("In LOOP: Adjusting the start of the token subsequence (forward) to find the leading ';' or '{': token = %s start_of_token_subsequence = %d \n",
                                        tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                 }

                           // DQ (12/19/2014): If we didn't find the "{" then reset it back to it's original value (this is an issue 
                           // for some template types used in variable declarations in template declarations (iostream header file).
                           // See inputmoveDeclarationToInnermostScope_test2014_18.C
                              if (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "{")
                                 {
                                   start_of_token_subsequence = saved_start_of_token_subsequence;
                                 }

                           // while ( (end_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE || tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme == "}") )
                              while ( (end_of_token_subsequence > original_start_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != "}") )
                                 {
                                   end_of_token_subsequence--;
#if 0
                                   printf ("In LOOP: Adjusting the end of the token subsequence (backward) to find the trailing ';' or '}': token = %s start_of_token_subsequence = %d \n",
                                        tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),end_of_token_subsequence);
#endif
                                 }

                           // DQ (12/19/2014): If we didn't find the "{" then reset it back to it's original value (this is an issue 
                           // for some template types used in variable declarations in template declarations (iostream header file).
                           // See inputmoveDeclarationToInnermostScope_test2014_18.C
                              if (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "{")
                                 {
                                   start_of_token_subsequence = saved_start_of_token_subsequence;
                                 }

                              if (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != "}")
                                 {
                                   end_of_token_subsequence = saved_end_of_token_subsequence;
                                 }

#if 0
                              printf ("AFTER LOOP:   --- start_of_token_subsequence = %d \n",start_of_token_subsequence);
                              printf ("AFTER LOOP:   --- end_of_token_subsequence   = %d \n",end_of_token_subsequence);
#endif
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }

                         SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(n);
                         SgTypedefDeclaration*  typedefDeclaration  = isSgTypedefDeclaration(n);

                      // DQ (11/30/2013): Added support for SgEnumDeclaration and SgClassDeclaration.
                         SgEnumDeclaration*     enumDeclaration     = isSgEnumDeclaration(n);
                         SgClassDeclaration*    classDeclaration    = isSgClassDeclaration(n);

                      // if (variableDeclaration != NULL || typedefDeclaration != NULL)
                         if (variableDeclaration != NULL || typedefDeclaration != NULL || enumDeclaration != NULL || classDeclaration != NULL)
                            {
                           // Note: this needs to be moved to the evaluateSynthesizedAttribute function so that it can use the base 
                           // and bound of the adjacent sibling nodes to bound the expansion (for both leading and trailing directions).
// #if 0
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                              printf ("NOTE: move fixup of start_of_token_subsequence and end_of_token_subsequence to evaluateSynthesizedAttribute() (n = %s) \n",n->class_name().c_str());
#endif
                           // Then this is a secondary declaration (function prototype).
// #if 0
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                              printf ("BEFORE LOOP: n=%s: Adjusting the end of the token subsequence to find the trailing ';' token = %s \n",n->class_name().c_str(),tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("   --- original_end_of_token_subsequence = %d \n",original_end_of_token_subsequence);
#endif
                           // while ( (end_of_token_subsequence <= original_end_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != ";") )
                              while ( (end_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != ";") )
                                 {
                                   end_of_token_subsequence++;
// #if 1
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                                   printf ("In LOOP: Adjusting the end of the token subsequence to find the trailing ';' token = %s end_of_token_subsequence = %d \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),end_of_token_subsequence);
#endif
                                 }
#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                              printf ("special case of variable, typedef, enum, or class: start_of_token_subsequence=%d end_of_token_subsequence=%d): n = %p = %s \n",
                                   start_of_token_subsequence,end_of_token_subsequence,n,n->class_name().c_str());
#endif
                            }

                      // DQ (12/14/2014): This is part of a bug fix where the ending position does not include the trailing ";" in EDG.
                         SgForStatement* parent_is_forStatement = isSgForStatement(n->get_parent());
                         if (parent_is_forStatement != NULL && n == parent_is_forStatement->get_test())
                            {
#if 0
                              printf ("Found the test statement in a SgForStatement (ending must be fixed to include semi-colon) \n");
#endif
#if 0
                              printf ("BEFORE LOOP: n=%s: Adjusting the end of the token subsequence to find the trailing ';' token = %s \n",n->class_name().c_str(),tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("   --- original_end_of_token_subsequence = %d \n",original_end_of_token_subsequence);
#endif
                              while ( (end_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme != ";") )
                                 {
                                   end_of_token_subsequence++;
#if 0
                                   printf ("In LOOP: Adjusting the end of the token subsequence to find the trailing ';' token = %s end_of_token_subsequence = %d \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),end_of_token_subsequence);
#endif
                                 }
                            }

                      // DQ (12/18/2014): Improve the representation of the leading tken sequence for the SgNamespaceDeclarationStatement
                      // (should start at after the function parameter list's closing ")" instead of at the start of the 
                      // fundection declaration).
                         SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(n);
                         if (namespaceDefinition != NULL)
                            {
                           // adjust the start_of_token_subsequence
#if 0
                              printf ("Found case of SgNamespaceDefinitionStatement: namespaceDefinition = %p \n",namespaceDefinition);
#endif
#if 0
                              printf ("BEFORE LOOP: SgNamespaceDefinitionStatement: Adjusting the start of the token subsequence to find the leading '{' token = %s \n",tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("BEFORE LOOP: SgNamespaceDefinitionStatement: Adjusting the end of the token subsequence to find the trailing ';' token  = %s \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("   --- original_start_of_token_subsequence = %d start_of_token_subsequence = %d \n",original_start_of_token_subsequence,start_of_token_subsequence);
                              printf ("   --- original_end_of_token_subsequence = %d end_of_token_subsequence = %d \n",original_end_of_token_subsequence,end_of_token_subsequence);
#endif
                           // while ( (start_of_token_subsequence < original_start_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_id == C_CXX_WHITESPACE || tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme == "{") )
                              while ( (start_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "{") )
                                 {
                                   start_of_token_subsequence++;
#if 0
                                   printf ("In LOOP: Adjusting the start of the token subsequence (forward) to find the leading ';' or '{': token = %s start_of_token_subsequence = %d \n",
                                        tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                 }
                            }

                      // DQ (12/27/2014): Handling the case of a SgBasicBlock in a SgSwitchStatement. This is because EDG does not 
                      // represent this specific case well (except in terms of the position os the case constant expressions).
                      // So we have to start there and backup to the first "{".
                         SgBasicBlock* basicBlock = isSgBasicBlock(locatedNode);
                         if (basicBlock != NULL)
                            {
                              SgSwitchStatement* switchStatement = isSgSwitchStatement(locatedNode->get_parent());
                              if (switchStatement != NULL)
                                 {
#if 0
                                   printf ("Setup the starting position of the SgBasicBlock = %p contained in a SgSwitchStatement = %p \n",basicBlock,switchStatement);
#endif
#if 0
                                   printf ("BEFORE LOOP: SgBasicBlock in SgSwitchStatement: Adjusting the start of the token subsequence to find the leading '{' token = %s \n",tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                                   printf ("BEFORE LOOP: SgBasicBlock in SgSwitchStatement: Adjusting the end of the token subsequence to find the trailing ';' token  = %s \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                                   printf ("   --- original_start_of_token_subsequence = %d start_of_token_subsequence = %d \n",original_start_of_token_subsequence,start_of_token_subsequence);
                                   printf ("   --- original_end_of_token_subsequence   = %d end_of_token_subsequence   = %d \n",original_end_of_token_subsequence,end_of_token_subsequence);
#endif
                                // while ( (start_of_token_subsequence < original_end_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "{") )
                                   while ( (start_of_token_subsequence > original_start_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "{") )
                                      {
                                     // start_of_token_subsequence++;
                                        start_of_token_subsequence--;
#if 0
                                        printf ("In LOOP: Adjusting the start of the token subsequence (backward) to find the leading ';' or '{': token = %s start_of_token_subsequence = %d \n",
                                             tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                      }

#if 0
                                   printf ("AFTER LOOP: Adjusting the start of the token subsequence (backward) to find the leading ';' or '{': token = %s start_of_token_subsequence = %d \n",
                                        tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                // DQ (12/28/2014): Report where token stream might be more accurate than the token based representation.
                                   int token_line_number     = tokenStream[start_of_token_subsequence]->beginning_fpi.line_num;
                                   int construct_line_number = basicBlock->get_startOfConstruct()->get_line();
                                   if (token_line_number < construct_line_number)
                                      {
                                        printf ("NOTE: basicBlock->get_startOfConstruct()->get_line() = %d not reset to token line = %d \n",construct_line_number,token_line_number);
                                      }
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }

                      // DQ (12/28/2014): As a result of setting the source positon for the switch body more accruately, I think this adjustment is no longer required.
                      // DQ (12/27/2014): Handling the case of a SgCaseOptionStmt in a SgSwitchStatement. This is because EDG does not 
                      // represent this specific case well (except in terms of the position os the case constant expressions).
                      // So we have to start there and backup to the first "case" keyword.
                         SgCaseOptionStmt* caseOptionStatement = isSgCaseOptionStmt(locatedNode);
                         if (caseOptionStatement != NULL)
                            {
#if 0
                              printf ("Setup the starting position of the SgCaseOptionStatement = %p \n",caseOptionStatement);
#endif
#if 0
                              printf ("BEFORE LOOP: SgCaseOptionStatement: Adjusting the start of the token subsequence to find the leading '{' token = %s \n",tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("BEFORE LOOP: SgCaseOptionStatement: Adjusting the end of the token subsequence to find the trailing ';' token  = %s \n",tokenStream[end_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                              printf ("   --- original_start_of_token_subsequence = %d start_of_token_subsequence = %d \n",original_start_of_token_subsequence,start_of_token_subsequence);
                              printf ("   --- original_end_of_token_subsequence   = %d end_of_token_subsequence   = %d \n",original_end_of_token_subsequence,end_of_token_subsequence);
#endif
                              while ( (start_of_token_subsequence > original_start_of_token_subsequence) && (tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != "case") )
                                 {
                                   start_of_token_subsequence--;
#if 0
                                   printf ("In LOOP: Adjusting the start of the token subsequence (forward) to find the leading ';' or '{': token = %s start_of_token_subsequence = %d \n",
                                        tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str(),start_of_token_subsequence);
#endif
                                 }
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
#if 0
                      // DQ (12/15/2014): I think this makes more sense to process on the way back up (in the synthesied attribute).
                      // DQ (12/15/2014): Improve the representation of the leading whitespace for the SgFunctionDefinition 
                      // (should start at after the function parameter list's closing ")" instead of at the start of the 
                      // fundection declaration).
                         SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(n);
                         if (functionDefinition != NULL)
                            {
                              SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionDefinition->get_parent());
                              ROSE_ASSERT(functionDeclaration != NULL);

                           // start_of_token_subsequence
                            }
#endif
                       }
                      else
                       {
                      // Turn off further processing of this now and it's subtree.
                         processed = false;

                      // Set to clear default values.
                         start_of_token_subsequence = -1;
                         end_of_token_subsequence   = -1;
#if 0
                         printf ("Skipping this SgLocatedNode: n = %p = %s \n",n,n->class_name().c_str());
#endif
                       }

                 // DQ (1/2/2015): Added support for the increment expression (even though it is not a SgStatement).
                    SgExpression* forStatementIncrementExpression = NULL;
                    SgExpression* expression = isSgExpression(locatedNode);
                    if (expression != NULL)
                       {
                      // Check if this is the increment in the SgForStmt.
                         SgForStatement* forStatement = isSgForStatement(locatedNode->get_parent());
                         if (forStatement != NULL && forStatement->get_increment() == expression)
                            {
#if 0
                              printf ("@@@@@@@@@@@@ Detected for loop increment expression: expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
                              forStatementIncrementExpression = expression;
                            }
                       }

                 // DQ (10/6/2013): Exclude the SgFunctionParameterList
                 // if (isSgStatement(n) != NULL)
                 // if (isSgStatement(n) != NULL && isSgFunctionParameterList(n) == NULL)
                    if ( (isSgStatement(n) != NULL && isSgFunctionParameterList(n) == NULL) || forStatementIncrementExpression != NULL)
                       {
#if 0
                         printf ("In evaluateInheritedAttribute(): n = %p = %s returning InheritedAttribute with start_of_token_subsequence = %d end_of_token_subsequence = %d \n",n,n->class_name().c_str(),start_of_token_subsequence,end_of_token_subsequence);
#endif
                      // Disallow the default value: -1
                         if (start_of_token_subsequence >= 0)
                            {
#if 0
                              printf ("   --- token string = ");
                              for (int i = start_of_token_subsequence; i <= end_of_token_subsequence; i++)
                                 {
                                   printf ("%s",tokenStream[i]->p_tok_elem->token_lexeme.c_str());
                                 }
                              printf ("\n");
#endif
                           // Save the subsequence associated with the of the token stream for this IR node.
                           // tokenStreamSequenceMap[n] = pair<int,int>(start_of_token_subsequence,end_of_token_subsequence);
                           // tokenStreamSequenceVector.push_back(pair<SgNode*,pair<int,int> >(n,pair<int,int>(start_of_token_subsequence,end_of_token_subsequence)));

                              int leading_whitespace_start  = -1;
                              int leading_whitespace_end    = -1;
                              int trailing_whitespace_start = -1;
                              int trailing_whitespace_end   = -1;
                              int else_whitespace_start     = -1;
                              int else_whitespace_end       = -1;

                           // DQ (12/6/2016): Need to enforce this to support fix for warning below.
                              ROSE_ASSERT(end_of_token_subsequence >= -1);

                           // DQ (12/6/2016): Fixing earnings now considered to be errors.
                              ROSE_ASSERT(end_of_token_subsequence == -1 || (size_t)end_of_token_subsequence < tokenStream.size());

                           // Generate a unique TokenStreamSequenceToNodeMapping for each interval defined by (start_of_token_subsequence,end_of_token_subsequence).
                           // TokenStreamSequenceToNodeMapping* element = new TokenStreamSequenceToNodeMapping(n,leading_whitespace_start,leading_whitespace_end,start_of_token_subsequence,end_of_token_subsequence,trailing_whitespace_start,trailing_whitespace_end);
                              TokenStreamSequenceToNodeMapping* element = 
                                   TokenStreamSequenceToNodeMapping::createTokenInterval(n,
                                        leading_whitespace_start,leading_whitespace_end,
                                        start_of_token_subsequence,end_of_token_subsequence,
                                        trailing_whitespace_start,trailing_whitespace_end,
                                        else_whitespace_start,else_whitespace_end);

                           // Add to vector (so that we can be the last element).  Note that we might be able to just lookup 
                           // the element that we need instead of using the last element in the vector.
                              tokenStreamSequenceVector.push_back(element);

                           // Add to the map so that we have the final desired data structure (to attach to the SgSourceFile).
                              tokenStreamSequenceMap[n] = element;
#if 0
                              printf ("Add TokenStreamSequenceToNodeMapping into vector for n = %p = %s tokenStreamSequenceVector.size() = %zu \n",n,n->class_name().c_str(),tokenStreamSequenceVector.size());
#endif
                           // DQ (1/14/2015): Insert test for macro (but make sure it is not a ";").  We want to have processed this IR node, 
                           // but we want to supress the processing of child IR nodes.  Child IR nodes could be nested statements in a complex
                           // macro expansion for which we can't define an associated token mapping.
                           // if ( (starting_line == ending_line) && (starting_column == ending_column) )
                           // if (start_of_token_subsequence == end_of_token_subsequence)
                              if (isNullForInitStatement == false && start_of_token_subsequence == end_of_token_subsequence)
                                 {
#if 0
                                   printf ("Detected a macro (disable processing on children) \n");
                                   printf ("   --- statement: n = %p = %s \n",n,n->class_name().c_str());
                                   printf ("   --- starting_line = %d starting_column = %d \n",starting_line,starting_column);
                                   printf ("   --- ending_line   = %d ending_column   = %d \n",ending_line,ending_column);
                                   printf ("   --- start_of_token_subsequence = %d \n",start_of_token_subsequence);
                                   printf ("   --- end_of_token_subsequence   = %d \n",end_of_token_subsequence);
                                   if (start_of_token_subsequence >= 0)
                                      {
                                        printf ("  --- tokenStream[start_of_token_subsequence]->beginning_fpi.line_num   = %d \n",tokenStream[start_of_token_subsequence]->beginning_fpi.line_num);
                                        printf ("  --- tokenStream[start_of_token_subsequence]->beginning_fpi.column_num = %d \n",tokenStream[start_of_token_subsequence]->beginning_fpi.column_num);
                                        printf ("  --- tokenStream[end_of_token_subsequence]->ending_fpi.line_num        = %d \n",tokenStream[end_of_token_subsequence]->ending_fpi.line_num);
                                        printf ("  --- tokenStream[end_of_token_subsequence]->ending_fpi.column_num      = %d \n",tokenStream[end_of_token_subsequence]->ending_fpi.column_num);
                                        printf ("   --- token = %s \n",tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str());
                                      }
#endif
                                   if (start_of_token_subsequence != end_of_token_subsequence)
                                      {
                                        printf ("Error: start_of_token_subsequence != end_of_token_subsequence: \n");
                                        locatedNode->get_startOfConstruct()->display("START: debug");
                                        locatedNode->get_startOfConstruct()->display("END: debug");
                                      }
                                   ROSE_ASSERT(start_of_token_subsequence == end_of_token_subsequence);

                                // if (start_of_token_subsequence >= 0 && tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme.c_str() != ";")
                                   if (start_of_token_subsequence >= 0 && tokenStream[start_of_token_subsequence]->p_tok_elem->token_lexeme != ";")
                                      {
#if 0
                                        printf ("Reset the processing value to false (to eliminate mapping children (child statements in a macro expansion) to the token stream) \n");
#endif
                                        processed = false;
                                      }
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }
                           else
                            {
#if 0
                              printf ("Not Handled: start_of_token_subsequence < 0: n = %p = %s \n",n,n->class_name().c_str());
#endif
                            }

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
                         printf ("end of adjustments in evaluateInheritedAttribute(): building InheritedAttribute(start_of_token_subsequence=%d,end_of_token_subsequence=%d,processed=%s): n = %p = %s \n",
                              start_of_token_subsequence,end_of_token_subsequence,processed ? "true" : "false",n,n->class_name().c_str());
#endif
                       }
                      else
                       {
#if 0
                         printf ("Not Handled: This is not a statement: n = %p = %s \n",n,n->class_name().c_str());
#endif
                       }
                  }
                 else
                  {
                 // DQ (8/1/2018): This fails for the combination of token based unparsing and unparse headers option.
                    ROSE_ASSERT(end_pos != NULL);

                    ROSE_ASSERT(end_pos->isFrontendSpecific() == true);
#if 0
                    printf ("Not Handled: This is a front-end specific SgLocatedNode: n = %p = %s \n",n,n->class_name().c_str());
#endif
                  }
             }
            else
             {
#if 0
               printf ("Not Handled: This is not a SgLocatedNode (or could be a SgInitializedName): n = %p = %s \n",n,n->class_name().c_str());
#endif
             }
#if 0
          SgExpression* expresion = isSgExpression(n);
          if (expresion != NULL)
             {
            // Ignore expressions for now since mapping the expressions to this level is at least more complex.
             }
#endif
        }

#if DEBUG_EVALUATE_INHERITATE_ATTRIBUTE
     printf ("Leaving evaluateInheritedAttribute(): building InheritedAttribute(start_of_token_subsequence=%d,end_of_token_subsequence=%d,processed=%s): n = %p = %s \n",
          start_of_token_subsequence,end_of_token_subsequence,processed ? "true" : "false",n,n->class_name().c_str());
#endif

     if (start_of_token_subsequence > end_of_token_subsequence)
        {
          printf ("Error: evaluateInheritedAttribute(): n = %p = %s \n",n,n->class_name().c_str());
          SgLocatedNode* locatedNode = isSgLocatedNode(n);
          if (locatedNode != NULL)
             {
               locatedNode->get_startOfConstruct()->display("Error: evaluateInheritedAttribute(): start");
               locatedNode->get_endOfConstruct()->display("Error: evaluateInheritedAttribute(): end");
             }
        }

#if 1
  // DQ (11/24/2018): Error that I need to debug.
     if (start_of_token_subsequence > end_of_token_subsequence)
        {
          printf ("ERROR: Failing test: (start_of_token_subsequence <= end_of_token_subsequence): \n");
          printf ("   --- start_of_token_subsequence = %d \n",start_of_token_subsequence);
          printf ("   --- end_of_token_subsequence   = %d \n",end_of_token_subsequence);
        }
#endif

     ROSE_ASSERT(start_of_token_subsequence <= end_of_token_subsequence);

  // DQ (12/6/2016): Added assertion as part of fix for warning now considered to be an error.
     ROSE_ASSERT(end_of_token_subsequence >= -1);
     ROSE_ASSERT(end_of_token_subsequence == -1 || (size_t)end_of_token_subsequence < tokenStream.size());

     ROSE_ASSERT(inheritedAttribute.sourceFile != NULL);

  // DQ (10/14/2013): Added consistancy test.
     consistancyCheck();

#if 0
  // DQ (12/14/2014): This is part of a bug fix where the ending position does not include the trailing ";" in EDG.
     SgForStatement* parent_is_forStatement = isSgForStatement(n->get_parent());
     if (parent_is_forStatement != NULL)
        {
          if (n == parent_is_forStatement->get_test())
             {
               printf ("Found a node where the end position is not properly represented in EDG \n");
               ROSE_ASSERT(false);
             }
        }
#endif


  // Select the representative statement to use in formatting transformations in the token based unparsing.
     SgScopeStatement* scopeStatement = isSgScopeStatement(n);
     if (scopeStatement != NULL)
        {
       // Save a statement from each scope.
          SgGlobal* globalScope                               = isSgGlobal(scopeStatement);
          SgBasicBlock* basicBlock                            = isSgBasicBlock(scopeStatement);
          SgClassDefinition* classDefinition                  = isSgClassDefinition(scopeStatement);
          SgNamespaceDefinitionStatement* namespaceDefinition = isSgNamespaceDefinitionStatement(scopeStatement);

          if (globalScope != NULL || basicBlock != NULL || classDefinition != NULL || namespaceDefinition != NULL)
             {
               SgStatement* firstStatement = NULL;

            // Note that this is the efficent way to access the first statement in any scope containing a list of statements or declarations.
               if (scopeStatement->containsOnlyDeclarations() == true)
                  {
                    if (scopeStatement->getDeclarationList().empty() == false)
                       {
                         SgDeclarationStatement* firstDeclaration = *(scopeStatement->getDeclarationList().begin());
                         firstStatement = firstDeclaration;
                       }
                      else
                       {
                      // Not clear what to do here.
                       }
                  }
                 else
                  {
                    if (scopeStatement->getStatementList().empty() == false)
                       {
                         firstStatement = *(scopeStatement->getStatementList().begin());
                       }
                      else
                       {
                      // Not clear what to do here.
                       }
                  }

            // ROSE_ASSERT(firstStatement != NULL);

               if (firstStatement != NULL)
                  {
                    ROSE_ASSERT(scopeStatement != NULL);
#if 0
                    printf ("Adding representativeWhitespaceStatementMap[%p = %s] = %p = %s \n",scopeStatement,scopeStatement->class_name().c_str(),firstStatement,firstStatement->class_name().c_str());
#endif
                    if (representativeWhitespaceStatementMap.find(scopeStatement) != representativeWhitespaceStatementMap.end())
                       {
                      // DQ (11/28/2015): This is a significant amount of output spew when running large applications with the move-tool.
                         printf ("NOTE: (representativeWhitespaceStatementMap.find(scopeStatement) != representativeWhitespaceStatementMap.end()): scope revisited \n");
#if 0
                         scopeStatement->get_startOfConstruct()->display("scopeStatement: representativeWhitespaceStatementMap: debug");
                         firstStatement->get_startOfConstruct()->display("firstStatement: representativeWhitespaceStatementMap: debug");
#endif
                       }
#if 0
                    ROSE_ASSERT(representativeWhitespaceStatementMap.find(scopeStatement) == representativeWhitespaceStatementMap.end());
                    representativeWhitespaceStatementMap[scopeStatement] = firstStatement;
#else
                 // Allow this case while we debug this.
                    if (representativeWhitespaceStatementMap.find(scopeStatement) == representativeWhitespaceStatementMap.end())
                       {
                         representativeWhitespaceStatementMap[scopeStatement] = firstStatement;
                       }
#endif
                  }
             } 
        }

     return InheritedAttribute(inheritedAttribute.sourceFile,start_of_token_subsequence,end_of_token_subsequence,processed);
   }


TokenMappingTraversal::TokenMappingTraversal(vector<stream_element*> & ts, bool input_useTokenSequenceToImproveSourcePositionInfo)
   : tokenStream(ts),
  // DQ (12/11/2015): Adding capability for token sequence to update the source position info in IR nodes.
     useTokenSequenceToImproveSourcePositionInfo(input_useTokenSequenceToImproveSourcePositionInfo)
   {
#if 0
     printf ("tokenStream.size() = %zu \n",tokenStream.size());
#endif

     ROSE_ASSERT(tokenStream.empty() == false);
   }


void 
ReplaceStringInPlace(std::string& subject, const std::string& search, const std::string& replace) 
   {
     size_t pos = 0;
     while ((pos = subject.find(search,pos)) != std::string::npos)
        {
          subject.replace(pos,search.length(),replace);

       // This might be a problem if we replaces "x" with "x".
       // pos += replace.length();
          pos = 0;
        }
   }


string
TokenMappingTraversal::generateTokenSubsequence( int start, int end)
   {
     string returnString;

     if (start < 0)
        {
          ROSE_ASSERT(start == -1);
          ROSE_ASSERT(end   == -1);
          return returnString;
        }
       else
        {
       // DQ (12/6/2016): Added assertion as part of fix for warning now considered to be an error.
       // if (end != -1 && end >= tokenStream.size())
          ROSE_ASSERT(end >= -1);
          if (end != -1 && (size_t)end >= tokenStream.size())
             {
               printf ("Error: In generateTokenSubsequence(): start = %d end = %d tokenStream.size() = %zu \n",start,end,tokenStream.size());
             }
          ROSE_ASSERT(end == -1 || (size_t)end < tokenStream.size());

#if 0
          for (int j = start; j <= end; j++)
             {
            // printf ("%s",tokenStream[j]->p_tok_elem->token_lexeme.c_str());
               returnString += tokenStream[j]->p_tok_elem->token_lexeme.c_str();
             }
#else
          for (int j = start; j <= end; j++)
             {
               if (tokenStream[j] == NULL)
                  {
                    printf ("Error: tokenStream[j=%d] == NULL: tokenStream.size() = %zu \n",j,tokenStream.size());
                  }
               ROSE_ASSERT(tokenStream[j] != NULL);
               ROSE_ASSERT(tokenStream[j]->p_tok_elem != NULL);

               string s = tokenStream[j]->p_tok_elem->token_lexeme.c_str();

            // remove "\n" with " " so that we get a single line (better output visually).
            // replace(s.begin(),s.end(),'\n',' ');

            // remove redundant spaces
            // ReplaceStringInPlace(s,"  "," ");

            // replace(s.begin(),s.end(),'\n',' ');

            // printf ("%s",s.c_str());
            // printf ("%s",escapeString(s).c_str());

               returnString += escapeString(s);
             }
#endif
        }

     return returnString;
   }


void
TokenMappingTraversal::outputTokenStreamSequenceMap()
   {
  // Check for unassigned tokens that are not white space.

#if 0
     printf ("\n\nIn outputTokenStreamSequenceMap(): check for tokens between assigned token sequences associated with IR nodes \n");
#endif

  // map<SgNode*,pair<int,int> >::iterator i = tokenStreamSequenceMap.begin();
  // vector<pair<SgNode*,pair<int,int> > > tokenStreamSequenceVector;

  // vector<pair<SgNode*,pair<int,int> > >::iterator i = tokenStreamSequenceVector.begin();
     vector<TokenStreamSequenceToNodeMapping*>::iterator i = tokenStreamSequenceVector.begin();

  // The map is the more useful data structure longer term, but we need the tokenStreamSequenceVector 
  // to build the initial sequence (though a better approach might not need the tokenStreamSequenceVector
  // data strcuture and could maybe use the tokenStreamSequenceMap exclusively.

  // DQ (12/3/2014): Make this an error message (failes for amr/Coarsen_particles.cc).
  // ROSE_ASSERT(tokenStreamSequenceMap.size() == tokenStreamSequenceVector.size());
     if (tokenStreamSequenceMap.size() != tokenStreamSequenceVector.size())
        {
          printf ("ERROR: TokenMappingTraversal::outputTokenStreamSequenceMap(): tokenStreamSequenceMap.size() != tokenStreamSequenceVector.size() \n");
          printf ("   --- tokenStreamSequenceMap.size()    = %zu \n",tokenStreamSequenceMap.size());
          printf ("   --- tokenStreamSequenceVector.size() = %zu \n",tokenStreamSequenceVector.size());
        }

  // This is a count of tokens in the SgSourceFile and SgGlobal, but not represented by the nested 
  // statements that are contained in the SgGlobal.
     int unaccountedForTokenSubsequences = 0;

  // int previous_end = 0;
     while (i != tokenStreamSequenceVector.end())
        {
       // SgNode* node = i->first;
       // int tokenStream_start = i->second.first;
       // int tokenStream_end = i->second.second;
          SgNode* node          = (*i)->node;

       // DQ (9/26/2018): Using macro DEBUG_TOKEN_OUTPUT.
#if DEBUG_TOKEN_OUTPUT
          int tokenStream_start = (*i)->token_subsequence_start;
          int tokenStream_end   = (*i)->token_subsequence_end;
          printf ("In outputTokenStreamSequenceMap(): node = %p = %s tokenStream_start = %d tokenStream_end = %d \n",node,node->class_name().c_str(),tokenStream_start,tokenStream_end);
#endif

       // if ( (tokenStream_start - previous_end) > 1)
       // if (isSgSourceFile(node) != NULL || isSgGlobal(node) != NULL)
          if (isSgSourceFile(node) != NULL)
             {
            // These nodes don't have properly set starting and ending source position information.
             }
            else
             {
            // Output the tokens between the end of the last token and the start of the current token.
            // printf ("\n\nSpace before node = %p = %s tokens previous_end = %d to tokenStream_start-1 = %d \n",node,node->class_name().c_str(),previous_end,tokenStream_start-1);
            // int node_start_line   = node->get_startOfConstruct()->get_line();

               int trailing_whitespace_end   = (*i)->trailing_whitespace_end;

       // DQ (9/26/2018): Using macro DEBUG_TOKEN_OUTPUT.
#if DEBUG_TOKEN_OUTPUT
               int node_start_line   = node->get_startOfConstruct()->get_physical_line();
               int node_start_column = node->get_startOfConstruct()->get_col();
            // int node_end_line     = node->get_endOfConstruct()->get_line();
               int node_end_line     = node->get_endOfConstruct()->get_physical_line();
               int node_end_column   = node->get_endOfConstruct()->get_col();

               int leading_whitespace_start  = (*i)->leading_whitespace_start;
               int leading_whitespace_end    = (*i)->leading_whitespace_end;
               int trailing_whitespace_start = (*i)->trailing_whitespace_start;

            // if (tokenStream.size() <= (size_t)trailing_whitespace_end)
               if ((int)tokenStream.size() <= trailing_whitespace_end)
                  {
                    printf ("Error: tokenStream.size() = %zu Trailing_whitespace_start = %d trailing_whitespace_end = %d \n",tokenStream.size(),trailing_whitespace_start,trailing_whitespace_end);
                 // printf ("Error: tokenStream.size() = %zu Trailing_whitespace_start = %zu trailing_whitespace_end = %zu \n",tokenStream.size(),(size_t)trailing_whitespace_start,(size_t)trailing_whitespace_end);
                  }
#endif
            // ROSE_ASSERT(tokenStream.size() > (size_t)trailing_whitespace_end);

            // DQ (12/6/2016): Added assertion as part of fix for warning now considered to be an error.
               ROSE_ASSERT(trailing_whitespace_end == -1 || tokenStream.size() > (size_t)trailing_whitespace_end);
#if 0
               printf ("\n\nToken stream for unassigned token locations: previous_end = %d to tokenStream_start-1 = %d \n",previous_end,tokenStream_start-1);
               if (previous_end >= 0)
                    printf ("   --- tokenStream[previous_end = %d]->ending_fpi.line_num = %d \n",previous_end,tokenStream[previous_end]->ending_fpi.line_num);

               if (tokenStream_start-1 >= 0)
                    printf ("   --- tokenStream[tokenStream_start-1 = %d]->beginning_fpi.line_num = %d \n",tokenStream_start-1,tokenStream[tokenStream_start-1]->beginning_fpi.line_num);
#endif
#if DEBUG_TOKEN_OUTPUT
               printf ("In outputTokenStreamSequenceMap(): node = %p = %s tokenStream_start = %d tokenStream_end = %d \n",node,node->class_name().c_str(),tokenStream_start,tokenStream_end);
               printf ("   --- Leading_whitespace_start = %d leading_whitespace_end = %d: ",leading_whitespace_start,leading_whitespace_end);
            // string presequenceTokens = generateTokenSubsequence(previous_end,tokenStream_start-1);
               string presequenceTokens = generateTokenSubsequence(leading_whitespace_start,leading_whitespace_end);
               printf ("presequenceTokens = -->|%s|<-- \n",presequenceTokens.c_str());
            // printf ("tokenStream_start = %d tokenStream_end = %d \n",tokenStream_start,tokenStream_end);

               ROSE_ASSERT(tokenStream_end == -1 || tokenStream_end < (int)tokenStream.size());

               printf ("   --- Token stream for node = %p = %s (%d,%d) to (%d,%d) (tokenStream_start=%d tokenStream_end=%d) = ",node,node->class_name().c_str(),node_start_line,node_start_column,node_end_line,node_end_column,tokenStream_start,tokenStream_end);
               string sequenceTokens = generateTokenSubsequence(tokenStream_start,tokenStream_end);
               printf ("-->|%s|<-- \n",sequenceTokens.c_str());

               printf ("   --- Trailing_whitespace_start = %d trailing_whitespace_end = %d: ",trailing_whitespace_start,trailing_whitespace_end);
               string postsequenceTokens = generateTokenSubsequence(trailing_whitespace_start,trailing_whitespace_end);
               printf ("postsequenceTokens = -->|%s|<-- \n\n",postsequenceTokens.c_str());
#endif
#if 0
               node->get_startOfConstruct()->display("node->get_startOfConstruct(): debug");
               node->get_endOfConstruct  ()->display("node->get_endOfConstruct(): debug");
#endif
#if 0
               if (tokenStream_start > tokenStream_end)
                  {
                    printf ("tokenStream_start = %d tokenStream_end = %d \n",tokenStream_start,tokenStream_end);
                    node->get_startOfConstruct()->display("node->get_startOfConstruct(): debug");
                  }
#endif
               unaccountedForTokenSubsequences++;
             }

       // previous_end = tokenStream_end+1;

#if 0
          printf ("In outputTokenStreamSequenceMap(): previous_end = %d \n",previous_end);
#endif
          i++;
        }

#if 0
     printf ("In outputTokenStreamSequenceMap(): tokenStream.size() = %zu unaccountedForTokenSubsequences = %d (in SgGlobal, but not in the union of tokens subsequences for all statements in SgGlobal) \n",tokenStream.size(),unaccountedForTokenSubsequences);
#endif
   }



// LexTokenStreamType* getTokenStream( SgSourceFile* file )
vector<stream_element*>
getTokenStream( SgSourceFile* file )
   {
  // Note that the return type is defined as:
  //    typedef std::list<stream_element*> LexTokenStreamType;
  // in general_token_defs.h", this might change in the future to support the SgToken IR nodes.
  // The advantages of using the SgToken IR node would be better support in ROSE, File I/O,
  // standard memory pool management, etc.

     string fileNameForTokenStream = file->getFileName();

#if 0
     printf ("In Unparser::unparseFile(): fileNameForTokenStream = %s \n",fileNameForTokenStream.c_str());
#endif

     ROSE_ASSERT(file->get_preprocessorDirectivesAndCommentsList() != NULL);
     ROSEAttributesListContainerPtr filePreprocInfo = file->get_preprocessorDirectivesAndCommentsList();

#if 0
     printf ("filePreprocInfo->getList().size() = %zu \n",filePreprocInfo->getList().size());
#endif

  // We should at least have the current files CPP/Comment/Token information (even if it is an empty file).
     ROSE_ASSERT(filePreprocInfo->getList().size() > 0);

  // This is an empty list not useful outside of the Flex file to gather the CPP directives, comments, and tokens.
     ROSE_ASSERT(mapFilenameToAttributes.empty() == true);

#if 0
     printf ("Evaluate what files are processed in map (filePreprocInfo->getList().size() = %zu) \n",filePreprocInfo->getList().size());
     std::map<std::string,ROSEAttributesList* >::iterator map_iterator = filePreprocInfo->getList().begin();
     while (map_iterator != filePreprocInfo->getList().end())
        {
          printf ("   --- map_iterator->first  = %s \n",map_iterator->first.c_str());
          printf ("   --- map_iterator->second = %p \n",map_iterator->second);

          map_iterator++;
        }
     printf ("DONE: Evaluate what files are processed in map (filePreprocInfo->getList().size() = %zu) \n",filePreprocInfo->getList().size());
#endif

  // std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = mapFilenameToAttributes.find(fileNameForTokenStream);
     std::map<std::string,ROSEAttributesList* >::iterator currentFileItr = filePreprocInfo->getList().find(fileNameForTokenStream);
  // ROSE_ASSERT(currentFileItr != mapFilenameToAttributes.end());
     ROSE_ASSERT(currentFileItr != filePreprocInfo->getList().end());

#if 0
     printf ("Get the ROSEAttributesList from the map iterator \n");
#endif

  // If there already exists a list for the current file then get that list.
     ROSE_ASSERT( currentFileItr->second != NULL);

     ROSEAttributesList* existingListOfAttributes = currentFileItr->second;

#if 0
     printf ("existingListOfAttributes = %p \n",existingListOfAttributes);
#endif

  // LexTokenStreamTypePointer tokenStream = existingListOfAttributes->get_rawTokenStream();
  // ROSE_ASSERT(tokenStream != NULL);

     LexTokenStreamType & tokenList = *(existingListOfAttributes->get_rawTokenStream());

#if 0
     printf ("Output token list (number of CPP directives and comments = %d): \n",existingListOfAttributes->size());
     printf ("Output token list (number of tokens = %zu): \n",tokenList.size());
#endif

#if 1
     int counter = 0;
     for (LexTokenStreamType::iterator i = tokenList.begin(); i != tokenList.end(); i++)
        {
#if 1
          ROSE_ASSERT((*i)->p_tok_elem != NULL);
#if 0
          printf ("   --- token #%d token id = %d position range (%d,%d) - (%d,%d): token = -->|%s|<-- \n",
               counter,(*i)->p_tok_elem->token_id,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num,
               (*i)->ending_fpi.line_num,(*i)->ending_fpi.column_num,(*i)->p_tok_elem->token_lexeme.c_str());
#endif
#else
          printf ("   --- token #%d token = %p \n",counter,(*i)->p_tok_elem);
          if ((*i)->p_tok_elem != NULL)
             {
               printf ("   --- --- token id = %d token = %s \n",(*i)->p_tok_elem->token_id,(*i)->p_tok_elem->token_lexeme.c_str());
             }

       // DQ (9/29/2013): Added support for reference to the PreprocessingInfo object in the token stream.
          printf ("   --- token #%d p_preprocessingInfo = %p \n",counter,(*i)->p_preprocessingInfo);
          printf ("   --- token #%d beginning_fpi line  = %d column = %d \n",counter,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num);
          printf ("   --- token #%d ending_fpi    line  = %d column = %d \n",counter,(*i)->ending_fpi.line_num,(*i)->ending_fpi.column_num);
#endif
          counter++;
        }
#endif

#if 1
  // LexTokenStreamType* tokenStream = getTokenStream(sourceFile);
     LexTokenStreamType* tokenStream = existingListOfAttributes->get_rawTokenStream();
     ROSE_ASSERT(tokenStream != NULL);

  // Set this value so that we can generate unique keys for any interval.
  // I think that a better mehcanism for generating unique keys would be possible (but this is simple).
     TokenStreamSequenceToNodeMapping::tokenStreamSize = tokenStream->size();

  // Convert this list to a vectors so that we can use integer indexing instead of iterators into a list.
     vector<stream_element*> tokenVector;
     for (LexTokenStreamType::iterator i = tokenStream->begin(); i != tokenStream->end(); i++)
        {
          tokenVector.push_back(*i);
        }

  // DQ (1/30/2014): I have added the corner case for an empty file, with zero tokens to find.
  // We need to make sure this is not an error (OK it issue a warning).
  // ROSE_ASSERT(tokenVector.empty() == false);
     if (tokenVector.empty() == true)
        {
          printf ("Warning: this is an empty file (no tokens found): not even a CR present! (but not an error using the token stream unparsing) \n");
        }

     return tokenVector;
#else
     return existingListOfAttributes->get_rawTokenStream();
#endif
   }


void
outputSourceCodeFromTokenStream_globalScope(SgSourceFile* sourceFile, vector<stream_element*> tokenVector, map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap, bool preferTrailingWhitespaceInOutput)
   {
#if 0
  // Define preference for leading or trailing token sequences.
#if 0
     bool preferTrailingWhitespaceInOutput = true;
#else
     bool preferTrailingWhitespaceInOutput = false;
#endif
#endif

  // Unparse the global scope as a test.
     SgGlobal* globalScope = sourceFile->get_globalScope();
     ROSE_ASSERT(globalScope != NULL);

  // string filename = "GlobalScopeDeclarationTokenStream";
  // string filename = "token_" + StringUtility::stripPathFromFileName(sourceFile->getFileName());
     string filename = "token_" + string(preferTrailingWhitespaceInOutput ? "trailing_" : "leading_") + StringUtility::stripPathFromFileName(sourceFile->getFileName());

  // string dot_header = filename;
     string dot_header = StringUtility::stripPathFromFileName(sourceFile->getFileName());
  // filename += ".c";

#if 1
     if ( SgProject::get_verbose() >= 1 )
        {
          printf ("In outputSourceCodeFromTokenStream_globalScope(): filename = %s \n",filename.c_str());
        }
#endif

  // Open file...(file is declared in the EDG_ROSE_Graph namespace).
     std::ofstream file;
     file.open(filename.c_str());

  // Output the opening header for a DOT file.
  // file << "digraph \"" << dot_header << "\" {" << endl;

     SgDeclarationStatementPtrList & declarationList = globalScope->getDeclarationList();

#if 0
     printf ("declarationList.size() = %zu \n",declarationList.size());
     printf ("tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif

  // The two allowed IR nodes are SgSourceFile and the SgGlobal (global scope)
  // if (declarationList.empty() == true)
     if (tokenStreamSequenceMap.size() <= 2)
        {
       // This case handled the trivial case of a file without any statements (could have #include files).
#if 0
          printf ("Note: tokenStreamSequenceMap.size() <= 2 \n");
#endif
          TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[globalScope];
          if (tokenSubsequence->token_subsequence_start != -1)
             {
               for (int j = tokenSubsequence->token_subsequence_start; j <= tokenSubsequence->token_subsequence_end; j++)
                  {
                    file << tokenVector[j]->p_tok_elem->token_lexeme;
                  }
             }
        }
       else
        {
          size_t firstDeclarationOutput = 0;

          bool ignore_firstDeclarationOutput = false;

          bool skipSharedSequences = false;

          for (size_t i = 0; i < declarationList.size(); i++)
             {
#if 0
               printf ("declarationList[i=%zu of %zu] = %p = %s \n",i,declarationList.size(),declarationList[i],declarationList[i]->class_name().c_str());
#endif
            // ROSE_ASSERT(tokenStreamSequenceMap.find(declarationList[i]) != tokenStreamSequenceMap.end());
               if (tokenStreamSequenceMap.find(declarationList[i]) != tokenStreamSequenceMap.end())
                  {
                    TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[declarationList[i]];
                    ROSE_ASSERT(tokenSubsequence != NULL);
#if 0
                    printf ("firstDeclarationOutput = %zu \n",firstDeclarationOutput);
#endif
                 // The first declaration is not alwasy the first tokenSubsequence.
                 // if (i == 0)
                    if (i == firstDeclarationOutput)
                       {
                         if (preferTrailingWhitespaceInOutput == true)
                            {
                           // Output the leading white space.
                              if (tokenSubsequence->leading_whitespace_start != -1)
                                 {
                                   for (int j = tokenSubsequence->leading_whitespace_start; j <= tokenSubsequence->leading_whitespace_end; j++)
                                     {
                                       file << tokenVector[j]->p_tok_elem->token_lexeme;
                                     }
                                 }
                            }

                         ignore_firstDeclarationOutput = true;
                       }

                    if (skipSharedSequences == false)
                       {
                         if (preferTrailingWhitespaceInOutput == false)
                            {
                           // We need to always output the leading token stream for each declaration.
                           // Output the leading white space.
#if 0
                              printf ("tokenSubsequence->leading_whitespace_start = %d tokenSubsequence->leading_whitespace_end = %d \n",tokenSubsequence->leading_whitespace_start,tokenSubsequence->leading_whitespace_end);
#endif
                              if (tokenSubsequence->leading_whitespace_start != -1)
                                 {
                                   for (int j = tokenSubsequence->leading_whitespace_start; j <= tokenSubsequence->leading_whitespace_end; j++)
                                      {
#if 0
                                        printf ("j = %d \n",j);
#endif
                                        file << tokenVector[j]->p_tok_elem->token_lexeme;
                                      }
                                 }
                            }

                      // Output the leading white space.
                         if (tokenSubsequence->token_subsequence_start != -1)
                            {
                              for (int j = tokenSubsequence->token_subsequence_start; j <= tokenSubsequence->token_subsequence_end; j++)
                                 {
                                   file << tokenVector[j]->p_tok_elem->token_lexeme;
                                 }
                            }

                         if (preferTrailingWhitespaceInOutput == true)
                            {
                           // We need to always output the trailing token stream for each declaration.
                           // if (i+1 == declarationList.size())
                           // Output the trailing white space.
#if 0
                              printf ("tokenSubsequence->trailing_whitespace_start = %d tokenSubsequence->trailing_whitespace_end = %d \n",tokenSubsequence->trailing_whitespace_start,tokenSubsequence->trailing_whitespace_end);
#endif
                              if (tokenSubsequence->trailing_whitespace_start != -1)
                                 {
                                   for (int j = tokenSubsequence->trailing_whitespace_start; j <= tokenSubsequence->trailing_whitespace_end; j++)
                                      {
#if 0
                                        printf ("j = %d \n",j);
#endif
                                        file << tokenVector[j]->p_tok_elem->token_lexeme;
                                      }
                                 }
                            }
                       }

                    if (preferTrailingWhitespaceInOutput == false)
                       {
                      // I think this might be a better way to handle the trailing edge of the last statement (when we 
                      // are giving a prefeence to generated the ouput using the leading subsequence between statements).
                      // If we are giving preference to leading white space and we are at the last 
                      // declaration, then output the trailing whitespace to close off the file.
                         if (i == declarationList.size()-1)
                            {
                           // Output the leading white space.
                              if (tokenSubsequence->trailing_whitespace_start != -1)
                                 {
                                   for (int j = tokenSubsequence->trailing_whitespace_start; j <= tokenSubsequence->trailing_whitespace_end; j++)
                                      {
                                        file << tokenVector[j]->p_tok_elem->token_lexeme;
                                      }
                                 }
                            }
                       }

                    if (tokenSubsequence->shared == true)
                       {
                         ROSE_ASSERT(tokenSubsequence->nodeVector.empty() == false);

                      // When we get to the last IR node for which this token sequence is shared then turn off skipSharedSequences.
                         if (declarationList[i] == tokenSubsequence->nodeVector[tokenSubsequence->nodeVector.size()-1])
                            {
                              skipSharedSequences = false;
                            }
                           else
                            {
                              skipSharedSequences = true;
                            }
#if 0
                         printf ("Set skipSharedSequences = %s \n",skipSharedSequences ? "true" : "false");
#endif
                       }
                  }
                 else
                  {
                 // We want to ignore the first declaration after we have output its associate leading tokens sequence.
                    if (ignore_firstDeclarationOutput == false)
                       {
                         firstDeclarationOutput++;
                       }
                  }
             }
        }

  // Close off the generated file.
     file.close();
   }


void
buildTokenStreamFrontier(SgSourceFile* sourceFile)
   {
#if 0
     printf ("In buildTokenStreamFrontier(): Calling simpleFrontierDetectionForTokenStreamMapping(): sourceFile = %p \n",sourceFile);
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_1 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In buildTokenStreamFrontier(): part 1: transformedStatementSet_1.size() = %zu \n",transformedStatementSet_1.size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgLocatedNode*> modifiedLocatedNodesSet_1 = SageInterface::collectModifiedLocatedNodes(sourceFile);
     printf ("In buildTokenStreamFrontier(): part 1: modifiedLocatedNodesSet_1.size() = %zu \n",modifiedLocatedNodesSet_1.size());
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

  // DQ (11/8/2015): This function sets the nodes as containing transforamtions (which is essential).
  // DQ (4/14/2015): After a more detailed evaluation of this function it does not acomplish it's objectives.
  // Note that we first detect the frontier based on a synthysised attribute traversal to record 
  // where nodes can contain transformation even if they are not a transformation directly.
     simpleFrontierDetectionForTokenStreamMapping(sourceFile);

#if 0
     printf ("In buildTokenStreamFrontier(): DONE: Calling simpleFrontierDetectionForTokenStreamMapping(): sourceFile = %p \n",sourceFile);
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): Calling detectMacroExpansionsToBeUnparsedAsAstTransformations(): sourceFile = %p \n",sourceFile);
#endif

  // DQ (11/8/2015): Add macro expansion detection to support where transformations are in part of the expanded macro.
  // However this must be called after all transformations have been done (in the frontier detection).
     detectMacroExpansionsToBeUnparsedAsAstTransformations(sourceFile);

#if 0
     printf ("In buildTokenStreamFrontier(): DONE: Calling detectMacroExpansionsToBeUnparsedAsAstTransformations(): sourceFile = %p \n",sourceFile);
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_2 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In buildTokenStreamFrontier(): part 2: transformedStatementSet_2.size() = %zu \n",transformedStatementSet_2.size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgLocatedNode*> modifiedLocatedNodesSet_2 = SageInterface::collectModifiedLocatedNodes(sourceFile);
     printf ("In buildTokenStreamFrontier(): part 2: modifiedLocatedNodesSet_2.size() = %zu \n",modifiedLocatedNodesSet_2.size());
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (12/6/2014): I think we need the frontier mechanism, and then the partial use of token 
  // streams on nodes containing transformations is required to provide a more precise generated 
  // code (precise representation with minimal diff).

#if 0
     printf ("In buildTokenStreamFrontier(): Calling frontierDetectionForTokenStreamMapping(): sourceFile = %p \n",sourceFile);
#endif

  // Note that we first detect the frontier.
     frontierDetectionForTokenStreamMapping(sourceFile);

#if 0
     printf ("In buildTokenStreamFrontier(): DONE: Calling frontierDetectionForTokenStreamMapping(): sourceFile = %p \n",sourceFile);
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 0
  // DQ (6/11/2015): Added to support debugging the difference between C and C++ support for token-based unparsing.
     std::set<SgStatement*> transformedStatementSet_3 = SageInterface::collectTransformedStatements(sourceFile);
     printf ("In buildTokenStreamFrontier(): part 3: transformedStatementSet_3.size() = %zu \n",transformedStatementSet_3.size());
#endif

  // ************************************************************************************************
  // DQ (11/29/2013): Mark those entries in the frontier map that are redundantly mapped to the same 
  // token subsequence.  This can happen with a variable declaration contains a list of variables and 
  // is mapped to several SgVariableDeclaration IR nodes in the ROSE AST.  This normalization then 
  // maps several IR nodes redundantly to a single token sequence.  So we have to identify all 
  // redundant mapping of IR nodes to a token sequence and allow/disallow them to be unparsed 
  // via the AST or token sequence as a group.
  // ************************************************************************************************

  // DQ (11/29/2013): Used to mark statements that are redundantly mapped to a single token sequence.
  // int lastTokenIndex = 0;
  // std::set<int> tokenSequenceEndSet;
     std::map<int,SgStatement*> tokenSequenceEndMap;
     std::multimap<int,SgStatement*> tokenSequenceEndMultimap;
     std::set<int> redundantTokenEndings;

  // DQ (11/29/2013): Get the token mapping to the AST.
     std::map<SgNode*,TokenStreamSequenceToNodeMapping*> & tokenStreamSequenceMap = sourceFile->get_tokenSubsequenceMap();

#if 0
     printf ("In buildTokenStreamFrontier(): tokenStreamSequenceMap.size() = %zu \n",tokenStreamSequenceMap.size());
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

  // DQ (2/28/2015): This assertion will be false where the input is an empty file.
  // ROSE_ASSERT(tokenStreamSequenceMap.empty() == false);

     map<SgStatement*,FrontierNode*> token_unparse_frontier_map = sourceFile->get_token_unparse_frontier();

  // DQ (11/29/2013): This can happen for test_CplusplusMacro_C.c (which has only CPP directives).
  // ROSE_ASSERT(token_unparse_frontier_map.empty() == false);

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 1
  // DQ (12/6/2014): I think we need the frontier mechanism, and then the partial use of token 
  // streams on nodes containing transformations is required to provide a more precise generated 
  // code (precise representation with minimal diff).
     map<SgStatement*,FrontierNode*>::iterator i = token_unparse_frontier_map.begin();

  // Iterator over all of the frontier nodes.
     int counter = 0;
     while (i != token_unparse_frontier_map.end())
        {
          SgStatement* statement = i->first;
          ROSE_ASSERT(statement != NULL);
          FrontierNode* frontier_node = i->second;
          ROSE_ASSERT(frontier_node != NULL);

#if 0
          printf ("In buildTokenStreamFrontier(): counter = %d sourceFile filename             = %s \n",counter,sourceFile->getFileName().c_str());
          printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

       // DQ (10/22/2018): We need to make sure that null entries are not insertedc into the TokenStreamSequenceToNodeMapping.
       // TokenStreamSequenceToNodeMapping* tokenSubsequence = tokenStreamSequenceMap[statement];
          TokenStreamSequenceToNodeMapping* tokenSubsequence = NULL;
          if (tokenStreamSequenceMap.find(statement) != tokenStreamSequenceMap.end())
             {
               tokenSubsequence = tokenStreamSequenceMap[statement];
             }
            else
             {
#if 0
               printf ("In buildTokenStreamFrontier(): statement = %p = %s not available in tokenSubsequence \n",statement,statement->class_name().c_str());
#endif
             }
       // ROSE_ASSERT(tokenSubsequence != NULL);

#if 0
          printf ("In buildTokenStreamFrontier(): counter = %d sourceFile filename             = %s \n",counter,sourceFile->getFileName().c_str());
          printf ("In buildTokenStreamFrontier(): statement = %p = %s \n",statement,statement->class_name().c_str());
          printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif
          if (tokenSubsequence != NULL)
             {
          std::map<int,SgStatement*>::iterator j = tokenSequenceEndMap.find(tokenSubsequence->token_subsequence_end);
          if (j != tokenSequenceEndMap.end())
             {
#if 0
                printf ("In buildTokenStreamFrontier(): Found associated tokenStreamSequence = %p for statement = %p = %s (%d,%d) \n",
                     tokenSubsequence,statement,statement->class_name().c_str(),tokenSubsequence->token_subsequence_start,tokenSubsequence->token_subsequence_end);
#endif
               i->second->redundant_token_subsequence = true;
               token_unparse_frontier_map[j->second]->redundant_token_subsequence = true;

            // Save this entry in the multimap.
               if (tokenSequenceEndMultimap.find(tokenSubsequence->token_subsequence_end) != tokenSequenceEndMultimap.end())
                  {
                    tokenSequenceEndMultimap.insert(std::pair<int,SgStatement*>(tokenSubsequence->token_subsequence_end,j->second));
                  }
               tokenSequenceEndMultimap.insert(std::pair<int,SgStatement*>(tokenSubsequence->token_subsequence_end,statement));

               redundantTokenEndings.insert(tokenSubsequence->token_subsequence_end);
#if 0
               printf ("Mark this as a frontier that is redundantly mapped to a token sequence: statement = %p = %s \n",statement,statement->class_name().c_str());
               ROSE_ASSERT(false);
#endif
#if 0
            // ROSE_ASSERT(tokenStreamSequenceMap.empty() == false);

               if (lastTokenIndex == tokenSubsequence->token_subsequence_end)
                  {
                 // Mark this as a frontier that is redundantly mapped to a token sequence.
                    printf ("Mark this as a frontier that is redundantly mapped to a token sequence \n");
                    ROSE_ASSERT(false);

                 // tokenSubsequence->redundant_token_subsequence = true;
                  }
                 else
                  {
                 // This is the typical case.
                  }

               lastTokenIndex = tokenSubsequence->token_subsequence_end;
#endif
             }
            else
             {
            // DQ (11/29/2013): Not certain this should be an error or a warning.
#if 0
               printf ("WARNING: There is no token sequence mapping already processed as redundant for this statement = %p = %s \n",statement,statement->class_name().c_str());
#endif
               tokenSequenceEndMap.insert(std::pair<int,SgStatement*>(tokenSubsequence->token_subsequence_end,statement));

            // Save this entry in the multimap.
            // tokenSequenceEndMultimap.insert(std::pair<int,SgStatement*>(tokenSubsequence->token_subsequence_end,statement));
             }
             }

          i++;

          counter++;
        }
#else

#error "DEAD CODE!"

  // DQ (12/6/2014): I have deactivated this feature since I think we might not need it.
     printf ("NOTE: Skipping the processing of the frontier nodes \n");
#endif

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

  // std::map<int,SgStatement*>::iterator k = tokenSequenceEndMap.begin();
  // while (k != tokenSequenceEndMap.end())
  // std::multimap<int,SgStatement*>::iterator k = tokenSequenceEndMultimap.begin();
  // while (k != tokenSequenceEndMultimap.end())
     std::set<int>::iterator k = redundantTokenEndings.begin();
     while (k != redundantTokenEndings.end())
        {
       // int lastTokenIndex = k->first;
          int lastTokenIndex = *k;
#if 0
          printf ("Redundant statement list: lastTokenIndex = %d \n",lastTokenIndex);
#endif
          std::pair<std::multimap<int,SgStatement*>::iterator,std::multimap<int,SgStatement*>::iterator> range_iterator = tokenSequenceEndMultimap.equal_range(lastTokenIndex);
          std::multimap<int,SgStatement*>::iterator first_iterator = range_iterator.first;
          std::multimap<int,SgStatement*>::iterator last_iterator  = range_iterator.second;

          bool unparseUsingTokenStream = true;
          std::multimap<int,SgStatement*>::iterator local_iterator = first_iterator;
          while (local_iterator != last_iterator)
             {
               SgStatement* stmt = local_iterator->second;
#if 0
               printf ("   --- redundant statement for lastTokenIndex = %d stmt = %p = %s \n",lastTokenIndex,stmt,stmt->class_name().c_str());
#endif
               if (token_unparse_frontier_map[stmt]->unparseUsingTokenStream == false)
                  {
                    unparseUsingTokenStream = false;
                  }

               local_iterator++;
             }

          if (unparseUsingTokenStream == false)
             {
            // Reset all of the frontier IR node data structures to unparse from the AST.
#if 0
               printf ("Reset all of the frontier IR node data structures to unparse from the AST \n");
#endif
               std::multimap<int,SgStatement*>::iterator local_iterator = first_iterator;
               while (local_iterator != last_iterator)
                  {
                    SgStatement* stmt = local_iterator->second;
#if 0
                    printf ("   --- redundant statement for lastTokenIndex = %d stmt = %p = %s (setting to unparse from AST) \n",lastTokenIndex,stmt,stmt->class_name().c_str());
#endif
                    ROSE_ASSERT(token_unparse_frontier_map.find(stmt) != token_unparse_frontier_map.end());
                    FrontierNode* token_unparse_frontier = token_unparse_frontier_map[stmt];
                    ROSE_ASSERT(token_unparse_frontier != NULL);

                    token_unparse_frontier->unparseUsingTokenStream = false;
                    token_unparse_frontier->unparseFromTheAST       = true;

                    local_iterator++;
                  }
             }

          k++;
        }

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

  // Save this so that the unparser can avoid duplication when unparsing the statements that
  // are redundantly mapped to a single token sequence (represented by the last token index).
     sourceFile->set_redundantTokenEndingsSet(redundantTokenEndings);
     sourceFile->set_redundantlyMappedTokensToStatementMultimap(tokenSequenceEndMultimap);

#if 0
     printf ("In buildTokenStreamFrontier(): sourceFile filename                          = %s \n",sourceFile->getFileName().c_str());
     printf ("In buildTokenStreamFrontier(): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 0
  // DQ (11/20/2013): Test using support for multiple files for Java testing.
  // Output an optional graph of the AST (just the tree, when active)
  // generateDOT ( *project );
  // SgProject* project = isSgProject(sourceFile->get_project());
     SgProject* project = sourceFile->get_project();
     ROSE_ASSERT(project != NULL);

     generateDOTforMultipleFile(*project,"in_buildTokenStreamFrontier");
#endif

#if 0
     printf ("Identify the frontier IR nodes that redundantly map to a single token sequence \n");
     ROSE_ASSERT(false);
#endif
   }



void
buildTokenStreamMapping(SgSourceFile* sourceFile)
   {
  // DQ (12/6/2014): This function seperates the initial generation of the token stream and it's mapping to the AST from 
  // the assocaited connection to the computed frontier after transformations have been done to define where the AST
  // should be using the token stream unparsing and where it should be using the AST unparsing.

  // DQ (9/5/2018): We should have already set the preprocessorDirectivesAndCommentsList, checked in getTokenStream().
     ROSE_ASSERT(sourceFile->get_preprocessorDirectivesAndCommentsList() != NULL);

     vector<stream_element*> tokenVector = getTokenStream(sourceFile);

  // DQ (11/29/2018): Debugging the token stream (for form-feeds).
#if 0
     printf ("In buildTokenStreamMapping(): (after getTokenStream()): tokenVector = %zu sourceFile->getFileName() = %s \n",tokenVector.size(),sourceFile->getFileName().c_str());
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (1/30/2014): Empty files are allowed (and tested).
  // ROSE_ASSERT(tokenVector.empty() == false);
     if (tokenVector.empty() == true)
        {
          printf ("In buildTokenStreamMapping(): No tokens found in file \n");
          return;
        }

#if 0
  // DQ (12/11/2015): Option to use the token stream mapping and local tests of the 
  // token sequence, to improve the source position information stored in the AST.
     printf ("Explicitly forcing sourceFile->get_use_token_stream_to_improve_source_position_info() == true \n");
     sourceFile->set_use_token_stream_to_improve_source_position_info(true);
#endif

  // Output a message that the AST source positon will be modified using the token sequence information.
     if (sourceFile->get_use_token_stream_to_improve_source_position_info() == true)
        {
          printf ("NOTE: token stream mapping will be used to improve the source position stored in the AST. \n");
        }

  // Build the inherited attribute
     bool processThisNode = true;
     InheritedAttribute inheritedAttribute(sourceFile,0,tokenVector.size()-1,processThisNode);

  // Build the traversal object
  // TokenMappingTraversal tokenMappingTraversal(tokenVector);
     TokenMappingTraversal tokenMappingTraversal(tokenVector,sourceFile->get_use_token_stream_to_improve_source_position_info());

#if 0
  // Output the depth of the AST.
     printf ("@@@@@@@@@@@@@@@@@@@@ In buildTokenStreamMapping(): (before traversal): sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
     printf ("   --- sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
     printf ("   --- sourceFile->depthOfSubtree()                 = %d \n",sourceFile->depthOfSubtree());
     printf ("   --- sourceFile->get_globalScope()                = %p \n",sourceFile->get_globalScope());
#endif

  // tokenMappingTraversal.traverse(sourceFile,inheritedAttribute);
  // tokenMappingTraversal.traverseInputFiles(sourceFile,inheritedAttribute);
     tokenMappingTraversal.traverse(sourceFile,inheritedAttribute);

#if 0
  // We need to set the positions of the trailing whitespace of the last element.
     if (tokenMappingTraversal.tokenStreamSequenceVector.empty() == false)
        {
          TokenStreamSequenceToNodeMapping* lastElement = tokenMappingTraversal.tokenStreamSequenceVector.back();

          lastElement->trailing_whitespace_start = lastElement->token_subsequence_end + 1;
          lastElement->trailing_whitespace_end   = tokenVector.size() - 1;

          printf ("In buildTokenStreamMapping(): Reset lastElement->trailing_whitespace_start = %d lastElement->trailing_whitespace_end = %d \n",lastElement->trailing_whitespace_start,lastElement->trailing_whitespace_end);

          if (lastElement->trailing_whitespace_start > lastElement->trailing_whitespace_end)
             {
#if 0
               printf ("Warning: lastElement->trailing_whitespace_start = %d lastElement->trailing_whitespace_end = %d (reset) \n",lastElement->trailing_whitespace_start,lastElement->trailing_whitespace_end);
#endif
               lastElement->trailing_whitespace_start = -1;
               lastElement->trailing_whitespace_end   = -1;
             }
          ROSE_ASSERT(lastElement->trailing_whitespace_start <= lastElement->trailing_whitespace_end);
#if 0
          printf ("Setting the last token's trailing_whitespace (%d,%d) tokenVector.size() = %zu \n",lastElement->trailing_whitespace_start,lastElement->trailing_whitespace_end,tokenVector.size());
#endif
        }
#endif

     tokenMappingTraversal.outputTokenStreamSequenceMap();

#if 1
  // DQ (12/26/2018): This is an error for badInput3.c (when using "-rose:verbose 2".
  // DQ (12/1/2013): Make the output of this graph consitional upon the verbose level.
     if ( SgProject::get_verbose() > 1 )
        {
       // DQ (12/3/2014): Note that this function fails for the Amr.cxx file in ARES.
       // Build a dot file of the AST and the token stream showing the mapping.
          Graph_TokenMappingTraversal::graph_ast_and_token_stream(sourceFile,tokenVector,tokenMappingTraversal.tokenStreamSequenceMap);
        }
#endif

#if 1
  // DQ (12/1/2013): Make the output of this graph consitional upon the verbose level.
  // This generates files: token_leading_<filename>.c and token_trailing_<filename>.c.
  // if ( SgProject::get_verbose() > 0 )
     if ( sourceFile->get_unparse_using_leading_and_trailing_token_mappings() == true )
        {
       // Output a file generated from the token sequences of each declaration statement directly within the global scope.
          bool preferTrailingWhitespaceInOutput = true;
          outputSourceCodeFromTokenStream_globalScope(sourceFile,tokenVector,tokenMappingTraversal.tokenStreamSequenceMap,preferTrailingWhitespaceInOutput);

          preferTrailingWhitespaceInOutput = false;
          outputSourceCodeFromTokenStream_globalScope(sourceFile,tokenVector,tokenMappingTraversal.tokenStreamSequenceMap,preferTrailingWhitespaceInOutput);
        }
#endif

  // DQ (10/27/2013): Build the SgToken IR nodes and the vector of them into the SgSourceFile IR node.
     SgTokenPtrList & roseTokenList = sourceFile->get_token_list();

  // DQ (11/29/2013): I think this should be empty at this point.
     ROSE_ASSERT(roseTokenList.empty() == true);

  // Setup the current file ID from the name in the source file.
     ROSE_ASSERT(sourceFile->get_file_info() != NULL);
     int currentFileId = sourceFile->get_file_info()->get_file_id();

  // This should now include all of the CPP directives and C/C++ style comments as tokens.
  // for (LexTokenStreamType::iterator i = tokenVector.begin(); i != tokenVector.end(); i++)
     int counter = 0;
     for (vector<stream_element*>::iterator i = tokenVector.begin(); i != tokenVector.end(); i++)
        {
          ROSE_ASSERT((*i)->p_tok_elem != NULL);
#if 0
          printf ("   --- token #%d token id = %d position range (%d,%d) - (%d,%d): token = -->|%s|<-- \n",
               counter,(*i)->p_tok_elem->token_id,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num,
               (*i)->ending_fpi.line_num,(*i)->ending_fpi.column_num,(*i)->p_tok_elem->token_lexeme.c_str());
#endif

          SgToken* roseToken = new SgToken( (*i)->p_tok_elem->token_lexeme,(*i)->p_tok_elem->token_id);
          ROSE_ASSERT(roseToken != NULL);

          roseToken->set_startOfConstruct(new Sg_File_Info(currentFileId,(*i)->beginning_fpi.line_num,(*i)->beginning_fpi.column_num));
          roseToken->set_endOfConstruct  (new Sg_File_Info(currentFileId,(*i)->ending_fpi.line_num,   (*i)->ending_fpi.column_num));

          roseTokenList.push_back(roseToken);
          counter++;
        }

  // Avoid the copy into the list held by SgSourceFile.
  // sourceFile->set_token_list(tokenList);

#if 1
  // Output the tokenStreamSequenceMap:
#if 0
     printf ("In buildTokenStreamMapping(): Output TokenStreamSequenceToNodeMapping \n");
#endif
     std::map<SgNode*,TokenStreamSequenceToNodeMapping*>::iterator i = tokenMappingTraversal.tokenStreamSequenceMap.begin();
     while (i != tokenMappingTraversal.tokenStreamSequenceMap.end())
        {
#if 0
          printf ("i->first = %p = %s: \n",i->first,i->first->class_name().c_str());
#endif
       // DQ (9/28/2018): Adding assertion.
          ROSE_ASSERT(i->second->node != NULL);
#if 0
          i->second->display("buildTokenStreamMapping(): 1: token sequence");
#endif
          i++;
        }
#endif
#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
     printf ("In buildTokenStreamMapping(): Calling sourceFile->set_tokenSubsequenceMap() \n");
#endif

#if 0
     printf ("In buildTokenStreamMapping(): (before calling set function): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

  // Note that the map is actually a member of the ROSE namespace, and that this is done because the 
  // ROSE IR can't support (as defined by ROSETTA) some more complex types as what we would need to 
  // support it as a data member of the SgSourceFile IR node.  This is due in part to ROSETTA and the
  // additional requirements of the generated serialization that is a part of the AST File I/O.
     sourceFile->set_tokenSubsequenceMap(tokenMappingTraversal.tokenStreamSequenceMap);

#if 0
     printf ("In buildTokenStreamMapping(): (after calling set function): sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
#endif

#if 1
  // Output the tokenStreamSequenceMap:
#if 0
     printf ("In buildTokenStreamMapping(): Output TokenStreamSequenceToNodeMapping \n");
#endif
     std::map<SgNode*,TokenStreamSequenceToNodeMapping*>::iterator j = sourceFile->get_tokenSubsequenceMap().begin();
     while (j != sourceFile->get_tokenSubsequenceMap().end())
        {
#if 0
          printf ("j->first = %p = %s: \n",j->first,j->first->class_name().c_str());
#endif
       // DQ (9/28/2018): Adding assertion.
          ROSE_ASSERT(j->second->node != NULL);
#if 0
          j->second->display("buildTokenStreamMapping(): 2: token sequence");
#endif
          j++;
        }
#endif
#if 0
     printf ("Completed the AST token stream mapping (before transformations) \n");
     ROSE_ASSERT(false);
#endif

#if 0
     if (sourceFile->get_representativeWhitespaceStatementMap().size() >= 0)
        {
          printf ("buildTokenStreamMapping(): sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
          printf ("   --- sourceFile->get_representativeWhitespaceStatementMap().size() = %zu \n",sourceFile->get_representativeWhitespaceStatementMap().size());
        }
#endif

  // DQ (11/20/2015): Now setup the representative whitespace to use in the output of transformations for each scope.
  // Since the transformations are output without surrounding whitespace, we need to collect representative 
  // statements from each scope so that we can use their whitespace when transformations in that scope are output.
     sourceFile->set_representativeWhitespaceStatementMap(tokenMappingTraversal.representativeWhitespaceStatementMap);

#if 0
  // DQ (11/20/2015): This should be true for most testing but does not have to be true for empty files and such pathological cases.
     printf ("Note temporary testing of (sourceFile->get_representativeWhitespaceStatementMap().empty() == false) is not ALWAYS true \n");
     ROSE_ASSERT(sourceFile->get_representativeWhitespaceStatementMap().empty() == false);
#endif

#if 0
  // DQ (9/25/2018): Check what the token mapping looks like for the SgGlobal scope (so that we can decide if we want to copy it
  // to the newly introduced SgGlobal IR nodes associated with the different include files within the header file unparsing support.
     if (sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope()) != sourceFile->get_tokenSubsequenceMap().end())
        {
          TokenStreamSequenceToNodeMapping* tokenSequenceToNodeMapping = sourceFile->get_tokenSubsequenceMap()[sourceFile->get_globalScope()];
          ROSE_ASSERT(tokenSequenceToNodeMapping != NULL);

          printf ("sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());

          printf ("Calling TokenStreamSequenceToNodeMapping::display() \n");
          tokenSequenceToNodeMapping->display("");
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

#if 1
  // DQ (9/25/2018): Test if the token stream mapping includes the global scope.
  // NOTE: this is an issue for the global scope added for each include file (which maybe should be done before the mapping is built).
  // ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope()) == sourceFile->get_tokenSubsequenceMap().end());
     ROSE_ASSERT(sourceFile->get_tokenSubsequenceMap().find(sourceFile->get_globalScope()) != sourceFile->get_tokenSubsequenceMap().end());
#endif

#if 0
  // Output the depth of the AST.
     printf ("In buildTokenStreamMapping(): (after traversal): sourceFile->getFileName() = %s \n",sourceFile->getFileName().c_str());
     printf ("   --- sourceFile->get_tokenSubsequenceMap().size() = %zu \n",sourceFile->get_tokenSubsequenceMap().size());
     printf ("   --- sourceFile->depthOfSubtree()                 = %d \n",sourceFile->depthOfSubtree());
     printf ("   --- sourceFile->get_globalScope()                = %p \n",sourceFile->get_globalScope());
#endif

   }



