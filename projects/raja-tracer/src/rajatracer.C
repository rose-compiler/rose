// An example ROSE plugin
#include <iostream>
#include <memory>

//Mandatory include headers
#include "rose.h"
#include "plugin.h"

// optional headers
#include "sageGeneric.h"
#include "sageBuilder.h"
#include "sageUtility.h"
#include "sageInterface.h"

namespace su = SageUtil;
namespace sb = SageBuilder;
namespace si = SageInterface;

namespace tracer
{
namespace
{
  template <class Derived>
  struct TraversalBase
  {
    static
    void _descend(SgNode& n, Derived&& self);

    void descend(SgNode& n) { _descend(n, std::move(static_cast<Derived&>(*this))); }
  };


  template <class Derived>
  void TraversalBase<Derived>::_descend(SgNode& n, Derived&& self)
  {
    self = std::move(sg::traverseChildren(std::move(self), n));
  }

  template <class SageDecl>
  bool setKeyDeclIf(bool cond, SageDecl& dcl, SageDecl*& var)
  {
    if (!cond) return false;

    var = &su::keyDecl(dcl);
    ROSE_ASSERT(var);
    return true;
  }

  template<class SageDecl>
  bool recReplIf(bool cond, SageDecl& n, SageDecl* f, std::map<SageDecl*, SageDecl*>& replmap)
  {
    if (!cond) return false;

    ROSE_ASSERT(f);
    SageDecl& key = su::keyDecl(n);
    replmap[&key] = f;

    return true;
  }

#if 0
  struct NeedsInstrumentation : sg::DispatchHandler<bool>
  {
      void handle(const SgNode& n)          { SG_UNEXPECTED_NODE(n); }
      void handle(const SgExpression&)      { /* default */ }
      void handle(const SgPointerDerefExp&) { res = true;   }
      void handle(const SgPntrArrRefExp&)   { res = true;   }
  };

  bool needsInstrumentation(const SgNode& n)
  {
    return sg::dispatch(NeedsInstrumentation(), &n);
  }
#endif
} // end anonymous namespace

  namespace instrument
  {
    //
    // accessor functions

    SgExpression& lhs(SgBinaryOp& n)              { return sg::deref(n.get_lhs_operand()); }
    SgExpression& operand(SgAssignInitializer& n) { return sg::deref(n.get_operand()); }
    SgExpression& identity(SgExpression& n)       { return n; }


    //
    // Generic Polymorphic actions



    /// Abstract base class to define essential polymorphic Transform functions.
    struct BaseTransform
    {
      virtual ~BaseTransform() = default;

      virtual void execute() const = 0;
      virtual BaseTransform* clone() const = 0;
    };

    /// Polymorphic wrapper for concrete actions.
    template <class ConcreteTransform>
    struct PolyTransform : BaseTransform
    {
      explicit
      PolyTransform(const ConcreteTransform& concreteTf)
      : tf(concreteTf)
      {}

      void execute() const ROSE_OVERRIDE
      {
        tf.execute();
      }

      PolyTransform<ConcreteTransform>*
      clone() const ROSE_OVERRIDE
      {
        return new PolyTransform(*this);
      }

      ConcreteTransform tf;
    };

    /// Generic walue wrapper around polymorphic actions.
    struct AnyTransform
    {
      // wrapping value ctors
      template <class ConcreteTransform>
      AnyTransform(const ConcreteTransform& a)
      : tf(new PolyTransform<ConcreteTransform>(a))
      {}

      template <class ConcreteTransform>
      AnyTransform(ConcreteTransform&& a)
      : tf(new PolyTransform<ConcreteTransform>(std::move(a)))
      {}

      // move ctor + assignment
      AnyTransform(AnyTransform&& other) = default;
      AnyTransform& operator=(AnyTransform&& other) = default;

      // copy ctor + assignment
      AnyTransform(const AnyTransform& other) = delete;
      AnyTransform& operator=(const AnyTransform& other) = delete;

      // dtor
      ~AnyTransform() = default;

      // business logic
      void execute() const { tf->execute(); }

      std::unique_ptr<BaseTransform> tf;
    };

    template <class U, class V>
    bool equals(U, V)
    {
      return false;
    }

    template <class U>
    bool equals(U lhs, U rhs)
    {
      return lhs == rhs;
    }

    template <class SageNode, class Accessor>
    struct Wrapper
    {
      explicit
      Wrapper(SageNode& n, Accessor accfn, SgExpression& wrapfn)
      : astnode(n), accessor(accfn), wrapperfn(wrapfn)
      {}

      void execute() const
      {
        SgExpression&  oldexp = accessor(astnode);
        SgExpression&  newexp = sg::deref( si::deepCopy(&oldexp) );
        SgExprListExp& args   = sg::deref(sb::buildExprListExp(&newexp));
        SgExpression&  wrpexp = sg::deref(sb::buildFunctionCallExp(&wrapperfn, &args));

        si::replaceExpression( &oldexp, &wrpexp, false /* do not keep */ );
      }

