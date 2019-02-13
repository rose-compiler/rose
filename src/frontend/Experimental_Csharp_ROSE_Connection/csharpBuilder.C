
#include <iostream>
#include <map>
#include <vector>
#include <sstream>

#include "sageBuilder.h"
#include "sageGeneric.h"

#include "csharpBuilder.h"


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
    std::map<int, SgVariableDeclaration*>       vardecls;
    std::map<int, SgClassDeclaration*>          classdecls;
    std::map<int, SgMemberFunctionDeclaration*> fundecls;

    //
    // helper functions to access the builder stacks

    template <class SageNode, class BaseSageNode>
    SageNode* pop_if(std::vector<BaseSageNode*>& cont, bool cond)
    {
      ROSE_ASSERT(cont.size() > 0);
      if (!cond) return NULL;

      BaseSageNode* node = nodes.back();

      nodes.pop_back();
      return sg::assert_sage_type<SageNode>(node);
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
      return sg::assert_sage_type<SageNode>(cont.back());
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

    //
    // maker (aka builder) functions

    SgInitializer& mkInit(SgExpression& exp, SgType& ty)
    {
      SgInitializer* res = sb::buildAssignInitializer(&exp, &ty);

      return sg::deref(res);
    }

    SgInitializer* mkInit(SgExpression* exp, SgType* ty)
    {
      return &mkInit(sg::deref(exp), sg::deref(ty));
    }

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

    template <class U, class V>
    U as(V val)
    {
      std::stringstream str;
      U                 res;

      str << val;
      str >> res;

      return res;
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
    nodes.push_back(n);
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
          SgFunctionDefinition*        fdef   = fdecl->get_definition();
          SgFunctionParameterList*     params = fdecl->get_parameterList();

          // \todo ??? is this the right scope, or do we ???
          //       ??? need to push the parameter scope  ???
          sb::pushScopeStack(fdef);
          res = params;
          msg = "paramlist";
          break;
        }

      case METHODBODYSEQ:
        {
          SgMemberFunctionDeclaration* fdecl  = top<SgMemberFunctionDeclaration>(nodes);
          SgFunctionDefinition*        fdef   = fdecl->get_definition();
          SgBasicBlock*                body   = sg::deref(fdef).get_body();

          ROSE_ASSERT(body);
          sb::pushScopeStack(body);
          res = body;
          msg = "methodbody";
          break;
        }

      default:
        msg = "NOT IMPLEMENTED seq (C++): " + kind;
        break;
    }

    std::cerr << "open: " << msg << std::endl;
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
      (cont.*adder)(sg::assert_sage_type<SageElement>(n));
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

  struct Incorporate
  {
    Incorporate(std::vector<SgNode*>& nodestack, size_t numnodes)
    : nodes(nodestack), num(numnodes)
    {
      ROSE_ASSERT(nodes.size() >= num);
    }

    template <class SageSequence, class SageElement>
    void incorporate(SageSequence& parent, void (SageSequence::*fn)(SageElement*))
    {
      std::for_each(nodes.end()-num, nodes.end(), nodeAdder(parent, fn));
      pop_n(nodes, num);
    }

    void handle(SgNode& n)                  { sg::unexpected_node(n); }
    void handle(SgClassDefinition& n)       { incorporate(n, &SgClassDefinition::append_member); }
    void handle(SgFunctionParameterList& n) { incorporate(n, &SgFunctionParameterList::append_arg); }
    void handle(SgGlobal& n)                { incorporate(n, &SgGlobal::append_declaration); }
    void handle(SgBasicBlock& n)            { incorporate(n, &SgBasicBlock::append_statement); }

    std::vector<SgNode*>& nodes;
    size_t                num;
  };

  struct ScopePopper
  {
    static inline
    void pop_s() { sb::popScopeStack(); }

    void handle(SgNode&)                    {}
    void handle(SgScopeStatement& n)        { pop_s(); }
    void handle(SgFunctionParameterList& n) { pop_s(); } // pop associated function def scope
  };

  void closeSeq()
  {
    size_t  pos  = pop(seqs);
    ROSE_ASSERT(pos <= nodes.size());

    SgNode* seq  = nodes.at(pos);
    size_t  num  = nodes.size() - pos;
    ROSE_ASSERT(num > 0); // needs to contain seq

    std::cerr << "closing (C++) " << typeid(sg::deref(seq)).name() << std::endl;

    sg::dispatch(Incorporate(nodes, num-1), seq);
    sg::dispatch(ScopePopper(), seq);

    SgNode* n = pop(nodes);
    ROSE_ASSERT(n == seq);
  }

  void predefinedType(const char* tyname)
  {
    std::string tn = tyname;
    SgType*     ty = NULL;

    if (tn == "int")
    {
      ty = sb::buildIntType();
    }
    else if (tn == "void")
    {
      ty = sb::buildVoidType();
    }

    ROSE_ASSERT(ty);
    types.push_back(ty);
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

    push(nodes, usgdecl);
  }

  void varDecl(int uid)
  {
    SgName                 nm = pop(names);
    SgType*                ty = pop(types);
    SgVariableDeclaration* vr = sb::buildVariableDeclaration(nm, ty, NULL);

    push(nodes, vr);
    vardecls[uid] = vr;
  }

  void initVarDecl(int uid)
  {
    SgVariableDeclaration* decl = retrieve(vardecls, uid);
    SgInitializedName*     var  = oneAndOnly(decl);
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

    push(nodes, cd);
    classdecls[uid] = cd;
  }

  void methodDecl(int uid)
  {
    SgName                       nm = pop(names);
    SgType*                      ty = pop(types);
    SgFunctionParameterList*     pl = sb::buildFunctionParameterList(); // pop<SgFunctionParameterList>(nodes);
    SgScopeStatement*            sp = sb::topScopeStack();
    SgMemberFunctionDeclaration* mf = sb::buildDefiningMemberFunctionDeclaration(nm, ty, pl, sp);

    push(nodes, mf);
    fundecls[uid] = mf;
  }

  void stageMethodDecl(int uid)
  {
    SgMemberFunctionDeclaration* mf = retrieve(fundecls, uid);

    push(nodes, mf);
  }

  //
  // expressions

  void valueInitializer()
  {
    SgExpression*  ex   = pop<SgExpression>(nodes);
    SgType*        ty   = pop(types);
    SgInitializer* init = sb::buildAssignInitializer(ex, ty);

    push(nodes, init);
  }

  struct LiteralMaker : sg::DispatchHandler<SgExpression*>
  {
    explicit
    LiteralMaker(std::string str)
    : sg::DispatchHandler<SgExpression*>(), rep(str)
    {}

    void handle(SgNode& n)  { sg::unexpected_node(n); }
    void handle(SgTypeInt&) { res = sb::buildIntVal(as<int>(rep)); }

    std::string rep;
  };

  void literal(const char* rep)
  {
    SgType*        ty   = pop(types);
    SgExpression*  lit  = sg::dispatch(LiteralMaker(rep), ty);

    ROSE_ASSERT(lit);
    push(nodes, lit);
  }

  void basicFinalChecks()
  {
    // no left overs - everything produced was consumed
    ROSE_ASSERT(names.size() == 0);
    ROSE_ASSERT(nodes.size() == 0);
    ROSE_ASSERT(types.size() == 0);
    ROSE_ASSERT(seqs.size()  == 0);
    ROSE_ASSERT(theGlobalScope != NULL);

    //~ ROSE_ASSERT(sb::emptyScopeStack());
  }
}

