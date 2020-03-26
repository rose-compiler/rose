// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "astPostProcessing.h"

#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge_support.h"
#include "buildMangledNameMap.h"
#include "buildReplacementMap.h"
#include "fixupTraversal.h"
#include "collectAssociateNodes.h"
#include "test_support.h"
#include "merge.h"

#ifdef _MSC_VER
#include <direct.h>     // chdir
#endif

// This implements the support for merging ASTs. It should merge perfectly
// if we handled the merging of everthing, but namespaces and some function declarations
// will be a problem because they will generate unique names that are not really unique.
//    1) Namespaces:
//       the same namespace will generate the same name for each instance of it in
//       the source program (remember that namespaces are re-entrant).  Thus we can't
//       namespaces unless we verify same position in source.
//    2) Function declarations:
//       function prototypes can be repeated and will all generate the same unique name
//       independent of if they declare parameters with default arguments.  Thus we can't
//       merge function declarations unless we verify same position in source.
//    3) Some declarations return an empty name (we might want to fix that) but these are
//       clearly not unique enough since that makes "class { int x; };" has no name.
//    4) variable declarations that use "extern" are mangled the same. e.g 
//       "int x;" and "extern int x;"

//#include "rose.h"

#include "../astVisualization/wholeAST_API.h"

#define MAX_NUMBER_OF_IR_NODES_TO_GRAPH 15000
#define DISPLAY_INTERNAL_DATA 0

using namespace std;

// Global variable that functions can use to make sure that there IR nodes were not deleted!
// This is used for debugging only (tests in assertions).
std::set<SgNode*> finalDeleteSet;

void mergeAST ( SgProject* project, bool skipFrontendSpecificIRnodes )
   {
  // DQ (5/31/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("AST merge:");

  // DQ (7/29/2010): Added support to hanlde type table.
     if (SgTypeDefault::numberOfNodes() == 0)
        {
       // Build a SgTypeDefault now so that it will not be built for the first time in the function: accumulateSaveSetForPreprocessingInfo().
          SgTypeDefault::createType();
        }

  // Generate the filename to be used as a base for generated dot files (graphs)
     string filename = SageInterface::generateProjectName(project);

     int numberOfASTnodesBeforeMerge = numberOfNodes();
     if (SgProject::get_verbose() > 0)
          printf ("numberOfASTnodesBeforeMerge = %d MAX_NUMBER_OF_IR_NODES_TO_GRAPH = %d \n",numberOfASTnodesBeforeMerge,MAX_NUMBER_OF_IR_NODES_TO_GRAPH);

     if ( (SgProject::get_verbose() > 0) && (numberOfASTnodesBeforeMerge < MAX_NUMBER_OF_IR_NODES_TO_GRAPH))
        {
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMerge");
          set<SgNode*> emptySet;
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST_unfiltered",emptySet);
          generateWholeGraphOfAST(filename+"_beforeMergeWholeAST_unfiltered",emptySet);
          set<SgNode*> skippedNodeSet = getSetOfFrontendSpecificNodes();
       // SimpleColorMemoryPoolTraversal::generateGraph(filename+"_beforeMergeWholeAST",skippedNodeSet);
          generateWholeGraphOfAST(filename+"_beforeMergeWholeAST",skippedNodeSet);
       // SimpleColorFilesTraversal::generateGraph(project,filename+"_beforeMerge",skippedNodeSet);
          generateGraphOfAST(project,filename+"_beforeMerge",skippedNodeSet);
        }

     std::list< pair<SgNode*,SgNode*> > ODR_Violations;

     int replacementHashTableSize = 1001;
     int mangledNameHashTableSize = 1001;

     std::set<SgNode*>  intermediateDeleteSet;

     MangledNameMapTraversal::MangledNameMapType mangledNameMap (mangledNameHashTableSize);

     ROSE_ASSERT(intermediateDeleteSet.empty() == true);
     generateMangledNameMap(mangledNameMap,intermediateDeleteSet);

     ReplacementMapTraversal::ReplacementMapType replacementMap(replacementHashTableSize);

     replacementMapTraversal(mangledNameMap,replacementMap,ODR_Violations,intermediateDeleteSet);

     std::set<SgNode*> problemSubtreeSet = ReplacementMapTraversal::buildListOfODRviolations(ODR_Violations);


  // ****************************************************************************
  // ***********************  Fixup AST to Share IR nodes  **********************
  // ****************************************************************************
  // This traversal of the replacement map modified the AST to reset pointers to subtrees that will be shared.
  // The whole AST is traversed (using the memory pool traversal) and the data member pointers to IR nodes that
  // are found in the replacement map are used to lookup the replacement values that are used to reset the 
  // pointers in the AST. As the replacement is computed the pointer values that are marked in the replacement
  // list for update are added to the intermediateDeleteSet.

     fixupTraversal(replacementMap,intermediateDeleteSet);

     finalDeleteSet = buildDeleteSet(project);

     int numberOfASTnodesBeforeDelete = numberOfNodes();

     deleteNodes(finalDeleteSet);

     int numberOfASTnodesAfterDelete = numberOfNodes();

  // Note that percentageCompression and percentageSpaceSavings are computed as defined at: http://en.wikipedia.org/wiki/Data_compression_ratio
     double percentageCompression  = ( ((double) numberOfASTnodesAfterDelete) / ((double) numberOfASTnodesBeforeDelete) ) * 100.0;
     double percentageSpaceSavings = 100.0 - ( ((double) numberOfASTnodesAfterDelete) / ((double) numberOfASTnodesBeforeDelete) ) * 100.0;
     double mergeFactor            = ( ((double) numberOfASTnodesBeforeDelete) / ((double) numberOfASTnodesAfterDelete) );

     double numberOfFiles  = project->numberOfFiles();
     double mergeEfficency = 0.0;

  // Handle the special case of a single file.
     if (numberOfFiles == 1)
        {
       // For a single file we expect no decrease to get 100% efficiency (but we do get something for better because of the sharing of types)
          mergeEfficency = 1.0 + percentageSpaceSavings;
        }
       else
        {
          double fileNumberMultiplier = numberOfFiles / (numberOfFiles - 1);
          mergeEfficency = percentageSpaceSavings * fileNumberMultiplier;
        }

     if (SgProject::get_verbose() > 0)
        {
          printf ("***************************************************************************************************************************************************************************************************************** \n");
          printf ("After AST delete: numberOfASTnodesBeforeMerge = %d numberOfASTnodesAfterDelete = %d (%d node decrease: %2.4lf percent compression, %2.4lf percent space savings, mergeEfficency = %2.4lf, mergeFactor = %2.4lf) \n",numberOfASTnodesBeforeMerge,numberOfASTnodesAfterDelete,numberOfASTnodesBeforeDelete-numberOfASTnodesAfterDelete,percentageCompression,percentageSpaceSavings,mergeEfficency,mergeFactor);
          printf ("***************************************************************************************************************************************************************************************************************** \n\n\n");
        }
   }




