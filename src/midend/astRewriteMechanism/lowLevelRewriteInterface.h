#ifndef __lowlevelrewrite
#define __lowlevelrewrite
// class AbsoluteNodeRewrite
class LowLevelRewrite
   {
  // Interface classification:
  //      Permits String Based Specification of Transformation: NO
  //      Permits Relative Specification of Target: NO
  //      Contains State information: NO

     friend class MidLevelInterfaceNodeCollection;
     friend class HighLevelInterfaceNodeCollection;

     private:
       // low level support for insert/replace/remove (each call replaceAppendPrependTreeFragment() below)
          static void insertAtTopOfScope ( 
                           const SgStatementPtrList & newTransformationStatementList, 
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           SgScopeStatement* currentScope );
          static void insertStatementUsingDependenceInformation ( 
                           SgScopeStatement* currentScope,
                           SgStatement* newTransformationStatement,
                           Rose_STL_Container<std::string> & typeNameStringList );
          static void insertAtBottomOfScope ( 
                           const SgStatementPtrList & newTransformationStatementList, 
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           SgScopeStatement* currentScope );

          static void insertBeforeCurrentStatement ( 
                           const SgStatementPtrList & currentStatementList, 
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           SgStatement* astNode );
          static void insertReplaceCurrentStatement ( 
                           const SgStatementPtrList & currentStatementList,
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           SgStatement* astNode );
          static void insertAfterCurrentStatement ( 
                           const SgStatementPtrList & currentStatementList, 
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           SgStatement* astNode );

       // Supporting function called by member functions above (handles insertion into 
       // declaration lists as appear on global scopes and statement lists as appear 
       // in basic blocks).
          static void replaceAppendPrependTreeFragment (
                           const SgStatementPtrList & newTransformationStatementList,
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           SgStatement* astNode,
                           bool removeOriginalStatement,
                           bool prependNewCode );

       // Supporing functions used in replaceAppendPrependTreeFragment()
          static void appendPrependTreeFragment (
                           SgStatement* newTransformationStatement,
                           SgStatement* astNode,
                           bool prependNewCode );

          static void removeStatement ( SgStatement* astNode );

       // Moves CPP and comments and reassociates them with other statements to 
       // avoid losing them when replacing/removing statements.  Reassociates
       // comments to preserve oder of statements and CPP directives during insertion.
       // Called by replaceAppendPrependTreeFragment() support function.
          static void reassociatePreprocessorDeclarations (
                           const SgStatementPtrList & newTransformationStatementList,
                           SgNode* astNode,
                           bool prependNewCode,
                           bool removeExistingNode );

     public:
       // Published interface for LowLevelRewrite Mechanism
          static void insert  ( SgStatement* targetStatement, SgStatement* newStatement, bool insertBeforeNode = true );
          static void replace ( SgStatement* targetStatement, SgStatement* newStatement );
          static void remove  ( SgStatement* targetStatement );

          static void insert  ( SgStatement* targetStatement, SgStatementPtrList newStatementList, bool insertBeforeNode = true );
          static void replace ( SgStatement* targetStatement, SgStatementPtrList newStatementList );
          static void remove  ( SgStatementPtrList targetStatementList );

          static bool isRemovableStatement ( SgStatement* s );

       // Note that markForOutputInCodeGeneration is a function in the AST Fixup mechanism
          static void markForOutputInCodeGenerationForRewrite ( SgStatement* newTransformationStatement );
   };




#endif










