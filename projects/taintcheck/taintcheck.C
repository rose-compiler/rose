/***********************************************************************
This code written by: Jim Leek leek2@llnl.gov
Date: March 16, 2008


Goal:

Taint and Information Flow checking
* input: Source code annotated with infoflow pragmas
* output: Error messages and Dot files detailing any informatiow security
          violations.

Major classes:
* InfoFlowAttribute: persistent attribute for information flow pragma node

* enum infoflow_enum: An enumeration of the different allowed infoflow 
                      pragma annotations.

-------------------------------------

The basic idea here is that we can check to make sure confidential data
does not leak out on a public output, and that tainted input does not
affect sensitive data.  These two operations are sides of the same coin,
so it is as easy to accomplish both as one.

The programmer is extexted to annotate their program with pragmas that 
declare if a given statment is interesting for information flow. We
have the programmer do this because there is no general way for us to 
decide which inputs and outputs are trusted or untrusted.  For example,
on a public computers system, console input my be untrusted, but on 
networked server, console input may be trusted because only approved 
people may be allowed physical access.

For checking that tainted input does not affect sensitive data, we 
have 3 pragmas:

#pragma leek tainted
#pragma leek trusted
#pragma leek sanatized

The tainted pragma is placed on dangrous input lines.
The trusted pramga is placed on sensitive data systems or outputs.

taintcheck takes a slice (based on Horwitz, et al.) from the sanatized node 
to see if there is any any tainted nodes are reachable.  If a tainted node
is reachable, that means there is a possible flow that allows tainted data
to affect sensitive data.  If there is no path from tainted data to sensitive
data, the tainted pragma will (proveably) not appear in the slice.

There are some cases where tainted data must affect trusted output.  For 
example, a web application that takes input from a user, and then bases
a query to a trusted database on that input.  In this case, the programmer
must have some system for "sanatizing" the input to make sure it is not
dangeous.  (For example, the user might input some string that forces 
the database to output all its sensitive data.)

In this case, the programmer may mark the santization routine with the
sanatized pragma.  This pragma caims that any data flow out of that node 
or function is safe, and can safely interact with trusted data.  This allows us
to verify programs like this:

//GOOD PROGRAM
#pragma leek sanatized
string sanatize(string in) {  clean;  }

#pragma leek tainted
user_input = getUserInput;
clean = sanatize(user_input)
#pragma leek trusted
output = queryDatabase(clean);

-----
And flag programs like this:
-----

// BAD PROGRAM!
#pragma leek sanatized
string sanatize(string in) {  clean;  }

#pragma leek tainted
user_input = getUserInput;
if(unrelated check)             //Stupid if
  clean = sanatize(user_input)
else
  clean = user_input            //tainted flow!
#pragma leek trusted
output = queryDatabase(clean);



Classified leak detection is similar, but reversed:

#pragma leek classified
#pragma leek public
#pragma leek declassified

For example, a printer driver may get a user's document to print 
(classified info), but then find it is out of ink.  It may then
want to send an order for new ink over the internet (public output).
We may want to verifiy that the user's document will never end up
on the internet.  

Of course, in this case, we don't need the declassified node, as
that sanatizies confidential input such that it can be made public.

-------------------------------------
Limitations:
* Our slice routine doesn't handle pointers well.
* Our slice routine is also not as preciece as possible, as we have
  not implements Horwitz's linkage grammar.

-------------------------------------
Acknowledgement:  
* Rose's System Dependence Graph generation and slicing is based on:

  Horwitz, S., Reps, T., and Binkley, D., [Interprocedural slicing using dependence graphs], 
    ACM Trans. Program. Lang. Syst. 12(1) pp. 26-60 (January 1990).

* Using System Dependence Graphs for information flow was mostly based
  on the ideas in:

  Hammer, C., Krinke, J., and Snelting, G. 2006. [Information flow control for java based on
    path conditions in dependence graphs.] In Proceedings of ISSSE\'06. IEEE Computer Society
    Press, 87-96.


*************************************************************************/

// ROSE related Headers:
#include "rose.h"
//   for using the system dependence graph:
#include <AstInterface.h>
#include <StmtInfoCollect.h>
#include <ReachingDefinition.h>
#include <DirectedGraph.h>
#include "DependenceGraph.h"
#include "SlicingInfo.h"
#include "CreateSlice.h"
//#include "ControlFlowGraph.h"
#include "DominatorTree.h"
#include "CreateSliceSet.h" 

#include <boost/filesystem/convenience.hpp>
#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;
using namespace SageInterface; 
using namespace SageBuilder;

