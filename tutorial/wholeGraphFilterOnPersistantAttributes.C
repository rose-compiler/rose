// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// rose.C: Example (default) ROSE Preprocessor: used for testing ROSE infrastructure

#include "rose.h"

using namespace std;

// This code uses mechanism implemented in these files???
// #include "AstAttributeDOT.h"
// #include "AstAttributeDOT.C"

template< typename Traversal >
class visitorTraversalSetAttribute:public Traversal
   {
     public:
          virtual void visit (SgNode * n);
   };

template< typename Traversal>
void
visitorTraversalSetAttribute<Traversal>::visit (SgNode * n)
   {
  // This implementation of AST visualization adds attributes to IR nodes intended to be represented, 
  // the attributes contain information about the DOT options available for the IR node or graph 
  // edge to be represented.

     if (isSgStatement(n) != NULL)
        {
          printf ("Found a for loop (read the attribute) ... \n");

       // Demonstrate how to attach a DOTGraphEdge persistant attribute
          std::vector< std::pair<SgNode *, std::string > > 	vectorOfPointers = n->returnDataMemberPointers ();
      
          std::vector< std::pair<SgNode *, std::string > >::iterator it_vec;
          for(it_vec = vectorOfPointers.begin(); it_vec != vectorOfPointers.end(); it_vec++)
             {
               DOTGraphEdge* y = new DOTGraphEdge(n,it_vec->first);

               y->set_style(AstAttributeDOT::filled);
               y->set_arrow_type(DOTGraphEdge::dot);
               y->set_color("blue");
               y->set_fontname("7x13bold");
               y->set_fontcolor("black");
               y->set_keep(true);

               n->setAttribute("EdgeAttribute"+it_vec->second, y);
             }

       // Demonstrate how to attach a DOTGraphNode persistant attribute
          DOTGraphNode* z = new DOTGraphNode(n);

          z->set_shape(DOTGraphNode::polygon);
          z->set_style(AstAttributeDOT::filled);
          z->set_color("blue");
          z->set_fillcolor("red");
          z->set_extra_options("regular=0");
          z->set_extra_options("sides=6");
          z->set_extra_options("peripheries=1");
          z->set_fontname("7x13bold");
          z->set_fontcolor("black");

          z->set_keep(true);
#if 0
          std::cout << "The edge string is: " << y.get_dot_options_string() << std::endl;
          std::cout << "The node string is: " << z->get_dot_options_string() << std::endl;
          std::cout << "Attaching new attribute" << std::endl; 
#endif
          n->setAttribute(std::string("MyNodeAttribute"),z);
        }
   }

/*******************************************************************************************************
 * The unary functional
 *     struct filterOnNodes
 * returns an AST_Graph::FunctionalReturnType and takes a std::pair<SgNode*,std::string> as a paramater.
 * The pair represents a variable of type std::string and a variablename. 
 * The type AST_Graph::FunctionalReturnType contains the variables
 *      * addToGraph : if false do not graph node or edge, else graph
 *      * DOTOptions : a std::string which sets the color of the node etc.
 *
 * PS!!! The std::string is currently not set to anything useful. DO NOT USE THE STRING.
 *       Maybe it does not make sence in this case and should be removed.
 ******************************************************************************************************/
struct filterOnNodes: public std::unary_function< pair< SgNode*, std::string>,AST_Graph::FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( argument_type x ) const;
   };


