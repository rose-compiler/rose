// This is a program evaluate similarity of names of user defined language constructs.

// Ratcliff/Obershelp pattern recognition:
// The Ratcliff/Obershelp algorithm computes the similarity of two strings as the doubled 
// number of matching characters divided by the total number of characters in the two strings. 
// Matching characters are those in the longest common subsequence plus, recursively, matching 
// characters in the unmatched region on either side of the longest common subsequence.
// Examples
//    1. The similarity of ALEXANDRE and ALEKSANDER is 2 * (3+3+1+1) / (9+10) = 0.84 (matching ALE, AND, E, R).

#include "rose.h"

using namespace std;

#define MAX_LCS 256	/*Maximum size of the longest common sequence. You might wish to change it*/
float simil( char *str1, char *str2);
char *LCS( char *str1, char *str2);


// Global collections of names
vector<string> nameList;
set<string> nameSet;


void
swap( unsigned **first, unsigned **second)
   {
  // Quick and dirty swap of the address of 2 arrays of unsigned int

     unsigned *temp;
     temp = *first;
     *first = *second;
     *second = temp;
   }

float
simil( char *str1, char *str2)
   {
  // Note that the order of the strings is significant 
  // (e.g. ("buffer","fer") = 0.5, while ("fer","buffer") = 1.0).

  // A function which returns how similar 2 strings are
  // Assumes that both point to 2 valid null terminated array of chars.
  // Returns the similarity between them.

     size_t len1 = strlen(str1), len2 = strlen(str2);
     float lenLCS;
     unsigned j, k, *previous, *next;

     if (len1 == 0 || len2 == 0)
          return 0.0;

     previous = (unsigned *) calloc( len1+1, sizeof(unsigned));
     next     = (unsigned *) calloc( len1+1, sizeof(unsigned));

     for(j=0; j<len2; ++j)
        {
          for(k=1; k<=len1; ++k)
               if( str1[k-1] == str2[j])
                    next[k]=previous[k-1]+1;
                 else
                    next[k] = previous[k] >= next[k-1] ? previous[k] : next[k-1];

          swap( &previous, &next);
        }

     lenLCS = (float)previous[len1];

     free(previous);
     free(next);

     return lenLCS /= len1;
   }

char*
LCS( char *str1, char *str2)
   {
  // Returns a pointer to the Longest Common Sequence in str1 and str2
  // Assumes str1 and str2 point to 2 null terminated array of char

     static char lcs[MAX_LCS];
     int i, r, c, len1 = (int)strlen(str1), len2=(int)strlen(str2);
     unsigned **align;

     if( len1==0 || len2==0)
          return 0;

     align = (unsigned **)calloc( len2+1, sizeof(unsigned *));

     for( r=0; r<=len2; ++r)
          align[r]=(unsigned *)calloc( len1+1, sizeof(unsigned));

     for(r=1; r<=len2; ++r)
          for(c=1; c<=len1; ++c)
               if( str1[c-1] == str2[r-1])
                    align[r][c]=align[r-1][c-1]+1;
                 else
                    align[r][c]=align[r-1][c]>=align[r][c-1]?align[r-1][c]:align[r][c-1];

     for (r = len2, c = len1, i = align[r][c], lcs[i]='\0'; i>0 && r>0 && c>0; i=align[r][c])
        {
          if ( align[r-1][c] == (unsigned) i)
               --r;
            else
             {
               if ( align[r][c-1] == (unsigned) i)
                    --c;
                 else
                  {
                 // if (align[r-1][c-1] == (unsigned) i-1)
                    if (align[r-1][c-1] == i-1)
                       {
                         lcs[i-1] = str2[--r];
                         --c;
                       }
                  }
             }
        }

     for (r = len2; r >= 0; --r)
          free (align[r]);

     free (align);

     return lcs;
   }


// typedef bool InheritedAttribute;
// typedef bool SynthesizedAttribute;

