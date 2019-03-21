
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

#include "sageBuilder.h"
#include "sageGeneric.h"

#include "csharpBuilder.h"
#include "ast2dot.hpp"

// proposed extensions to SageBuilder
namespace SageBuilderX
{
  SgUsingDeclarationStatement*
  buildUsingDeclaration(SgNamespaceDeclarationStatement* nsp)
  {
    ROSE_ASSERT(nsp);

    SgUsingDeclarationStatement* uds = new SgUsingDeclarationStatement(nsp, NULL);

    //~ uds.set_scope();
    return uds;
  }
}

namespace sb = SageBuilder;
namespace sbx = SageBuilderX;

// AST builder functions
namespace csharp_translator
{
  namespace
  {
    // builder stack
    std::vector<SgName>  names;
    std::vector<SgNode*> nodes;
    std::vector<SgType*> types;
    std::vector<size_t>  seqs;
    SgGlobal*            theGlobalScope = NULL; // only one global scope

    // mappings from UIDs to ROSE declaration nodes
    std::map<int, SgInitializedName*>           vardecls;
    std::map<int, SgClassDeclaration*>          classdecls;
    std::map<int, SgMemberFunctionDeclaration*> fundecls;

    //
    // helper functions to access the builder stacks

    SgNode* assert_node_type(SgNode* n, SgNode*)
    {
      return n;
    }

    template <class SageNode>
    SageNode* assert_node_type(SgNode* n, SageNode*)
    {
      return SG_ASSERT_TYPE(SageNode, n);
    }

    template <class SageNode, class BaseSageNode>
    SageNode* pop_if(std::vector<BaseSageNode*>& cont, bool cond)
    {
      if (!cond) return NULL;

      ROSE_ASSERT(cont.size() > 0);
      BaseSageNode* node = cont.back();

      std::cerr << "< -1" << std::endl;
      cont.pop_back();
      return assert_node_type(node, static_cast<SageNode*>(NULL));
    }

    template <class SageNode, class BaseSageNode>
    SageNode* pop(std::vector<BaseSageNode*>& cont)
    {
      return pop_if<SageNode>(cont, true);
    }

    template <class SageNode, class BaseSageNode>
    SageNode* top(std::vector<BaseSageNode*>& cont)
    {
      ROSE_ASSERT(cont.size() > 0);
      return SG_ASSERT_TYPE(SageNode, cont.back());
    }

    template <class T>
    T pop(std::vector<T>& cont)
    {
      ROSE_ASSERT(cont.size() > 0);

      T res = cont.back();

      cont.pop_back();
      return res;
    }

    template <class T>
    void pop_n(std::vector<T>& cont, size_t num)
    {
      ROSE_ASSERT(cont.size() >= num);

      cont.resize(cont.size() - num);
    }

    template <class T>
    void assert_valid_ptr(const T&) {}

    template <class T>
    void assert_valid_ptr(T* ptr)
    {
      ROSE_ASSERT(ptr);
    }

    template <class T, class U>
    void push(std::vector<T>& cont, U elem)
    {
      assert_valid_ptr(elem);
      //~ std::cerr << ">> " << typeid(U).name() << std::endl;

      cont.push_back(elem);
    }

    template <class SageDecl>
    SageDecl* retrieve(const std::map<int, SageDecl*>& map, int uid)
    {
      typename std::map<int, SageDecl*>::const_iterator pos = map.find(uid);

      ROSE_ASSERT(pos != map.end() && pos->second != NULL);
      return pos->second;
    }

    SgGlobal* globalScope()
    {
      ROSE_ASSERT(theGlobalScope);
      return theGlobalScope;
    }

    template <class SageNode, class SageChild>
    void setChild(SageNode& n, void (SageNode::*setter)(SageChild*), SageChild& child)
    {
      (n.*setter)(&child);
      child.set_parent(&n);
    }

    //
    // maker (aka builder) functions

/*
    SgInitializer& mkInit(SgExpression& exp, SgType& ty)
    {
      SgInitializer* res = sb::buildAssignInitializer(&exp, &ty);

      return sg::deref(res);
    }

    SgInitializer* mkInit(SgExpression* exp, SgType* ty)
    {
      return &mkInit(sg::deref(exp), sg::deref(ty));
    }
*/

