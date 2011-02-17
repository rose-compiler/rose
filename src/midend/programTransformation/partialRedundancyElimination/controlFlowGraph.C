// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <sstream>
#include <vector>
#include "sageBuilder.h"
#include "CFG.h"
#include <AstInterface_ROSE.h>
#include "controlFlowGraph.h"

using namespace std;

class CfgConfig: public BuildCFGConfig<int> {
  PRE::ControlFlowGraph& graph;

  public:
  CfgConfig(PRE::ControlFlowGraph& graph):
    graph(graph) {}

  virtual PRE::Vertex* CreateNode() {
    // cerr << "Creating node" << endl;
    PRE::Vertex vertex = graph.graph.add_vertex();
    graph.node_statements.push_back(vector<SgNode*>());
    return new PRE::Vertex(vertex);
  }

  virtual void CreateEdge(PRE::Vertex* src, PRE::Vertex* tgt,
                                CFGConfig::EdgeType condval) {
    // cerr << "Creating edge between " << *src << " and " << *tgt << endl;
    /* PRE::Edge edge = */ graph.graph.add_edge(*src, *tgt);
    graph.edge_type.push_back(condval);
    graph.edge_insertion_point.push_back(make_pair((SgNode*)0, false));
  }

  virtual void AddNodeStmt(PRE::Vertex* n, const AstNodePtr& s) {
    ROSE_ASSERT(s != NULL);
 // printf ("Adding node = %p = %s \n",s,s->class_name().c_str());
 // cerr << "Adding statement " << s->unparseToString() << " to node " << *n << endl;
    graph.node_statements[*n].push_back(AstNodePtrImpl(s).get_ptr());
  }
};

void
PRE::makeCfg ( SgFunctionDefinition* n, PRE::ControlFlowGraph& graph )
   {
     CfgConfig cfgconfig(graph);
     AstInterfaceImpl astiImpl = AstInterfaceImpl(n);
     AstInterface asti(&astiImpl);
     ROSE_Analysis::BuildCFG(asti, AstNodePtrImpl(n), cfgconfig);
   }

