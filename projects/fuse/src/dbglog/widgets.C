#include "widgets.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>

using namespace std;

//#define ROOT_PATH "/cygdrive/c/work/code/dbglog"
#define DOT_PATH ""

namespace dbglog {

/*****************
 ***** scope *****
 *****************/
scope::scope(std::string label, scopeLevel level, int curDebugLevel, int targetDebugLevel) : 
  block(label)
{
  init(level, curDebugLevel, targetDebugLevel);
}

scope::scope(string label, const anchor& pointsTo, scopeLevel level, int curDebugLevel, int targetDebugLevel): 
  block(label, pointsTo)
{
  init(level, curDebugLevel, targetDebugLevel);
}

scope::scope(string label, const set<anchor>& pointsTo, scopeLevel level, int curDebugLevel, int targetDebugLevel) :
  block(label, pointsTo)
{
  init(level, curDebugLevel, targetDebugLevel);
}

std::vector<std::string> scope::colors;
int scope::colorIdx=0; // The current index into the list of colors 

// Common initialization code
void scope::init(scopeLevel level, int curDebugLevel, int targetDebugLevel)
{
  //cout << "scope::init() anchor="<<getAnchorRef().str()<<endl;
  
  // If the colors list has not yet been initialized, do so now
  if(colors.size() == 0) {
    // Initialize colors with a list of light pastel colors 
    colors.push_back("FF97E8");
    colors.push_back("75D6FF");
    colors.push_back("72FE95");
    colors.push_back("8C8CFF");
    colors.push_back("57BCD9");
    colors.push_back("99FD77");
    colors.push_back("EDEF85");
    colors.push_back("B4D1B6");
    colors.push_back("FF86FF");
    colors.push_back("4985D6");
    colors.push_back("D0BCFE");
    colors.push_back("FFA8A8");
    colors.push_back("A4F0B7");
    colors.push_back("F9FDFF");
    colors.push_back("FFFFC8");
    colors.push_back("5757FF");
    colors.push_back("6FFF44");
  }
  
  if(curDebugLevel >= targetDebugLevel) {
    active = true;
    this->level = level;
    // If this block corresponds to a new file, this string will be set to the Javascript command to 
    // load this file into the current view
    string loadCmd="";
    if(level == high) {
      colorIdx++; // Advance to a new color for this func
      loadCmd = dbg.enterFileLevel(this);
    } else if(level == medium) {
      colorIdx++; // Advance to a new color for this func
      dbg.enterBlock(this, false);
    }
    else if(level == low || level == min)
      dbg.enterBlock(this, false);
  }
  else
    active = false;
}

scope::~scope()
{ 
  if(active) {
    if(level == high) {
      dbg.exitFileLevel();
      colorIdx--; // Return to the last color for this func's parent
    }
    else if(level == medium) {
      dbg.exitBlock();
      colorIdx--; // Return to the last color for this func's parent
    } else if(level == low || level == min)
      dbg.exitBlock();
    assert(colorIdx>=0);
  }
}

// Called to enable the block to print its entry and exit text
void scope::printEntry(string loadCmd) {
  dbg.ownerAccessing();
  //dbg << "blockID="<<getBlockID()<<endl;
  if(dbg.blockIndex()==0) dbg << "\t\t\t"<<tabs(dbg.blockDepth())<<"</td></tr>\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth())<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"<table bgcolor=\"#"<<colors[(colorIdx-1)%colors.size()]<<"\" width=\"100%\" id=\"table"<<getBlockID()<<"\" style=\"border:1px solid white\" onmouseover=\"this.style.border='1px solid black'; highlightLink('"<<getBlockID()<<"', '#F4FBAA');\" onmouseout=\"this.style.border='1px solid white'; highlightLink('"<<getBlockID()<<"', '#FFFFFF');\" onclick=\"focusLinkSummary('"<<getBlockID()<<"', event);\">\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
  if(level == high || level == medium || level == low) {
    dbg << "<h2>\n";
    dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"<a name=\"anchor"<<getBlockID()<<"\" href=\"javascript:unhide('"<<getBlockID()<<"');\">";
  }
  dbg.userAccessing();
  dbg << getLabel();
  dbg.ownerAccessing();
  dbg << "(" << blockCount << ")";
  if(level == high || level == medium || level == low) {
    dbg << "</a>\n";
  
    if(loadCmd != "") {
      dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1);
      dbg << "<a href=\"javascript:"<<loadCmd<<")\">";
      //dbg << "<a href=\"javascript:loadURLIntoDiv(top.detail.document, '"<<detailContentURL<<".body', 'div"<<getBlockID()<<"'); loadURLIntoDiv(top.summary.document, '"<<summaryContentURL<<".body', 'sumdiv"<<getBlockID()<<"')\">";
      dbg << "<img src=\"img/divDL.gif\" width=25 height=35></a>\n";
      dbg << "\t\t\t<a target=\"_top\" href=\"index."<<getFileID()<<".html\">";
      dbg << "<img src=\"img/divGO.gif\" width=35 height=25></a>\n";
    }

    dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"</h2>\n";
  }
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"</td></tr>\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"<div id=\"div"<<getBlockID()<<"\" class=\"unhidden\">\n";
  dbg.flush();
  dbg.userAccessing();  
}

