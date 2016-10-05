#if 0
compiling the code:
class SgNode;
class AstNodePtr {
  SgNode* repr;
 public:
  typedef SgNode PtrBaseClass;
  AstNodePtr( SgNode* n = 0)  {}
  AstNodePtr(  AstNodePtr& that) {}
  operator SgNode* () const { return repr; }
};

class AstInterface
{
 public:
  static AstNodePtr CreateFunctionCall( AstNodePtr& f);
  AstNodePtr CreateFunctionCall( );

};

AstNodePtr  CodeGen( AstInterface &_fa)
{
  AstNodePtr f;
  if(f!=0)
      return _fa.CreateFunctionCall( f);
}

gives the following error:

identityTranslator:
/home/saebjornsen1/ROSE-October/NEW_ROSE/src/frontend/EDG_SAGE_Connection/sage_gen_be.C:16405:
SgExpression* sage_gen_expr(an_expr_node*, int): Assertion "expressionFileInfo->ok()" failed.
#endif


class SgNode;
class AstNodePtr
   {
     SgNode* repr;
     public:
          typedef SgNode PtrBaseClass;
          AstNodePtr( SgNode* n = 0)  {}
          AstNodePtr(  AstNodePtr& that) {}
          operator SgNode* () const { return repr; }
   };

class AstInterface
   {
     public:
          static AstNodePtr CreateFunctionCall( AstNodePtr& f);
          AstNodePtr CreateFunctionCall( );
   };

AstNodePtr  CodeGen( AstInterface &_fa)
   {
     AstNodePtr f;
     if (f != 0)
        {
          return _fa.CreateFunctionCall(f);
        }

  // return AstNodePtr(0);
   }

