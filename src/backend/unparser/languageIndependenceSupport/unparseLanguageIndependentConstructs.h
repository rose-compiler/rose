 
/* unparser.h
 * This header file contains the class declaration for the newest unparser. Six
 * C files include this header file: unparser.C, modified_sage.C, unparse_stmt.C, 
 * unparse_expr.C, unparse_type.C, and unparse_sym.C.
 */

#ifndef UNPARSER_LANGUAGE_INDEPENDENT_SUPPORT
#define UNPARSER_LANGUAGE_INDEPENDENT_SUPPORT

#include "unparser.h"
#include "modified_sage.h"

#include <Rose/Diagnostics.h>

/* support for handling precedence and associativity */
typedef int PrecedenceSpecifier;
#define ROSE_UNPARSER_NO_PRECEDENCE -1
enum AssociativitySpecifier {
    e_assoc_none = 0,
    e_assoc_right,
    e_assoc_left,
    e_assoc_last
};

class Unparser;

// This is a base class for the language dependent parts of the unparser.
class UnparseLanguageIndependentConstructs
   {
     protected:
          Unparser* unp;
          std::string currentOutputFileName;

     public:
          static Sawyer::Message::Facility mlog;
          static void initDiagnostics();

       // DQ (12/6/2014): This type permits specification of what bounds to use in the specifiation of token stream subsequence boundaries.
          enum token_sequence_position_enum_type
             {
               e_leading_whitespace_start,
               e_leading_whitespace_end,
               e_token_subsequence_start,
               e_token_subsequence_end,
               e_trailing_whitespace_start,
               e_trailing_whitespace_end,
            // DQ (12/31/2014): Added to support the middle subsequence of tokens in the SgIfStmt as a special case.
               e_else_whitespace_start,
               e_else_whitespace_end
             };

       // DQ (6/6/2021): Adding the support to provide offsets to modify the starting and ending token sequence to unparse.
       // Single statement specification of token subsequence.
          void unparseStatementFromTokenStream (SgStatement* stmt, token_sequence_position_enum_type e_leading_whitespace_start, token_sequence_position_enum_type e_token_subsequence_start, 
                                                SgUnparse_Info& info, int start_offset = 0, int end_offset = 0 );

       // DQ (6/6/2021): Adding the support to provide offsets to modify the starting and ending token sequence to unparse.
       // Two statement specification of token subsequence (required for "else" case in SgIfStmt).
          void unparseStatementFromTokenStream (SgLocatedNode* stmt_1, SgLocatedNode* stmt_2, token_sequence_position_enum_type e_leading_whitespace_start, 
                                                token_sequence_position_enum_type e_token_subsequence_start, SgUnparse_Info& info, bool unparseOnlyWhitespace = false, 
                                                int start_offset = 0, int end_offset = 0 );

          std::string token_sequence_position_name( token_sequence_position_enum_type e );

          enum unparsed_as_enum_type
             {
                e_unparsed_as_error,
                e_unparsed_as_AST,
                e_unparsed_as_partial_token_sequence,
                e_unparsed_as_token_stream,
                e_unparsed_as_last
             };

          std::string unparsed_as_kind(unparsed_as_enum_type x);

          UnparseLanguageIndependentConstructs(Unparser* unp, std::string fname) : unp(unp)
             {
               currentOutputFileName = fname;
             };

          virtual ~UnparseLanguageIndependentConstructs() {};

       // Where all the language specific statement unparsing is done
          virtual void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info) = 0;
          virtual void unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info) = 0;

      //! Support for unparsing of line directives into generated code to support debugging
          virtual void unparseLineDirectives ( SgStatement* stmt );

          void unparseOneElemConInit(SgConstructorInitializer* con_init, SgUnparse_Info& info);

      //! get the filename from a SgStatement Object
          std::string getFileName(SgNode* stmt);
  
      //! get the filename from the SgFile Object
          std::string getFileName();

      //! used to support the run_unparser function
      //! (support for #line 42 "filename" when it appears in source code)
          bool statementFromFile ( SgStatement* stmt, std::string sourceFilename, SgUnparse_Info& info );

      //! Generate a CPP directive  
          void outputDirective ( PreprocessingInfo* directive );
  
      //! counts the number of statements in a basic block
          int num_stmt_in_block(SgBasicBlock*);

          std::string resBool(bool val) const;

       // DQ (7/1/2013): This needs to be defined in the header file, else the GNU 4.5 and 4.6 compilers will have undefined references at link time.
       // Note that the older GNU compilers have not had any problems with the previous version with the function definition in the *.C file.
          template<typename T>
          std::string tostring(T t) const
             {
               std::ostringstream myStream;                   // Creates an ostringstream object
               myStream << std::showpoint << t << std::flush; // Distinguish integer and floating-point numbers
               return myStream.str();                         // Returns the string form of the stringstream object
             }

          void curprint (const std::string & str) const;
          void printOutComments ( SgLocatedNode* locatedNode ) const;

      //! Unparser support for compiler-generated statments
          void outputCompilerGeneratedStatements( SgUnparse_Info & info );

      //! This function unparses any attached comments or CPP directives.
          virtual void unparseAttachedPreprocessingInfo(SgLocatedNode* stmt, SgUnparse_Info& info, PreprocessingInfo::RelativePositionType whereToUnparse);
          virtual bool unparseLineReplacement(SgLocatedNode* stmt, SgUnparse_Info& info);
  
          bool RemoveArgs(SgExpression* expr);

      //! Support for Fortran numeric labels (can appear on any statement), this is an empty function for C/C++.
          virtual void unparseStatementNumbers ( SgStatement* stmt, SgUnparse_Info& info );

          virtual std::string languageName() const = 0;

      //! unparse expression functions implemented in unparse_expr.C
          virtual void unparseExpression              (SgExpression* expr, SgUnparse_Info& info);

       // DQ (3/27/2017): Eliminate Clang warning about hidden virtual function (derived class functions 
       // now use the same signature (which eliminates the warning).
          virtual void unparseExprList                (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseMatrixExp               (SgExpression* expr, SgUnparse_Info& info);
          
          virtual void unparseUnaryExpr               (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseBinaryExpr              (SgExpression* expr, SgUnparse_Info& info);

       // DQ (11/10/2005): Added general support for SgValue (so that we could unparse 
       // expression trees from constant folding)
          virtual void unparseValue                   (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseBoolVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseShortVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseCharVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseSCharVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseUCharVal                (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseWCharVal                (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseChar16Val               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseChar32Val               (SgExpression* expr, SgUnparse_Info& info);  

       // Unparsing strings is language dependent.
          virtual void unparseStringVal               (SgExpression* expr, SgUnparse_Info& info) = 0;  
          virtual void unparseUShortVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseEnumVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseIntVal                  (SgExpression* expr, SgUnparse_Info& info);     
          virtual void unparseUIntVal                 (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongIntVal              (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongLongIntVal          (SgExpression* expr, SgUnparse_Info& info);    
          virtual void unparseULongLongIntVal         (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseULongIntVal             (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseFloatVal                (SgExpression* expr, SgUnparse_Info& info); 
          virtual void unparseDoubleVal               (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseLongDoubleVal           (SgExpression* expr, SgUnparse_Info& info);  
          virtual void unparseBFloat16Val             (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseFloat16Val              (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseFloat32Val              (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseFloat64Val              (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseComplexVal              (SgExpression* expr, SgUnparse_Info& info);

       // DQ (7/31/2014): Adding support for C++11 nullptr const value expressions.
          virtual void unparseNullptrVal              (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseNullExpression          (SgExpression* expr, SgUnparse_Info& info);

      //! unparse statement functions implemented in unparse_stmt.C
          virtual void unparseStatement        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseGlobalStmt       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncTblStmt      (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseNullStatement    (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseIncludeDirectiveStatement     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDefineDirectiveStatement      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseUndefDirectiveStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfdefDirectiveStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfndefDirectiveStatement      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDeadIfDirectiveStatement      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfDirectiveStatement          (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseElseDirectiveStatement        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseElseifDirectiveStatement      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseEndifDirectiveStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLineDirectiveStatement        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWarningDirectiveStatement     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseErrorDirectiveStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseEmptyDirectiveStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIdentDirectiveStatement       (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIncludeNextDirectiveStatement (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLinemarkerDirectiveStatement  (SgStatement* stmt, SgUnparse_Info& info);

         // Liao 10/20/2010 common unparsing support for OpenMP AST 
          virtual void unparseOmpPrefix                     (SgUnparse_Info& info); // = 0;
          virtual void unparseOmpDefaultClause              (SgOmpClause* clause, SgUnparse_Info& info); 
          virtual void unparseOmpProcBindClause             (SgOmpClause* clause, SgUnparse_Info& info); 
          virtual void unparseOmpAtomicClause               (SgOmpClause* clause, SgUnparse_Info& info); 
          virtual void unparseOmpScheduleClause             (SgOmpClause* clause, SgUnparse_Info& info);
          virtual void unparseOmpVariablesClause            (SgOmpClause* clause, SgUnparse_Info& info);
          virtual void unparseOmpExpressionClause           (SgOmpClause* clause, SgUnparse_Info& info);
          virtual void unparseOmpClause                     (SgOmpClause* clause, SgUnparse_Info& info);

          virtual void unparseOmpSimpleStatement             (SgStatement* stmt,     SgUnparse_Info& info);
          virtual void unparseOmpThreadprivateStatement      (SgStatement* stmt,     SgUnparse_Info& info);
          virtual void unparseOmpFlushStatement              (SgStatement* stmt,     SgUnparse_Info& info);
          virtual void unparseOmpDeclareSimdStatement        (SgStatement* stmt,     SgUnparse_Info& info);
        
          // This is necessary since some clauses should only appear with the begin part of a directive
          virtual void unparseOmpDirectivePrefixAndName     (SgStatement* stmt,     SgUnparse_Info& info);
          virtual void unparseOmpEndDirectivePrefixAndName  (SgStatement* stmt,     SgUnparse_Info& info);
          virtual void unparseOmpBeginDirectiveClauses      (SgStatement* stmt,     SgUnparse_Info& info); 
          virtual void unparseOmpEndDirectiveClauses        (SgStatement* stmt,     SgUnparse_Info& info); 
          virtual void unparseOmpGenericStatement           (SgStatement* stmt,     SgUnparse_Info& info);

          virtual void unparseMapDistDataPoliciesToString (std::vector< std::pair< SgOmpClause::omp_map_dist_data_enum, SgExpression * > > policies, SgUnparse_Info& info);

          bool isTransformed(SgStatement* stmt);
          void markGeneratedFile() const;

       // DQ (10/14/2004): Supporting function shared by unparseClassDecl and unparseClassType
          void initializeDeclarationsFromParent ( SgDeclarationStatement* declarationStatement,
                                                  SgClassDefinition* & cdefn,
                                                  SgNamespaceDefinitionStatement* & namespaceDefn, int debugSupport = 0 );

       // Support for language-independent precedence
          virtual bool requiresParentheses(SgExpression* expr, SgUnparse_Info& info);
          virtual PrecedenceSpecifier getPrecedence(SgExpression* exp);
          virtual AssociativitySpecifier getAssociativity(SgExpression* exp);

       // DQ (4/14/2013): Added to support the mixed use of both overloaded operator names and operator syntax.
          bool isRequiredOperator( SgBinaryOp* binary_op, bool current_function_call_uses_operator_syntax, bool parent_function_call_uses_operator_syntax );

       // DQ (10/29/2013): Adding support to unparse statements using the token stream.
       // int unparseStatementFromTokenStream(SgSourceFile* sourceFile, SgStatement* stmt);

       // DQ (6/6/2021): I think this is not needed (was modified to be the version of the function below).
          int unparseStatementFromTokenStream(SgSourceFile* sourceFile, SgStatement* stmt, SgUnparse_Info & info, bool & lastStatementOfGlobalScopeUnparsedUsingTokenStream);

       // DQ (11/4/2014): Unparse a partial sequence of tokens up to the next AST node.
          int unparseStatementFromTokenStreamForNodeContainingTransformation(SgSourceFile* sourceFile, SgStatement* stmt, SgUnparse_Info & info, bool & lastStatementOfGlobalScopeUnparsedUsingTokenStream, unparsed_as_enum_type unparsed_as);

          bool canBeUnparsedFromTokenStream(SgSourceFile* sourceFile, SgStatement* stmt);

       // DQ (1/6/2021): Adding support to detect use of unparseToString() functionality.  This is required to avoid premature saving of state
       // regarding the static previouslyUnparsedTokenSubsequences which is required to support multiple statements (e.g. a variable declarations 
       // with containing multiple variables which translates (typically) to multiple variable declarations (each with one variable) within the AST).
       // DQ (11/29/2013): Added support to detect redundant statements (e.g. variable declarations 
       // with multiple variables that are mapped to a single token sequence).
       // bool redundantStatementMappingToTokenSequence(SgSourceFile* sourceFile, SgStatement* stmt);
          bool redundantStatementMappingToTokenSequence(SgSourceFile* sourceFile, SgStatement* stmt, SgUnparse_Info & info);

       // DQ (11/30/2013): Adding support to suppress redundant unparsing of CPP directives and comments.
       // bool isTransitionFromTokenUnparsingToASTunparsing(SgStatement* statement);

      // DQ (1/23/2014): This function support detecting when the supress the output of the SgDotExp
      // in the access of data members from un-named unions.
         bool isDotExprWithAnonymousUnion(SgExpression* expr);

      // DQ (9/3/2014): Adding support to supress output of SgThisExp as part of support for C++11 lambda functions code generation.
         bool isImplicitArrowExpWithinLambdaFunction(SgExpression* expr, SgUnparse_Info& info);

      // DQ (11/13/2014): Detect when to unparse the leading and trailing edge tokens for attached CPP directives and comments.
         bool unparseAttachedPreprocessingInfoUsingTokenStream(SgLocatedNode* stmt, SgUnparse_Info& info, PreprocessingInfo::RelativePositionType whereToUnparse);

         friend void unparse_alignas(SgInitializedName * decl_item, class Unparse_ExprStmt & unparser, SgUnparse_Info & info);
   };

#endif