    std::string mkScopedName(size_t lv)
    {
      //~ if (lv == 0) return "";
      ROSE_ASSERT(lv > 0);

      std::string curr = pop(names);

      while (--lv)
      {
        curr = pop(names) + "." + curr;
      }

      //~ std::cerr << "proudly made: " << curr << std::endl;
      return curr;
    }

    SgNamespaceDeclarationStatement* mkHiddenNamespace(SgName nspname)
    {
      return sb::buildNamespaceDeclaration(nspname, globalScope());
    }


    //
    // miscellaneous convenience functions

    /// creates a dummy file info (for now)
    Sg_File_Info* dummyFileInfo()
    {
      return new Sg_File_Info("<nowhere>", 0, 0);
    }

    /// extracts the only initialized name from a var decl
    SgInitializedName* oneAndOnly(SgVariableDeclaration* n)
    {
      SgInitializedNamePtrList& lst = sg::deref(n).get_variables();

      ROSE_ASSERT(lst.size() == 1 && lst.back() != NULL);
      return lst.back();
    }

    typedef SgExpression* (*mk_binary_fun)(SgExpression*, SgExpression*);

    // homogeneous return types instead of covariant ones
    template <class R, R* (*mkexp) (SgExpression*, SgExpression*)>
    SgExpression* mk2_wrapper(SgExpression* lhs, SgExpression* rhs)
    {
      return mkexp(lhs, rhs);
    }

    SgExpression* mkCall(SgExpression* callee, SgExpression* args)
    {
      SgExprListExp* lst = SG_ASSERT_TYPE(SgExprListExp, args);

      return sb::buildFunctionCallExp(callee, lst);
    }

    std::map<std::string, mk_binary_fun> binary_maker_map;

    static inline
    SgExpression*
    mk_binary(const std::string& op, SgExpression* lhs, SgExpression* rhs)
    {
      if (binary_maker_map.size() == 0)
      {
        binary_maker_map["+"]  = mk2_wrapper<SgAddOp,            sb::buildAddOp>;
        binary_maker_map["-"]  = mk2_wrapper<SgSubtractOp,       sb::buildSubtractOp>;
        binary_maker_map["*"]  = mk2_wrapper<SgMultiplyOp,       sb::buildMultiplyOp>;
        binary_maker_map["/"]  = mk2_wrapper<SgDivideOp,         sb::buildDivideOp>;
        binary_maker_map["%"]  = mk2_wrapper<SgModOp,            sb::buildModOp>;

        binary_maker_map["<"]  = mk2_wrapper<SgLessThanOp,       sb::buildLessThanOp>;
        binary_maker_map[">"]  = mk2_wrapper<SgGreaterThanOp,    sb::buildGreaterThanOp>;
        binary_maker_map["=="] = mk2_wrapper<SgEqualityOp,       sb::buildEqualityOp>;
        binary_maker_map[">="] = mk2_wrapper<SgGreaterOrEqualOp, sb::buildGreaterOrEqualOp>;
        binary_maker_map["<="] = mk2_wrapper<SgLessThanOp,       sb::buildLessThanOp>;
        binary_maker_map["!="] = mk2_wrapper<SgNotEqualOp,       sb::buildNotEqualOp>;

        binary_maker_map[">>"] = mk2_wrapper<SgRshiftOp,         sb::buildRshiftOp>;
        binary_maker_map["<<"] = mk2_wrapper<SgLshiftOp,         sb::buildLshiftOp>;
        binary_maker_map["^"]  = mk2_wrapper<SgBitXorOp,         sb::buildBitXorOp>;
        binary_maker_map["&"]  = mk2_wrapper<SgBitAndOp,         sb::buildBitAndOp>;
        binary_maker_map["|"]  = mk2_wrapper<SgBitOrOp,          sb::buildBitOrOp>;

        binary_maker_map["&&"] = mk2_wrapper<SgAndOp,            sb::buildAndOp>;
        binary_maker_map["||"] = mk2_wrapper<SgOrOp,             sb::buildOrOp>;
        binary_maker_map["()"] = mkCall;
      }

      mk_binary_fun fn = binary_maker_map[op];

      ROSE_ASSERT(fn != NULL);
      return fn(lhs, rhs);
    }