      SageNode&     astnode;
      Accessor      accessor;
      SgExpression& wrapperfn;
    };

    template <class SageNode, class Accessor>
    inline
    Wrapper<SageNode, Accessor>
    wrap(SageNode& n, Accessor accfn, SgExpression& wrapfn)
    {
      return Wrapper<SageNode, Accessor>(n, accfn, wrapfn);
    }

    template <class SageNode>
    inline
    auto
    wrap(SageNode& n, SgExpression& wrapfn) -> decltype( wrap(n, identity, wrapfn) )
    {
      return wrap(n, identity, wrapfn);
    }

    struct Replacer
    {
      explicit
      Replacer(SgExpression& oldnode, SgExpression& newnode)
      : oldexp(oldnode), rplexp(newnode)
      {}

      void execute() const
      {
        si::replaceExpression( &oldexp, &rplexp );
      }

      SgExpression& oldexp;
      SgExpression& rplexp;
    };

    Replacer
    repl(SgExpression& oldnode, SgExpression& newnode)
    {
      return Replacer(oldnode, newnode);
    }
  }


  struct Explorer : TraversalBase<Explorer>
  {
      typedef std::map<SgFunctionDeclaration*, SgFunctionDeclaration*> FunctionReplacementMap;
      typedef std::vector<instrument::AnyTransform>                    InstrumentationStore;

      Explorer()
      : trRead(nullptr), trWrite(nullptr), trReadWrite(nullptr), trCudaMallocManaged(nullptr),
        replmap(), replacements()
      {}

      Explorer(Explorer&&) = default;
      Explorer& operator=(Explorer&&) = default;

      Explorer(const Explorer&) = delete;
      Explorer& operator=(const Explorer&) = delete;

      //
      // auxiliary functions
      bool descendTrue(SgNode& n) { descend(n); return true; }

      // picks
      bool tracerFunction(SgFunctionDeclaration& n)
      {
        SgName name = n.get_name();

        return matchFirstOf
               || setKeyDeclIf(name == "traceR",              n, trRead)
               || setKeyDeclIf(name == "traceW",              n, trWrite)
               || setKeyDeclIf(name == "traceRW",             n, trReadWrite)
               || setKeyDeclIf(name == "tracerMallocManaged", n, trCudaMallocManaged)
               || setKeyDeclIf(name == "malloc",              n, trMalloc)
               ;
      }

      bool skipInstrumentation(const SgDeclarationStatement&)
      {
        // delay descending into function bodies until the main
        //   instrumentation functions have been identified.
        return !(trReadWrite && trWrite && trRead);
      }

      template <class Action>
      void record(const Action& action)
      {
        if ((replacements.size() % 1024) == 0)
          std::cerr << '\r' << replacements.size() << "  ";

        replacements.emplace_back(action);
      }

      bool mallocFunction(SgFunctionDeclaration& n)
      {
        SgName funname = n.get_name();

        return matchFirstOf
               || recReplIf(funname == "cudaMallocManaged", n, trCudaMallocManaged, replmap)
               || recReplIf(funname == "malloc",            n, trMalloc,            replmap)
               ;
      }

      void handleExpr(SgExpression& n);
      void instrumentExpr(SgExpression& n);
      bool skipExpr(SgExpression& n);

      //
      // handlers over AST

      void handle(SgNode& n)                 { SG_UNEXPECTED_NODE(n); }

      // support nodes
      void handle(SgProject& n)              { descend(n); }
      void handle(SgFileList& n)             { descend(n); }
      void handle(SgSourceFile& n)           { descend(n); }
      void handle(SgInitializedName& n)      { descend(n); }

      void handle(SgFunctionParameterList&)  { /* skip; */ }

      void handle(SgStatement& n)            { descend(n); }

      void handle(SgDeclarationStatement& n)
      {
        if (skipInstrumentation(n)) return;

        descend(n);
      }

      void handle(SgFunctionDeclaration& n)
      {
        matchFirstOf
        || tracerFunction(n)       /* needs to be ordered before skipInstrumentation. */
        || skipInstrumentation(n)
        || mallocFunction(n)
        || descendTrue(n)
        ;
      }

      void handle(SgLambdaCapture& n)        { descend(n); }
      void handle(SgLambdaCaptureList& n)    { descend(n); }
      void handle(SgCudaKernelExecConfig& n) { descend(n); }

      void handle(SgExpression& n)           { handleExpr(n); }
      void handle(SgExprListExp& n)          { descend(n); }

      void handle(SgConstructorInitializer& n)
      {
        SgMemberFunctionDeclaration* dcl = n.get_declaration();

        // if (dcl) std::cerr << dcl->unparseToString() << std::endl;
        if (!dcl || !dcl->get_type()) return;

        descend(n);
      }

      void handle(SgArrowExp& n)
      {
        if (skipExpr(n)) return;

        descend(instrument::lhs(n));

        instrumentExpr(n);
      }