enum infoflow_enum {
  i_none = 0,
 
  i_tainted = 1,
  i_sanatized = 2,
  i_trusted = 3,

  i_classified = 4,
  i_declassified = 5,
  i_public = 6
};

string infoflow_names[7] = {"i_none", "i_tainted", "i_sanatized", "i_trusted",
                            "i_classified", "i_declassified", "i_public"};

string infoflow_verbs[7] = {"noned", "tainted", "sanatized", "trusted",
                            "classified", "declassified", "public"};


//------------------------------------------------------------------
//------------------------------------------------------------------
// the persistent attribute attached to an OpenMP pragma node in SAGE III AST
// different types of pragmas need different information in some cases
// e.g.
//    'leek i_tainted' 
//------------------------------------------------------------------
//------------------------------------------------------------------
class InfoFlowAttribute : public AstAttribute 
{
private:
  void init() 
    {   
        parent = NULL;
        parentPragma = NULL;
    }

 public:
  InfoFlowAttribute * parent; //upper-level OMP pragma's attribute
  SgPragmaDeclaration *pragma;
  SgPragmaDeclaration *parentPragma;
  enum infoflow_enum pragmaType; 

  //constructor
   InfoFlowAttribute()
    {
	init();
        pragmaType = i_none; 
    }
   InfoFlowAttribute(enum infoflow_enum ifType):pragmaType(ifType){
        init();
   }

  //   enum infoflow_enum getType(SgInitializedName *);
  //bool isInClause(SgInitializedName*, enum infoflow_enum);
   void print() {
     cout<<"----------------------"<<endl;
     cout<<"InfoFlow Attribute: pragma type: "<<pragmaType<<endl;
     cout<<"pragma:"<<pragma->get_pragma()->get_pragma()<<endl;
     cout<<"Parent pragma is:";
     if(parentPragma != NULL) cout<<parentPragma->get_pragma()->get_pragma()<<endl;
     else cout<<"none"<<endl;
   } 
};



infoflow_enum recognizePragma(SgPragmaDeclaration* pragmaDecl)
{
  //    formalizePragma(pragmaDecl);
  string pragmaString = pragmaDecl->get_pragma()->get_pragma();
  
  if (pragmaString.find("leek tainted")!=string::npos)
    return i_tainted;
  if (pragmaString.find("leek classified")!=string::npos)
    return i_classified;
  if (pragmaString.find("leek trusted")!=string::npos)
    return i_trusted;
  if (pragmaString.find("leek sanatized")!=string::npos) 
    return i_sanatized;
  if (pragmaString.find("leek declassified")!=string::npos) 
    return i_sanatized;
  if (pragmaString.find("leek public")!=string::npos) 
    return i_public;

  return i_none;
}


// For now I'll assume that only assignments can be tainted or classified.
// So, if you use a scanf to get input that is tainted, you'll have to do something
// silly like this:
//
// scanf("%s", &input);
// #pragma leek tainted
// data = input;
// to identify the data as tainted.  

// Note: trusted and tainted can apply to outputs as well.  So functions can also
// be annotated?

// Of course, sanatization is also done on functions....

void annotateAST(SgProject *project)
{ 
  Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(project, V_SgPragmaDeclaration);
  for (Rose_STL_Container<SgNode*>::iterator listElement=pragmaList.begin();
       listElement!=pragmaList.end(); ++listElement)
  {
    SgPragmaDeclaration * decl = isSgPragmaDeclaration(*listElement);
    ROSE_ASSERT(decl != NULL);

    enum infoflow_enum pragmaType = recognizePragma(decl);

    // If we know this pragma, annotate the next statment with it 
    //Ok, we also need to use the "Is Interesting filter" to make sure it lands on something interesting.
    if(pragmaType) {
      SgNode *node = getNextStatement(decl); 
      ROSE_ASSERT(node != NULL);
      
      int numChildren = node->get_numberOfTraversalSuccessors();
      int thisChild = 0;

      while(!IsImportantForSliceSgFilter(node)) {
        assert(thisChild < numChildren);  //We're lamely assuming we won't have to go deeper than 1 level
        node = node->get_traversalSuccessorByIndex(thisChild);
        ++thisChild;
      }
      
      InfoFlowAttribute* ifa = new InfoFlowAttribute(pragmaType);
      
      node->addNewAttribute(infoflow_names[pragmaType], ifa);
    }
  
  }
}