// The argument to the function is 
filterOnNodes::result_type
filterOnNodes::operator()(filterOnNodes::argument_type x) const
   {
     AST_Graph::FunctionalReturnType returnValue;
     //Determine if the node is to be added to the graph. true=yes
     returnValue.addToGraph = true;

     //Demonstrate how to read a DOTGraphNode persistant attribute
     if (isSgLocatedNode(x.first)!=NULL)
        {
          if (isSgLocatedNode(x.first)->attributeExists("MyNodeAttribute"))
             {
               AstAttribute *existingAttribute = x.first->getAttribute ("MyNodeAttribute");

               if (dynamic_cast<DOTGraphNode*>(existingAttribute)!=NULL)
                  {
                    DOTGraphNode* graphNode = dynamic_cast<DOTGraphNode*>(existingAttribute);
    
                 // set colors etc. for the graph Node
                    returnValue.DOTOptions = graphNode->get_dot_options_string();
                    returnValue.addToGraph = graphNode->get_keep();
                    returnValue.DOTLabel   = graphNode->get_label() + " plus your label";
                  }
             }
        }

     return returnValue;
   }




/*******************************************************************************************************************
 * The binary functional
 *       struct filterOnEdges
 * returns an AST_Graph::FunctionalReturnType and takes a node and a std::pair<SgNode*,std::string> as a paramater.
 * The paramater represents an edge 'SgNode*'->'std::pair<>' where the pair is a variable declaration with name 
 * 'std::string' within the node in the first paramater. 
 *
 * The type AST_Graph::FunctionalReturnType contains the variables
 *      * addToGraph : if false do not graph node or edge, else graph
 *      * DOTOptions : a std::string which sets the color of the node etc.
 *******************************************************************************************************************/
struct filterOnEdges: public std::binary_function< SgNode*,pair< SgNode*, std::string>,AST_Graph::FunctionalReturnType >
   {
  // This functor filters SgFileInfo objects and IR nodes from the GNU compatability file
     result_type operator() ( SgNode*,AST_Graph::NodeType & x ) const;
   };


//The edge goes from 'node'->'x'
filterOnEdges::result_type
filterOnEdges::operator()(SgNode* node, pair<SgNode*,std::string>& x) const
   {
     AST_Graph::FunctionalReturnType returnValue;
  // Determine if the node is to be added to the graph. true=yes
     returnValue.addToGraph = true;

  // Demonstrate how to read a DOTGraphEdge persistant attribute
     if ( isSgLocatedNode(node) != NULL )
        {
          if (isSgLocatedNode(node)->attributeExists(std::string("EdgeAttribute"+x.second)) == true)
             {
               AstAttribute *existingAttribute = isSgLocatedNode(node)->getAttribute (std::string("EdgeAttribute"+x.second));
               if (dynamic_cast<DOTGraphEdge*>(existingAttribute)!=NULL)
                  {
                    DOTGraphEdge* graphNode = dynamic_cast<DOTGraphEdge*>(existingAttribute);
     
                 // set colors etc. for the graph Node
                    returnValue.DOTOptions = graphNode->get_dot_options_string();
                    returnValue.addToGraph = graphNode->get_keep();
	                 returnValue.DOTLabel   = x.second + " " + graphNode->get_label() + " add your edge label";
                  }
             }
        }

     return returnValue;
   }





int
main (int argc, char *argv[])
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build the AST
     SgProject *project = frontend (argc, argv);
     ROSE_ASSERT (project != NULL);

  // Build the traversal object to set persistant AST attributes
     visitorTraversalSetAttribute<AstSimpleProcessing> exampleTraversalSettingAttribute;

  // Call the traversal starting at the project node of the AST
     exampleTraversalSettingAttribute.traverseInputFiles (project, preorder);

  // specify that NULL pointers should be represented in the DOT graph
     AST_Graph::pointerHandling pHandling = AST_Graph::graph_NULL;

  // Traverse the AST using the whole graph subtree traversal and filter on the persistant attributes on nodes and edges 
     AST_Graph::writeGraphOfAstSubGraphToFile("wholeGraphPersistantAttributeAST.dot",project,pHandling,filterOnNodes(),filterOnEdges());

  // Traverse the AST using the memory pool traversal and filter on the persistant attributes on nodes and edges 
     AST_Graph::writeGraphOfMemoryPoolToFile("wholeGraphPersistantAttributeASTPool.dot",pHandling,filterOnNodes(),filterOnEdges());

     return 0;
   }
