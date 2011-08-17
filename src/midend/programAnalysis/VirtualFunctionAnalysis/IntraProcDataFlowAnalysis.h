/* 
 * File:   IntraProcDataFlowAnalyis.h
 * Author: rahman2
 *
 * Created on August 15, 2011, 10:28 AM
 */
#ifndef INTRAPROCDATAFLOWANALYIS_H
#define INTRAPROCDATAFLOWANALYIS_H

#include "sage3basic.h"


template<class Node,class Data>
class IntraProcDataFlowAnalysis  
{
  
  virtual Data meet_data( const Data& d1, const Data& d2) = 0;
  
  virtual Data get_empty_data() const = 0;
  
  virtual Data getCFGInData(Node *) = 0;
  
  virtual Data getCFGOutData(Node *) = 0;
  
  virtual void setCFGInData(Node*, Data &) = 0;

  virtual void applyCFGTransferFunction(Node* ) = 0;

  virtual void buildCFG() = 0;
 public:
  
  // Creating an empty DAG
  IntraProcDataFlowAnalysis(SgNode *head);

  virtual void run();
  // Get all the nodes
  virtual std::vector<Node *> getAllNodes() = 0;
  
  // Get all the Predecessors of a current Node
  virtual std::vector<Node *> getPredecessors(Node *n) = 0;
  
protected:
  SgNode *head;
  
};

#endif  /* INTRAPROCDATAFLOWANALYIS_H */