void writeSlicedDot(DependenceGraph *graph, set < DependenceNode * > _nodes, char* filename) {
  
  ofstream f(filename);
  std::vector<std::string> styles;
  
  f << "digraph \"G" << filename << "\" {" << std::endl;
  // output all of the nodes
  std::set < DependenceNode * >::iterator i;
  for (i = _nodes.begin(); i != _nodes.end(); i++)
    {
      styles.clear();
      DependenceNode *node = dynamic_cast < DependenceNode * >(*i);
      
      DependenceNode *d = *i;
      char buf[sizeof(DependenceNode *) * 2 + 3];
      
      sprintf(buf, "%p", d);
      f << "\"" << buf << "\" [";
      // label
      f<<"label = \"";
      d->writeOut(f); //_displayData(d, f);
      f << "\"";
      if (node->isHighlighted())
			{
        f<<",color=red";
        styles.push_back(string("bold"));
      }
			
      //append a shape
      switch(node->getType())
			{
      case DependenceNode::ENTRY:
        f<<",shape=hexagon";
        f<<",fillcolor=deepskyblue";
        styles.push_back(string("filled"));
        break;
      case DependenceNode::ACTUALIN:
      case DependenceNode::ACTUALOUT:
      case DependenceNode::FORMALIN:
      case DependenceNode::FORMALOUT:
      case DependenceNode::FORMALRETURN:
      case DependenceNode::ACTUALRETURN:
        f<<",shape=box";
        break;					
      default:
        //	f<<"shape=box";
        break;
				
      }
      f<<",style=\"";
      for (unsigned int st=0;st<styles.size();st++)
      {
        if(st!=0)
          f<<",";
        f<<styles[st];					
      }
      f<<"\"";
      // end the properties for that node
      f<<"];" << std::endl;
    }
  
  // output all of the edges (we'll just use successor edges
  for (i = _nodes.begin(); i != _nodes.end(); i++)
  {
    DependenceNode *d1 = dynamic_cast < DependenceNode * >(*i);
    
    std::set < SimpleDirectedGraphNode * >succs = d1->getSuccessors();
    std::set < SimpleDirectedGraphNode * >::iterator j;
    for (j = succs.begin(); j != succs.end(); j++)
    {
      DependenceNode *d2 = dynamic_cast < DependenceNode * >(*j);
      
      std::set <DependenceNode*>::iterator found = _nodes.find(d2);

      if(found != _nodes.end()) {
        char buf1[sizeof(DependenceNode *) * 2 + 3];
        char buf2[sizeof(DependenceNode *) * 2 + 3];
        
        sprintf(buf1, "%p", d1);
        sprintf(buf2, "%p", d2);
        
        f << "\"" << buf1 << "\" -> \"" << buf2 << "\"";
        std::set < DependenceGraph::EdgeType > ets = graph->edgeType(d1, d2);
        DependenceGraph::EdgeType et = *(ets.begin());
        
        bool highlightEdge=false;
        if (d1->isHighlighted() && d2->isHighlighted())
          {
            highlightEdge=true;
          }
        
        f << "[";
        f<<"label=\"" << graph->getEdgeName(et) << "\"";
        if (!highlightEdge) {
          switch(et) {
          case DependenceGraph::SUMMARY:
            f<<",color=cyan";
            break;
          case DependenceGraph::CONTROL:
            f<<",color=blue";
            break;
          case DependenceGraph::DATA:
            f<<",color=green";
            break;
          case DependenceGraph::CALL:
            f<<",color=gold1";
            break;
          case DependenceGraph::SYNTACTIC:
            f<<",color=gray";
          default:
            break;
          } 
        } else {
          f<<",style=bold";
          f<<",color=red";
        }
        f<<"];" << std::endl;
      }
    }
  }

  f << "}" << std::endl;
}

int getLineNumber(SgNode *sgNode) {
  SgLocatedNode *locatedNode = isSgLocatedNode(sgNode);
  int lineNum = -1;
  if(locatedNode) { 
    const Sg_File_Info* info = locatedNode->get_startOfConstruct();
    ROSE_ASSERT(info);
    lineNum = info->get_line();
  }
  return lineNum;
}

string getFilename(SgNode *sgNode) {
  SgLocatedNode *locatedNode = isSgLocatedNode(sgNode);
  string filename;
  string fullFilename;
  if(locatedNode) { 
    const Sg_File_Info* info = locatedNode->get_startOfConstruct();
    ROSE_ASSERT(info);
    fullFilename = info->get_filename();
  }
  boost::filesystem::path path1(fullFilename); 
  filename = boost::filesystem::basename (path1); 
  return filename;
}


