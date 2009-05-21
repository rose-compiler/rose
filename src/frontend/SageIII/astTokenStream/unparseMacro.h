
namespace UnparseMacro {
  //The algorithm matchMacroToSubtress will return true if the macro correspond to a complete
  //subtree in the AST or a sequence of statements. That subtree or those statementsa are
  //returned in matchingSubtree
  bool matchMacroToSubtrees(SgNode* searchTree, PreprocessingInfo* macroCall, std::vector<SgNode*>& matchingSubtree);
  //Replace the unparsing of expanded macro calls with the actual macro call wherever possible
  void unparseMacroCalls(SgNode* searchTree);


};