      void handle(SgFunctionRefExp& n)
      {
        if (skipExpr(n)) return;

        SgFunctionDeclaration*           fundecl = n.getAssociatedFunctionDeclaration();
        if (!fundecl) return;

        SgFunctionDeclaration&           key = su::keyDecl(*fundecl);
        FunctionReplacementMap::iterator pos = replmap.find(&key);

        if (pos == replmap.end()) return;
        ROSE_ASSERT(pos->second);

        record( instrument::repl(n, sg::deref(sb::buildFunctionRefExp(pos->second))) );
      }

      void handle(SgAssignInitializer& n)
      {
        if (skipExpr(n)) return;

        // skip traversal of operand (mixed up lvalue handling)
        ROSE_ASSERT(!isSgCudaKernelExecConfig(&instrument::operand(n)));
        descend(instrument::operand(n));

        // instrument read operations:
        //   => all non-temporary values (l-values)
        //      that are not used in a write context.
        if (!n.isLValue() || n.isUsedAsLValue()) return;

        ROSE_ASSERT(trRead);
        record( instrument::wrap(n, instrument::operand, sg::deref(sb::buildFunctionRefExp(trRead))) );
      }

      void handle(SgVarRefExp& n)
      {
        if (skipExpr(n)) return;

        // skip over values
        // \todo restrict to local variables
        if (!si::isReferenceType(&su::skipTypeModifier(sg::deref(n.get_type())))) return;

        handleExpr(n);
      }

      void handle(SgFunctionCallExp& n)
      {
        if (skipExpr(n)) return;

        // skip any calls to templates
        //   lvalues for arguments cannot be resolved properly.
        if (isSgNonrealRefExp(n.get_function())) return;

        // Only reference types can be Lvalues, so we can skip other calls
        SgType& ty = su::skipTypeModifier(sg::deref(n.get_type()));

        if (!si::isReferenceType(&ty))
        {
          descend(n);
          return;
        }

        handleExpr(n);
      }

/*
      void handle(SgAssignOp& n)
      {
        descend(n);

        ROSE_ASSERT(trWrite);
        record( instrument::wrap(n, instrument::lhs, sg::deref(sb::buildFunctionRefExp(trWrite))) );
      }
*/

      void executeTransformation()
      {
        size_t i = 0;

        for (instrument::AnyTransform& t : replacements)
        {
          t.execute();

          if ((i % 32) == 0)
            std::cerr << '\r' << i << "   ";

          ++i;
        }
      }

      size_t numTransforms() const { return replacements.size(); }

    private:
      SgFunctionDeclaration* trRead;
      SgFunctionDeclaration* trWrite;
      SgFunctionDeclaration* trReadWrite;
      SgFunctionDeclaration* trCudaMallocManaged;
      SgFunctionDeclaration* trMalloc;
      FunctionReplacementMap replmap;
      InstrumentationStore   replacements;

      // a simple tag
      // - false is required to start matching patterns.
      static constexpr bool matchFirstOf = /* do not change */ false;
  };

  void Explorer::handleExpr(SgExpression& n)
  {
    // skip anything inside a template that does not have
    //   a proper type.
    if (skipExpr(n)) return;

    descend(n);

    instrumentExpr(n);
  }

  bool Explorer::skipExpr(SgExpression& n)
  {
    return isSgTypeUnknown(n.get_type());
  }

  void Explorer::instrumentExpr(SgExpression& n)
  {
    // instrument read operations:
    //   => all non-temporary values (l-values)
    //      that are not used in a write context.
    if (!n.isLValue() || !isSgExpression(n.get_parent())) return;

    if (!isSgExprListExp(n.get_parent()) && n.isUsedAsLValue())
    {
      ROSE_ASSERT(trWrite);
      record( instrument::wrap(n, sg::deref(sb::buildFunctionRefExp(trWrite))) );
    }
    else
    {
      ROSE_ASSERT(trRead);
      record( instrument::wrap(n, sg::deref(sb::buildFunctionRefExp(trRead))) );
    }
  }



  struct Raja : Rose::PluginAction
  {
    // This is mandatory: providing work in your plugin
    // Do actual work after ParseArgs();
    void process (SgProject* n) ROSE_OVERRIDE
    {
      std::cerr << "\nRT traverses..." << std::endl;
      Explorer expl = sg::traverseChildren(Explorer(), sg::deref(n));
      std::cerr << '\r' << expl.numTransforms() << " transformations found." << std::endl;

      std::cerr << "\nRT transforms..." << std::endl;
      expl.executeTransformation();
      std::cerr << '\r' << expl.numTransforms() << " transformations executed." << std::endl;

      std::cerr << "\nRT done." << std::endl;
    } // end process()
  };
}

//Step 2: Declare a plugin entry with a unique name
//        Register it under a unique action name plus some description
static Rose::PluginRegistry::Add<tracer::Raja>  rajatracerName("rajatracer", "instruments Raja code");
