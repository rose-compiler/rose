#ifndef UNPARSER_JOVIAL
#define UNPARSER_JOVIAL

#include "unparser.h"

#include "Cxx_Grammar.h"
#include "unparseLanguageIndependentConstructs.h"


class Unparse_Jovial : public UnparseLanguageIndependentConstructs
   {
     public:
          Unparse_Jovial(Unparser* unp, std::string fname);

          virtual ~Unparse_Jovial();

          virtual std::string languageName() const { return "Jovial Unparser"; }

          void unparseJovialFile(SgSourceFile *file, SgUnparse_Info &info);

          virtual void unparseLanguageSpecificStatement  (SgStatement* stmt,  SgUnparse_Info& info);
          virtual void unparseLanguageSpecificExpression (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseStringVal              (SgExpression* expr, SgUnparse_Info& info);  

          virtual void unparseDefineDeclStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDirectiveStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseCompoolStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseProgHdrStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseProcDeclStmt           (SgStatement* stmt, SgUnparse_Info& info);
      //  virtual void unparseFuncDeclStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseFuncDefnStmt           (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseNamespaceDeclarationStatement(SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseNamespaceDefinitionStatement (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseBasicBlockStmt         (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseLabelStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseForStatement           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseJovialForThenStmt      (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseWhileStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseGotoStmt               (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseIfStmt                 (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseSwitchStmt             (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseCaseStmt               (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseDefaultStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseBreakStmt              (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTypeDefStmt            (SgStatement* stmt, SgUnparse_Info& info);


          virtual void unparseProcessControlStmt     (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseReturnStmt             (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseEnumDeclStmt           (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseOverlayDeclStmt        (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseTableDeclStmt          (SgStatement* stmt, SgUnparse_Info& info);

          virtual void unparseVarDeclStmt            (SgStatement* stmt, SgUnparse_Info& info);
          virtual void unparseVarDecl                (SgStatement* stmt, SgInitializedName* initializedName, SgUnparse_Info& info);

          virtual void unparseExprStmt               (SgStatement* stmt, SgUnparse_Info& info);

       // Types
       //
          virtual void unparseType (SgType* type,  SgUnparse_Info& info);

          template <class T>
          void unparseJovialType (T* type, SgUnparse_Info& info);

          void unparseJovialType (SgArrayType*       type, SgUnparse_Info& info);
          void unparseJovialType (SgEnumType*        type, SgUnparse_Info& info);
          void unparseJovialType (SgFunctionType*    type, SgUnparse_Info& info);
          void unparseJovialType (SgModifierType*    type, SgUnparse_Info& info);
          void unparseJovialType (SgJovialTableType* type, SgUnparse_Info& info);
          void unparseJovialType (SgPointerType*     type, SgUnparse_Info& info);
          void unparseJovialType (SgTypedefType*     type, SgUnparse_Info& info);

          void unparseTypeDesc (SgType* type, SgUnparse_Info& info);

          void unparseTypeSize (SgType* type,       SgUnparse_Info& info);
          void unparseTypeSize (SgTypeFixed* type,  SgUnparse_Info& info);
          void unparseTypeSize (SgTypeString* type, SgUnparse_Info& info);
          void unparseTypeSize (SgJovialBitType* type, SgUnparse_Info& info);

       // Expressions
          virtual void unparseUnaryOperator  (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseBinaryOperator (SgExpression* expr, const char* op, SgUnparse_Info& info);
          virtual void unparseCastExp        (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseSubscriptExpr     (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseArrayOp           (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseAsteriskShapeExpr (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseAssignOp       (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseFuncRef        (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseVarRef         (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparsePtrDeref       (SgExpression* expr, SgUnparse_Info& info);
          virtual void unparseTypeExpr       (SgExpression* expr, SgUnparse_Info& info);

          virtual void unparseFuncCall       (SgExpression* expr, SgUnparse_Info& info);

          void unparseOverlayExpr (SgExprListExp* overlay, SgUnparse_Info& info);

       // Initializers
          void unparseAssnInit      (SgExpression* expr, SgUnparse_Info& info);
          void unparseTablePreset   (SgExpression* expr, SgUnparse_Info& info);
          void unparseReplicationOp (SgExpression* expr, SgUnparse_Info& info);

       // Table dimension list
          void unparseDimInfo (SgExprListExp* dim_info, SgUnparse_Info& info);

     private:
          std::string ws_prefix(int nesting_level)
             {
                std::stringstream code;
                for (int i = 0; i < nesting_level; i++) {
                   code << "  ";
                }
                return code.str();
             }

          void curprint_indented(std::string txt, SgUnparse_Info& info)
             {
                curprint( ws_prefix(info.get_nestingLevel()) );
                curprint( txt );
             }

   };

#endif
