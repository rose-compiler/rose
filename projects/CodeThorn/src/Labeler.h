#ifndef LABELER_H
#define LABELER_H

/*********************************
 * Author: Markus Schordan, 2012 *
 *********************************/

#include <set>
#include "rose.h"
#include "MyAst.h"

using namespace std;

#define NO_STATE -3
#define NO_ESTATE -4
typedef signed int Label;

class LabelSet : public set<Label> {
 public:
LabelSet operator+(LabelSet& s2) {
  LabelSet result;
  result=*this;
  for(LabelSet::iterator i2=s2.begin();i2!=s2.end();++i2)
	result.insert(*i2);
  return result;
}

LabelSet& operator+=(LabelSet& s2) {
  for(LabelSet::iterator i2=s2.begin();i2!=s2.end();++i2)
	insert(*i2);
  return *this;
 }
 std::string toString() {
   std::stringstream ss;
   ss<<"{";
   for(LabelSet::iterator i=begin();i!=end();++i) {
	 if(i!=begin())
	   ss<<",";
	 ss<<*i;
   }
   ss<<"}";
   return ss.str();
 }
};

class Labeler {
 public:
  static const int NO_LABEL=-1;

  Labeler(SgNode* start);
  static string labelToString(Label lab);
  int isLabelRelevantNode(SgNode* node);
  void createLabels(SgNode* node);

  /* Labels are numbered 0..n-1 where n is the number of labeled nodes (not all nodes are labeled).
	 A return value of NO_LABEL means that this node has no label.
  */
  Label getLabel(SgNode* node);
  LabelSet getLabelSet(set<SgNode*>& nodeSet);
  
  /* Returns the node with the label 'label'. If the return value is 0 then no node exists for this label -
	 this can only be the case if label is errornously higher than the number of labeled nodes or NO_LABEL.
  */
  SgNode* getNode(Label label);
  long numberOfLabels();
  std::string toString();
  Label functionCallLabel(SgNode* node);
  Label functionCallReturnLabel(SgNode* node);
  Label blockBeginLabel(SgNode* node);
  Label blockEndLabel(SgNode* node);
  Label functionEntryLabel(SgNode* node);
  Label functionExitLabel(SgNode* node);
  bool isFunctionEntryLabel(Label lab);
  bool isFunctionExitLabel(Label lab);
  bool isBlockBeginLabel(Label lab);
  bool isBlockEndLabel(Label lab);
  bool isFunctionCallLabel(Label lab);
  bool isFunctionCallReturnLabel(Label lab);
 private:
  vector<SgNode*> labelNodeMapping;
};

#endif
