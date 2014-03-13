#ifndef	OFP_NODES_H
#define	OFP_NODES_H

#include <aterm2.h>
#include <string>
#include <vector>

namespace OFP {

  class String;

  class EquivOp;
  class OrOp;
  class AndOp;
  class NotOp;
  class RelOp;
  class ConcatOp;
  class AddOp;
  class MultOp;
  class PowerOp;

//--------------------------------------------------------------
// TODO - use unique_ptr when available (after at least gcc 4.4)
// typedef std::vector<std::unique_ptr<base>> container;
//

class Node
{
 public:
   Node() optionType(0) {}
   virtual ~Node()      {}

   enum OptionType
     {
        DEFAULT = 0
     };

   int  getOptionType()       {return optionType;}
   void setOptionType(int ot) {optionType = ot;}

   ATerm term;
   int   optionType;
};

/* Terminals
 */
class Terminal : public Node
{
 public:
   Terminal() : pValue(NULL)                 {}
   Terminal(std::string * val) : pValue(val) {}
   virtual ~Terminal()                       {if (pValue) delete pValue;}

   std::string * getValue()           {return pValue;}
   void          setValue(char * str) {pValue = new std::string(str);}

 protected:
   std::string * pValue;
};

class Dop : public Terminal
{
 public:
   Dop() : Terminal() {}
   Dop(std::string * val) : Terminal(val) {}

   Dop * newDop()
     {
        Dop * node = new Dop(pValue);
        pValue = NULL;
        return node;
     }
};

class HexConstant : public Terminal
{
 public:
   HexConstant() : Terminal() {}
   HexConstant(std::string * val) : Terminal(val) {}

   HexConstant * newHexConstant()
     {
        HexConstant * node = new HexConstant(pValue);
        pValue = NULL;
        return node;
     }
};

class OctalConstant : public Terminal
{
 public:
   OctalConstant() : Terminal() {}
   OctalConstant(std::string * val) : Terminal(val) {}

   OctalConstant * newOctalConstant()
     {
        OctalConstant * node = new OctalConstant(pValue);
        pValue = NULL;
        return node;
     }
};

class BinaryConstant : public Terminal
{
 public:
   BinaryConstant() : Terminal() {}
   BinaryConstant(std::string * val) : Terminal(val) {}

   BinaryConstant * newBinaryConstant()
     {
        BinaryConstant * node = new BinaryConstant(pValue);
        pValue = NULL;
        return node;
     }
};

class Rcon : public Terminal
{
 public:
   Rcon() : Terminal() {}
   Rcon(std::string * val) : Terminal(val) {}

   Rcon * newRcon()
     {
        Rcon * node = new Rcon(pValue);
        pValue = NULL;
        return node;
     }
};

class Scon : public Terminal
{
 public:
   Scon() : Terminal() {}
   Scon(std::string * val) : Terminal(val) {}

   Scon * newScon()
     {
        Scon * node = new Scon(pValue);
        pValue = NULL;
        return node;
     }
};

class Icon : public Terminal
{
 public:
   Icon() : Terminal() {}
   Icon(std::string * val) : Terminal(val) {}

   Icon * newIcon()
     {
        Icon * node = new Icon(pValue);
        pValue = NULL;
        return node;
     }
};

class Ident : public Terminal
{
 public:
   Ident() : Terminal() {}
   Ident(std::string * val) : Terminal(val) {}

   Ident * newIdent()
     {
        Ident * node = new Ident(pValue);
        pValue = NULL;
        return node;
     }
};

class Letter : public Terminal
{
 public:
   Letter() : Terminal() {}
   Letter(std::string * val) : Terminal(val) {}

   Letter * newLetter()
     {
        Letter * node = new Letter(pValue);
        pValue = NULL;
        return node;
     }
};

class Label : public Terminal
{
 public:
   Label() : Terminal() {}
   Label(std::string * val) : Terminal(val) {}

   Label * newLabel()
     {
        Label * node = new Label(pValue);
        pValue = NULL;
        return node;
     }
};

class LblRef : public Terminal
{
 public:
   LblRef() : Terminal() {}
   LblRef(std::string * val) : Terminal(val) {}

   LblRef * newLblRef()
     {
        LblRef * node = new LblRef(pValue);
        pValue = NULL;
        return node;
     }
};

class StartCommentBlock : public Terminal
{
 public:
   StartCommentBlock() : Terminal() {}
   StartCommentBlock(std::string * val) : Terminal(val) {}

