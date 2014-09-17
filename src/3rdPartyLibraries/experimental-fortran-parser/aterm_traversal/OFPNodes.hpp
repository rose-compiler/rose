#ifndef	OFP_NODES_H
#define	OFP_NODES_H

#if COMPILED_WITH_ROSE==0
//#include "UntypedNodes.h"
#endif

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
   Node() : optionType(0), payload(NULL) {}
   virtual ~Node()        {} //TODO-CER-2014.3.15 {if (payload) delete payload;  payload = NULL;}

   enum OptionType
     {
        DEFAULT = 0
     };

   int  getOptionType()       {return optionType;}
   void setOptionType(int ot) {optionType = ot;}
   
   SgUntypedNode* getPayload()                    {return payload;}
   SgUntypedNode* givePayload()                   {SgUntypedNode* tmp = payload; payload = NULL; return tmp;}
   void           setPayload(SgUntypedNode* node) {payload = node;}
   void           inheritPayload(Node* node)      {payload = node->getPayload();  node->setPayload(NULL);}

   ATerm           term;
   int             optionType;
   SgUntypedNode*  payload;
};

/* Terminals
 */
class Terminal : public Node
{
 public:
   Terminal() : value("")                  {}
   Terminal(std::string val) : value(val)  {}
   virtual ~Terminal()                     {}

   std::string & getValue()           {return value;}
   void          setValue(char * str) {value = std::string(str);}

   std::string getName() const        {return value;}

 protected:
   //TODO - change from pointer
   std::string value;
};

class Dop : public Terminal
{
 public:
   Dop() : Terminal() {}
   Dop(std::string val) : Terminal(val) {}

   Dop * newDop()
     {
        Dop * node = new Dop(value);
        node->inheritPayload(this);
        return node;
     }
};

class HexConstant : public Terminal
{
 public:
   HexConstant() : Terminal() {}
   HexConstant(std::string val) : Terminal(val) {}

   HexConstant * newHexConstant()
     {
        HexConstant * node = new HexConstant(value);
        node->inheritPayload(this);
        return node;
     }
};

class OctalConstant : public Terminal
{
 public:
   OctalConstant() : Terminal() {}
   OctalConstant(std::string val) : Terminal(val) {}

   OctalConstant * newOctalConstant()
     {
        OctalConstant * node = new OctalConstant(value);
        node->inheritPayload(this);
        return node;
     }
};

class BinaryConstant : public Terminal
{
 public:
   BinaryConstant() : Terminal() {}
   BinaryConstant(std::string val) : Terminal(val) {}

   BinaryConstant * newBinaryConstant()
     {
        BinaryConstant * node = new BinaryConstant(value);
        node->inheritPayload(this);
        return node;
     }
};

class Rcon : public Terminal
{
 public:
   Rcon() : Terminal() {}
   Rcon(std::string val) : Terminal(val) {}

   Rcon * newRcon()
     {
        Rcon * node = new Rcon(value);
        node->inheritPayload(this);
        return node;
     }
};

class Scon : public Terminal
{
 public:
   Scon() : Terminal() {}
   Scon(std::string val) : Terminal(val) {}

   Scon * newScon()
     {
        Scon * node = new Scon(value);
        node->inheritPayload(this);
        return node;
     }
};

class Icon : public Terminal
{
 public:
   Icon() : Terminal() {}
   Icon(std::string val) : Terminal(val) {}

   Icon * newIcon()
     {
        Icon * node = new Icon(value);
        node->inheritPayload(this);
        return node;
     }
};

class Ident : public Terminal
{
 public:
   Ident() : Terminal() {}
   Ident(std::string val) : Terminal(val) {}

   Ident * newIdent()
     {
        Ident * node = new Ident(value);
        node->inheritPayload(this);
        return node;
     }
};

class Letter : public Terminal
{
 public:
   Letter() : Terminal() {}
   Letter(std::string val) : Terminal(val) {}

   Letter * newLetter()
     {
        Letter * node = new Letter(value);
        node->inheritPayload(this);
        return node;
     }
};