    std::map<std::string, SgType*> type_maker_map;

    static inline
    SgType* mk_type(const std::string& op)
    {
      if (type_maker_map.size() == 0)
      {
        type_maker_map["void"] = sb::buildVoidType();
        type_maker_map["bool"] = sb::buildBoolType();
        type_maker_map["int"]  = sb::buildIntType();
      }

      SgType* res = type_maker_map[op];

      ROSE_ASSERT(res != NULL);
      return res;
    }

    template <class U, class V>
    U as(V val)
    {
      std::stringstream str;
      U                 res;

      str << val;
      str >> res;

      return res;
    }

    /// sets the symbols defining decl
    template <class SageSymbol, class SageDecl>
    void link_decls(SageSymbol& funcsy, SageDecl& func)
    {
      SageDecl& sdcl = sg::deref(funcsy.get_declaration());

      sdcl.set_definingDeclaration(&func);
      func.set_firstNondefiningDeclaration(&sdcl);

      // \todo \pp is sdcl == func allowed in ROSE/C++?
    }

    SgBasicBlock&
    mkBasicBlock()
    {
      SgBasicBlock& bdy = sg::deref(sb::buildBasicBlock());

      //~ markCompilerGenerated(bdy);
      return bdy;
    }

    SgFunctionDefinition&
    mkFunctionDefinition(SgFunctionDeclaration& dcl)
    {
      SgFunctionDefinition& def = *new SgFunctionDefinition(&dcl, NULL);

      setChild(dcl, &SgFunctionDeclaration::set_definition, def);
      //~ markCompilerGenerated(def);
      return def;
    }

    template <class SageSymbol, class SageDecl>
    void promoteToDefinition(SageDecl& func)
    {
      SgSymbol*             baseSy = func.search_for_symbol_from_symbol_table();
      SageSymbol&           funcSy = *SG_ASSERT_TYPE(SageSymbol, baseSy);

      link_decls(funcSy, func);
      func.set_definingDeclaration(&func);
      func.unsetForward();

      SgFunctionDefinition& fdef   = mkFunctionDefinition(func);
      SgBasicBlock&         body   = mkBasicBlock();

      setChild(fdef, &SgFunctionDefinition::set_body, body);
    }
  }

  /// the simplest function
  void helloFromCxx()
  {
    std::cout << "Hello from C++" << std::endl;
  }

  void beginSeq(SgNode* n)
  {
    seqs.push_back(nodes.size());

    std::cerr << ">seq +1" << std::endl;
    push(nodes, n);
  }

  void beginSeq(SeqKind kind)
  {
    SgNode*     res = NULL;
    std::string msg;

    switch (kind)
    {
      case GLOBALSCOPESEQ:
        {
          ROSE_ASSERT(theGlobalScope == NULL);

          theGlobalScope = new SgGlobal(dummyFileInfo());
          sb::pushScopeStack(theGlobalScope);

          res = theGlobalScope;
          msg = "globalscope";
          break;
        }

      case CLASSMEMBERSEQ:
        {
          SgClassDeclaration* cdecl = top<SgClassDeclaration>(nodes);
          SgClassDefinition*  cdef  = cdecl->get_definition();

          sb::pushScopeStack(cdef);
          res = cdef;
          msg = "classscope";
          break;
        }

      case PARAMETERSEQ:
        {
          SgMemberFunctionDeclaration* fdecl  = top<SgMemberFunctionDeclaration>(nodes);
          SgScopeStatement*            prmscp = fdecl->get_functionParameterScope();
          SgFunctionParameterList*     params = fdecl->get_parameterList();

          sb::pushScopeStack(prmscp);
          res = params;
          msg = "paramlist";
          break;
        }

      case METHODBODYSEQ:
        {
          SgMemberFunctionDeclaration* decl = top<SgMemberFunctionDeclaration>(nodes);

          promoteToDefinition<SgMemberFunctionSymbol>(*decl);

          SgFunctionDefinition*        fdef = decl->get_definition();
          SgBasicBlock*                body = sg::deref(fdef).get_body();

          ROSE_ASSERT(body);
          sb::pushScopeStack(body);
          res = fdef;
          msg = "methodbody";
          break;
        }

      case EXPRLISTSEQ:
        {
          SgExprListExp* lst = sb::buildExprListExp();

          res = lst;
          msg = "exprlist";
          break;
        }

      case IFSTMT:
        {
          SgStatement* nullstmt = NULL;
          SgIfStmt*    ifstmt = new SgIfStmt(nullstmt, nullstmt, nullstmt);

          sb::pushScopeStack(ifstmt);
          res = ifstmt;
          msg = "if";
          break;
        }

      default:
        msg = "NOT IMPLEMENTED seq (C++): " + kind;
        break;
    }

    std::cerr << "open: " << msg << "  *" << nodes.size() << std::endl;
    beginSeq(res);
  }

