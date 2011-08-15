#include "IntraProcDataFlowAnalysis.h"
          
template<class Node, class Data>
IntraProcDataFlowAnalysis<Node, Data>::IntraProcDataFlowAnalysis(SgNode *_head)
{
    head = _head;
}
template<class Node, class Data>
void IntraProcDataFlowAnalysis<Node, Data>::run() {

  bool change = true;
  int iteration =0;
  while (change) {
    change = false; 
    iteration++;
    std::vector<Node *> nodes = getAllNodes();
    for (unsigned int index=0; index < nodes.size(); index++) {
      Node* cur = nodes[index];
      Data inOrig = getCFGInData(cur);
      Data in = inOrig;

      // Get the list of Predecessors of s
      std::vector<Node *> preds = getPredecessors(cur);
      //In[s] = Union of Out[p], where p is a predecessor of s.
      for(unsigned int p_index=0;p_index< preds.size(); p_index++) {
        Node* pred = preds[p_index];
        Data predout = getCFGOutData(pred);
        in = meet_data(in, predout);
      } 
      //Out[s]=Gen[s] Union (In[s]-Kill[s]), s is current node
      if (in != inOrig) //{
        setCFGInData(cur,in);
          
      Data outOrig = getCFGOutData(cur);
      applyCFGTransferFunction(cur); 

      if (outOrig != getCFGOutData(cur)) {
          change = true; 
      } 

      /*
           std::stringstream ss;
           ss << isSgFunctionDeclaration(head)->get_mangled_name().getString()<< "_" << iteration<< "_"<< index << ".dot";          
           std::string name;
            ss >> name;
           getCFGOutData(cur).get()->toDot(name); 
      */
    }
    
  }
  std::cout <<"Total IntraProcedural Iteration :" << iteration << std::endl;
}