class Label : public Terminal
{
 public:
   Label() : Terminal() {}
   Label(std::string val) : Terminal(val) {}

   Label * newLabel()
     {
        Label * node = new Label(value);
        node->inheritPayload(this);
        return node;
     }
};

class LblRef : public Terminal
{
 public:
   LblRef() : Terminal() {}
   LblRef(std::string val) : Terminal(val) {}

   LblRef * newLblRef()
     {
        LblRef * node = new LblRef(value);
        node->inheritPayload(this);
        return node;
     }
};

class StartCommentBlock : public Terminal
{
 public:
   StartCommentBlock() : Terminal() {}
   StartCommentBlock(std::string val) : Terminal(val) {}

   StartCommentBlock * newStartCommentBlock()
     {
        StartCommentBlock * node = new StartCommentBlock(value);
        node->inheritPayload(this);
        return node;
     }
};

class EOS : public Terminal
{
 public:
   EOS() : Terminal() {}
   EOS(std::string val) : Terminal(val) {}

   EOS * newEOS()
     {
        EOS * node = new EOS(value);
        node->inheritPayload(this);
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
    virtual ~Statement()
      {
         if (pLabel) delete pLabel;
         if (pEOS)   delete pEOS;
      }

   Statement * newStatement()
     {
        Statement* node = new Statement();
        node->pLabel = pLabel;  pLabel = NULL;
        node->pEOS   = pEOS;    pEOS   = NULL;
        node->inheritPayload(this);
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
    virtual ~Name()       {}

   Name * newName()
     {
        Name * node = new Name();
        node->pIdent = pIdent;  pIdent = NULL;
        node->inheritPayload(this);
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
    virtual ~Variable()       {}

   Variable* newVariable()
     {
        Variable* node = newVariable();
        node->pValue = pValue;  pValue = NULL;
        node->inheritPayload(this);
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
    virtual ~Expr()      {}

   Expr* newExpr()
     {
        Expr* node = newExpr();
        node->pExpr = pExpr;  pExpr = NULL;
        node->inheritPayload(this);
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
    virtual ~AssignmentStmt() {}

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
    virtual ~MainProgram() {}

   // Constructs a new Node on the heap from one on the stack (that is about to be reclaimed).
   MainProgram* newMainProgram()
      {
         MainProgram* node = new MainProgram();
         node->pProgramStmt = pProgramStmt; pProgramStmt = NULL;
         node->pEndProgramStmt = pEndProgramStmt; pEndProgramStmt = NULL;
         node->pSpecificationPart = pSpecificationPart; pSpecificationPart = NULL;
         node->pExecutionPart = pExecutionPart; pExecutionPart = NULL;
         node->pInternalSubprogramPart = pInternalSubprogramPart; pInternalSubprogramPart = NULL;
         node->inheritPayload(this);
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
   virtual ~ProgramStmt() {}

   ProgramStmt* newProgramStmt()
     {
        ProgramStmt * node = new ProgramStmt();
        setStatementValues(node);
        node->pProgramName = pProgramName;  pProgramName = NULL;
        node->inheritPayload(this);
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
   virtual ~EndProgramStmt() {}

   EndProgramStmt* newEndProgramStmt()
     {
        EndProgramStmt* node = new EndProgramStmt();
        setStatementValues(node);
        node->pProgramName = pProgramName;  pProgramName = NULL;
        node->inheritPayload(this);
        return node;
     }

   Name* getProgramName() {return pProgramName;}
   void setProgramName(Name* programname) {pProgramName = programname;}

 private:
   Name* pProgramName;
};
#endif

#ifdef NO_STMT_INHERIT
class ImplicitPartStmt : public Statement
{
 public:
 ImplicitPartStmt() : Statement(), pStmt(NULL) {}
   virtual ~ImplicitPartStmt() {}

   ImplicitPartStmt * newImplicitPartStmt()
     {
        ImplicitPartStmt* node = new ImplicitPartStmt();
        node->pStmt = pStmt;  pStmt = NULL;
        node->inheritPayload(this);
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
   virtual ~DeclarationConstruct()      {}

   DeclarationConstruct * newDeclarationConstruct()
     {
        DeclarationConstruct* node = new DeclarationConstruct();
        node->pNode = pNode;  pNode = NULL;
        node->inheritPayload(this);
        return node;
     }

   void setTypeDeclarationStmt(Node* typedeclarationstmt)  {pNode = typedeclarationstmt;}

 private:
   Node* pNode;
};
#endif

#ifdef NEEDS_LIST
{
 public:
    ImplicitPart()
       {
          //pImplicitPartStmt = NULL;
          pImplicitStmt = NULL;
          pList = new std::vector<ImplicitPartStmt*>();
       }
    virtual ~ImplicitPart() {}

    ImplicitPart* newImplicitPart()
       {
          ImplicitPart* node = new ImplicitPart();
          //node->pImplicitPartStmt = pImplicitPartStmt;  pImplicitPartStmt = NULL;
          node->pImplicitStmt = pImplicitStmt;  pImplicitStmt = NULL;
          node->inheritPayload(this);
          return node;
       }

    //ImplicitPartStmt* getImplicitPartStmt() {return pImplicitPartStmt;}
    ImplicitStmt* getImplicitStmt() {return pImplicitStmt;}

    //void setImplicitPartStmt(ImplicitPartStmt* implicitpartstmt) {pImplicitPartStmt = implicitpartstmt;}
    void setImplicitStmt(ImplicitStmt* implicitstmt) {pImplicitStmt = implicitstmt;}

    void appendImplicitPartStmt(ImplicitPartStmt* implicitpartstmt) {pList->push_back(implicitpartstmt);}

 private:
    //ImplicitPartStmt* pImplicitPartStmt;
    ImplicitStmt* pImplicitStmt;
    std::vector<ImplicitPartStmt*>* pList;
};
#endif

#ifdef OBSOLETE
class TypeDeclarationStmt : public Statement
{
 public:
   TypeDeclarationStmt() : Statement() {}
   virtual ~TypeDeclarationStmt()      {}

   TypeDeclarationStmt* newTypeDeclarationStmt()
     {
        TypeDeclarationStmt* node = new TypeDeclarationStmt();
        setStatementValues(node);
        node->inheritPayload(this);
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
   virtual ~DeclarationTypeSpec() {}

   DeclarationTypeSpec* newDeclarationTypeSpec()
     {
        DeclarationTypeSpec* node = new DeclarationTypeSpec;
        node->inheritPayload(this);
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
   IntrinsicTypeSpec()           {}
   virtural ~IntrinsicTypeSpec() {}

   IntrinsicTypeSpec* newIntrinsicTypeSpec()
     {
        IntrinsicTypeSpec* node = new IntrinsicTypeSpec;
        node->inheritPayload(this);
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
   virtual ~AttrSpecList() {if (pList) delete pList;}

   AttrSpecList* newAttrSpecList()
     {
        AttrSpecList* node = new AttrSpecList();
        node->pList = pList;  pList = NULL;
        node->inheritPayload(this);
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
   virtual ~OptAttrSpecList() {if (pAttrSpecList) delete pAttrSpecList;}

   OptAttrSpecList* newOptAttrSpecList()
     {
        OptAttrSpecList* node = new OptAttrSpecList();
        node->pAttrSpecList = pAttrSpecList;  pAttrSpecList = NULL;
        node->inheritPayload(this);
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
   EntityDecl()          {}
   virtual ~EntityDecl() {}

   EntityDecl* newEntityDecl()
     {
        EntityDecl* node = new EntityDecl();
        node->inheritPayload(this);
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
   virtual ~EntityDeclList() {if (pList) delete pList;}

   EntityDeclList* newEntityDeclList()
     {
        EntityDeclList* node = new EntityDeclList();
        node->pList = pList;  pList = NULL;
        node->inheritPayload(this);
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
        node->inheritPayload(this);
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
        DEFAULT = 0,
        ImplicitStmt_NONE
     };

   ImplicitStmt* newImplicitStmt()
     {
        ImplicitStmt* node = new ImplicitStmt();
        setStatementValues(node);
        node->option = option;
        node->inheritPayload(this);
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
