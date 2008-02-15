class NullTreeMemoryPoolTraversal 
   : public ROSE_VisitTraversal
   {
     public:
      //! Required traversal function
          void visit (SgNode* node);

          virtual ~NullTreeMemoryPoolTraversal() {};
   };

void NullTree();