// ****************************************************************
// ****************************************************************
//  Functions supporting deletion of disconnected parts of the AST
// ****************************************************************
// ****************************************************************

void
accumulateSaveSet ( SgNode* node, set<SgNode*> & saveSet )
   {
  // This function accumulates all the children of the current IR node into the saveSet 
  // so that we can assemble the set of IR nodes that are connected in the AST.  Using this
  // set we will traverse the memory pools of the AST and identify disconnected IR nodes
  // from the AST and save them to a separate list of nodes to be deleted as part of the 
  // AST merge.

#if 0
     printf ("Inside of accumulateSaveSet ( node = %p = %s, saveSet.size() = %" PRIuPTR " ) \n",node,node->class_name().c_str(),saveSet.size());
#endif

  // Save the current IR node
     saveSet.insert(node);

  // Traverse the child nodes and add them to the list to save
     typedef vector<pair<SgNode*,string> > DataMemberMapType;
     DataMemberMapType dataMemberMap = node->returnDataMemberPointers();

     DataMemberMapType::iterator i = dataMemberMap.begin();
     while (i != dataMemberMap.end())
        {
       // Ignore the parent pointer since it will be reset differently if required
          SgNode* childPointer = i->first;
          string  debugString  = i->second;

          if (childPointer != NULL)
             {
#if 0
               printf ("At node = %p = %s on edge %s found child %p = %s \n",node,node->class_name().c_str(),debugString.c_str(),childPointer,childPointer->class_name().c_str());
#endif
               if (saveSet.find(childPointer) == saveSet.end())
                  {
                    accumulateSaveSet(childPointer,saveSet);
                  }
             }

          i++;
        }
   }