   StartCommentBlock * newStartCommentBlock()
     {
        StartCommentBlock * node = new StartCommentBlock(pValue);
        pValue = NULL;
        return node;
     }
};

class EOS : public Terminal
{
 public:
   EOS() : Terminal() {}
   EOS(std::string * val) : Terminal(val) {}

   EOS * newEOS()
     {
        EOS * node = new EOS(pValue);
        pValue = NULL;
        return node;
     }
};

/* Statements
 */
class Statement : public Node
{
 public:
    Statement() : pLabel(NULL), pEOS(NULL) {}
    Statement(Label * label, EOS * eos) : pLabel(label), pEOS(eos) {}
   ~Statement()
      {
         if (pLabel) delete pLabel;
         if (pEOS)   delete pEOS;
      }

   Statement * newStatement()
     {
        Statement* node = new Statement();
        node->pLabel = pLabel;  pLabel = NULL;
        node->pEOS   = pEOS;    pEOS   = NULL;
        return node;
     }

   Label * getLabel()              {return pLabel;}
   void    setLabel(Label * label) {pLabel = label;}

   EOS *   getEOS()                {return pEOS;}
   void    setEOS(EOS * eos)       {pEOS = eos;}

 protected:
   void setStatementValues(Statement * node)
          {
             node->pLabel = pLabel;  pLabel = NULL;
             node->pEOS   = pEOS;    pEOS   = NULL;
          }

 protected:
   Label * pLabel;
   EOS   * pEOS;
};

#ifdef OBSOLETE
class Name : public Node
{
 public:
    Name() : pIdent(NULL) {}
   ~Name();

   Name * newName()
     {
        Name * node = new Name();
        node->pIdent = pIdent;  pIdent = NULL;
        return node;
     }

   Ident *   getIdent() {return pIdent;}
   void      setIdent(Ident * ident) {pIdent = ident;}

 private:
   Ident * pIdent;
};
#endif

#ifdef NEEDS_THOUGHT
class Variable : public Node
{
 public:
    Variable() : pValue(NULL) {}
   ~Variable();

   Variable* newVariable()
     {
        Variable* node = newVariable();
        node->pValue = pValue;  pValue = NULL;
        return node;
     }

   std::string * getValue() {return pValue;}
   void          setValue(char * str) {pValue = new std::string(str);}

 private:
   std::string * pValue;
};
#endif

#ifdef OBSOLETE
class Expr : public Node
{
 public:
    Expr() : pExpr(NULL) {}
   ~Expr();

   Expr* newExpr()
     {
        Expr* node = newExpr();
        node->pExpr = pExpr;  pExpr = NULL;
        return node;
     }

   std::string * getValue() {return pExpr;}
   void          setValue(char * str) {pExpr = new std::string(str);}

 private:
   std::string * pExpr;
};
#endif

#ifdef OBSOLETE
class AssignmentStmt : public Statement
{
 public:
    AssignmentStmt() : Statement(), pVariable(NULL), pExpr(NULL) {}
   ~AssignmentStmt();

   Variable* getVariable() {return pVariable;}
   void setVariable(Variable* variable) {pVariable = variable;}

   Expr* getExpr() {return pExpr;}
   void setExpr(Expr* expr) {pExpr = expr;}

 private:
   Variable* pVariable;
   Expr* pExpr;
};
#endif

#ifdef OBSOLETE
class MainProgram : public Node
{
 public:
    MainProgram()
      {
         pProgramStmt = NULL;
         pEndProgramStmt = NULL;
         pSpecificationPart = NULL;
         pExecutionPart = NULL;
         pInternalSubprogramPart = NULL;
      }
   ~MainProgram();

   // Constructs a new Node on the heap from one on the stack (that is about to be reclaimed).
   MainProgram* newMainProgram()
      {
         MainProgram* node = new MainProgram();
         node->pProgramStmt = pProgramStmt; pProgramStmt = NULL;
         node->pEndProgramStmt = pEndProgramStmt; pEndProgramStmt = NULL;
         node->pSpecificationPart = pSpecificationPart; pSpecificationPart = NULL;
         node->pExecutionPart = pExecutionPart; pExecutionPart = NULL;
         node->pInternalSubprogramPart = pInternalSubprogramPart; pInternalSubprogramPart = NULL;
         return node;
      }

   ProgramStmt* getProgramStmt() {return pProgramStmt;}
   EndProgramStmt* getEndProgramStmt() {return pEndProgramStmt;}
   SpecificationPart* getSpecificationPart() {return pSpecificationPart;}
   ExecutionPart* getExecutionPart() {return pExecutionPart;}
   InternalSubprogramPart* getInternalSubprogramPart() {return pInternalSubprogramPart;}

