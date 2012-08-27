/**
 * \file    variable_name_similarity.cpp
 * \author  Justin Too <too1@llnl.gov>, Dan Quinlan <quinlan1@llnl.gov>
 * \date    August 23, 2012
 */

#include "rose.h"
#include "string_functions.h"

#include <sstream>
#include <vector>

#include "compass2/compass.h"

using std::string;
using std::vector;
using namespace StringUtility;

extern const Compass::Checker* const variableNameSimilarityChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_VARIABLE_NAME_SIMILARITY_H
#define COMPASS_VARIABLE_NAME_SIMILARITY_H

namespace CompassAnalyses
{
/**
 * \brief Detect default arguments in function parameter lists.
 */
namespace VariableNameSimilarity
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;

  /**
   * \brief Specificaiton of checker results.
   */
  class CheckerOutput: public Compass::OutputViolationBase {
   public:
    CheckerOutput(SgNode *const first_node,
                  SgNode *const second_node,
                  std::string description);

   private:
    DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
  };

  CheckerOutput*
  CreateCheckerOutput(int percent_similarity,
                      SgLocatedNode* first_node,
                      SgLocatedNode* second_node);
} // ::CompassAnalyses
} // ::VariableNameSimilarity
#endif // COMPASS_VARIABLE_NAME_SIMILARITY_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace VariableNameSimilarity
 {
  const string checker_name      = "VariableNameSimilarity";
  const string short_description = "Similar variable name detected: ";
  const string long_description  = "This analysis looks for variables \
      whose names are similar (Ratcliff/Obershelp pattern recognition \
      algorithm.";
 }
}

CompassAnalyses::VariableNameSimilarity::
CheckerOutput::CheckerOutput(SgNode *const first_node,
                             SgNode *const second_node,
                             std::string description)
    : OutputViolationBase(first_node,
                          ::variableNameSimilarityChecker->checkerName,
                          ::variableNameSimilarityChecker->shortDescription +
                          description) {}

CompassAnalyses::VariableNameSimilarity::CheckerOutput*
CompassAnalyses::VariableNameSimilarity::CreateCheckerOutput(
    int percent_similarity,
    SgLocatedNode* first_node,
    SgLocatedNode* second_node)
  {
    std::stringstream description (std::stringstream::in |
                                   std::stringstream::out);

    std::string first_node_name =
        SageInterface::get_name(first_node);
    std::string second_node_name =
        SageInterface::get_name(second_node);

    description
      << percent_similarity
      << "% similarity "
      << std::endl

      << first_node_name
      << " (" << first_node->class_name().c_str() << ")"
      << " in "
      << first_node->get_file_info()->get_filename()
      << ":" << first_node->get_file_info()->get_line()
      << std::endl

      << second_node_name
      << " (" << second_node->class_name().c_str() << ")"
      << " in "
      << second_node->get_file_info()->get_filename()
      << ":" << second_node->get_file_info()->get_line()
      << std::endl;

    return new CompassAnalyses::VariableNameSimilarity::CheckerOutput(
        first_node,
        second_node,
        description.str());
  }

//////////////////////////////////////////////////////////////////////////////

/////// START HERE

#define MAX_LCS 256 ///< Maximum size of the longest common sequence

float similarity_threshold = 0.75;

/**
 * Quick and dirty swap of the address of 2 arrays
 * of `unsigned int`.
 */
void
swap(unsigned** first, unsigned** second)
  {
    unsigned *temp;
    temp = *first;
    *first = *second;
    *second = temp;
  }

/**
 * \return percent similarity of two strings
 *
 * Assumes that both strings point to two valid, null-terminated
 * char arrays.
 *
 * Note that the order of the strings is significant.
 *
 * For example,
 *
 *  ("buffer", "fer") = 0.5
 *  ("fer", "buffer") = 1.0
 */
float
similarityMetric(const char* strX, const char* strY)
  {
    size_t  lenX = strlen(strX),
            lenY = strlen(strY);

    const char *str1 = (lenX > lenY) ? strX : strY,
               *str2 = (lenX > lenY) ? strY : strX;

    size_t len1 = strlen(str1), len2 = strlen(str2);

    ROSE_ASSERT(len1 >= len2);

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

     // Note that this as a function might eliminate oportunities for optimization.
        swap( &previous, &next);
      }

    lenLCS = (float)previous[len1];

    free(previous);
    free(next);

    return lenLCS /= len1;
  }

/**
 * \return A pointer to the Longest Common Sequence in str1 and str2
 * Assumes str1 and str2 point to 2 null terminated array of char
 */
