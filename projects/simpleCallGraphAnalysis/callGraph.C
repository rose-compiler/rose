
#include <rose.h>

#include <AstInterface.h>
#include <GraphDotOutput.h>
#include <VirtualGraphCreate.h>

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

class FunctionData
   {
     public:
          FunctionData ( SgFunctionDeclaration* functionDeclaration );

       // Relevant data for call graph
          SgFunctionDeclaration* functionDeclaration;
          Rose_STL_Container<SgFunctionDeclaration*> functionList;
   };

FunctionData::FunctionData ( SgFunctionDeclaration* inputFunctionDeclaration )
   {
     functionDeclaration = inputFunctionDeclaration;
     ROSE_ASSERT(functionDeclaration != NULL);

  // printf ("In FunctionData constructor (function = %s) \n",inputFunctionDeclaration->get_name().str());

     SgFunctionDefinition* functionDefinition = functionDeclaration->get_definition();
     ROSE_ASSERT (functionDefinition != NULL);

     Rose_STL_Container<SgNode*> functionCallExpList;
     functionCallExpList = NodeQuery::querySubTree (functionDefinition,V_SgFunctionCallExp);

  // printf ("functionCallExpList.size() = %zu \n",functionCallExpList.size());

  // list<SgFunctionDeclaration*> functionList;
     Rose_STL_Container<SgNode*>::iterator i = functionCallExpList.begin();
     while (i != functionCallExpList.end())
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(*i);
          ROSE_ASSERT (functionCallExp != NULL);

          SgExpression* functionExp = functionCallExp->get_function();
          ROSE_ASSERT (functionExp != NULL);

          SgFunctionRefExp* functionRefExp             = isSgFunctionRefExp(functionExp);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionExp);
          ROSE_ASSERT ( functionRefExp != NULL || memberFunctionRefExp != NULL );

          SgFunctionSymbol* functionSymbol = NULL;
          if (functionRefExp)
             {
               functionSymbol = functionRefExp->get_symbol();
             }
            else
             {
               ROSE_ASSERT ( memberFunctionRefExp != NULL );
               functionSymbol = memberFunctionRefExp->get_symbol();
             }

          ROSE_ASSERT ( functionSymbol != NULL );

          SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
          ROSE_ASSERT (functionDeclaration != NULL);

          functionList.push_back(functionDeclaration);
          i++;
        }
   }

class CallGraphNode: public MultiGraphElem
   {
     public:
         string label;
         SgFunctionDeclaration* functionDeclaration;

         CallGraphNode ( string label, SgFunctionDeclaration* functionDeclaration )
            : MultiGraphElem(NULL), label(label), functionDeclaration(functionDeclaration) {};
   };

class CallGraphEdge: public MultiGraphElem
   {
     public:
         string label;

         CallGraphEdge ( string label = "default edge" ) : MultiGraphElem(NULL), label(label) {};
   };

class CallGraphCreate : public VirtualGraphCreateTemplate<CallGraphNode,CallGraphEdge>
   {
     public:
          void addNode ( CallGraphNode* node );
          void addEdge ( CallGraphNode *src, CallGraphNode *snk, CallGraphEdge* edge );

          int size();

          CallGraphCreate () : VirtualGraphCreateTemplate<CallGraphNode,CallGraphEdge> (NULL) {}

         ~CallGraphCreate() { printf ("Inside of ~CallGraphCreate() \n"); }
   };

void
CallGraphCreate::addNode ( CallGraphNode* node )
   { VirtualGraphCreateTemplate<CallGraphNode,CallGraphEdge>::AddNode ( node ); }

void
CallGraphCreate::addEdge ( CallGraphNode *src, CallGraphNode *snk, CallGraphEdge* edge )
   { VirtualGraphCreateTemplate<CallGraphNode,CallGraphEdge>::AddEdge ( src, snk, edge ); }

int
CallGraphCreate::size()
   {
     int numberOfNodes = 0;
     for (NodeIterator i = GetNodeIterator(); !i.ReachEnd(); ++i) 
        {
          numberOfNodes++;
        }

     return numberOfNodes;
   }


CallGraphNode* 
findNode ( Rose_STL_Container<CallGraphNode*> & nodeList, SgFunctionDeclaration* functionDeclaration )
   {
     Rose_STL_Container<CallGraphNode*>::iterator k = nodeList.begin();

     CallGraphNode* returnNode = NULL;

     bool found = false;
     while ( !found && (k != nodeList.end()) )
        {
          if ((*k)->functionDeclaration == functionDeclaration)
             {
               returnNode = *k;
               found = true;
             }
          k++;
        }

     ROSE_ASSERT (returnNode != NULL);
     return returnNode;
   }

