// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <fstream>
#include "walrus.h"

using namespace std;

void
WalrusGraph::Counter::visit ( SgNode* astNode )
   {
     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     if ( (isBinary == true) || (locatedNode != NULL && locatedNode->get_file_info()->isFrontendSpecific() == false))
        {
          node_to_index_map[astNode] = counter++;
        }
   }

void
WalrusGraph::OutputEdges::visit ( SgNode* astNode )
   {
  // printf ("astNode = %p = %s \n",astNode,astNode->class_name().c_str());

     int numberOfNodes = node_to_index_map.size()+1;
     static int num = 0;

     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     if ((isBinary == true) || (locatedNode != NULL && locatedNode->get_file_info()->isFrontendSpecific() == false))
        {
          if (isSgGlobal(astNode) == NULL)
             {
               printf ("            { @source=%d; @destination=%d; }",node_to_index_map[astNode->get_parent()],node_to_index_map[astNode]);
               *outputFilePtr << "            { @source=" << node_to_index_map[astNode->get_parent()] << "; @destination=" << node_to_index_map[astNode] << "; }";

               if (num < numberOfNodes-2)
                  {
                    printf (",");
                    *outputFilePtr << ",";
                  }
               printf ("\n");
               *outputFilePtr << endl;
             }
          
          num++;
        }
   }

void
WalrusGraph::OutputNodes::visit ( SgNode* astNode )
   {
     int numberOfNodes = node_to_index_map.size();
     static int num = 0;

     SgLocatedNode* locatedNode = isSgLocatedNode(astNode);
     if ((isBinary == true) || (locatedNode != NULL && locatedNode->get_file_info()->isFrontendSpecific() == false))
        {
       // if (node_to_index_map[astNode] < numberOfNodes-2);
          if (num < numberOfNodes-1)
             {
               printf ("            { @id=%d; @value=T; }",node_to_index_map[astNode]);
               *outputFilePtr << "            { @id=" << node_to_index_map[astNode] << "; @value=T; }";
               if (num < numberOfNodes-2)
                  {
                    printf (",");
                    *outputFilePtr << ",";
                  }
               printf ("\n");
               *outputFilePtr << endl;
             }

          num++;
        }
   }

void
WalrusGraph::generateEdges( SgNode* node )
   {
     int numberOfNodes = node_to_index_map.size();
     int numberOfEdges = node_to_index_map.size()-1;

     printf ("Graph\n{\n   ### metadata ###\n   @name=\"ROSE AST\";\n   @description=\"A complete ROSE AST.\";\n   @numNodes=%d;\n   @numLinks=%d;\n   @numPaths=0;\n   @numPathLinks=0;\n   ### structural data ###\n   @links=[\n",numberOfNodes,numberOfEdges);
     *outputFilePtr << "Graph\n{\n   ### metadata ###\n   @name=\"ROSE AST\";\n   @description=\"A complete ROSE AST.\";\n   @numNodes=" << numberOfNodes << ";\n   @numLinks=" << numberOfEdges << ";\n   @numPaths=0;\n   @numPathLinks=0;\n   ### structural data ###\n   @links=[\n";

     OutputEdges traversal;

     traversal.traverse(node,preorder);

     printf ("   ];\n   @paths=;\n\n   ### attribute data ###\n   @enumerations=;\n   @attributeDefinitions=[\n      {\n         @name=$root;\n         @type=bool;\n         @default=|| false ||;\n         @nodeValues=[ { @id=0; @value=T; } ];\n");
     printf ("         @linkValues=;\n         @pathValues=;\n      },\n      {\n         @name=$tree_link;\n         @type=bool;\n         @default=|| false ||;\n         @nodeValues=;\n         @linkValues=[\n");

     *outputFilePtr << "   ];\n   @paths=;\n\n   ### attribute data ###\n   @enumerations=;\n   @attributeDefinitions=[\n      {\n         @name=$root;\n         @type=bool;\n         @default=|| false ||;\n         @nodeValues=[ { @id=0; @value=T; } ];\n";
     *outputFilePtr << "         @linkValues=;\n         @pathValues=;\n      },\n      {\n         @name=$tree_link;\n         @type=bool;\n         @default=|| false ||;\n         @nodeValues=;\n         @linkValues=[\n";

#if 0
     printf ("Exiting as a test ... processing SgGraph_outputEdges::generateEdges() \n");
     ROSE_ASSERT(false);
#endif
   }
void
WalrusGraph::generateNodes( SgNode* node )
   {
     OutputNodes traversal;

     traversal.traverse(node,preorder);

     printf ("         ];\n         @pathValues=;\n      }\n");
     printf ("   ];\n   @qualifiers=[\n      {\n         @type=$spanning_tree;\n         @name=$sample_spanning_tree;\n         @description=;\n         @attributes=[\n");
     printf ("            { @attribute=0; @alias=$root; },\n            { @attribute=1; @alias=$tree_link; }\n         ];\n      }\n   ];\n   ### visualization hints ###\n");
     printf ("   @filters=;\n   @selectors=;\n   @displays=;\n   @presentations=;\n\n   ### interface hints ###\n   @presentationMenus=;\n   @displayMenus=;\n   @selectorMenus=;\n   @filterMenus=;\n   @attributeMenus=;\n}\n");

     *outputFilePtr << "         ];\n         @pathValues=;\n      }\n";
     *outputFilePtr << "   ];\n   @qualifiers=[\n      {\n         @type=$spanning_tree;\n         @name=$sample_spanning_tree;\n         @description=;\n         @attributes=[\n";
     *outputFilePtr << "            { @attribute=0; @alias=$root; },\n            { @attribute=1; @alias=$tree_link; }\n         ];\n      }\n   ];\n   ### visualization hints ###\n";
     *outputFilePtr << "   @filters=;\n   @selectors=;\n   @displays=;\n   @presentations=;\n\n   ### interface hints ###\n   @presentationMenus=;\n   @displayMenus=;\n   @selectorMenus=;\n   @filterMenus=;\n   @attributeMenus=;\n}\n";

#if 0
     printf ("Exiting as a test ... processing SgGraph_outputNodes::generateNodes() \n");
     ROSE_ASSERT(false);
#endif
   }

void
WalrusGraph::generateWalrusGraph( SgNode* node, string outputFilename )
   {
// Notes: The generated file in the Walrus format typically requies some editing by hand.
// For large files the "comma" always has to be remove at the end of the edge list and the node list.
// For Binary files the first edge which points to itself ("source=0 distination=0") has to be removed.
// Then the last entry for the node list has to be removed.
// Then is can often work.


     SgProject* project = isSgProject(node);
     if (project != NULL)
        {
          printf ("Found the SgProject \n");

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
          SgFile* fileNode = project->get_fileList()[0];
          if (isSgBinaryComposite(fileNode) != NULL)
             {
               if (project->get_binary_only() == false)
                    printf ("This should have been set already, what is wrong here! \n");

               project->set_binary_only(true);
             }
#endif

          if (project->get_binary_only() == true)
             {
               printf ("Found a binary file \n");
               WalrusGraph::isBinary = true;
             }
        }

     Counter traversal;
     traversal.traverse(node,preorder);

     printf ("node_to_index_map.size() = %" PRIuPTR " \n",node_to_index_map.size());

     filebuf fb;
     fb.open (outputFilename.c_str(),ios::out);
     ostream os(&fb);

     ostream outputFile(&fb);
     outputFilePtr = &outputFile;
     ROSE_ASSERT(outputFilePtr != NULL);

     generateEdges(node);
     generateNodes(node);

     outputFile.flush();
   }

