#ifndef LABELER_H
#define LABELER_H

#include <limits>
#include <set>
#include "RoseAst.h"
#include "VariableIdMapping.h"
#include "Label.h"

namespace CodeThorn {

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
  virtual std::string sourceLocationToString(Label lab, size_t maxFileNameLength, size_t maxSourceLength) =0;
  virtual size_t numberOfLabels() = 0;
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
  virtual bool isExprLabel(Label lab) = 0;
  virtual bool isExprOrDeclLabel(Label lab) = 0;

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

  // checks that id range is correct
  virtual bool isValidLabelIdRange(Label lab);

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

  int numberOfAssociatedLabels(SgNode* node) override;
  void createLabels(SgNode* node) override;
  void initialize(SgNode* node) override;

  /** Labels are numbered 0..n-1 where n is the number of labels
      associated with AST nodes (not all nodes are labeled, and some
      nodes are associated with more than one label).
     A return value of NO_LABEL means that this node has no label.
  */
  Label getLabel(SgNode* node) override;
  LabelSet getLabelSet(std::set<SgNode*>& nodeSet) override;

  /** Returns the node with the label 'label'. If the return value is 0 then no node exists for this label -
     this can only be the case if label is erroneously higher than the number of labeled nodes or NO_LABEL.
  */
  SgNode* getNode(Label label) override;
  std::string sourceLocationToString(Label lab, size_t maxFileNameLength, size_t maxSourceLength) override;
  size_t numberOfLabels() override;
  std::string toString() override;
  Label functionCallLabel(SgNode* node) override;
  Label functionCallReturnLabel(SgNode* node) override;
  Label functionEntryLabel(SgNode* node) override;
  Label functionExitLabel(SgNode* node) override;
  Label blockBeginLabel(SgNode* node) override;
  Label blockEndLabel(SgNode* node) override;
  Label joinLabel(SgNode *node) override;
  Label forkLabel(SgNode *node) override;
  Label workshareLabel(SgNode *node) override;
  Label barrierLabel(SgNode *node) override;

  // info obtained from LabelProperty
  bool isFunctionCallLabel(Label lab) override;
  bool isFunctionCallReturnLabel(Label lab) override;
  bool isFunctionEntryLabel(Label lab) override;
  bool isFunctionExitLabel(Label lab) override;
  bool isBlockBeginLabel(Label lab) override;
  bool isBlockEndLabel(Label lab) override;
  bool isEmptyStmtLabel(Label lab) override;
  bool isFirstLabelOfMultiLabeledNode(Label lab) override;
  bool isSecondLabelOfMultiLabeledNode(Label lab) override;

  // info obtained from AST node
  bool isForkLabel(Label lab) override;
  bool isJoinLabel(Label lab) override;
  bool isWorkshareLabel(Label lab) override;
  bool isBarrierLabel(Label lab) override;
  bool isConditionLabel(Label lab) override;
  bool isLoopConditionLabel(Label lab) override;
  bool isSwitchExprLabel(Label lab) override;
  bool isExprLabel(Label lab) override;
  bool isExprOrDeclLabel(Label lab) override;

  /** tests if @ref call and @ref ret are call and return labels of
   *  the same function call
   */
  bool areCallAndReturnLabels(Label call, Label ret) override;

  /** returns the call label for the provided return label. */
  Label getFunctionCallLabelFromReturnLabel(Label retnLabel) override;

  LabelProperty getProperty(Label lbl) override;

  // by default false for all labels. This must be set by the CF analysis.
  bool isExternalFunctionCallLabel(Label lab) override;
  void setExternalFunctionCallLabel(Label lab) override;
  Label getFunctionCallReturnLabelFromCallLabel(Label callLabel) override;

  Labeler::iterator begin() override;
  Labeler::iterator end() override;
  void setIsFunctionCallFn(std::function<bool(SgNode*)>); // non-virtual setter
 protected:
  virtual void computeNodeToLabelMapping();
  virtual void registerLabel(LabelProperty);
  virtual void ensureValidNodeToLabelMapping();

  // bool isFunctionCallNode(SgNode*) const override; repl. with function pointer

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

  void initialize(SgNode* n) override;

 protected:
  VariableIdMapping* _variableIdMapping;
};



} // end of namespace CodeThorn

// backward compatibility
namespace SPRAY = CodeThorn;

#endif
