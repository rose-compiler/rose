// tps : Switching from rose.h to sage3 changed size from 17,7 MB to 7,4MB
#include "sage3basic.h"

#include <SAGE2OA.h>

#include <AstInterface_ROSE.h>
#include <OAWrap.h>
#include <CFG.h>
#include <PtrMap.h>


#ifndef TEMPLATE_ONLY
OpenAnalysis::ROSE_CFG_Wrap ::ROSE_CFG_Wrap( const AstNodePtr& _head )
{
  SgNode* head = AstNodePtrImpl(_head).get_ptr();
  SgStatement *s = isSgStatement(head);
  if (head->variantT() == V_SgFunctionDefinition) {
      s = isSgFunctionDefinition(s)->get_body();
  }
  SageIRInterface ir;

  SgStatementPtrList stmts;
  stmts.push_back( s );
  SageIRStmtIterator *stmtIter = new SageIRStmtIterator(stmts); 
  oaCfg = new CFG(ir, stmtIter, 0, true);
}
#endif

#ifndef TEMPLATE_ONLY
OpenAnalysis::ROSE_CFG_Wrap :: ~ROSE_CFG_Wrap()
{
  delete oaCfg;
}
#endif

#ifndef _MSC_VER

// DQ (1/9/2010): This should not be required now (see fix below)!
// #ifndef USE_ROSE
// DQ (8/29/2009): EDG reports  this as an error:
// error: declaration is incompatible with function template "void OpenAnalysis::BuildCFG(AstInterface &, const AstNodePtr &, BuildCFGConfig<Node> &)" 
// (declared at line 54 of "/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/CFG/CFG.h")
// void OpenAnalysis::BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node>& ng)

// DQ (1/9/2010): The Intel compiler also catches this problem (class Edge template parameter not used in the template declaration)
// template <class Node, class Edge>
template <class Node>
void OpenAnalysis::BuildCFG ( AstInterface& fa, const AstNodePtr& head, BuildCFGConfig<Node>& ng)
{
  ROSE_CFG_Wrap wrap( head);
  OA2ROSE_CFG_Translate( wrap, ng);
}
// #endif
#endif


#ifndef _MSC_VER

// DQ (1/9/2010): This should not be required now (see fix below)!
// #ifndef USE_ROSE
// DQ (8/29/2009): EDG reports  this as an error:
// error: declaration is incompatible with function template "void OpenAnalysis::OA2ROSE_CFG_Translate(OpenAnalysis::ROSE_CFG_Wrap &, BuildCFGConfig<Node> &)"
// (declared at line 51 of "/home/dquinlan/ROSE/svn-rose/src/midend/programAnalysis/CFG/CFG.h")
// void OpenAnalysis::OA2ROSE_CFG_Translate ( ROSE_CFG_Wrap& wrap, BuildCFGConfig<Node>& ng)

// DQ (1/9/2010): The Intel compiler also catches this problem (class Edge template parameter not used in the template declaration)
// template <class Node, class Edge>
template <class Node>
void OpenAnalysis::OA2ROSE_CFG_Translate ( ROSE_CFG_Wrap& wrap, BuildCFGConfig<Node>& ng)
{
  PtrMapWrap <CFG::Node,Node> nodeMap;
  CFG& g = wrap.get_OA_CFG();
  std::cerr << "start in my code " << std::endl;
  for (CFG::DFSIterator nodeIter(g); nodeIter; ++nodeIter) {
        CFG::Node* n = nodeIter;
        Node *n1 = ng.CreateNode();
        nodeMap.InsertMapping(n, n1);
        for (CFG::NodeStatementsIterator  stmts(n); stmts; ++stmts) {
            StmtHandle cur = stmts;
            AstNodePtr p = SageIRInterface::StmtHandle2SgNode(cur);
            ng.AddNodeStmt(n1, p);
        }
  }
  for (CFG::DFSIterator nodeIter1(g); nodeIter1; ++nodeIter1) {
        CFG::Node* n = nodeIter1;
        Node *nn = nodeMap.Map(n); 
        for (DGraph::OutgoingEdgesIterator edgeIter(n); edgeIter; ++edgeIter) {
            DGraph::Edge *etmp = edgeIter;
            CFG::Edge *e = static_cast<CFG::Edge*>(etmp);
            CFG::Node *n1 = static_cast<CFG::Node*>(e->sink());
            CFGConfig::EdgeType t = CFGConfig::ALWAYS;
            switch( e->getType()) {
            case CFG::TRUE_EDGE: t = CFGConfig::COND_TRUE; break;
            case CFG::FALSE_EDGE: t = CFGConfig::COND_FALSE; break;
            default: ROSE_ASSERT (!"Bad CFG edge case");
            } 
            Node *nn1 = nodeMap.Map(n1);
            ng.CreateEdge(nn, nn1,t); 
        }
   }
}
// #endif

#endif