  template <class SageSequence, class SageElement>
  struct NodeAdder
  {
    NodeAdder(SageSequence& seq, void (SageSequence::*fn)(SageElement*))
    : cont(seq), adder(fn)
    {}

    template <class BaseSageNode>
    void operator()(BaseSageNode* n)
    {
      (cont.*adder)(SG_ASSERT_TYPE(SageElement, n));
    }

    SageSequence& cont;
    void (SageSequence::*adder)(SageElement*);
  };

  template <class SageSequence, class SageElement>
  NodeAdder<SageSequence, SageElement>
  nodeAdder(SageSequence& seq, void (SageSequence::*fn)(SageElement*))
  {
    return NodeAdder<SageSequence, SageElement>(seq, fn);
  }

  struct ConvertToStmt : sg::DispatchHandler<SgStatement*>
  {
    void handle(const SgNode& n) { SG_UNEXPECTED_NODE(n); }

    void handle(SgStatement& n)  { res = &n; }
    void handle(SgExpression& n) { res = sb::buildExprStatement(&n); }
  };

  struct Incorporate
  {
    Incorporate(std::vector<SgNode*>& nodestack, size_t numnodes)
    : nodes(nodestack), num(numnodes)
    {
      ROSE_ASSERT(nodes.size() >= num);
    }

    template <class SageSequence, class SageElement>
    void _incorporate(SageSequence& parent, void (SageSequence::*fn)(SageElement*), SgNode* ctrl)
    {
      std::for_each(nodes.end()-num, nodes.end(), nodeAdder(parent, fn));
      std::cerr << "< -" << num << std::endl;
      pop_n(nodes, num);

      ROSE_ASSERT(nodes.back() == ctrl);
    }

    template <class SageSequence, class SageElement>
    void incorporate(SageSequence& parent, void (SageSequence::*fn)(SageElement*))
    {
      _incorporate(parent, fn, &parent);
    }

    void handle(SgNode& n)                  { sg::unexpected_node(n); }

    // true sequences
    void handle(SgClassDefinition& n)       { incorporate(n, &SgClassDefinition::append_member); }
    void handle(SgFunctionParameterList& n) { incorporate(n, &SgFunctionParameterList::append_arg); }
    void handle(SgExprListExp& n)           { incorporate(n, &SgExprListExp::append_expression); }
    void handle(SgGlobal& n)                { incorporate(n, &SgGlobal::append_declaration); }
    void handle(SgBasicBlock& n)            { incorporate(n, &SgBasicBlock::append_statement); }

    void handle(SgFunctionDefinition& n)
    {
      _incorporate(sg::deref(n.get_body()), &SgBasicBlock::append_statement, &n);
    }

    // pseudo sequences
    void handle(SgIfStmt& n)
    {
      assert(num == 2 || num == 3);

      // std::cerr << " || " << nodes.size() << " " << num << std::endl ;

      if (num > 2)
      {
        setChild(n, &SgIfStmt::set_false_body, *pop<SgStatement>(nodes));
      }

      setChild(n, &SgIfStmt::set_true_body, *pop<SgStatement>(nodes));
      setChild(n, &SgIfStmt::set_conditional, *sg::dispatch(ConvertToStmt(), pop<SgNode>(nodes)));

      ROSE_ASSERT(nodes.back() == &n);
    }

    std::vector<SgNode*>& nodes;
    size_t                num;
  };

