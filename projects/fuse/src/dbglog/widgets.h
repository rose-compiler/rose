#pragma once

#include <list>
#include <vector>
#include <set>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include "dbglog.h"

namespace dbglog {

class scope: public block
{
  static std::vector<std::string> colors;
  static int colorIdx; // The current index into the list of colors 
  
  public:
  bool active;
  typedef enum {high, medium, low, min} scopeLevel;
  scopeLevel level;
  
  scope(std::string label, scopeLevel level=medium, int curDebugLevel=0, int targetDebugLevel=0);
  scope(std::string label, const anchor& pointsTo, scopeLevel level=medium, int curDebugLevel=0, int targetDebugLevel=0);
  scope(std::string label, const std::set<anchor>& pointsTo, scopeLevel level=medium, int curDebugLevel=0, int targetDebugLevel=0);
  
  private:
  // Common initialization code
  void init(scopeLevel level, int curDebugLevel, int targetDebugLevel);
  
  public:
    
  // Called to notify this block that a sub-block was started/completed inside of it. 
  // Returns true of this notification should be propagated to the blocks 
  // that contain this block and false otherwise.
  bool subBlockEnterNotify(block* subBlock) { return true; }
  bool subBlockExitNotify (block* subBlock) { return true; }
  
  // Called to enable the block to print its entry and exit text
  void printEntry(std::string loadCmd);
  void printExit();
  
  ~scope();
}; // scope

class graph;

class graphEdge {
  anchor from;
  anchor to;
  bool directed;
  
  friend class graph;
  
  public:
  graphEdge(anchor from, anchor to, bool directed) :
    from(from), to(to), directed(directed)
  {}
    
  const anchor& getFrom() { return from; }
  const anchor& getTo()   { return to; }
  
  bool operator<(const graphEdge& that) const {
    return (from < that.from) ||
           (from == that.from && to < that.to) ||
           (from == that.from && to == that.to && directed < that.directed);
  }
};

class dottable
{
  public:
  virtual ~dottable() {}
  // Returns a string that containts the representation of the object as a graph in the DOT language
  // that has the given name
  virtual std::string toDOT(std::string graphName)=0;
};

class graph: public block
{
  protected:
  class node {
    public:
    int ID;
    std::string label;
    anchor a;
      
    node() : ID(-1), label("") {}
    node(int ID, std::string label, const anchor& a) : ID(ID), label(label), a(a) {}
  };
  
  // Maps a block's location to its ID and label
  std::map<location, node> nodes;
  // The maximum ID associated with any node in this graph
  int maxNodeID;
  
  std::list<graphEdge> edges;
  
  // The path the directory where output files of the graph widget are stored
  // Relative to current path
  static std::string outDir;
  // Relative to root of HTML document
  static std::string htmlOutDir;
    
  // Unique ID of this graph object
  int widgetID;
  
  // Maximum ID assigned to any graph object
  static int maxWidgetID;
  
  // Records whether this graph has already been output by a call to outputCanvizDotGraph()
  bool graphOutput;
  
  // Records whether this graph will be emitted to the output (is active) or not because we're not at a sufficiently high debug level
  bool active;

  public:
  
  graph(int curDebugLevel=0, int targetDebugLevel=0);
  ~graph();
  
  // Generates and returns the dot graph code for this graphgenDotGraph
  virtual std::string genDotGraph();
  
  // Given a string representation of a dot graph, emits the graph's visual representation 
  // as a Canviz widget into the debug output.
  void outputCanvizDotGraph(std::string dot);
  
  // Given a reference to an object that can be represented as a dot graph,  create an image from it and add it to the output.
  // Return the path of the image.
  static void genGraph(dottable& obj);

  // Given a representation of a graph in dot format, create an image from it and add it to the output.
  // Return the path of the image.
  static void genGraph(std::string dot);
  
  // Initialize the environment within which generated graphs will operate, including
  // the JavaScript files that are included as well as the directories that are available.
  static void initEnvironment();
  
  // Add a directed edge from the location of the from anchor to the location of the to anchor
  void addDirEdge(anchor from, anchor to);
  
  // Add an undirected edge between the location of the a anchor and the location of the b anchor
  void addUndirEdge(anchor a, anchor b);
  
  // Called to notify this block that a sub-block was started/completed inside of it. 
  // Returns true of this notification should be propagated to the blocks 
  // that contain this block and false otherwise.
  bool subBlockEnterNotify(block* subBlock);
  bool subBlockExitNotify (block* subBlock);
};

} // namespace dbglog
