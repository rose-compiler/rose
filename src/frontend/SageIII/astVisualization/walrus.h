namespace WalrusGraph
   {
  // This class (AST traversal) supports the traversal of the AST required 
  // to translate the source position using the CPP linemarkers.

     int counter = 0;
     std::map<SgNode*,int> node_to_index_map;

     std::ostream* outputFilePtr = NULL;

  // Flag to represent binary
     bool isBinary = false;

     class Counter : public AstSimpleProcessing
        {
          public:
              void visit ( SgNode* astNode );
        };

     class OutputEdges : public AstSimpleProcessing
        {
          public:
              OutputEdges() {};

              void visit ( SgNode* astNode );
        };

     class OutputNodes : public AstSimpleProcessing
        {
          public:
              OutputNodes() {};

              void visit ( SgNode* astNode );
        };

     void generateEdges( SgNode* node );
     void generateNodes( SgNode* node );

     void generateWalrusGraph( SgNode* node, std::string outputFilename );
   }