  /// pops nodes from scope- and node-stacks as needed
  ///   e.g., none for SgExprListExp ...
  struct ScopePopper
  {
    static inline
    void pop_scope() { sb::popScopeStack(); }

    static inline
    void pop_node()  { pop(nodes); }

    void handle(SgNode&)                    {}
    void handle(SgGlobal& n)                { pop_scope(); dot::save_dot("csharp.dot", n); }
    void handle(SgScopeStatement& n)        { pop_scope(); }

    void handle(SgFunctionDefinition& n)    { pop_scope(); pop_node(); }
    void handle(SgClassDefinition& n)       { pop_scope(); pop_node(); }
    void handle(SgFunctionParameterList& n) { pop_scope(); pop_node(); } // pop associated function def scope
  };

  void closeSeq()
  {
    size_t  pos  = pop(seqs);
    ROSE_ASSERT(pos <= nodes.size());

    SgNode* seq  = nodes.at(pos);
    size_t  num  = nodes.size() - pos;
    ROSE_ASSERT(num > 0); // needs to contain seq

    std::cerr << "x= " << typeid(*seq).name() << std::endl;
    sg::dispatch(Incorporate(nodes, num-1), seq);
    sg::dispatch(ScopePopper(), seq);

    std::cerr << "closing (C++) " << typeid(sg::deref(seq)).name()
              << "  *" << nodes.size()
              << "  #" << num-1
              << std::endl;
  }

  void predefinedType(const char* tyname)
  {
    types.push_back(mk_type(tyname));
  }

  void name(const char* n)
  {
    names.emplace_back(n);
  }

  void usingDirective(int uid, int levels)
  {
    ROSE_ASSERT(unsigned(levels) <= names.size());

    std::string                      nspname = mkScopedName(levels);
    SgNamespaceDeclarationStatement* nspdecl = mkHiddenNamespace(nspname);
    SgUsingDeclarationStatement*     usgdecl = sbx::buildUsingDeclaration(nspdecl);

    std::cerr << ">usn +1" << std::endl;
    push(nodes, usgdecl);
  }

  void varDecl(int uid)
  {
    SgName                 nm = pop(names);
    SgType*                ty = pop(types);
    SgVariableDeclaration* vr = sb::buildVariableDeclaration(nm, ty, NULL);

    std::cerr << ">vardecl +1" << std::endl;
    push(nodes, vr);
    vardecls[uid] = oneAndOnly(vr);
  }

  void paramDecl(int uid)
  {
    SgName                    nm  = pop(names);
    SgType*                   ty  = pop(types);
    SgInitializedName*        prm = sb::buildInitializedName(nm, ty);
    SgFunctionParameterScope* scp = isSgFunctionParameterScope(sb::topScopeStack());
    SgSymbolTable*            tab = sg::deref(scp).get_symbol_table();

    ROSE_ASSERT(tab);
    prm->set_scope(scp);
    tab->insert(nm, new SgVariableSymbol(prm));

    std::cerr << ">prm +1" << std::endl;
    push(nodes, prm);
    vardecls[uid] = prm;
  }

  void refVarParamDecl(int uid)
  {
    SgInitializedName* var = retrieve(vardecls, uid);
    SgVariableSymbol*  sym = SG_ASSERT_TYPE( SgVariableSymbol,
                                             sg::deref(var).get_symbol_from_symbol_table()
                                           );
    ROSE_ASSERT(sym != NULL);
    std::cerr << ">var +1" << std::endl;
    push(nodes, sb::buildVarRefExp(sym));
  }

  void refFunDecl(int uid)
  {
    SgMemberFunctionDeclaration* fun = retrieve(fundecls, uid);

    ROSE_ASSERT(fun != NULL);
    std::cerr << ">fun +1" << std::endl;
    push(nodes, sb::buildFunctionRefExp(fun));
  }


  void initVarParamDecl(int uid)
  {
    SgInitializedName*     var  = retrieve(vardecls, uid);
    SgInitializer*         init = pop<SgInitializer>(nodes);

    var->set_initializer(init);
  }