void scope::printExit() {
 // Close this scope
  dbg.ownerAccessing();
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"</td></tr>\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth()+1)<<"</table>\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth())<<"</td></tr>\n";
  dbg << "\t\t\t"<<tabs(dbg.blockDepth())<<"<tr width=\"100%\"><td width=50></td><td width=\"100%\">\n";
  dbg.userAccessing();  
}

/*****************
 ***** graph *****
 *****************/

// The path the directory where output files of the graph widget are stored
// Relative to current path
std::string graph::outDir="";
// Relative to root of HTML document
std::string graph::htmlOutDir="";

// Maximum ID assigned to any graph object
int graph::maxWidgetID=0;

graph::graph(int curDebugLevel, int targetDebugLevel) : block("Graph") {
  active = (curDebugLevel >= targetDebugLevel); 
  if(!active) return;

  dbg.enterBlock(this, false);
  //imgPath = dbg.addImage("svg");
  maxNodeID = 0;
  
  initEnvironment();  
    
  widgetID = maxWidgetID;
  maxWidgetID++;
  
  graphOutput = false;
  
  dbg.ownerAccessing();
  dbg << "<div id=\"graph_container_"<<widgetID<<"\"></div>\n";
  //dbg << "<div id=\"debug_output\"></div>\n";
  dbg.userAccessing();
}

// Initialize the environment within which generated graphs will operate, including
// the JavaScript files that are included as well as the directories that are available.
void graph::initEnvironment() {
  static bool initialized=false;
  
  if(initialized) return;
  initialized = true;
  
  pair<string, string> paths = dbg.createWidgetDir("graph");
  outDir = paths.first;
  htmlOutDir = paths.second;
  cout << "outDir="<<outDir<<" htmlOutDir="<<htmlOutDir<<endl;
  
  dbg.includeFile("canviz-0.1");
  
  //<!--[if IE]><script type="text/javascript" src="excanvas/excanvas.js"></script><![endif]-->
  //dbg.includeWidgetScript("canviz-0.1/prototype/excanvas/excanvas.js", "text/javascript");
  dbg.includeWidgetScript("canviz-0.1/prototype/prototype.js", "text/javascript");
  dbg.includeWidgetScript("canviz-0.1/path/path.js",         "text/javascript");
  dbg.includeWidgetScript("canviz-0.1/canviz.css",           "css");
  dbg.includeWidgetScript("canviz-0.1/canviz.js",            "text/javascript");
  dbg.includeWidgetScript("canviz-0.1/x11colors.js",         "text/javascript");
  //dbg.includeWidgetScript("canviz-0.1/graphs/graphlist.js",  "text/javascript");
  //dbg.includeWidgetScript("canviz-0.1/graphs/layoutlist.js", "text/javascript");

}

graph::~graph() {
  if(!active) return;
  
  if(!graphOutput) {
    outputCanvizDotGraph(genDotGraph());
  }
  
  dbg.exitBlock();
}

// Generates and returns the dot graph code for this graph
string graph::genDotGraph() {
  ostringstream dot;
  dot << "digraph G {"<<endl;

  /*cout << "nodes="<<endl;
  for(map<location, pair<int, string> >::iterator b=nodes.begin(); b!=nodes.end(); b++)
    cout << "    " << dbg.blockGlobalStr(b->first)<< " => [" << b->second.first << ", " << b->second.second << "]"<<endl;*/

  for(map<location, node>::iterator b=nodes.begin(); b!=nodes.end(); b++)
    dot << "\tnode_"<<b->second.ID<<" [shape=box, label=\""<<b->second.label<<"\", href=\"javascript:"<<b->second.a.getLinkJS()<<"\"];\n";

  // Between the time when an edge was inserted into edges and now, the anchors on both sides of each
  // edge should have been located (attached to a concrete location in the output). This means that
  // some of the edges are now redundant (e.g. multiple forward edges from one location that end up
  // arriving at the same location). We thus create a new set of edges based on the original list.
  // The set's equality checks will eliminate all duplicates.

  set<graphEdge> uniqueEdges;
  for(list<graphEdge>::iterator e=edges.begin(); e!=edges.end(); e++)
    uniqueEdges.insert(*e);

  /*cout << "edges="<<endl;
  for(set<graphEdge>::iterator e=uniqueEdges.begin(); e!=uniqueEdges.end(); e++) {
    cout << "    from="<<e->from.str("    ")<<" : found="<<(nodes.find(e->from.getLocation())!=nodes.end())<<" : "<<dbg.blockGlobalStr(e->from.getLocation())<<endl;
    cout << "    to="<<e->to.str("    ")    <<" : found="<<(nodes.find(e->to.getLocation())!=nodes.end())  <<" : "<<dbg.blockGlobalStr(e->to.getLocation())  <<endl;
    cout << "    from="<<e->from.str("    ")<<" : "<<nodes[e->from.getLocation()].first<<endl;
    cout << "    to="<<e->to.str("    ")    <<" : "<<nodes[e->to.getLocation()].first<<endl;
  }*/
  for(set<graphEdge>::iterator e=uniqueEdges.begin(); e!=uniqueEdges.end(); e++) {
    dot << "\tnode_" << nodes[e->from.getLocation()].ID << 
           " -> "<<
           "node_" << nodes[e->to.getLocation()].ID << 
           (e->directed? "": "[dir=none]") << ";\n";
  }

  dot << " }";

  return dot.str();
}