class InheritedAttribute{};
class SynthesizedAttribute{};

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
       // Functions required
          InheritedAttribute evaluateInheritedAttribute (
             SgNode* astNode, 
             InheritedAttribute inheritedAttribute );

          SynthesizedAttribute evaluateSynthesizedAttribute (
             SgNode* astNode,
             InheritedAttribute inheritedAttribute,
             SubTreeSynthesizedAttributes synthesizedAttributeList );
   };

InheritedAttribute
Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
     if (isSgFunctionDefinition(astNode))
        {
       // The inherited attribute is true iff we are inside a function.
          return true;
        }
     return inheritedAttribute;
   }

SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
     if (inheritedAttribute == false)
        {
       // The inherited attribute is false, i.e. we are not inside any
       // function, so there can be no loops here.
          return false;
        }
     else
        {
       // Fold up the list of child attributes using logical or, i.e. the local
       // result will be true iff one of the child attributes is true.
          SynthesizedAttribute localResult =
          std::accumulate(childAttributes.begin(), childAttributes.end(),
                          false, std::logical_or<bool>());
          if (isSgFunctionDefinition(astNode) && localResult == true)
             {
               printf ("Found a function containing a for loop ...\n");
             }
          if (isSgForStatement(astNode))
             {
               localResult = true;
             }
          return localResult;
        }
   }

#if 0
class visitorTraversal : public AstSimpleProcessing
   {
     public:
          virtual void visit(SgNode* n);
   };

void
visitorTraversal::visit(SgNode* n)
   {
  // Looking for names of functions
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
     if (functionDeclaration != NULL)
        {
          SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
          string name = functionDeclaration->get_name().str();
          if (functionDefinition != NULL)
             {
               printf ("SgFunctionDefinition: %s \n",name.c_str());
             }
            else
             {
               printf ("SgFunctionDeclaration: %s \n",name.c_str());
             }

          nameList.push_back(name);
          nameSet.insert(name);
        }

     SgInitializedName* initializedName = isSgInitializedName(n);
     if (initializedName != NULL)
        {
          string name = initializedName->get_name().str();
          printf ("SgInitializedName: %s \n",name.c_str());
          nameList.push_back(name);
          nameSet.insert(name);
        }

     SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(n);
     if (namespaceDeclaration != NULL)
        {
          string name = namespaceDeclaration->get_name().str();
          printf ("SgNamespaceDeclaration: %s \n",name.c_str());
          nameList.push_back(name);
          nameSet.insert(name);
        }
   }
#endif

int
main( int argc, char * argv[] )
   {
     SgProject* project = new SgProject(argc, argv);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute = false;

  // Define the traversal
     Traversal myTraversal;

  // Call the traversal starting at the project (root) node of the AST
     myTraversal.traverseInputFiles(project,inheritedAttribute);

#if 0
  // Build the traversal object (for CFG)
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
  // exampleTraversal.traverseInputFiles(project,preorder);
     exampleTraversal.traverse(project,preorder);
#endif

     printf ("nameList.size() = %zu \n",nameList.size());
     printf ("nameSet.size()  = %zu \n",nameSet.size());

     cout << "Generating DOT...(for debugging)\n";
     generateDOT( *project );
     cout << "Done with DOT\n";

#if 1
  // Test the string similarity support.
#define MAX_STR	256

     char str1[MAX_STR], str2[MAX_STR], dump;
     printf("Type the 2 strings to compare.\nTo finish leave first string blank.\n");
     while(scanf("%[^\n]", str1) !=0 )
        {
          scanf("\n%[^\n]%c", str2, &dump);
          printf("\n\"%s\" and \"%s\" are %3.0f%% similar.\nOne of the longest common sequences is \"%s\".\n\n", str1, str2,simil(str1, str2)*100, LCS(str1, str2));
	     }
#endif

     printf ("\nLeaving main program ... \n");

  // backend(project);
     return 0;
   }