   void setProgramStmt(ProgramStmt* programstmt) {pProgramStmt = programstmt;}
   void setEndProgramStmt(EndProgramStmt* endprogramstmt) {pEndProgramStmt = endprogramstmt;}
   void setSpecificationPart(SpecificationPart* specificationpart) {pSpecificationPart = specificationpart;}
   void setExecutionPart(ExecutionPart* executionpart) {pExecutionPart = executionpart;}
   void setInternalSubprogramPart(InternalSubprogramPart* internalsubprogrampart) {pInternalSubprogramPart = internalsubprogrampart;}

 private:
   ProgramStmt* pProgramStmt;
   EndProgramStmt* pEndProgramStmt;
   SpecificationPart* pSpecificationPart;
   ExecutionPart* pExecutionPart;
   InternalSubprogramPart* pInternalSubprogramPart;
};
#endif

#ifdef OBSOLETE
class ProgramStmt : public Statement
{
 public:
   ProgramStmt() : Statement(), pProgramName(NULL) {}
  ~ProgramStmt();

   ProgramStmt* newProgramStmt()
     {
        ProgramStmt * node = new ProgramStmt();
        setStatementValues(node);
        node->pProgramName = pProgramName;  pProgramName = NULL;
        return node;
     }

   Name* getProgramName() {return pProgramName;}
   void setProgramName(Name* programname) {pProgramName = programname;}

 private:
   Name* pProgramName;
};
#endif

#ifdef OBSOLETE
class EndProgramStmt : public Statement
{
 public:
    EndProgramStmt() : Statement(), pProgramName(NULL) {}
   ~EndProgramStmt();

   EndProgramStmt* newEndProgramStmt()
     {
        EndProgramStmt* node = new EndProgramStmt();
        setStatementValues(node);
        node->pProgramName = pProgramName;  pProgramName = NULL;
        return node;
     }

   Name* getProgramName() {return pProgramName;}
   void setProgramName(Name* programname) {pProgramName = programname;}

 private:
   Name* pProgramName;
};
#endif

#ifdef NEEDS_GEN_LIST
class SpecificationPart : public Node
{
 public:
   SpecificationPart() {pList = new std::vector<Node*>();}
  ~SpecificationPart();

   SpecificationPart* newSpecificationPart()
     {
        SpecificationPart* node = new SpecificationPart();
        node->pList = pList;  pList = NULL;
        return node;
     }

   void appendDeclarationConstruct(DeclarationConstruct * declarationconstruct);

 private:
   std::vector<Node*>* pList;
};
#endif

#ifdef NO_STMT_INHERIT
class ImplicitPartStmt : public Statement
{
 public:
 ImplicitPartStmt() : Statement(), pStmt(NULL) {}
   ~ImplicitPartStmt();

   ImplicitPartStmt * newImplicitPartStmt()
     {
        ImplicitPartStmt* node = new ImplicitPartStmt();
        node->pStmt = pStmt;  pStmt = NULL;
        return node;
     }

   void setImplicitStmt(Statement* implicitstmt)  {pStmt = implicitstmt;}

 private:
   Statement* pStmt;
};
#endif

#ifdef OBSOLETE
class DeclarationConstruct : public Node
{
 public:
   DeclarationConstruct() : pNode(NULL) {}
  ~DeclarationConstruct();

   DeclarationConstruct * newDeclarationConstruct()
     {
        DeclarationConstruct* node = new DeclarationConstruct();
        node->pNode = pNode;  pNode = NULL;
        return node;
     }

   void setTypeDeclarationStmt(Node* typedeclarationstmt)  {pNode = typedeclarationstmt;}

 private:
   Node* pNode;
};
#endif

#ifdef NEEDS_GEN_LIST
class ImplicitPart : public Node
{
 public:
    ImplicitPart() {pList = new std::vector<Statement*>();}
   ~ImplicitPart();

   void appendImplicitPartStmt(ImplicitPartStmt* implicitpartstmt);
   void setImplicitStmt(ImplicitStmt* implicitstmt);

 private:
   std::vector<Statement*>* pList;
};
#endif

#ifdef OBSOLETE
class TypeDeclarationStmt : public Statement
{
 public:
   TypeDeclarationStmt() : Statement() {}
  ~TypeDeclarationStmt();

