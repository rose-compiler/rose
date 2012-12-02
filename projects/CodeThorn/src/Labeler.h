#ifndef LABELER_H
#define LABELER_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <set>
#include "rose.h"
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

   void makeTerminationIrrelevant(bool t) {assert(_isTerminationRelevant); _isTerminationRelevant=false;}
   bool isTerminationRelevant() {assert(_isValid); return _isTerminationRelevant;}
   bool isLTLRelevant() {assert(_isValid); return _isLTLRelevant;}
   SgNode* getNode() { assert(_isValid); return _node;}
   bool isStdOutLabel() { assert(_isValid); return _ioType==LABELIO_STDOUT; }
   bool isStdInLabel() { assert(_isValid); return _ioType==LABELIO_STDIN; }
   bool isStdErrLabel() { assert(_isValid); return _ioType==LABELIO_STDERR; }
   bool isIOLabel() { assert(_isValid); return isStdOutLabel()||isStdInLabel()||isStdErrLabel(); }
   bool isFunctionCallLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONCALL; }
   bool isFunctionCallReturnLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONCALLRETURN; }
   bool isFunctionEntryLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONENTRY; }
   bool isFunctionExitLabel() { assert(_isValid); return _labelType==LABEL_FUNCTIONEXIT; }
   bool isBlockBeginLabel() { assert(_isValid); return _labelType==LABEL_BLOCKBEGIN; }
   bool isBlockEndLabel() { assert(_isValid); return _labelType==LABEL_BLOCKEND; }
   VariableId getIOVarId() { assert(_ioType!=LABELIO_NONE); return _variableId; }
 LabelProperty():_isValid(false),_node(0),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_variableId(0),_isTerminationRelevant(false),_isLTLRelevant(false) {
   }
 LabelProperty(SgNode* node):_isValid(false),_node(node),_labelType(LABEL_UNDEF),_ioType(LABELIO_NONE),_variableId(0),_isTerminationRelevant(false),_isLTLRelevant(false) {
	 initialize();
	 assert(_isValid);
   }
 LabelProperty(SgNode* node, LabelType labelType):_isValid(false),_node(node),_labelType(labelType),_ioType(LABELIO_NONE),_variableId(0),_isTerminationRelevant(false),_isLTLRelevant(false) {
	 initialize(); 
	 assert(_isValid);
   }
   void initialize() {
	 _isValid=true; // to be able to use access functions in initialization
	 SgVarRefExp* varRefExp=0;
	 _ioType=LABELIO_NONE;
	 if((varRefExp=SgNodeHelper::Pattern::matchSingleVarPrintf(_node))) {
	   _ioType=LABELIO_STDOUT;
	 } else if((varRefExp=SgNodeHelper::Pattern::matchSingleVarScanf(_node))) {
	   _ioType=LABELIO_STDIN;
	 } else if((varRefExp=SgNodeHelper::Pattern::matchSingleVarFPrintf(_node))) {
	   _ioType=LABELIO_STDERR;
	 }
	 if(varRefExp) {
	   SgSymbol* sym=SgNodeHelper::getSymbolOfVariable(varRefExp);
	   assert(sym);
	   _variableId=VariableId(sym);
	 }
	 _isTerminationRelevant=SgNodeHelper::isLoopCond(_node);
	 _isLTLRelevant=(isIOLabel()||isTerminationRelevant());
	 assert(varRefExp==0 ||_ioType!=LABELIO_NONE);
   }

   string toString() {
	 assert(_isValid);
	 stringstream ss;
	 ss<<_node<<":"<<SgNodeHelper::nodeToString(_node)<<", ";
	 ss<<"var:"<<_variableId.toString()<<", ";
	 ss<<"labelType:"<<_labelType<<", ";
	 ss<<"ioType:"<<_ioType<<", ";
	 ss<<"ltl:"<<isLTLRelevant()<<", ";
	 ss<<"termination:"<<isTerminationRelevant();
	 return ss.str();
   }
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
  bool isStdInLabel(Label label, VariableId* id=0);
  bool isStdOutLabel(Label label, VariableId* id=0);
  bool isStdErrLabel(Label label, VariableId* id=0);

 private:
  //vector<SgNode*> labelNodeMapping;
  vector<LabelProperty> labelNodeMapping;
};

} // end of namespace CodeThorn

#endif
