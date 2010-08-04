void getLiveVariables(LivenessAnalysis * liv, SgForStatement* forstmt) 
{
  ROSE_ASSERT(liv != NULL);
  ROSE_ASSERT(forstmt!= NULL);
  std::vector<SgInitializedName*> liveIns, liveOuts; 

  // For SgForStatement, virtual CFG node which is interesting has an index number of 2, 
  // as shown in its dot graph's node caption.
  // "<SgForStatement> @ 8: 2" means this node is for a for statement at source line 8, with an index 2.
  CFGNode cfgnode(forstmt,2);
  FilteredCFGNode<IsDFAFilter> filternode= FilteredCFGNode<IsDFAFilter> (cfgnode);

  // Check edges
  vector<FilteredCFGEdge < IsDFAFilter > > out_edges = filternode.outEdges();
  ROSE_ASSERT(out_edges.size()==2);
  vector<FilteredCFGEdge < IsDFAFilter > >::iterator iter= out_edges.begin();

  for (; iter!=out_edges.end();iter++)
  {
    FilteredCFGEdge < IsDFAFilter > edge= *iter;
    // one true edge going into the loop body
    //x. Live-in (loop) = live-in (first-stmt-in-loop)
    if (edge.condition()==eckTrue)
    {
      SgNode* firstnode= edge.target().getNode();
      liveIns = liv->getIn(firstnode);
    }
    // one false edge going out of loop
    //x. live-out(loop) = live-in (first-stmt-after-loop)
    else if (edge.condition()==eckFalse)
    {
      SgNode* firstnode= edge.target().getNode();
      liveOuts0 = liv->getIn(firstnode);
    }
    else
    {
      cerr<<"Unexpected CFG out edge type for SgForStmt!"<<endl;
      ROSE_ASSERT(false);
    }
  } // end for (edges)
}
