// Author: Markus Schordan
// $Id: DOTRepresentation.h,v 1.2 2006/04/24 00:21:27 dquinlan Exp $

#ifndef DOTREPRESENTATION_H
#define DOTREPRESENTATION_H

// #include "rose_config.h"

#include <string>
#include <sstream>

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

template<class NodeType>
class DOTRepresentation {
public:
  typedef unsigned int TraceType;
  typedef unsigned int ContainerIndexType;

  void writeToFileAsGraph(std::string filename);
  void writeToFile(std::string filename);

  DOTRepresentation();
  ~DOTRepresentation();
  void clear();
    
  void addNode(NodeType node, std::string nodelabel, std::string option="");

  void addEdge(NodeType node1, TraceType downtrace, TraceType uptrace, std::string edgelabel, NodeType node2, std::string option="");
  void addEdge(NodeType node1, TraceType downtrace, std::string edgelabel, NodeType node2, std::string option="");
  void addEdge(NodeType node1, std::string edgelabel, NodeType node2, std::string option="");
  void addEdge(NodeType node1, NodeType node2, std::string option="");

  void addNullValue(NodeType node, TraceType trace, std::string varname, std::string option="");
  void addNullValue(NodeType node, std::string nodelabel, std::string edgelabel, std::string option="");

  void addEmptyContainer(NodeType node, TraceType trace, std::string varname, std::string option="");
  std::string containerEdgeLabel(std::string containerVarName, ContainerIndexType count);

  std::string traceFormat(TraceType td, TraceType bu);
  std::string traceFormat(TraceType tdPos, TraceType buPos, TraceType tdCount, TraceType buCount);
  std::string traceFormat(TraceType tracepos);

 protected:
  std::string nodeName(NodeType node);
  std::string nullNodeName(NodeType node, std::string extention);
  std::string graphStart(std::string graphName="G0");
  std::string graphEnd();

  std::string nonQuotedNodeName(NodeType node);
  std::ostringstream* dotout;
 private:
};

// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
   #include "DOTRepresentationImpl.h" 
// #endif 

#endif
