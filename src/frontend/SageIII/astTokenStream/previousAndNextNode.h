#ifndef PREVIOUS_AND_NEXT_NODE_HEADER
#define PREVIOUS_AND_NEXT_NODE_HEADER

class PreviousAndNextNodeData
   {
     public:
          SgNode* previous;
          SgNode* next;

          PreviousAndNextNodeData(SgNode* previous, SgNode* next);
   };

#if 0
class PreviousAndNextNodeTraversal : public AstPrePostProcessing
   {
     public:
       // PreviousAndNextNodeTraversal( SgFile* file);
          PreviousAndNextNodeTraversal();

       // virtual function must be defined
          void preOrderVisit(SgNode *astNode);

      //! this method is called at every traversed node after its children were traversed
          void postOrderVisit(SgNode *astNode);

      //! functions called when the traversal starts and ends, respectively
          void atTraversalStart();
          void atTraversalEnd();

          std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeMap;

#if 0
       // virtual function must be defined
          FrontierDetectionForTokenStreamMapping_InheritedAttribute evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute);

       // virtual function must be defined
          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
#endif
   };


std::map<SgNode*,PreviousAndNextNodeData*> previousAndNextNodeTraversal ( SgFile* file );
#endif

class PreviousAndNextAttribute : public AstAttribute
   {
  // This class supports marking the AST in the normal ROSE AST graph generation.
  // We use this ROSE feature to mark the previous, and next nodes in the frontier.

     private:
          SgNode* from;
          SgNode* to;
          std::string name;
          std::string options;

     public:

       // PreviousAndNextAttribute(SgNode* n, std::string name, std::string options);
          PreviousAndNextAttribute(SgNode* from, SgNode* to, std::string name, std::string options);

          PreviousAndNextAttribute(const PreviousAndNextAttribute & X);

       // Support for graphics output of IR nodes using attributes (see the DOT graph of the AST)
          virtual std::string additionalNodeOptions() override;
          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo() override;
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo() override;

       // Support for the coping of AST and associated attributes on each IR node (required for attributes
       // derived from AstAttribute, else just the base class AstAttribute will be copied).
          virtual AstAttribute* copy() const override;

       // DQ (6/11/2017): Added virtual function now required to eliminate warning at runtime.
          virtual AstAttribute::OwnershipPolicy getOwnershipPolicy() const override; // { return CONTAINER_OWNERSHIP; }
   };


std::map<SgNode*,PreviousAndNextNodeData*> computePreviousAndNextNodes(SgGlobal* globalScope, std::vector<FrontierNode*> frontierNodes);

#endif