void
PRE::addEdgeInsertionPoints(PRE::ControlFlowGraph& controlflow)
   {
  // printf ("In addEdgeInsertionPoints: controlflow.graph.vertices().size()     = %zu \n",controlflow.graph.vertices().size());
  // printf ("In addEdgeInsertionPoints: controlflow.edge_insertion_point.size() = %zu \n",controlflow.edge_insertion_point.size());

     VertexIter i = controlflow.graph.vertices().begin(), iend = controlflow.graph.vertices().end();
     for (; i != iend; ++i)
        {
       // printf ("vertexCount = %d \n",vertexCount++);

          OutEdgeIter out    = controlflow.graph.out_edges(*i).begin(),
                      outend = controlflow.graph.out_edges(*i).end();

       // printf ("Problem location: controlflow.node_statements[*i] = %p \n",&(controlflow.node_statements[*i]));

          const vector<SgNode*>& stmts = controlflow.node_statements[*i];
          SgNode* last_stmt = stmts.empty() ? 0 : stmts[stmts.size() - 1];
          int out_edge_count = distance(out, outend);
          SgStatement* expr_parent = 0;

       // printf ("stmts.size() = %zu last_stmt = %p expr_parent = %p \n",stmts.size(),last_stmt,expr_parent);

       // Wouldn't this make more sense as a while loop?
          for (vector<SgNode*>::const_iterator j = stmts.begin(); j != stmts.end(); ++j)
             {
               ROSE_ASSERT((*j) != NULL);
#if 0
               if ((*j)->get_parent() == NULL)
                  {
                    (*j)->get_file_info()->display("Location of error: debug");
                    printf ("Error: statement = %s \n",(*j)->unparseToString().c_str());
                  }
                 else
                  {
                    ROSE_ASSERT((*j)->get_parent() != NULL);
                 // printf ("Statement at vertex (*i): (*j)->class_name() = %s (*j)->get_parent()->class_name()\n",(*j)->class_name().c_str(),(*j)->get_parent()->class_name().c_str());
                  }
#endif
               if (expr_parent) continue;
               if (isSgExpression(*j))
                  {
                    expr_parent = isSgStatement((*j)->get_parent());
                    ROSE_ASSERT (expr_parent);
                  }
                 else
                  {
                    if (isSgExprStatement(*j))
                       {
                         expr_parent = isSgStatement((*j)->get_parent());
                         if (isSgBasicBlock(expr_parent) || isSgForInitStatement(expr_parent))
                              expr_parent = 0;
                       }
                  }
             }

       // printf ("out_edge_count = %d \n",out_edge_count);
       // printf ("expr_parent = %p = %s \n",expr_parent,(expr_parent != NULL) ? expr_parent->class_name().c_str() : "NULL");

          if (!expr_parent)
             {
            // printf ("Fixup the expr_parent to be the last_stmt = %p \n",last_stmt);
               expr_parent = isSgStatement(last_stmt);
             }
#if 0
       // DQ (3/17/2006): It is a problem if at this point the expr_parent is NULL
          if (expr_parent == NULL)
             {
               printf ("Warning expr_parent == NULL \n");
             }
#endif
          if (out_edge_count == 1 && !isSgForStatement(expr_parent))
             {
#if 0
            // printf ("Edge count == 1: and not a SgForStatement, adding and edge using %p \n",expr_parent);
               if (expr_parent == NULL)
                  {
                    printf ("Warning adding an edge using a NULL pointer! at %p \n",&(controlflow.edge_insertion_point[*out]));
                  }
#endif
               controlflow.edge_insertion_point[*out] = make_pair(expr_parent, 
                    (isSgGotoStatement(expr_parent) ? 
                 /* Put insertion before goto statements but after other statement types */ true :
                    false));
             }
            else
             {
               if (out_edge_count == 1 && isSgForStatement(expr_parent))
                  {
                 // Increment of for statement
                    controlflow.edge_insertion_point[*out] = make_pair(isSgForStatement(expr_parent)->get_increment(),false); // Put in after expression using operator,
                  }
                 else
                  {
                    if (out_edge_count >= 2)
                       {
                         for (; out != outend; ++out)
                            {
                              pair<SgStatement*, bool> insert_point;
                              CFGConfig::EdgeType kind = controlflow.edge_type[*out];
                           // printf ("CFGConfig::EdgeType kind = %d \n",(int)kind);
                              try_next_ancestor:
                              ROSE_ASSERT(expr_parent != NULL);
                              switch (expr_parent->variantT())
                                 {
                                   case V_SgDoWhileStmt:
                                        if (kind == CFGConfig::COND_FALSE)
                                           {
                                             insert_point = make_pair(expr_parent, false);
                                           }
                                          else
                                           {
                                          // DQ (3/13/2006): Make this an error (partly as just a test)
                                          // FIXME -- We can't really use
                                          // do-while in tests anymore, since
                                          // there isn't a good way to put
                                          // something on the true branch of
                                          // the test
                                             printf ("kind != CFGConfig::COND_FALSE in SgDoWhileStmt \n");
                                             ROSE_ASSERT(false);
                                           }
                                        break;

                                   case V_SgIfStmt:
                                        if (kind == CFGConfig::COND_TRUE)
                                           {
                                             insert_point = make_pair(isSgIfStmt(expr_parent)->get_true_body(), true);
                                           }
                                          else
                                           {
                                             if (isSgIfStmt(expr_parent)->get_false_body() == NULL) {
                                               SgBasicBlock* bb = SageBuilder::buildBasicBlock();
                                               isSgIfStmt(expr_parent)->set_false_body(bb);
                                               bb->set_parent(expr_parent);
                                             }
                                             ROSE_ASSERT (isSgIfStmt(expr_parent)->get_false_body());
                                             insert_point = make_pair(isSgIfStmt(expr_parent)->get_false_body(), true);
                                           }
                                         break;

                                   case V_SgWhileStmt:
                                        if (kind == CFGConfig::COND_TRUE)
                                           {
                                             insert_point = make_pair(isSgWhileStmt(expr_parent)->get_body(), true);
                                           } 
                                          else
                                           {
                                             insert_point = make_pair(expr_parent, false);
                                           }
                                        break;

                                   case V_SgSwitchStatement:
                                        assert (!"switch FIXME");
                                        break;

                                   case V_SgForStatement:
                                     // printf ("Found a SgForStatement, but this should not be used now the the conditional test is a SgExprStatement \n");
                                     // ROSE_ASSERT(false);
                                        if (kind == CFGConfig::COND_TRUE)
                                           {
                                             insert_point = make_pair(isSgForStatement(expr_parent)->get_loop_body(), true);
                                           }
                                          else
                                           {
                                          // DQ (3/17/2006): this effects the placement of the cachevar__10 = (_var_0 + 15); in pass3.C
                                             ROSE_ASSERT(expr_parent != NULL);
                                             insert_point = make_pair(expr_parent, false);
                                          // insert_point = make_pair(expr_parent, true);
                                           }
                                        break;
#if 0
                                // DQ (3/13/2006): Added case for SgExpressionStatment, since it is now the conditonal 
                                // test in a SgForStatement (due to a recent change in the IR to support such things 
                                // which are leagal code in C and C++).
                                   case V_SgExprStatement:
                                        printf ("Found a SgExprStatement, likely the test in a SgForStatment or such \n");
                                        ROSE_ASSERT (false);
                                        ROSE_ASSERT(isSgForStatement(expr_parent->get_parent()) != NULL);
                                        ROSE_ASSERT(isSgForStatement(expr_parent->get_parent())->get_test() == expr_parent);
                                        if (kind == CFGConfig::COND_TRUE)
                                           {
                                             printf ("Building an edge to the for loop body \n");
                                             insert_point = make_pair(isSgForStatement(expr_parent->get_parent())->get_loop_body(), true);
                                           }
                                          else
                                           {
                                             insert_point = make_pair(expr_parent, false);
                                           }
                                        break;
#endif
                                   case V_SgExprStatement:
                                   case V_SgGotoStatement:
                                   case V_SgLabelStatement: {
                                     expr_parent = isSgStatement(expr_parent->get_parent());
                                     goto try_next_ancestor;
                                   }
                                   default:
                                        cerr << "Unknown variant " << expr_parent->sage_class_name() << endl;
                                        expr_parent->get_file_info()->display("Location in input code");
                                        ROSE_ASSERT (false);
                                        break;
                                 }

                              if (insert_point.first)
                                   controlflow.edge_insertion_point[*out] = insert_point;
                            }
                       }
                      else
                       {
                      // out_edge_count == 0
                       }
                  }
             }
        }

     EdgeIter ei   = controlflow.graph.edges().begin(), 
              eend = controlflow.graph.edges().end();
     for (; ei != eend; ++ei)
        {
          if (!controlflow.edge_insertion_point[*ei].first)
             {
               Vertex src = controlflow.graph.source(*ei);
               if (controlflow.graph.out_edges(src).size() == 1 && !controlflow.node_statements[src].empty())
                  {
                    vector<SgNode*> stmts = controlflow.node_statements[src];
                    controlflow.edge_insertion_point[*ei] = 
                    make_pair(stmts[stmts.size() - 1], false);
                  }
             }
        }

  // printf ("Leaving addEdgeInsertionPoints: controlflow.graph.vertices().size()     = %zu \n",controlflow.graph.vertices().size());
  // printf ("Leaving addEdgeInsertionPoints: controlflow.edge_insertion_point.size() = %zu \n",controlflow.edge_insertion_point.size());
   }