/* Finds nodes marked by infoflow pragmas on the AST, (Which means tained 
 * data should not affect them in anyway), and returns a vector
 * of them. */

vector<DependenceNode*> findMarkedNodes(SgProject* project, SystemDependenceGraph* sys_dependence_graph, enum infoflow_enum mark) {

  vector<DependenceNode*> markedNodes;

  // Yes, this is totally inefficent.  Is there a better way?
  Rose_STL_Container<SgNode*> nodeList = NodeQuery::querySubTree(project, V_SgNode);
  for (Rose_STL_Container<SgNode*>::iterator listElement=nodeList.begin();
       listElement!=nodeList.end(); ++listElement) {
    SgNode* node = *listElement;
    AstAttribute* pragmaAttribute = node->getAttribute(infoflow_names[mark]);
    if(pragmaAttribute) {
      DependenceNode *markedNode = NULL;
      markedNode = sys_dependence_graph->getExistingNode( DependenceNode::SGNODE, node);
      assert(markedNode != NULL);
      markedNodes.push_back(markedNode);
      int lineNum = getLineNumber(node);
      cout << infoflow_verbs[mark] << " node found on line: "<< lineNum << endl;
    }
  }
  return markedNodes;
}

// Here we find all the nodes that sanatize data, and apply them.

void sanatizeSDG(SgProject* project, SystemDependenceGraph* graph, enum infoflow_enum santype) {
  
  vector<DependenceNode*> sanatizeNodes = findMarkedNodes(project, graph,
                                                          santype);
  
  for (vector<DependenceNode*>::iterator ii = sanatizeNodes.begin(); ii != sanatizeNodes.end();
       ii++)
  {
    DependenceNode *d1 = dynamic_cast < DependenceNode * >(*ii);
    
    std::set < SimpleDirectedGraphNode * >pres = d1->getPredecessors();
    
    for (std::set < SimpleDirectedGraphNode * >::iterator jj = pres.begin(); 
         jj != pres.end(); jj++) {
      DependenceNode *d2 = dynamic_cast < DependenceNode * >(*jj);
      
      std::set < DependenceGraph::EdgeType > ets = graph->edgeType(d2, d1);
      for (std::set < DependenceGraph::EdgeType >::iterator kk = ets.begin(); 
         kk != ets.end(); kk++) {
        DependenceGraph::EdgeType oldType = *kk;

        graph->removeEdge(d2, d1, oldType);
        //WARNING: SERIOUS TYPE SYSTEM ABUSE!
        DependenceGraph::EdgeType newType = static_cast<DependenceGraph::EdgeType>( oldType | DependenceGraph::DO_NOT_FOLLOW);
        graph->establishEdge(d2, d1, newType);
        
      }
      DependenceGraph::EdgeType et = *(ets.begin());
    }
  }
}


// Here we find all the nodes that sanatize data, and apply them.

void unSanatizeSDG(SgProject* project, SystemDependenceGraph* graph, enum infoflow_enum santype) {
  
  vector<DependenceNode*> sanatizeNodes = findMarkedNodes(project, graph,
                                                          santype);;
  
  for (vector<DependenceNode*>::iterator ii = sanatizeNodes.begin(); ii != sanatizeNodes.end();
       ii++)
  {
    DependenceNode *d1 = dynamic_cast < DependenceNode * >(*ii);
    
    std::set < SimpleDirectedGraphNode * >pres = d1->getPredecessors();
    
    for (std::set < SimpleDirectedGraphNode * >::iterator jj = pres.begin(); 
         jj != pres.end(); jj++) {
      DependenceNode *d2 = dynamic_cast < DependenceNode * >(*jj);
      
      std::set < DependenceGraph::EdgeType > ets = graph->edgeType(d2, d1);
      for (std::set < DependenceGraph::EdgeType >::iterator kk = ets.begin(); 
         kk != ets.end(); kk++) {
        DependenceGraph::EdgeType oldType = *kk;
        graph->removeEdge(d2, d1, oldType);
        //WARNING: SERIOUS TYPE SYSTEM ABUSE!        
        DependenceGraph::EdgeType newType = static_cast<DependenceGraph::EdgeType>( oldType & (DependenceGraph::DO_NOT_FOLLOW ^ 0xFFFFFFFF));
        graph->establishEdge(d2, d1, newType);
        
      }
      DependenceGraph::EdgeType et = *(ets.begin());
    }
  }
}

