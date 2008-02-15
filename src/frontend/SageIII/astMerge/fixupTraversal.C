// #include "merge.h"
#include "rose.h"

using namespace std;

FixupTraversal::FixupTraversal ( const ReplacementMapTraversal::ReplacementMapType & inputReplacementMap, const listToDeleteType & inputListToDelete )
   : replacementMap(inputReplacementMap), deleteList(inputListToDelete)
   {
     numberOfNodes                                                           = 0;
     numberOfNodesTested                                                     = 0;
     numberOfDataMemberPointersEvaluated                                     = 0;
     numberOfValidDataMemberPointersEvaluated                                = 0;
     numberOfValidDataMemberPointersWithValidKeyEvaluated                    = 0;
     numberOfValidDataMemberPointersWithValidKeyButNotInReplacementMap       = 0;
     numberOfValidDataMemberPointersWithValidKeyAndInReplacementMap          = 0;
     numberOfValidDataMemberPointersWithValidKeyAndInReplacementMapEvaluated = 0;
     numberOfValidDataMemberPointersReset                                    = 0;
   }

void
FixupTraversal::visit ( SgNode* node)
   {
  // This visit function will visit ALL IR nodes of the memory pool
  // Any pointers to data member in replacementMap will be reset to 
  // new values (or the merged AST IR nodes) also stored in the replacementMap.

     ROSE_ASSERT(node != NULL);
  // printf ("FixupTraversal::visit: node = %p = %s \n",node,node->class_name().c_str());

  // Symbol tables can't be manipulated this way if the keys can change
  // ROSE_ASSERT(isSgSymbolTable(node) == NULL);

  // Keep a count of the number of IR nodes visited
     numberOfNodes++;

#if 0
  // DQ (2/9/2007): We must process symbols because symbols have pointers to declarations
  // Skip the processing of the SgSymbolTable since entries their are organized based on internal values
  // we will have to handle this as a special case.
  // SgSymbolTable* symbolTable = isSgSymbolTable(node);
  // if (symbolTable != NULL)
  //    return;
#endif
#if 0
// These IR nodes must be process because they contain pointers which must be reset.
  // DQ (2/8/2007): Skip resetting IR nodes that have been identified to be deleted.  This will 
  // allow us to reference their unmodified scope (for example) and find symbols in the symbol table.
  // This allows new lists to be build for both required IR nodes and IR nodes to be deleted,
  // this set_difference permits any shared nodes that appeared on both list to be removed 
  // from the final delete list (which is handed off to the deleteNodes() function).
  // bool nodeIsInDeleteList = deleteList.find(node) != deleteList.end();
  // if (nodeIsInDeleteList == true)
  //    {
  //       printf ("FixupTraversal::visit(): node found in deleteList = %p = %s = %s \n",node,node->class_name().c_str(),SageInterface::get_name(node).c_str());
  //       return;
  //    }
#endif

  // Keep a count of the number of IR nodes tested (whose data members are evaluated)
     numberOfNodesTested++;

  // Only traverse list of IR node pointers on each IR node.
     typedef vector<pair<SgNode**,string> > DataMemberVectorType;

  // We require the reference version of this function because we want to update the (referenced) pointer values.
  // Question: Can this be a const ref for the dataMemberMap?  This would avoid copying of STL maps.
  // DataMemberMapType dataMemberMap = node->returnDataMemberPointers();
     const DataMemberVectorType & dataMemberVector = node->returnDataMemberReferenceToPointers();

  // printf ("dataMemberVector.size() = %ld \n",dataMemberVector.size());
     bool traceReplacement = false;
#if 0
     if (isSg_File_Info(node) == NULL)
          traceReplacement = true;
#endif
#if 0
     if (isSgSymbolTable(node) != NULL)
          traceReplacement = true;
#endif
     if (traceReplacement == true)
        {
          printf ("FixupTraversal::visit: node = %p = %s dataMemberVector.size() = %zu \n",node,node->class_name().c_str(),dataMemberVector.size());
        }

     DataMemberVectorType::const_iterator i = dataMemberVector.begin();
     while (i != dataMemberVector.end())
        {
       // Ignore the parent pointer since it will be reset differently if required
          SgNode** pointerToKey = i->first;

       // Keep a count of the number of IR nodes evaluated.
          numberOfDataMemberPointersEvaluated++;

       // DQ (5/20/2006): Data members that are NULL will have been put into the dataMemberVector as NULL pointers so we have to skip over them.
          if (pointerToKey != NULL)
             {
            // For node, this is the reference to the field containing a pointer 
            // to another IR node (this will be a key into the replacementMap).
               SgNode* key           = *(i->first);
            // printf ("key  = %p = %p = %s \n",key,dynamic_cast<SgNode*>(key),key != NULL ? key->class_name().c_str() : "NULL");
            // printf ("pointerToKey != NULL: key  = %p \n",key);

            // Keep a count of the number of IR data members that are non-null
               numberOfValidDataMemberPointersEvaluated++;

            // DQ (2/8/2007): Only handle non-NULL valued pointers (only valid pointers to IR nodes that need be reset).
            // However, since we have few NULL pointers in the AST by design this is not a significant optimization.
               if (key != NULL)
                  {
                 // Keep a count of the number of IR data members that are non-null (where the key is also non-null)
                    numberOfValidDataMemberPointersWithValidKeyEvaluated++;

                 // printf ("key  = %p = %p = %s \n",key,dynamic_cast<SgNode*>(key),key->class_name().c_str());
                 // printf ("key  = %p = %s \n",key,key->class_name().c_str());
                 // printf ("key  = %p \n",key);
#if 0
                 // This is the field name of the data member (helps with debugging to know what it is)
                    const string & debugString  = i->second;

                 // Lookup the key in the replacementMap.  If present then the replacement map stores the 
                 // values required to reset the pointer FROM the unshared IR node and TO the shared IR node.
                    ReplacementMapTraversal::ReplacementMapType::const_iterator lowerBound = replacementMap.lower_bound(key);
                    ReplacementMapTraversal::ReplacementMapType::const_iterator upperBound = replacementMap.upper_bound(key);

                    if (lowerBound == upperBound)
                       {
                      // Keep a count of the number of case where the replacement list was empty.
                         numberOfValidDataMemberPointersWithValidKeyButNotInReplacementMap++;
#if 0
                         if (traceReplacement == true)
                            {
                              ROSE_ASSERT(key != NULL);
                              ROSE_ASSERT(node != NULL);
                              printf ("debugString = %s \n",debugString.c_str());
                           // printf ("node = %p \n",node);
                              printf ("node = %p = %s \n",node,node->class_name().c_str());
                              printf ("key  = %p \n",key);
                           // printf ("dynamic_cast<SgNode*>(key) = %p \n",dynamic_cast<SgNode*>(key));
                           // printf ("key  = %p = %s \n",key,dynamic_cast<SgNode*>(key)->class_name().c_str());
                           // printf ("FixupTraversal::visit(): Key not present in replacementMap, node = %p = %s: key = %p = %s at data member %s is not in the replacementMap to be updated to a shared IR node \n",
                           //      node,node->class_name().c_str(),key,key->class_name().c_str(),debugString.c_str());
                            }
#endif
                       }
                      else
                       {
                      // Keep a count of the number of case where the replacement list was valid.
                         numberOfValidDataMemberPointersWithValidKeyAndInReplacementMap++;
                       }
                    
                 // Loop over all those in the replacement map that need to have child pointers to be updated
                    int loopCounter = 0;
                    while (lowerBound != upperBound)
                       {
                         SgNode* originalNode = (replacementMap.lower_bound(key))->second;

                      // Keep a count of the number of case where the replacement list was valid.
                         numberOfValidDataMemberPointersWithValidKeyAndInReplacementMapEvaluated++;

                         if (traceReplacement == true)
                            {
                              printf ("FixupTraversal::visit(): Set the key (IR node) (%p = %s) on %p = %s to %p = %s \n",key,debugString.c_str(),node,node->class_name().c_str(),originalNode,originalNode->class_name().c_str());
                            }

                         ROSE_ASSERT(*pointerToKey == key);

                         if (key != originalNode)
                            {
                           // Now reset the pointer to the subtree identified as redundent with
                           // a subtree in the original AST to the subtree in the original AST.
                           // *pointerToKey = node;
                              *pointerToKey = originalNode;

                           // Keep a count of the number of IR nodes that are reset.
                              numberOfValidDataMemberPointersReset++;
                            }

                         lowerBound++;
                         loopCounter++;
                       }

                 // DQ (2/8/2007): If the loop trip is only 0 or 1 then we don't need a loop here!
                 // I think that by design the loop trip is at most 1!
                 // printf ("FixupTraversal::visit(): loop from replacementMap.lower_bound(key) to replacementMap.upper_bound(key): loopCounter = %d \n",loopCounter);
                    ROSE_ASSERT(loopCounter < 2);
                 // end of block for "if(key != NULL)"
#else
                 // Since the ReplacementMapTraversal::ReplacementMapType is a now a map rather 
                 // than a multi-map, the use of it is simple and should be more efficient.

                 // Keep a count of the number of case where the replacement list was valid.
                    numberOfValidDataMemberPointersWithValidKeyAndInReplacementMapEvaluated++;

                 // Check that the key is in the map
                 // DQ (2/19/2007): This is more efficient since it looks up the element from the map only once.
                    ReplacementMapTraversal::ReplacementMapType::const_iterator replacementMap_it = replacementMap.find(key);
                    if (replacementMap_it != replacementMap.end())
                       {
                         SgNode* originalNode = replacementMap_it->second;

                         ROSE_ASSERT(*pointerToKey == key);

                         if (traceReplacement == true)
                            {
                              const string & debugString  = i->second;
                              printf ("FixupTraversal::visit(): Set the key (IR node) (%p = %s) on %p = %s to %p = %s \n",key,debugString.c_str(),node,node->class_name().c_str(),originalNode,originalNode->class_name().c_str());
                            }

                      // Skip the trival case of resetting the pointer value to itself!
                         if (key != originalNode)
                            {
                           // Now reset the pointer to the subtree identified as redundent with a
                           // subtree in the original AST to the subtree in the original (merged) AST.
                              *pointerToKey = originalNode;

                           // Keep a count of the number of IR nodes that are reset.
                              numberOfValidDataMemberPointersReset++;
                            }
                       }
                      else
                       {
                      // printf ("replacementMap_it == replacementMap.end() \n");
                       }
#endif
                  }
                 else
                  {
                 // printf ("key == NULL \n");
                  }
             }
            else
             {
            // printf ("pointerToKey == NULL \n");
             }
          
          i++;
        }
   }