char*
longestCommonSubstring( const char *str1, const char *str2)
  {
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
               // if (align[r-1][c-1] == i-1)
                  if (align[r-1][c-1] == (unsigned) i-1)
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

/**
 * This structure is used to hold names and their links to the AST.
 * When matches are found this allows for more information to be 
 * output about where the names came from.  Identical names may
 * match and in this case the information as to how they are used
 * and what nested scope they came from, etc.
 */
class NameStructure
  {
    public:
      string  name;
      SgNode* associatedNode;

      NameStructure(string name, SgNode* associatedNode = NULL)
        : name(name),
          associatedNode(associatedNode)
        {
          ROSE_ASSERT(associatedNode != NULL);
        }

      NameStructure(const NameStructure & X)
        : name(X.name),
          associatedNode(X.associatedNode)
        {}

      size_t size() { return name.size(); }
      const char* c_str() { return name.c_str(); }
  };
typedef NameStructure NameStructureType;

/**
 *  This is used to pass context down in the AST traversal (but not required).
 */
class InheritedAttribute
  {
  };

/**
 * This is used to pass information up in the AST traversal.
 */
class SynthesizedAttribute
  {
    public:
      vector<NameStructureType> nameList;
  };

/**
 * TODO:
 */
class Traversal :
  public SgTopDownBottomUpProcessing<InheritedAttribute, SynthesizedAttribute>
  {
    public:
      Traversal(Compass::Parameters parameters,
                Compass::OutputObject* output);

      //== Functions required to support the AST traversal:

      InheritedAttribute
      evaluateInheritedAttribute(
          SgNode* astNode,
          InheritedAttribute inheritedAttribute);

      SynthesizedAttribute
      evaluateSynthesizedAttribute(
          SgNode* astNode,
          InheritedAttribute inheritedAttribute,
          SubTreeSynthesizedAttributes synthesizedAttributeList );

      /**
       *  Extract the name and add it to the list/set.
       */
      void processNode (SgNode* n, SynthesizedAttribute& synthesizedAttribute);

      /**
       *  Match names for similarity (applies similarity metric)
       */
      void processNames( SgNode* n, SynthesizedAttribute & synthesizedAttribute );

    string source_directory() const { return source_directory_; }
    void set_source_directory(const string &source_directory)
      {
        source_directory_ = source_directory;
      }

    private:
      string source_directory_; ///< Restrict analysis to user input files.
      Compass::OutputObject* output_;

      /*-----------------------------------------------------------------------
       * Utilities
       *---------------------------------------------------------------------*/
      DISALLOW_COPY_AND_ASSIGN(Traversal);
  };

Traversal::
Traversal(Compass::Parameters parameters, Compass::OutputObject* output)
    : output_(output)
  {
    try
    {
        string target_directory =
            parameters["general::target_directory"].front();
        source_directory_.assign(target_directory);
    }
    catch (Compass::ParameterNotFoundException e)
    {
        std::cout << "ParameterNotFoundException: " << e.what() << std::endl;
        homeDir(source_directory_);
    }
  }

void
Traversal::processNode(SgNode* n, SynthesizedAttribute& synthesizedAttribute )
  {
    // Look for names of functions
    SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(n);
    if (functionDeclaration != NULL)
    {
        string name = functionDeclaration->get_name().str();

        #if DEBUG > 3
          SgFunctionDefinition* functionDefinition =
              functionDeclaration->get_definition();
          if (functionDefinition != NULL)
              printf ("SgFunctionDefinition: %s \n",name.c_str());
          else
              printf ("SgFunctionDeclaration: %s \n",name.c_str());
        #endif

        synthesizedAttribute.nameList.push_back(
            NameStructureType(name,n));
        // nameSet.insert(name);
    }

    SgInitializedName* initializedName = isSgInitializedName(n);
    if (initializedName != NULL)
    {
        string name = initializedName->get_name().str();

        #if DEBUG > 3
          printf ("SgInitializedName: %s \n",name.c_str());
        #endif

        synthesizedAttribute.nameList.push_back(
            NameStructureType(name,n));
        // nameSet.insert(name);
    }

    SgNamespaceDeclarationStatement* namespaceDeclaration = isSgNamespaceDeclarationStatement(n);
    if (namespaceDeclaration != NULL)
    {
        string name = namespaceDeclaration->get_name().str();

        #if DEBUG > 3
          printf ("SgNamespaceDeclaration: %s \n",name.c_str());
        #endif

        synthesizedAttribute.nameList.push_back(
            NameStructureType(name,n));
        // nameSet.insert(name);
    }
  }

void
Traversal::processNames(SgNode* n, SynthesizedAttribute& synthesizedAttribute )
  {
    // Now process the list of names for matches

    // Matching names (eventually we have to map this back to the AST)
    vector< std::pair<NameStructureType,NameStructureType> > results;

    SgScopeStatement* scopeStatement = isSgScopeStatement(n);
    ROSE_ASSERT(scopeStatement != NULL);

    int i_index = 0;
    vector<NameStructureType>::iterator i;
    for (i = synthesizedAttribute.nameList.begin();
         i != synthesizedAttribute.nameList.end();
         ++i)
    {
        ++i_index;

        // size_t i_length = i->size();

        int j_index = 0;
        vector<NameStructureType>::iterator j;
        for (j = synthesizedAttribute.nameList.begin();
             j != synthesizedAttribute.nameList.end();
             ++j)
        {
            ++j_index;

            // We only want to visit the lower triangular part of the n^2
            // matchings of names to each other.  This reduces the number of
            // comparisions required.
            if (j_index <= i_index)
            {
                #if DEBUG > 1
                printf ("Skipping case of "
                        "j_index = %d <= i_index = %d (%s,%s) \n",
                        j_index,
                        i_index,
                        i->c_str(),
                        j->c_str());
                #endif

                continue;
            }

            #if DEBUG > 2
            printf ("Evaluating greatestPossibleSimilarity of "
                    "j_index = %d <= i_index = %d (%s,%s) \n",
                    j_index,
                    i_index,
                    i->c_str(),
                    j->c_str());
            #endif

            size_t i_length = i->size();
            size_t j_length = j->size();

            float greatestPossibleSimilarity =
                ((float)j_length) / ((float)i_length);

            if (greatestPossibleSimilarity > 1.0)
            {
                greatestPossibleSimilarity =
                    1.0 / greatestPossibleSimilarity;
            }

            if (greatestPossibleSimilarity < similarity_threshold)
            {
                #if DEBUG > 1
                printf ("Skipping case of "
                        "j_index = %d i_index = %d (%s,%s) "
                        "greatestPossibleSimilarity = %f \n",
                        j_index,
                        i_index,
                        i->c_str(),
                        j->c_str(),
                        greatestPossibleSimilarity);
                #endif

                continue;
            }

            #if DEBUG > 2
              printf ("Evaluating similarityMetric of"
                      "j_index = %d <= i_index = %d (%s,%s) \n",
                      j_index,
                      i_index,
                      i->c_str(),
                      j->c_str());
            #endif

            float similarity =
                similarityMetric(
                    i->c_str(),
                    j->c_str());

            if (similarity > similarity_threshold)
            {
                string lcs = longestCommonSubstring(i->c_str(), j->c_str());

                #if DEBUG > 1
                printf("\n\"%s\" and \"%s\" are %3.0f%% similar.\n"
                       "One of the longest common sequences is \"%s\".\n\n",
                        i->c_str(),
                        j->c_str(),
                        similarity*100,
                        lcs.c_str());
                #endif

                results.push_back(
                    std::pair<NameStructureType, NameStructureType> (*i, *j));
            }
        }
    }// for each synthesized attribute

    // Output the resulting matches of any non-empty list of results
    if (results.empty() == false)
    {
        if (SgProject::get_verbose() > 2)
        {
            printf ("Processing matches of name in "
                    "scope = %p = %s = %s \n",
                    scopeStatement,
                    scopeStatement->class_name().c_str(),
                    SageInterface::get_name(scopeStatement).c_str());
        }

        vector< std::pair<NameStructureType, NameStructureType> >::iterator i;
        for (i = results.begin(); i != results.end(); ++i)
        {
            // Output the matching names

            SgNode* firstNode  = i->first.associatedNode;
            ROSE_ASSERT(firstNode != NULL);

            SgNode* secondNode = i->second.associatedNode;
            ROSE_ASSERT(secondNode != NULL);

            float similarity =
            similarityMetric(i->first.c_str(), i->second.c_str());
            int similarityPercentage = 100 * similarity;

            SgLocatedNode* first_node =
                isSgLocatedNode(i->first.associatedNode);
            SgLocatedNode* second_node =
                isSgLocatedNode(i->second.associatedNode);
            if (first_node  != NULL &&
                second_node != NULL &&
                first_node  != second_node)
            {
                if (Compass::IsNodeInUserLocation(first_node, source_directory_) &&
                    Compass::IsNodeInUserLocation(second_node, source_directory_))
                {
                    output_->addOutput(
                        CompassAnalyses::VariableNameSimilarity::
                        CreateCheckerOutput(
                            similarityPercentage,
                            first_node,
                            second_node));
                }
            }
        }
    }
  }


InheritedAttribute
Traversal::evaluateInheritedAttribute (
    SgNode* astNode,
    InheritedAttribute inheritedAttribute)
  {
    if (isSgScopeStatement(astNode) != NULL)
    {
        // Build a new inherited attribute.
        return InheritedAttribute();
    }

    return inheritedAttribute;
  }

SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
  {
    SynthesizedAttribute result;

    #if 1
    // Accumulate the names in the children into the names at the parent (current node).
    SynthesizedAttributesList::iterator i;
    for (i = childAttributes.begin(); i != childAttributes.end(); ++i)
    {
        vector<NameStructureType>::iterator n;
        for (n = i->nameList.begin(); n != i->nameList.end(); ++n)
        {
            result.nameList.push_back(*n);
        }
    }
    #endif

    if (isSgScopeStatement(astNode) != NULL)
    {
        // Now process the collected names.
        processNames(astNode, result);
    }
    else
    {
        processNode(astNode,result);
    }

    return result;
  }


////// END HERE



// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject* output)
  {
    //CompassAnalyses::VariableNameSimilarity::Traversal(params, output).run(
    //  Compass::projectPrerequisite.getProject());
    SgProject* project = Compass::projectPrerequisite.getProject();

    InheritedAttribute inheritedAttribute;

    Traversal myTraversal(params, output);
    myTraversal.traverse(project, inheritedAttribute);
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const variableNameSimilarityChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::VariableNameSimilarity::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::VariableNameSimilarity::short_description,
      CompassAnalyses::VariableNameSimilarity::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