/* Takes a slice of the SDG from the trustedNodes.  If any taint is found in
   that slice, a warning is printed, the tainted node is highlighted,
   and a DOT file dumped.
*/
void findDangerousFlow(SgProject* project, SystemDependenceGraph* sys_dependence_graph, vector<DependenceNode*> trustedNodes, enum infoflow_enum tainttype) {

  int count = 0;

  for( vector < DependenceNode * >::iterator itNode = trustedNodes.begin();
       itNode != trustedNodes.end(); ++itNode) {
    
    DependenceNode * trustedNode = *itNode;
    
    bool dangerFound = false;
    vector<DependenceNode * > taintedNodes;
    
    set < DependenceNode * > trustedSet = sys_dependence_graph->getSlice(trustedNode);
    for( set < DependenceNode * >::iterator slicedNode = trustedSet.begin();
         slicedNode != trustedSet.end(); ++slicedNode) {
      DependenceNode* thisDepNode = *slicedNode; 
      SgNode* depSgNode = thisDepNode->getSgNode();
      AstAttribute* taintAttribute = depSgNode->getAttribute(infoflow_names[tainttype]);
      if(taintAttribute) {
        dangerFound = true;
        thisDepNode->highlightNode();
        trustedNode->highlightNode();
        taintedNodes.push_back(thisDepNode);
        int thisDepLine = getLineNumber(depSgNode);
        int thisTrustLine = getLineNumber(trustedNode->getSgNode());
        cout << infoflow_verbs[tainttype] << " node found on Line: " << thisDepLine << " from: " 
             << thisTrustLine << endl;
      }
    }

    if(dangerFound) {
      char buf[256];
      sprintf(buf, "%s.%s_%d.dot", getFilename(trustedNode->getSgNode()).c_str(), 
              infoflow_verbs[tainttype].c_str(), count);
      writeSlicedDot(sys_dependence_graph, trustedSet, buf);
    }

    // Un-hightlight the tainted nodes before the next trusted node is sliced
    for( vector < DependenceNode * >::iterator itNode = taintedNodes.begin();
         itNode != taintedNodes.end(); ++itNode) {
      DependenceNode * taintedNode = *itNode;
      taintedNode->unHighlightNode();
      trustedNode->unHighlightNode();
    }
    ++count;
  }
  
}

/* This program finds pragmas related to taint checking.  If finds 4 pragmas:
 *
 * #pragma leek tained
 * #pragma leek classified
 * #pragma leek trusted
 * #pragma leek sanatized
 *
 * It then annotates the AST with attributes on the nodes the pragmas are 
 * refering to.  Hopefully this will allow us to do information flow analysis
 * on the system dependence graph.
 */
int main ( int argc, char* argv[] )  {


  // ****************** build AST ******************
  
  SgProject* project = frontend(argc,argv);
  ROSE_ASSERT (project != NULL);

  annotateAST(project);


  // ******************* Dump annotated AST to Dot ****************
  generateDOT( *project );
  
  // ****************** build SDG ******************
  SystemDependenceGraph* sys_dependence_graph = new SystemDependenceGraph();
  cout << "BEGIN: generating the SDG " << endl;
  sys_dependence_graph->parseProject(project);
  cout << "END: generating the SDG " << endl;

  //Hack to get the filename.  Neew pretty much any SgLocatedNode
  Rose_STL_Container<SgNode*> locatedList = NodeQuery::querySubTree(project, V_SgLocatedNode);
  SgLocatedNode* lnode = dynamic_cast<SgLocatedNode*>(*(locatedList.begin()));

  string filename = getFilename(lnode) + string(".sdg.dot");
  cout << "Dumping SDG to : " << filename << endl;
  sys_dependence_graph->writeDot(const_cast<char*>(filename.c_str()));

  // Check for taint:

  sanatizeSDG(project, sys_dependence_graph, i_sanatized);

  vector<DependenceNode*> trustedNodes = findMarkedNodes(project, sys_dependence_graph, i_trusted);

  findDangerousFlow(project, sys_dependence_graph, trustedNodes, i_tainted);
  
  unSanatizeSDG(project, sys_dependence_graph, i_sanatized);

  //Check for classified data leaks

  sanatizeSDG(project, sys_dependence_graph, i_declassified);

  vector<DependenceNode*> publicNodes = findMarkedNodes(project, sys_dependence_graph, i_public);

  findDangerousFlow(project, sys_dependence_graph, publicNodes, i_classified);
  
  unSanatizeSDG(project, sys_dependence_graph, i_declassified);  

  return 0;
  
}
