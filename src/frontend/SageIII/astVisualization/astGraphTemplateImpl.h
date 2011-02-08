namespace AST_Graph{

template<typename Functional1, typename Functional2>
DataMemberPointersToIR<Functional1,Functional2>::DataMemberPointersToIR(const Functional1& addNodeFunctional, const Functional2& addEdgeFunctional,traversalType tT, pointerHandling graphEmpty) : DOTRepresentation<SgNode*>(),
         nodeFunctional(addNodeFunctional), edgeFunctional(addEdgeFunctional),whichTraversal(tT)        
    {
          graphNull = graphEmpty;
 
          if(tT == memory_pool_traversal)
            traverseMemoryPool();
     };



/*************************************************************************************************************
 * The function
 *      AST_Graph::DataMemberPointersToIR<Functional1, Functional2>::visit( SgNode* node)
 * is an implementation of the virtual visit member function in the AST memory pool traversal.
 * This implementation does not affect the tailored whole AST traversal.
 **************************************************************************************************************/
template<typename Functional1, typename Functional2>
void
DataMemberPointersToIR<Functional1, Functional2>::visit( SgNode* node)
    { 
       std::pair<SgNode*,std::string> nodePair(node,node->class_name());
       FunctionalReturnType nodeReturnValue = nodeFunctional(nodePair);
       if( nodeReturnValue.addToGraph == true )
          generateGraph(node,-10);
    }



//See header file for comment
template<typename Functional1>
void writeGraphOfMemoryPoolToFile(std::string filename, AST_Graph::pointerHandling graphNullPointers, Functional1 addNodeFunctional){
     DataMemberPointersToIR<Functional1,defaultFilterBinary> graph(addNodeFunctional, defaultFilterBinary(), memory_pool_traversal, graphNullPointers);
     graph.writeToFileAsGraph(filename);
}

//See header file for comment
template<typename Functional1, typename Functional2>
void writeGraphOfMemoryPoolToFile(std::string filename, AST_Graph::pointerHandling graphNullPointers, Functional1 addNodeFunctional, Functional2 addEdgeFunctional){
     DataMemberPointersToIR<Functional1, Functional2> graph(addNodeFunctional, addEdgeFunctional, AST_Graph::memory_pool_traversal,graphNullPointers);
     graph.writeToFileAsGraph(filename);
}

//See header file for comment
template<typename Functional1, typename Functional2>
void writeGraphOfAstSubGraphToFile(std::string filename, SgNode* node, AST_Graph::pointerHandling graphNullPointers, Functional1 func1, Functional2 func2, int depth){
     DataMemberPointersToIR<Functional1, Functional2> graph(func1, func2, AST_Graph::whole_graph_AST, graphNullPointers);
     graph.generateGraph(node,depth); 
     graph.writeToFileAsGraph(filename);
}

//See header file for comment
template<typename Functional1>
void writeGraphOfAstSubGraphToFile(std::string filename, SgNode* node, AST_Graph::pointerHandling graphNullPointers, Functional1 func1, int depth){
//     AST_Graph::writeGraphOfAstSubGraphToFile(filename,node,func1,func1,depth,graphNullPointers);
     DataMemberPointersToIR<Functional1,defaultFilterBinary> graph(func1, defaultFilterBinary(), AST_Graph::whole_graph_AST, graphNullPointers);
     graph.generateGraph(node,depth); 
     graph.writeToFileAsGraph(filename);

}


template<typename Functional1, typename Functional2>
void 
DataMemberPointersToIR<Functional1,Functional2>::generateGraph(SgNode* graphNode,int depth){
     ROSE_ASSERT(std::find(NodeExists.begin(),NodeExists.end(),graphNode)==NodeExists.end());
     ROSE_ASSERT(graphNode!=NULL);

     //The functional edgeFunctional implements the criteria for if a node is to be graphed. If this is NULL
     std::pair<SgNode*,std::string> pairGraphNode(graphNode,std::string("WARNING: this is just a name to have a name DO NOT USE"));

     FunctionalReturnType nodeReturnValue = nodeFunctional(pairGraphNode); 

     if(nodeReturnValue.addToGraph==true){

       //std::cout << "The node currently graphed is " << graphNode.second << ": " << graphNode.first << std::endl;
       std::vector<std::pair<SgNode*,std::string> > vectorOfPointers =
            graphNode->returnDataMemberPointers();

       //Add a graph node for the node Node 
       if(nodeReturnValue.DOTLabel == "")
           addNode(graphNode,graphNode->class_name(),nodeReturnValue.DOTOptions);
       else
           addNode(graphNode,nodeReturnValue.DOTLabel,nodeReturnValue.DOTOptions);

       NodeExists.push_back(graphNode);

       //Generate code to graph this data member pointer and generate all edges pointing from this 
       //data member pointer to other data member pointers
       for (NodeTypeVector::size_type i = 0; i < vectorOfPointers.size(); i++)
       {
         FunctionalReturnType edgeReturnValue;
         std::pair<SgNode*,std::string> edgeEndPoint = vectorOfPointers[i];
  
         if( edgeEndPoint.first == NULL ){
            //Hangle NULL pointers 
            edgeReturnValue.addToGraph = false;
            
            if(graphNull==graph_NULL)
                addNullValue(graphNode,"",edgeEndPoint.second,"");
         }else{ 
            edgeReturnValue = edgeFunctional(graphNode,edgeEndPoint);

            if( (whichTraversal==memory_pool_traversal) ){
                //If the node the edge points to should not be graphed and the edge
                //is to be graphed, do not graph the edge if the memory pool traversal
                //is used. 
                if( nodeFunctional(edgeEndPoint).addToGraph == false )
                   edgeReturnValue.addToGraph = false;

            }//END IF MemoryPoolTraversal
  
         }//END IF-ELSE
     
         //If the edge functional says that the edge should be added and the edge end point
         //is a node which should be graphed then graph it if depth is not 0. Depth is a control
         //stucture for the whole graph traversal which gives the user an option to set the number
         //of levels to be followed from a node. 
         if( (depth!=0) & ( edgeReturnValue.addToGraph == true ) & ( nodeFunctional(edgeEndPoint).addToGraph == true ) ){
              //If the functional says that this node is to be added, do so

              //This recursive function call is specific to the whole AST traversal and is how it
              //operates
              if(whichTraversal == whole_graph_AST){
                    //See if the node has already been graphed
                    if(std::find(NodeExists.begin(),NodeExists.end(),vectorOfPointers[i].first) == NodeExists.end())
                         generateGraph(edgeEndPoint.first,depth-1);
              }

              if(edgeReturnValue.DOTLabel == "")
                   addEdge(graphNode,edgeEndPoint.second,edgeEndPoint.first,
                                   edgeReturnValue.DOTOptions);
              else
                   addEdge(graphNode,edgeReturnValue.DOTLabel,edgeEndPoint.first,
                                   edgeReturnValue.DOTOptions);

         }//END IF

       }//END FOR

     }//END IF addToGraph == true

};

};// END NAMESPACE AST_Graph
