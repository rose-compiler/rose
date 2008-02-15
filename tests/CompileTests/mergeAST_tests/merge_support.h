void displaySet ( const std::set<SgNode*> & inputSet, const std::string & label );
std::string generateUniqueName ( SgNode* node, bool ignoreDifferenceBetweenDefiningAndNondefiningDeclarations );
std::set<SgNode*> generateNodeListFromAST ( SgNode* node );
std::set<SgNode*> generateNodeListFromMemoryPool();
std::set<SgNode*> getSetOfSharedNodes();

