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
  //Copy assignment operator
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
  bool isValid() const;
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

   bool isValid() const { return _isValid; }

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
  static Label NO_LABEL; // default initialized label (used to check for non-existing labels)

  static std::string labelToString(Label lab);

  Labeler() = default; // \todo do we need this constructor
  virtual ~Labeler() = default;

  /// call after object has been constructed to build the map w/ labels
  virtual void initialize(SgNode* node) = 0;
  virtual int numberOfAssociatedLabels(SgNode* node) = 0;
  virtual void createLabels(SgNode* node) = 0;

  /** Labels are numbered 0..n-1 where n is the number of labels
      associated with AST nodes (not all nodes are labeled, and some
      nodes are associated with more than one label).
     A return value of NO_LABEL means that this node has no label.
  */
  virtual Label getLabel(SgNode* node) = 0;
  virtual LabelSet getLabelSet(std::set<SgNode*>& nodeSet) = 0;

  /** Returns the node with the label 'label'. If the return value is 0 then no node exists for this label -
     this can only be the case if label is erroneously higher than the number of labeled nodes or NO_LABEL.
  */
  virtual SgNode* getNode(Label label) = 0;
  virtual long numberOfLabels() = 0;
  virtual std::string toString() = 0;
  virtual Label functionCallLabel(SgNode* node) = 0;
  virtual Label functionCallReturnLabel(SgNode* node) = 0;
  virtual Label functionEntryLabel(SgNode* node) = 0;
  virtual Label functionExitLabel(SgNode* node) = 0;
  virtual Label blockBeginLabel(SgNode* node) = 0;
  virtual Label blockEndLabel(SgNode* node) = 0;
  virtual Label joinLabel(SgNode *node) = 0;
  virtual Label forkLabel(SgNode *node) = 0;
  virtual Label workshareLabel(SgNode *node) = 0;
  virtual Label barrierLabel(SgNode *node) = 0;

  // info obtained from LabelProperty
  virtual bool isFunctionCallLabel(Label lab) = 0;
  virtual bool isFunctionCallReturnLabel(Label lab) = 0;
  virtual bool isFunctionEntryLabel(Label lab) = 0;
  virtual bool isFunctionExitLabel(Label lab) = 0;
  virtual bool isBlockBeginLabel(Label lab) = 0;
  virtual bool isBlockEndLabel(Label lab) = 0;
  virtual bool isEmptyStmtLabel(Label lab) = 0;
  virtual bool isFirstLabelOfMultiLabeledNode(Label lab) = 0;
  virtual bool isSecondLabelOfMultiLabeledNode(Label lab) = 0;

  // info obtained from AST node
  virtual bool isForkLabel(Label lab) = 0;
  virtual bool isJoinLabel(Label lab) = 0;
  virtual bool isWorkshareLabel(Label lab) = 0;
  virtual bool isBarrierLabel(Label lab) = 0;
  virtual bool isConditionLabel(Label lab) = 0;
  virtual bool isLoopConditionLabel(Label lab) = 0;
  virtual bool isSwitchExprLabel(Label lab) = 0;

  /** tests if @ref call and @ref ret are call and return labels of
   *  the same function call
   */
  virtual
  bool areCallAndReturnLabels(Label call, Label ret) = 0;

  /** returns the call label for the provided return label. */
  virtual
  Label getFunctionCallLabelFromReturnLabel(Label retnLabel) = 0;

  virtual LabelProperty getProperty(Label lbl) = 0;

  // by default false for all labels. This must be set by the CF analysis.
  virtual bool isExternalFunctionCallLabel(Label lab) = 0;
  virtual void setExternalFunctionCallLabel(Label lab) = 0;

  virtual
  Label getFunctionCallReturnLabelFromCallLabel(Label callLabel) = 0;

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

  virtual iterator begin() = 0;
  virtual iterator end() = 0;
 //~ protected:
  //~ virtual void computeNodeToLabelMapping() = 0;
  //~ virtual void registerLabel(LabelProperty) = 0;
  //~ virtual void ensureValidNodeToLabelMapping() = 0;
};

class CLabeler : public Labeler {
 public:
  CLabeler();
  //~ explicit CLabeler(SgNode*);
  ~CLabeler() = default;

  int numberOfAssociatedLabels(SgNode* node) ROSE_OVERRIDE;
  void createLabels(SgNode* node) ROSE_OVERRIDE;
  void initialize(SgNode* node) ROSE_OVERRIDE;

  /** Labels are numbered 0..n-1 where n is the number of labels
      associated with AST nodes (not all nodes are labeled, and some
      nodes are associated with more than one label).
     A return value of NO_LABEL means that this node has no label.
  */
  Label getLabel(SgNode* node) ROSE_OVERRIDE;
  LabelSet getLabelSet(std::set<SgNode*>& nodeSet) ROSE_OVERRIDE;