void PRE::printCfgAsDot(ostream& dotfile,
                   const PRE::ControlFlowGraph& controlflow) {
  dotfile << "digraph cfg {" << endl;
  VertexIter i = controlflow.graph.vertices().begin(), 
             end = controlflow.graph.vertices().end();
  for (; i != end; ++i) {
    std::ostringstream statementdata;
    statementdata << "{";
    statementdata << *i;
    const vector<SgNode*>& stmts = controlflow.node_statements[*i];
    for (vector<SgNode*>::const_iterator j = stmts.begin();  
         j != stmts.end(); ++j) {
      statementdata << "\\n";
      if (isSgExpression(*j)) {
        statementdata << " | ";
      }
      statementdata << (*j)->unparseToString();
      SgStatement* expr_parent = 0;
      if (isSgExpression(*j)) {
        expr_parent = isSgStatement((*j)->get_parent()->get_parent());
      } else if (isSgExprStatement(*j)) {
        expr_parent = isSgStatement((*j)->get_parent());
        if (isSgBasicBlock(expr_parent))
          expr_parent = 0;
      }
      if (expr_parent) {
        statementdata << string(" in ") + 
                         expr_parent->sage_class_name();
      }
    }
    statementdata << "}";
    string statementdatastr = statementdata.str();
    // Escape < and > and " in statementdata
    string statementdata_new;
    for (unsigned int j = 0; j < statementdatastr.size(); ++j) {
      if (statementdatastr[j] == '<' || statementdatastr[j] == '>' || 
          statementdatastr[j] == '"')
        statementdata_new += string("\\") + statementdatastr[j];
      else
        statementdata_new += statementdatastr[j];
    }
    dotfile << *i << " [label = \"" << statementdata_new << "\", shape=record];" << endl;
  }

  EdgeIter j = controlflow.graph.edges().begin(), 
           jend = controlflow.graph.edges().end();
  for (; j != jend; ++j) {
    CFGConfig::EdgeType kind = controlflow.edge_type[*j];
    string color = (kind == CFGConfig::COND_TRUE ? "green" : 
                    kind == CFGConfig::COND_FALSE ? "red" : 
                    kind == CFGConfig::ALWAYS ? "black" : 
                    "blue");
    pair<SgNode*, bool> insert_point = controlflow.edge_insertion_point[*j];
    dotfile << controlflow.graph.source(*j) << " -> " << controlflow.graph.target(*j) << " [color=" << color << ", label = \"" << (insert_point.first ? (string(insert_point.first->sage_class_name()) + " " + (insert_point.second ? "before" : "after")) : "") << "\"];" << endl;
  }
  dotfile << "}" << endl;
}