  void classDecl(int uid)
  {
    SgName              nm = pop(names);
    SgScopeStatement*   sp = sb::topScopeStack();

    // \todo why does the call below not work ???
    // SgClassDeclaration* cd = sb::buildClassDeclaration(nm, sp);
    SgClassDeclaration* cd = sb::buildClassDeclaration_nfi( nm,
                                                            SgClassDeclaration::e_class,
                                                            sp,
                                                            NULL /* nondef decl */,
                                                            false /* no template */,
                                                            NULL /* no template args */
                                                          );

    std::cerr << ">clsdecl +1" << std::endl;
    push(nodes, cd);
    classdecls[uid] = cd;
  }

  void methodDecl(int uid)
  {
    SgName                       nm  = pop(names);
    SgType*                      ty  = pop(types);
    SgFunctionParameterList*     pl  = sb::buildFunctionParameterList(); // pop<SgFunctionParameterList>(nodes);
    SgScopeStatement*            sp  = sb::topScopeStack();
    SgMemberFunctionDeclaration* mf  = sb::buildNondefiningMemberFunctionDeclaration(nm, ty, pl, sp);
    SgFunctionParameterScope*    psc = new SgFunctionParameterScope(dummyFileInfo());

    mf->set_functionParameterScope(psc);

    std::cerr << ">methdecl +1" << std::endl;
    push(nodes, mf);
    fundecls[uid] = mf;
  }

  void stageMethodDecl(int uid)
  {
    SgMemberFunctionDeclaration* mf = retrieve(fundecls, uid);

    std::cerr << ">method +1" << std::endl;
    push(nodes, mf);
  }

  //
  // statement builders

  void returnStmt(int args)
  {
    ROSE_ASSERT(args == 0 || args == 1);
    SgExpression* expr = (args == 0) ? NULL : pop<SgExpression>(nodes);

    std::cerr << ">ret " << args << std::endl;
    push(nodes, sb::buildReturnStmt(expr));
  }

  //
  // expressions

  void valueInitializer()
  {
    SgExpression*  ex   = pop<SgExpression>(nodes);
    SgType*        ty   = pop(types);
    SgInitializer* init = sb::buildAssignInitializer(ex, ty);

    std::cerr << ">val-ini =" << std::endl;
    push(nodes, init);
  }

  struct LiteralMaker : sg::DispatchHandler<SgExpression*>
  {
    explicit
    LiteralMaker(std::string str)
    : sg::DispatchHandler<SgExpression*>(), rep(str)
    {}

    void handle(SgNode& n)   { sg::unexpected_node(n); }
    void handle(SgTypeInt&)  { res = sb::buildIntVal(as<int>(rep)); }

    void handle(SgTypeBool&)
    {
      if ("true" == rep)
      {
        res = sb::buildBoolValExp(1);
        return;
      }

      ROSE_ASSERT("false" == rep);
      res = sb::buildBoolValExp(0);
    }

    std::string rep;
  };

  void literal(const char* rep)
  {
    SgType*        ty   = pop(types);
    SgExpression*  lit  = sg::dispatch(LiteralMaker(rep), ty);

    ROSE_ASSERT(lit);
    std::cerr << ">lit" << std::endl;
    push(nodes, lit);
  }

  void binary(const char* rep)
  {
    /*SgType* ty = */     pop(types);
    SgExpression*  rhs  = pop<SgExpression>(nodes);
    SgExpression*  lhs  = pop<SgExpression>(nodes);
    SgExpression*  res  = mk_binary(rep, lhs, rhs);

    ROSE_ASSERT(res);
    std::cerr << ">bin -1 " << rep << std::endl;
    push(nodes, res);
  }

  void basicFinalChecks()
  {
    // no left overs - everything produced was consumed
    ROSE_ASSERT(names.size() == 0);
    ROSE_ASSERT(nodes.size() == 1); // SgGlobal remains on the stack
    ROSE_ASSERT(types.size() == 0);
    ROSE_ASSERT(seqs.size()  == 0);
    ROSE_ASSERT(theGlobalScope != NULL);

    //~ ROSE_ASSERT(sb::emptyScopeStack());
  }

}

SgNode* popBuiltAST()
{
  return csharp_translator::pop<SgNode>(csharp_translator::nodes);
}