  /** Returns the node with the label 'label'. If the return value is 0 then no node exists for this label -
     this can only be the case if label is erroneously higher than the number of labeled nodes or NO_LABEL.
  */
  SgNode* getNode(Label label) ROSE_OVERRIDE;
  long numberOfLabels() ROSE_OVERRIDE;
  std::string toString() ROSE_OVERRIDE;
  Label functionCallLabel(SgNode* node) ROSE_OVERRIDE;
  Label functionCallReturnLabel(SgNode* node) ROSE_OVERRIDE;
  Label functionEntryLabel(SgNode* node) ROSE_OVERRIDE;
  Label functionExitLabel(SgNode* node) ROSE_OVERRIDE;
  Label blockBeginLabel(SgNode* node) ROSE_OVERRIDE;
  Label blockEndLabel(SgNode* node) ROSE_OVERRIDE;
  Label joinLabel(SgNode *node) ROSE_OVERRIDE;
  Label forkLabel(SgNode *node) ROSE_OVERRIDE;
  Label workshareLabel(SgNode *node) ROSE_OVERRIDE;
  Label barrierLabel(SgNode *node) ROSE_OVERRIDE;

  // info obtained from LabelProperty
  bool isFunctionCallLabel(Label lab) ROSE_OVERRIDE;
  bool isFunctionCallReturnLabel(Label lab) ROSE_OVERRIDE;
  bool isFunctionEntryLabel(Label lab) ROSE_OVERRIDE;
  bool isFunctionExitLabel(Label lab) ROSE_OVERRIDE;
  bool isBlockBeginLabel(Label lab) ROSE_OVERRIDE;
  bool isBlockEndLabel(Label lab) ROSE_OVERRIDE;
  bool isEmptyStmtLabel(Label lab) ROSE_OVERRIDE;
  bool isFirstLabelOfMultiLabeledNode(Label lab) ROSE_OVERRIDE;
  bool isSecondLabelOfMultiLabeledNode(Label lab) ROSE_OVERRIDE;

  // info obtained from AST node
  bool isForkLabel(Label lab) ROSE_OVERRIDE;
  bool isJoinLabel(Label lab) ROSE_OVERRIDE;
  bool isWorkshareLabel(Label lab) ROSE_OVERRIDE;
  bool isBarrierLabel(Label lab) ROSE_OVERRIDE;
  bool isConditionLabel(Label lab) ROSE_OVERRIDE;
  bool isLoopConditionLabel(Label lab) ROSE_OVERRIDE;
  bool isSwitchExprLabel(Label lab) ROSE_OVERRIDE;

  /** tests if @ref call and @ref ret are call and return labels of
   *  the same function call
   */
  bool areCallAndReturnLabels(Label call, Label ret) ROSE_OVERRIDE;

  /** returns the call label for the provided return label. */
  Label getFunctionCallLabelFromReturnLabel(Label retnLabel) ROSE_OVERRIDE;

  LabelProperty getProperty(Label lbl) ROSE_OVERRIDE;

  // by default false for all labels. This must be set by the CF analysis.
  bool isExternalFunctionCallLabel(Label lab) ROSE_OVERRIDE;
  void setExternalFunctionCallLabel(Label lab) ROSE_OVERRIDE;
  Label getFunctionCallReturnLabelFromCallLabel(Label callLabel) ROSE_OVERRIDE;

  Labeler::iterator begin() ROSE_OVERRIDE;
  Labeler::iterator end() ROSE_OVERRIDE;
  void setIsFunctionCallFn(std::function<bool(SgNode*)>); // non-virtual setter
 protected:
  virtual void computeNodeToLabelMapping();
  virtual void registerLabel(LabelProperty);
  virtual void ensureValidNodeToLabelMapping();

  // bool isFunctionCallNode(SgNode*) const ROSE_OVERRIDE; repl. with function pointer

  bool _isValidMappingNodeToLabel = false;
  typedef std::vector<LabelProperty> LabelToLabelPropertyMapping;
  LabelToLabelPropertyMapping mappingLabelToLabelProperty;
  typedef std::map<SgNode*,Label> NodeToLabelMapping;
  NodeToLabelMapping mappingNodeToLabel;

  std::function<bool(SgNode*)> isFunctionCallNode;
};


class IOLabeler : public CLabeler {
 public:
  //~ IOLabeler(SgNode* start, VariableIdMapping* variableIdMapping);
  explicit IOLabeler(VariableIdMapping* variableIdMapping);
  virtual bool isStdIOLabel(Label label);
  virtual bool isStdInLabel(Label label, VariableId* id=0);
  virtual bool isStdOutLabel(Label label);
  virtual bool isStdOutVarLabel(Label label, VariableId* id=0);
  virtual bool isStdOutConstLabel(Label label, int* constvalue=0);
  virtual bool isStdErrLabel(Label label, VariableId* id=0);
  virtual  ~IOLabeler();

  void initialize(SgNode* n) ROSE_OVERRIDE;

 protected:
  VariableIdMapping* _variableIdMapping;
};



} // end of namespace CodeThorn

// backward compatibility
namespace SPRAY = CodeThorn;

#endif