void
accumulateSaveSetForPreprocessingInfo ( set<SgNode*> & saveSet )
   {
  // Traverse the Memory pools and build a set of IR nodes that are not in the saveSet.
  // Note: This function need only traverse the Sg_File_Info IR node memory pool.
  // We will save EVERY Sg_File_Info object that is associated with a SgPreprocessingInfo 
  // object.

  // Class declaration used only by this function (a memory pool traversal).
     class Traversal : public ROSE_VisitTraversal
        {
          public:
               set<SgNode*> & saveSet;

               Traversal(set<SgNode*> & s) : saveSet(s) {}

               void visit (SgNode* node)
                  {
                    ROSE_ASSERT(node != NULL);
                    if (saveSet.find(node) == saveSet.end())
                       {
                         SgNode* parent = node->get_parent();
#if 0
                         printf ("Inside of accumulateSaveSetForPreprocessingInfo ( node = %p = %s ) saveSet.size() = %" PRIuPTR " \n",node,node->class_name().c_str(),saveSet.size());
                         if (parent != NULL)
                              printf ("Inside of accumulateSaveSetForPreprocessingInfo() parent = %p = %s ) \n",parent,parent->class_name().c_str());
#endif
                      // Test for the implicit signature of a Sg_File_Info object that is used as the location in a PreprocessingInfo object.
                         Sg_File_Info* fileInfo = isSg_File_Info(node);
                      // if (fileInfo != NULL && isSgTypeDefault(parent) != NULL)
                         if (fileInfo != NULL && parent == SgTypeDefault::createType())
                            {
#if 0
                              printf ("saving this Sg_File_Info node to the delete list (associated with PreprocessingInfo object) fileInfo = %p \n",fileInfo);
#endif

                           // Note that parents of Sg_File_Info objects which are SgTypeDefault should 
                           // be associated with comments or CPP directives. So test this.
                              ROSE_ASSERT(fileInfo->isCommentOrDirective() == true);

                           // Save the current IR node (and it type (parent) and the SgTypeDefault parts (SgTypedefSeq).
                              accumulateSaveSet(node,saveSet);
                            }
                       }
                  }
        };
#if 0
     printf ("Inside of accumulateSaveSetForPreprocessingInfo(): This function need only traverse the Sg_File_Info IR node memory pool. \n");
#endif

  // Build the traversal and call it on the memory pools.
     Traversal t(saveSet);
     t.traverseMemoryPool();
   }

set<SgNode*>
accumulateDeleteSet ( SgProject* project, const set<SgNode*> & saveSet )
   {
  // Traverse the Memory pools and build a set of IR nodes that are not in the saveSet.

     class Traversal : public ROSE_VisitTraversal
        {
          public:
               const set<SgNode*> & saveSet;
               set<SgNode*> deleteSet;

               Traversal(const set<SgNode*> & s) : saveSet(s) 
                  {
                  }

               void visit (SgNode* node)
                  {

                    if (isSgTypeUnknown(node)) return;
                    if (isSgTypeChar(node)) return;
                    if (isSgTypeSignedChar(node)) return;
                    if (isSgTypeUnsignedChar(node)) return;
                    if (isSgTypeShort(node)) return;
                    if (isSgTypeSignedShort(node)) return;
                    if (isSgTypeUnsignedShort(node)) return;
                    if (isSgTypeInt(node)) return;
                    if (isSgTypeSignedInt(node)) return;
                    if (isSgTypeUnsignedInt(node)) return;
                    if (isSgTypeLong(node)) return;
                    if (isSgTypeSignedLong(node)) return;
                    if (isSgTypeUnsignedLong(node)) return;
                    if (isSgTypeVoid(node)) return;
                    if (isSgTypeGlobalVoid(node)) return;
                    if (isSgTypeWchar(node)) return;
                    if (isSgTypeFloat(node)) return;
                    if (isSgTypeDouble(node)) return;
                    if (isSgTypeLongLong(node)) return;
                    if (isSgTypeSignedLongLong(node)) return;
                    if (isSgTypeUnsignedLongLong(node)) return;
                    if (isSgTypeLongDouble(node)) return;
                    if (isSgTypeBool(node)) return;
                    if (isSgNamedType(node)) return;
                    if (isSgPartialFunctionModifierType(node)) return;
                    if (isSgTypeEllipse(node)) return;
                    if (isSgTypeDefault(node)) return;
                    if (saveSet.find(node) == saveSet.end())
                       {
                      // Test if this node is already in the set.
                         if (deleteSet.find(node) == deleteSet.end())
                            {
                              if (isSgStorageModifier(node) == NULL)
                                 {
                                   deleteSet.insert(node);
                                 }
                            }
                       }
                  }
        };

     Traversal t(saveSet);
     t.traverseMemoryPool();

     return t.deleteSet;
   }