void
fixupTraversal( const ReplacementMapTraversal::ReplacementMapType & replacementMap, const std::set<SgNode*> & deleteList )
   {
  // DQ (2/2/2007): Introduce tracking of performance of within AST merge
     TimingPerformance timer ("Reset the AST to share IR nodes:");

     printf ("In fixupTraversal(): replacementMap.size() = %zu deleteList.size() = %zu \n",replacementMap.size(),deleteList.size());
     FixupTraversal traversal(replacementMap,deleteList);
     traversal.traverseMemoryPool();

     printf ("numberOfNodes                                                           = %d \n",traversal.numberOfNodes);
     printf ("numberOfNodesTested                                                     = %d \n",traversal.numberOfNodesTested);
     printf ("numberOfDataMemberPointersEvaluated                                     = %d \n",traversal.numberOfDataMemberPointersEvaluated);
     printf ("numberOfValidDataMemberPointersEvaluated                                = %d \n",traversal.numberOfValidDataMemberPointersEvaluated);
     printf ("numberOfValidDataMemberPointersWithValidKeyEvaluated                    = %d \n",traversal.numberOfValidDataMemberPointersWithValidKeyEvaluated);
     printf ("numberOfValidDataMemberPointersWithValidKeyButNotInReplacementMap       = %d \n",traversal.numberOfValidDataMemberPointersWithValidKeyButNotInReplacementMap);
     printf ("numberOfValidDataMemberPointersWithValidKeyAndInReplacementMap          = %d \n",traversal.numberOfValidDataMemberPointersWithValidKeyAndInReplacementMap);
     printf ("numberOfValidDataMemberPointersWithValidKeyAndInReplacementMapEvaluated = %d \n",traversal.numberOfValidDataMemberPointersWithValidKeyAndInReplacementMapEvaluated);
     printf ("numberOfValidDataMemberPointersReset                                    = %d \n",traversal.numberOfValidDataMemberPointersReset);
   }