   TypeDeclarationStmt* newTypeDeclarationStmt()
     {
        TypeDeclarationStmt* node = new TypeDeclarationStmt();
        setStatementValues(node);
        return node;
     }

   void setDeclarationTypeSpec(DeclarationTypeSpec * declarationtypespec) {}
   void setOptAttrSpecList(OptAttrSpecList* optattrspeclist) {}
   void setEntityDeclList(EntityDeclList* entitydecllist) {}

 private:
   
};
#endif

#ifdef OBSOLETE
class DeclarationTypeSpec : public Node
{
 public:
   DeclarationTypeSpec() {}
  ~DeclarationTypeSpec() {}

   DeclarationTypeSpec* newDeclarationTypeSpec()
     {
        DeclarationTypeSpec* node = new DeclarationTypeSpec;
        return node;
     }

   void setIntrinsicTypeSpec(IntrinsicTypeSpec* intrinsictypespec) {}

 private:
   
};
#endif

#ifdef OBSOLETE
class IntrinsicTypeSpec : public Node
{
 public:
   IntrinsicTypeSpec() {}
  ~IntrinsicTypeSpec() {}

   IntrinsicTypeSpec* newIntrinsicTypeSpec()
     {
        IntrinsicTypeSpec* node = new IntrinsicTypeSpec;
        return node;
     }

   //void setIntrinsicTypeSpec(IntrinsicTypeSpec* intrinsictypespec) {}

 private:
   
};
#endif

#ifdef NEEDS_LIST
class AttrSpecList : public Node
{
 public:
   AttrSpecList() {pList = new std::vector<AttrSpec*>();}
  ~AttrSpecList() {if (pList) delete pList;}

   AttrSpecList* newAttrSpecList()
     {
        AttrSpecList* node = new AttrSpecList();
        node->pList = pList;  pList = NULL;
        return node;
     }

   void appendAttrSpec(AttrSpec* attrspec) {pList->push_back(attrspec);}

 private:
   std::vector<AttrSpec*>* pList;
};
#endif

#ifdef OBSOLETE
class OptAttrSpecList : public Node
{
 public:
   OptAttrSpecList() : pAttrSpecList(NULL) {}
  ~OptAttrSpecList() {if (pAttrSpecList) delete pAttrSpecList;}

   OptAttrSpecList* newOptAttrSpecList()
     {
        OptAttrSpecList* node = new OptAttrSpecList();
        node->pAttrSpecList = pAttrSpecList;  pAttrSpecList = NULL;
        return node;
     }

   void setAttrSpecList(AttrSpecList* attrspeclist) {pAttrSpecList = attrspeclist;}

 private:
   Node* pAttrSpecList;
};
#endif

#ifdef OBSOLETE
class EntityDecl : public Node
{
 public:
   EntityDecl() {}
  ~EntityDecl() {}

   EntityDecl* newEntityDecl()
     {
        EntityDecl* node = new EntityDecl();
        return node;
     }

   Name* getObjectName() {return pObjectName;}
   void setObjectName(Name* objectname) {pObjectName = objectname;}

 private:
   Name* pObjectName;
};
#endif

#ifdef NEEDS_LIST
class EntityDeclList : public Node
{
 public:
   EntityDeclList() {pList = new std::vector<EntityDecl*>();}
  ~EntityDeclList() {if (pList) delete pList;}

   EntityDeclList* newEntityDeclList()
     {
        EntityDeclList* node = new EntityDeclList();
        node->pList = pList;  pList = NULL;
        return node;
     }

   void appendEntityDecl(EntityDecl* entitydecl) {pList->push_back(entitydecl);}

 private:
   std::vector<EntityDecl*>* pList;
};
#endif

#ifdef OBSOLETE
class AttrSpec : public Node
{
 public:
   AttrSpec() {}
  ~AttrSpec() {}

   AttrSpec* newAttrSpec()
     {
        AttrSpec* node = new AttrSpec();
        return node;
     }

 private:
};
#endif

#ifdef OBSOLETE
class ImplicitStmt : public Statement
{
 public:
   ImplicitStmt() : Statement(), option(UNKNOWN) {}
  ~ImplicitStmt();

   enum OptionType
     {
        UNKNOWN,
        NONE
     };

   ImplicitStmt* newImplicitStmt()
     {
        ImplicitStmt* node = new ImplicitStmt();
        setStatementValues(node);
        node->option = option;
        return node;
     }

   void setOptionType(OptionType ot) {option = ot;}
   OptionType getOptionType() {return option;}

 private:
   OptionType option;
};
#endif

} // namespace OFP

#endif
