#ifndef LABELER_H
#define LABELER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include "RoseAst.h"
#include "VariableIdMapping.h"
#include "Miscellaneous.h"

using namespace std;

#define NO_STATE -3
#define NO_ESTATE -4

namespace CodeThorn {

typedef size_t Label;

 class LabelProperty {
 public:
   enum LabelType { LABEL_UNDEF=1, LABEL_OTHER=2, 
                    LABEL_FUNCTIONCALL=100, LABEL_FUNCTIONCALLRETURN,
                    LABEL_FUNCTIONENTRY, LABEL_FUNCTIONEXIT,
                    LABEL_BLOCKBEGIN, LABEL_BLOCKEND
   };
   enum IOType { LABELIO_NONE, LABELIO_STDIN, LABELIO_STDOUT, LABELIO_STDERR
   };

   LabelProperty();
   LabelProperty(SgNode* node, VariableIdMapping* variableIdMapping);
   LabelProperty(SgNode* node, LabelType labelType, VariableIdMapping* variableIdMapping);
   void initialize(VariableIdMapping* variableIdMapping);
   string toString();

   void makeTerminationIrrelevant(bool t);
   bool isTerminationRelevant();
   bool isLTLRelevant();
   SgNode* getNode();
   bool isStdOutLabel();
   bool isStdInLabel();
   bool isStdErrLabel();
   bool isIOLabel();
   bool isFunctionCallLabel();
   bool isFunctionCallReturnLabel();
   bool isFunctionEntryLabel();
   bool isFunctionExitLabel();
   bool isBlockBeginLabel();
   bool isBlockEndLabel();
   VariableId getIOVarId();
 private:
   bool _isValid;
   SgNode* _node;
   LabelType _labelType;
   IOType _ioType;
   VariableId _variableId;
   bool _isTerminationRelevant;
   bool _isLTLRelevant;
 };

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
  static const Label NO_LABEL=-1;

  Labeler(SgNode* start, VariableIdMapping* variableIdMapping);
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
  bool isStdInLabel(Label label, VariableId* id=0);
  bool isStdOutLabel(Label label, VariableId* id=0);
  bool isStdErrLabel(Label label, VariableId* id=0);

 private:
  void computeNodeToLabelMapping();
  void registerLabel(LabelProperty);
  vector<LabelProperty> mappingLabelToNode;
  map<SgNode*,Label> mappingNodeToLabel;
  VariableIdMapping* _variableIdMapping;
  bool _isValidMappingNodeToLabel;
};

} // end of namespace CodeThorn

#endif