// Given a string representation of a dot graph, emits the graph's visual representation 
// as a Canviz widget into the debug output.
void graph::outputCanvizDotGraph(std::string dot) {
  //cout << "graph::outputCanvizDotGraph("<<dot<<")"<<endl;
  #ifdef DOT_PATH
  ostringstream origDotFName;   origDotFName   << outDir << "/orig."   << widgetID << ".dot";
  ostringstream placedDotFName; placedDotFName << outDir << "/placed." << widgetID << ".dot";

  ofstream dotFile;
  dotFile.open(origDotFName.str().c_str());
  dotFile << dot;
  dotFile.close();

  // Create the SVG file's picture of the dot file
  //ostringstream cmd; cmd << DOT_PATH << "dot -Tsvg -o"<<imgPath<<" "<<dotFName.str() << "-Tcmapx -o"<<mapFName.str()<<"&"; 
  // Create the explicit DOT file that details the graph's layout
  ostringstream cmd; cmd << DOT_PATH << "dot "<<origDotFName.str()<<" -Txdot -o"<<placedDotFName.str();//<<"&"; 
  struct timeval startTime; gettimeofday(&startTime, NULL);
  system(cmd.str().c_str());
  struct timeval endTime; gettimeofday(&endTime, NULL);
  cout << "Command \""<<cmd.str()<<"\", elapsed="<<(((endTime.tv_sec*1000000 + endTime.tv_usec) - (startTime.tv_sec*1000000 + startTime.tv_usec))/1000000.0)<<"\n";
  
  dbg.widgetScriptCommand(txt() << 
     "  var canviz_"<<widgetID<<";\n" <<
     "  canviz_"<<widgetID<<" = new Canviz('graph_container_"<<widgetID<<"');\n" <<
     //dbg << "  canviz_"<<widgetID<<".setImagePath('graphs/images/');\n";
     "  canviz_"<<widgetID<<".setScale(1);\n" <<
     "  canviz_"<<widgetID<<".load('"<<htmlOutDir<<"/placed." << widgetID << ".dot');\n"); 

  #else
  dbg << "<b>graphviz not available</b>" << endl;
  #endif

  graphOutput = true;
}

// Given a reference to an object that can be represented as a dot graph,  create an image from it and add it to the output.
// Return the path of the image.
void graph::genGraph(dottable& obj) {
  graph g;
  g.outputCanvizDotGraph(obj.toDOT("graph_"+g.widgetID));
}

// Given a representation of a graph in dot format, create an image from it and add it to the output.
// Return the path of the image.
void graph::genGraph(std::string dot) {
  graph g;
  g.outputCanvizDotGraph(dot);
}

// Add a directed edge from the location of the from anchor to the location of the to anchor
void graph::addDirEdge(anchor from, anchor to) {
  edges.push_back(graphEdge(from, to, true)); 
}

// Add an undirected edge between the location of the a anchor and the location of the b anchor
void graph::addUndirEdge(anchor a, anchor b) {
  edges.push_back(graphEdge(a, b, false));
}
  
// Called to notify this block that a sub-block was started/completed inside of it. 
// Returns true of this notification should be propagated to the blocks 
// that contain this block and false otherwise.
bool graph::subBlockEnterNotify(block* subBlock) {
  // If this block is immediately contained inside this graph
  location common = dbgStream::commonSubLocation(getLocation(), subBlock->getLocation());
  
  // If subBlock is nested immediately inside the graph's block
  assert(subBlock->getLocation().size()>0);
  if(common == getLocation() &&
     subBlock->getLocation().size() == common.size() &&
     subBlock->getLocation().back().second.size()-1 == common.back().second.size())
  { 
    //cout << "subBlock->getLocation="<<dbg.blockGlobalStr(subBlock->getLocation())<<endl;
    nodes[subBlock->getLocation()] = node(maxNodeID, subBlock->getLabel(), subBlock->getAnchor());
    maxNodeID++;
  }
  
  return false;
}
bool graph::subBlockExitNotify(block* subBlock)
{
  return false;  
}

} // namespace dbglog
