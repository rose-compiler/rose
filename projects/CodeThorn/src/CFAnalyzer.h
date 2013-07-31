#ifndef CFANALYZER_H
#define CFANALYZER_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include "rose.h"
#include "SgNodeHelper.h"
#include "Labeler.h"
#include "CommandLineOptions.h"

namespace CodeThorn {

  enum EdgeType { EDGE_UNKNOWN=0, EDGE_FORWARD, EDGE_BACKWARD, EDGE_TRUE, EDGE_FALSE, EDGE_LOCAL, EDGE_CALL, EDGE_CALLRETURN, EDGE_EXTERNAL, EDGE_PATH };

class Edge {
 public:
  Edge();
  Edge(Label source0,Label target0);
  Edge(Label source0,EdgeType type0,Label target0);
  Edge(Label source0,set<EdgeType> type0,Label target0);
  string toString() const;
  string toStringNoType() const;
  string toDot() const;
  //string typeToString() const;
  string typesToString() const;
  static string typeToString(EdgeType et);
  Label source;
  //EdgeType type;
  Label target;
  bool isType(EdgeType et) const;
  void addType(EdgeType et);
  void addTypes(set<EdgeType> ets);
  void removeType(EdgeType et);
  set<EdgeType> types() const;
  long typesCode() const;
  string color() const;
  string dotEdgeStyle() const;
  long hash() const;
 private:
  set<EdgeType> _types;
};

bool operator==(const Edge& e1, const Edge& e2);
bool operator!=(const Edge& e1, const Edge& e2);
bool operator<(const Edge& e1, const Edge& e2);

class Flow : public set<Edge> {
 public:  
  Flow();
  Flow operator+(Flow& s2);
  Flow& operator+=(Flow& s2);
  string toDot(Labeler *labeler);
  LabelSet nodeLabels();
  LabelSet sourceLabels();
  LabelSet targetLabels();
  LabelSet pred(Label label);
  LabelSet succ(Label label);
  Flow inEdges(Label label);
  Flow outEdges(Label label);
  Flow outEdgesOfType(Label label, EdgeType edgeType);
  void setDotOptionDisplayLabel(bool opt);
  void setDotOptionDisplayStmt(bool opt);
  void setTextOptionPrintType(bool opt) { _stringNoType=!opt;}
  string toString();
 private:
  bool _dotOptionDisplayLabel;
  bool _dotOptionDisplayStmt;
  bool _stringNoType;
};

class InterEdge {
 public:
  InterEdge(Label call, Label entry, Label exit, Label callReturn);
  string toString() const;
  Label call;
  Label entry;
  Label exit;
  Label callReturn;
};

bool operator<(const InterEdge& e1, const InterEdge& e2);
bool operator==(const InterEdge& e1, const InterEdge& e2);
bool operator!=(const InterEdge& e1, const InterEdge& e2);

class InterFlow : public set<InterEdge> {
 public:
  string toString() const;
};

class CFAnalyzer {
 public:
  CFAnalyzer(Labeler* l);
  Label getLabel(SgNode* node);
  SgNode* getNode(Label label);
  Label initialLabel(SgNode* node);
  LabelSet finalLabels(SgNode* node);
  LabelSet functionCallLabels(Flow& flow);
  Flow flow(SgNode* node);
  Flow flow(SgNode* s1, SgNode* s2);
  Labeler* getLabeler();
  // computes from existing intra-procedural flow graph(s) the inter-procedural call information
  InterFlow interFlow(Flow& flow); 
  void intraInterFlow(Flow&, InterFlow&);
  int reduceBlockBeginNodes(Flow& flow);
 private:
  Flow WhileAndDoWhileLoopFlow(SgNode* node, Flow edgeSet, EdgeType param1, EdgeType param2);
  Labeler* labeler;
};	

} // end of namespace CodeThorn

#endif
