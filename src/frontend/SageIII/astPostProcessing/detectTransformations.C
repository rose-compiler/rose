// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "detectTransformations.h"

// #ifdef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT
//    #include "transformationSupport.h"
// #endif

void
detectTransformations( SgNode* node )
   {
  // DQ (7/7/2005): Introduce tracking of performance of ROSE.
     TimingPerformance timer ("detectTransformations(): Testing declarations (no side-effects to AST):");

     class DetectTransformationsOnMemoryPool : public ROSE_VisitTraversal
        {
          public:
            // Required traversal function
               void visit (SgNode* node)
                  {
                 // For now we just do this for the Sg_File_Info directly.
                    Sg_File_Info* fileInfo = isSg_File_Info(node);
                    if (fileInfo != NULL)
                       {
                         if (fileInfo->isTransformation() == true)
                            {
                              printf ("ERROR: detected fileInfo->isTransformation() == true (using memory pool traversal) for fileInfo = %p \n",fileInfo);
                            }
                      // ROSE_ASSERT(fileInfo->isTransformation() == false);
                       }
                  }

            // This avoids a warning by g++
               virtual ~DetectTransformationsOnMemoryPool() {};         
        };

  // This simplifies how the traversal is called!
     DetectTransformations detectTransformationsTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     detectTransformationsTraversal.traverse(node,preorder);

  // This will traverse the whole memory pool (it double checks the previous test by testing 
  // every possible IR node, more than just those in the AST).
     DetectTransformationsOnMemoryPool traversal;
     traversal.traverseMemoryPool();

  // This would traverse only the Sg_File_Info IR nodes (a performance improvement).
  // Sg_File_Info::traverseMemoryPoolNodes(traversal);
   }


void
detectTransformations_local( SgNode* node )
   {
  // This simplifies how the traversal is called!
     DetectTransformations detectTransformationsTraversal;

  // I think the default should be preorder so that the interfaces would be more uniform
     detectTransformationsTraversal.traverse(node,preorder);
   }


void
DetectTransformations::visit (SgNode* node)
   {
     ROSE_ASSERT(node != NULL);

     SgLocatedNode* locatedNode = isSgLocatedNode(node);
     if (locatedNode != NULL)
        {
       // Tests that we expect to pass for all SgLocatedNode IR nodes.
       // This test first checks for valid pointers at startOfConstruct and endOfConstruct
       // then checks to make sure it is not marked as a transformation.

          if (locatedNode->get_startOfConstruct() == NULL)
             {
               printf ("ERROR: locatedNode->get_startOfConstruct() == NULL locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          ROSE_ASSERT(locatedNode->get_startOfConstruct() != NULL);
#if 0
       // DQ (5/3/2012): SgInitializedName did not used to have a valid endOfConstruct, but this has been fixed to make the rules more uniform.
          if (isSgInitializedName(locatedNode) == NULL && locatedNode->get_endOfConstruct() == NULL)
             {
               printf ("ERROR: locatedNode->get_endOfConstruct() == NULL locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          ROSE_ASSERT(isSgInitializedName(locatedNode) != NULL || locatedNode->get_endOfConstruct() != NULL);
#else
       // DQ (5/3/2012): SgInitializedName did not used to have a valid endOfConstruct, but this has been fixed to make the rules more uniform.
          if (locatedNode->get_endOfConstruct() == NULL)
             {
               printf ("ERROR: locatedNode->get_endOfConstruct() == NULL locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          ROSE_ASSERT(locatedNode->get_endOfConstruct() != NULL);
#endif

          if (locatedNode->get_startOfConstruct()->isTransformation() == true)
             {
               printf ("ERROR: locatedNode->get_startOfConstruct()->isTransformation() == true locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          ROSE_ASSERT(locatedNode->get_startOfConstruct()->isTransformation() == false);

#if 0
       // DQ (5/3/2012): SgInitializedName did not used to have a valid endOfConstruct, but this has been fixed to make the rules more uniform.
          if (isSgInitializedName(locatedNode) == NULL && locatedNode->get_endOfConstruct()->isTransformation() == true)
             {
               printf ("ERROR: locatedNode->get_endOfConstruct()->isTransformation() == true locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          ROSE_ASSERT(isSgInitializedName(locatedNode) != NULL || locatedNode->get_endOfConstruct()  ->isTransformation() == false);
#else
       // DQ (5/3/2012): SgInitializedName did not used to have a valid endOfConstruct, but this has been fixed to make the rules more uniform.
          if (locatedNode->get_endOfConstruct()->isTransformation() == true)
             {
               printf ("ERROR: locatedNode->get_endOfConstruct()->isTransformation() == true locatedNode = %p = %s \n",locatedNode,locatedNode->class_name().c_str());
             }
          ROSE_ASSERT(locatedNode->get_endOfConstruct()->isTransformation() == false);
#endif
        }
       else
        {
       // Handle case of SgPragma (which should be moved to be a SgLocatedNode).
          SgPragma* pragma = isSgPragma(node);
       // if ( (pragma != NULL) && (pragma->get_startOfConstruct() == NULL) )
          if (pragma != NULL)
             {
            // pragma->set_startOfConstruct(Sg_File_Info::generateDefaultFileInfoForTransformationNode());
            // pragma->get_startOfConstruct()->set_parent(pragma);

               if (pragma->get_startOfConstruct() == NULL)
                  {
                    printf ("ERROR: pragma->get_startOfConstruct() == NULL pragma = %p \n",pragma);
                  }
               ROSE_ASSERT(pragma->get_startOfConstruct() != NULL);

               if (pragma->get_endOfConstruct() == NULL)
                  {
                    printf ("ERROR: pragma->get_endOfConstruct() == NULL pragma = %p \n",pragma);
                  }
               ROSE_ASSERT(pragma->get_endOfConstruct() != NULL);

               if (pragma->get_startOfConstruct()->isTransformation() == true)
                  {
                    printf ("ERROR: pragma->get_startOfConstruct()->isTransformation() == true pragma = %p \n",pragma);
                  }
               ROSE_ASSERT(pragma->get_startOfConstruct()->isTransformation() == false);

               if (pragma->get_endOfConstruct()->isTransformation() == true)
                  {
                    printf ("ERROR: pragma->get_endOfConstruct()->isTransformation() == true pragma = %p \n",pragma);
                  }
               ROSE_ASSERT(pragma->get_endOfConstruct()->isTransformation() == false);
             }
        }
   }