set<SgNode*>
buildDeleteSet( SgProject* project )
   {
  // DQ (7/3/2010): Implementation of alternative appraoch to define the list 
  // of redundant nodes to delete based on the detection of nodes disconnected 
  // from the modified AST after the merge.

     std::set<SgNode*> saveSet;
     std::set<SgNode*> returnDeleteSet;

     int numberOfASTnodesBeforeComputingDeleteSet = numberOfNodes();
  // printf ("numberOfASTnodesBeforeComputingDeleteSet = %d \n",numberOfASTnodesBeforeComputingDeleteSet);

  // Step 1: Compute the set of IR nodes in the current AST.
     accumulateSaveSet(project,saveSet);
  // printf ("Computing the IR nodes to be deleted saveSet.size() = %" PRIuPTR " \n",saveSet.size());

  // DQ (7/10/2010): These are not handled in the MangledNameMapTraversal constructor (types are handled directly)
    ROSE_ASSERT(SgNode::get_globalFunctionTypeTable() != NULL);
     accumulateSaveSet(SgNode::get_globalFunctionTypeTable(),saveSet);

     ROSE_ASSERT(SgNode::get_globalTypeTable() != NULL);
     accumulateSaveSet(SgNode::get_globalTypeTable(),saveSet);

  // Use a simple macro to simplify the generation of the correct code.
#define MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(TYPE) if (TYPE::numberOfNodes() > 0) { TYPE* t = TYPE::createType(); accumulateSaveSet(t,saveSet); }

#if 1
  // DQ (7/10/2010): Note that this will cause any required types to be built which can 
  // then be used to as references to those types via the p_builtin_type static data member.
  // This should be refactored to be a function generated by ROSETTA.

  // Call the macro for every kind of type used in ROSE.
  // Note that this should later be a function generated by ROSETTA.
  // There are arbitrarily many binary types, so exclude them from this list. [Robb P. Matzke 2014-07-21]
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeUnknown)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeChar)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeSignedChar)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeUnsignedChar)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeShort)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeSignedShort)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeUnsignedShort)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeInt)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeSignedInt)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeUnsignedInt)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeLong)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeSignedLong)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeUnsignedLong)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeVoid)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeGlobalVoid)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeWchar)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeFloat)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeDouble)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeLongLong)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeSignedLongLong)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeUnsignedLongLong)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeLongDouble)
  // MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeString)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeBool)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgNamedType)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgPartialFunctionModifierType)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeEllipse)
     MACRO_ADD_STATIC_TYPE_TO_SAVE_SET(SgTypeDefault)
#endif

  // Note that there are Sg_File_Info objects that are associated with PreprocessingInfo objects, and since the SgPreprocessingInfo class
  // is not generated by ROSETTA, the partent of each Sg_File_Info object is defined (somewhat arbitrarily) to be a SgDefaultType object.
  // A special function is implemented to process these specific IR nodes since they are disconnected from the AST proper and we don't 
  // want them to be removed as part of the AST merge (compression).
  // printf ("Handle Sg_File_Info objects that are associated with PreprocessingInfo objects: Computing the IR nodes to be deleted saveSet.size() = %" PRIuPTR " \n",saveSet.size());
     accumulateSaveSetForPreprocessingInfo(saveSet);

#if 0
     printf ("Computing the IR nodes to be deleted saveSet.size() = %" PRIuPTR " \n",saveSet.size());
#endif

     set<SgNode*> tempDeleteSet = accumulateDeleteSet(project,saveSet);

     returnDeleteSet = tempDeleteSet;

     int numberOfASTnodesAfterComputingDeleteSet = numberOfNodes();

#if 0
     printf ("numberOfASTnodesBeforeComputingDeleteSet = %d numberOfASTnodesAfterComputingDeleteSet = %d \n",
          numberOfASTnodesBeforeComputingDeleteSet,numberOfASTnodesAfterComputingDeleteSet);
#endif

  // DQ (7/11/2010): Test this since I would like to assert that it is true, but we can't do that yet!
     if (numberOfASTnodesBeforeComputingDeleteSet != numberOfASTnodesAfterComputingDeleteSet)
        {
          printf ("Warning: numberOfASTnodesBeforeComputingDeleteSet = %d != numberOfASTnodesAfterComputingDeleteSet = %d \n",
               numberOfASTnodesBeforeComputingDeleteSet,numberOfASTnodesAfterComputingDeleteSet);
        }

     return returnDeleteSet;
   }