CallGraphCreate*
buildCallGraph ( SgProject* project )
   {
     Rose_STL_Container<FunctionData*> callGraphData;

     Rose_STL_Container<SgNode*> functionList = NodeQuery::querySubTree (project,V_SgFunctionDeclaration);
     Rose_STL_Container<SgNode*>::iterator i = functionList.begin();

     printf ("Inside of buildCallGraph functionList.size() = %zu \n",functionList.size());

     while (i != functionList.end())
        {
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT (functionDeclaration != NULL);

          FunctionData* functionData = new FunctionData(functionDeclaration);
          callGraphData.push_back(functionData);
          i++;
        }

  // printf ("Build the graph \n");

  // Build the graph
     CallGraphCreate* returnGraph = new CallGraphCreate();
     ROSE_ASSERT (returnGraph != NULL);

     Rose_STL_Container<FunctionData*>::iterator j = callGraphData.begin();

  // printf ("Build the node list callGraphData.size() = %zu \n",callGraphData.size());

     Rose_STL_Container<CallGraphNode*> nodeList;
     while (j != callGraphData.end())
        {
          string functionName = (*j)->functionDeclaration->get_name().str();
       // printf ("functionName = %s \n",functionName.c_str());

       // Generate a unique name to test against later
          SgFunctionDeclaration* id = (*j)->functionDeclaration;

          CallGraphNode* node = new CallGraphNode(functionName,id);
          nodeList.push_back(node);

          returnGraph->addNode(node);
          j++;
        }

  // printf ("Build the edges nodeList.size() = %zu \n",nodeList.size());

     j = callGraphData.begin();
     while (j != callGraphData.end())
        {
          printf ("Calling findNode in outer loop (*j)->functionDeclaration->get_name() = %s \n",(*j)->functionDeclaration->get_name().str());
          CallGraphNode* startingNode = findNode(nodeList,(*j)->functionDeclaration);
          ROSE_ASSERT (startingNode != NULL);

          Rose_STL_Container<SgFunctionDeclaration*> & functionList = (*j)->functionList;
          Rose_STL_Container<SgFunctionDeclaration*>::iterator k = functionList.begin();

       // printf ("Now iterate over the list (size = %d) for function %s \n",
       //      functionList.size(),(*j)->functionDeclaration->get_name().str());

          while (k != functionList.end())
             {
               printf ("Calling findNode in inner loop (*k)->get_name() = %s \n",(*k)->get_name().str());
               CallGraphNode* endingNode = findNode(nodeList,*k);
               ROSE_ASSERT (endingNode != NULL);

               CallGraphEdge* edge = new CallGraphEdge("edgeName");
               ROSE_ASSERT (edge != NULL);

               returnGraph->addEdge(startingNode,endingNode,edge);
               k++;
             }
          j++;
        }

  // printf ("Return graph \n");

     return returnGraph;
   }


void
GenerateDotGraph ( CallGraphCreate* graph )
   {
     ROSE_ASSERT(graph != NULL);

#if 1
     printf ("Output the graph information! (number of nodes = %d) \n",graph->size());

  // This iteration over the graph verifies that we have a valid graph!
     CallGraphCreate::NodeIterator nodeIterator;
     for (nodeIterator = graph->GetNodeIterator(); !nodeIterator.ReachEnd(); ++nodeIterator) 
        {
          printf ("In loop using node iterator ... \n");
       // DAGBaseNodeImpl* nodeImpl = nodeIterator.Current();
          CallGraphNode* node = *nodeIterator;
          CallGraphCreate::EdgeIterator edgeIterator;
          CallGraphCreate::EdgeDirection direction = CallGraphCreate::EdgeOut;
          for (edgeIterator = graph->GetNodeEdgeIterator(node,direction); !edgeIterator.ReachEnd(); ++edgeIterator) 
             {
               printf ("     In loop using edge iterator ... \n");
             }
        }

     printf ("Building the GraphDotOutput object ... \n");
     GraphDotOutput<CallGraphCreate> output(*graph);

     printf ("Leaving GenerateDotGraph ... \n");

#else
     printf ("Building the GraphDotOutput object ... \n");
     GraphDotOutput output(*graph);

 #if 1
     cout << graph->toString();
 #else
  #if 1
     fstream f;
     f.open(fname.c_str(),ios_base::out);
     f << graph.toString();
  #else
     string fileName = "callgraph.dot";
     printf ("Write out file %s \n",fileName.c_str());
     output.writeToDOTFile(fileName); 
  #endif
 #endif
#endif
   }

int 
main (int argc, char* argv[])
   {
     printf ("File moved to development directory for simplified development ... \n");
     ROSE_ASSERT(false);

  // Build the project object (AST) which we will fill up with multiple files and use as a
  // handle for all processing of the AST(s) associated with one or more source files.
     SgProject* project = new SgProject (argc,argv);
     ROSE_ASSERT (project != NULL);

  // printf ("Calling buildCallGraph \n");

     CallGraphCreate* graph = buildCallGraph(project);
     ROSE_ASSERT (graph != NULL);

     GenerateDotGraph(graph);

     return 0;
   }











