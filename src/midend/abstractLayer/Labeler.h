#ifndef LABELER_H
#define LABELER_H

/*************************************************************
 * Author   : Markus Schordan                                *
 *************************************************************/

#include <limits>
#include <set>
#include "RoseAst.h"
#include "VariableIdMapping.h"

#define NO_STATE -3
#define NO_ESTATE -4
#define NO_LABEL_ID std::numeric_limits<size_t>::max()

namespace CodeThorn {

/*!
  * \author Markus Schordan
  * \date 2012, 2014.
 */
class Label {
 public:
  Label();
  Label(size_t labelId);
  //Copy constructor
  Label(const Label& other);
  //Copy assignemnt operator
  Label& operator=(const Label& other);
  bool operator<(const Label& other) const;
  bool operator==(const Label& other) const;
  bool operator!=(const Label& other) const;
  bool operator>(const Label& other) const;
  bool operator>=(const Label& other) const;
  Label& operator+(int num);
  // prefix inc operator
  Label& operator++();
  // postfix inc operator
  Label operator++(int);
  size_t getId() const;
  std::string toString() const;
  friend std::ostream& operator<<(std::ostream& os, const Label& label);

 protected:
  size_t _labelId;
};

std::ostream& operator<<(std::ostream& os, const Label& label);

/*!
  * \author Markus Schordan
  * \date 2012.
 */

// internal data structure (not used in Labeler's interface)
class LabelProperty {
 public:
   enum LabelType { LABEL_UNDEF=1, LABEL_OTHER=2,
                    LABEL_FUNCTIONCALL=100, LABEL_FUNCTIONCALLRETURN,
                    LABEL_FUNCTIONENTRY, LABEL_FUNCTIONEXIT,
                    LABEL_BLOCKBEGIN, LABEL_BLOCKEND,
                    LABEL_EMPTY_STMT,
                    // Labels for OpenMP parallel constructs
                    LABEL_FORK, LABEL_JOIN, LABEL_WORKSHARE, LABEL_BARRIER
   };
   std::string labelTypeToString(LabelType lt);

   LabelProperty();
   LabelProperty(SgNode* node);
   LabelProperty(SgNode* node, LabelType labelType);
   LabelProperty(SgNode* node, VariableIdMapping* variableIdMapping);
   LabelProperty(SgNode* node, LabelType labelType, VariableIdMapping* variableIdMapping);
   std::string toString();
   SgNode* getNode();
   bool isFunctionCallLabel();
   bool isFunctionCallReturnLabel();
   bool isFunctionEntryLabel();
   bool isFunctionExitLabel();
   bool isBlockBeginLabel();
   bool isBlockEndLabel();
   bool isEmptyStmtLabel();
   // OpenMP related query functions
   bool isForkLabel();
   bool isJoinLabel();
   bool isWorkshareLabel();
   bool isBarrierLabel();

 public:
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

   bool isExternalFunctionCallLabel();
   void setExternalFunctionCallLabel();

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
   bool _isExternalFunctionCallLabel;
};

/*!
  * \author Markus Schordan
  * \date 2012.
 */
class LabelSet : public std::set<Label> {
 public:
  LabelSet operator+(LabelSet& s2);
  LabelSet& operator+=(LabelSet& s2);
  LabelSet operator-(LabelSet& s2);
  LabelSet& operator-=(LabelSet& s2);

  std::string toString();
  bool isElement(Label lab);
};

typedef std::set<LabelSet> LabelSetSet;

/*!
  * \author Markus Schordan
  * \date 2012, 2013.
 */
class Labeler {
 public:
  Labeler();
  static Label NO_LABEL; // default initialized label (used to check for non-existing labels)
  Labeler(SgNode* start);
  static std::string labelToString(Label lab);
  int numberOfAssociatedLabels(SgNode* node);
  virtual void createLabels(SgNode* node);

  /** Labels are numbered 0..n-1 where n is the number of labels
      associated with AST nodes (not all nodes are labeled, and some
      nodes are associated with more than one label).
     A return value of NO_LABEL means that this node has no label.
  */
  Label getLabel(SgNode* node);
  LabelSet getLabelSet(std::set<SgNode*>& nodeSet);

  /** Returns the node with the label 'label'. If the return value is 0 then no node exists for this label -
     this can only be the case if label is erroneously higher than the number of labeled nodes or NO_LABEL.
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
  Label joinLabel(SgNode *node);
  Label forkLabel(SgNode *node);
  Label workshareLabel(SgNode *node);
  Label barrierLabel(SgNode *node);
  bool isFunctionEntryLabel(Label lab);
  bool isFunctionExitLabel(Label lab);
  bool isEmptyStmtLabel(Label lab);
  bool isBlockBeginLabel(Label lab);
  bool isBlockEndLabel(Label lab);
  bool isFunctionCallLabel(Label lab);
  bool isFunctionCallReturnLabel(Label lab);
  bool isConditionLabel(Label lab);
  bool isSwitchExprLabel(Label lab);
  bool isFirstLabelOfMultiLabeledNode(Label lab);
  bool isSecondLabelOfMultiLabeledNode(Label lab);
  bool isForkLabel(Label lab);
  bool isJoinLabel(Label lab);
  bool isWorkshareLabel(Label lab);
  bool isBarrierLabel(Label lab);
  
  /** tests if @ref call and @ref ret are call and return labels of 
   *  the same function call
   */ 
  virtual 
  bool areCallAndReturnLabels(Label call, Label ret);

  /** returns the call label for the provided return label. */
  virtual 
  Label getCallForReturnLabel(Label ret);
  
  virtual
  LabelProperty getProperty(Label lbl); 

#if 1
  // by default false for all labels. This must be set by the CF analysis.
  bool isExternalFunctionCallLabel(Label lab);
  void setExternalFunctionCallLabel(Label lab);
#endif

  class iterator {
  public:
    iterator();
    iterator(Label start, size_t numLabels);
    bool operator==(const iterator& x) const;
    bool operator!=(const iterator& x) const;
    Label operator*() const;
    iterator& operator++(); // prefix
    iterator operator++(int); // postfix
  private:
    bool is_past_the_end() const;
    Label _currentLabel;
    size_t _numLabels;
  };
  iterator begin();
  iterator end();
  virtual ~Labeler();
 protected:
  void computeNodeToLabelMapping();
  void registerLabel(LabelProperty);
  typedef std::vector<LabelProperty> LabelToLabelPropertyMapping;
  LabelToLabelPropertyMapping mappingLabelToLabelProperty;
  typedef std::map<SgNode*,Label> NodeToLabelMapping;
  NodeToLabelMapping mappingNodeToLabel;
  bool _isValidMappingNodeToLabel;
  void ensureValidNodeToLabelMapping();
};

class IOLabeler : public Labeler {
 public:
  IOLabeler(SgNode* start, VariableIdMapping* variableIdMapping);
  virtual bool isStdIOLabel(Label label);
  virtual bool isStdInLabel(Label label, VariableId* id=0);
  virtual bool isStdOutLabel(Label label);
  virtual bool isStdOutVarLabel(Label label, VariableId* id=0);
  virtual bool isStdOutConstLabel(Label label, int* constvalue=0);
  virtual bool isStdErrLabel(Label label, VariableId* id=0);
  virtual  ~IOLabeler();

 protected:
  VariableIdMapping* _variableIdMapping;
};


} // end of namespace CodeThorn

// backward compatibility
namespace SPRAY = CodeThorn;

#endif
