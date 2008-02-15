class SgNode;

SgNode* isSgNode ( SgNode* inputDerivedClassPointer );

class SgNode
   {
     public: friend SgNode* isSgNode ( SgNode* inputDerivedClassPointer );
   };
