#ifndef LABELER_H
#define LABELER_H

/*************************************************************
 * Copyright: (C) 2012 Markus Schordan                       *
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <set>
#include "RoseAst.h"
#include "VariableIdMapping.h"

using namespace std;

#define NO_STATE -3
#define NO_ESTATE -4

typedef size_t Label;

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
 class LabelProperty {
 public:
   enum LabelType { LABEL_UNDEF=1, LABEL_OTHER=2, 
                    LABEL_FUNCTIONCALL=100, LABEL_FUNCTIONCALLRETURN,
                    LABEL_FUNCTIONENTRY, LABEL_FUNCTIONEXIT,
                    LABEL_BLOCKBEGIN, LABEL_BLOCKEND
   };
   LabelProperty();
   LabelProperty(SgNode* node);
   LabelProperty(SgNode* node, LabelType labelType);
   LabelProperty(SgNode* node, VariableIdMapping* variableIdMapping);
   LabelProperty(SgNode* node, LabelType labelType, VariableIdMapping* variableIdMapping);
   string toString();
   SgNode* getNode();
   bool isFunctionCallLabel();
   bool isFunctionCallReturnLabel();
   bool isFunctionEntryLabel();
   bool isFunctionExitLabel();
   bool isBlockBeginLabel();
   bool isBlockEndLabel();

 protected:
   void initializeIO(VariableIdMapping* variableIdMapping);

 public:
   enum IOType { LABELIO_NONE, LABELIO_STDIN, LABELIO_STDOUTVAR, LABELIO_STDOUTCONST, LABELIO_STDERR
   };

   bool isStdOutLabel();
   bool isStdOutVarLabel();
   bool isStdOutConstLabel();
   bool isStdInLabel();
   bool isStdErrLabel();
   bool isIOLabel();
   VariableId getIOVarId();
   int getIOConst();

   void makeTerminationIrrelevant(bool t);
   bool isTerminationRelevant();
   bool isLTLRelevant();

 private:
   bool _isValid;
   SgNode* _node;
   LabelType _labelType;

 private:
   IOType _ioType;
   VariableId _variableId;
   int _ioValue;
   bool _isTerminationRelevant;
   bool _isLTLRelevant;
 };

/*! 
  * \author Markus Schordan
  * \date 2012.
 */
class LabelSet : public set<Label> {
 public:

   // temporary until all sets are properly using the std:algorithms for set operations
#if 1
LabelSet operator+(LabelSet& s2) {
  LabelSet result;
  result=*this;
  for(LabelSet::iterator i2=s2.begin();i2!=s2.end();++i2)
    result.insert(*i2);
  return result;
}
#endif

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
 bool isElement(Label lab) {
   return find(lab)!=end();
 }
};

 typedef std::set<LabelSet> LabelSetSet;

/*! 
  * \author Markus Schordan
  * \date 2012, 2013.
 */
class Labeler {
 public:
  static const Label NO_LABEL=-1;
  Labeler(SgNode* start);
  static string labelToString(Label lab);
  int isLabelRelevantNode(SgNode* node);
  virtual void createLabels(SgNode* node);

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
  bool isConditionLabel(Label lab);

 protected:
  void computeNodeToLabelMapping();
  void registerLabel(LabelProperty);
  vector<LabelProperty> mappingLabelToLabelProperty;
  map<SgNode*,Label> mappingNodeToLabel;
  bool _isValidMappingNodeToLabel;
};

class IOLabeler : public Labeler {
 public:
  IOLabeler(SgNode* start, VariableIdMapping* variableIdMapping);
  virtual void createLabels(SgNode* node);

 public:
  bool isStdInLabel(Label label, VariableId* id=0);
  bool isStdOutLabel(Label label); // deprecated
  bool isStdOutVarLabel(Label label, VariableId* id=0);
  bool isStdOutConstLabel(Label label, int* constvalue=0);
  bool isStdErrLabel(Label label, VariableId* id=0);

 private:
  VariableIdMapping* _variableIdMapping;
};

#endif
