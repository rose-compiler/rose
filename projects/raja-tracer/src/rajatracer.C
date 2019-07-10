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
    void _descend(SgNode& n, Derived& self)
    {
      self = sg::traverseChildren(self, n);
    }

    void descend(SgNode& n) { _descend(n, static_cast<Derived&>(*this)); }
  };

  bool setFunctionIf(bool cond, SgFunctionDeclaration& n, SgFunctionDeclaration*& f)
  {
    if (!cond) return false;

    f = &su::keyDecl(n);
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
      AnyTransform(const AnyTransform& other)
      : tf(other.tf ? other.tf->clone() : nullptr)
      {}

      AnyTransform& operator=(const AnyTransform& other)
      {
        AnyTransform tmp(other);

        swap(this->tf, tmp.tf);
        return *this;
      }

      // dtor
      ~AnyTransform() = default;

      // business logic
      void execute() const { tf->execute(); }

      std::unique_ptr<BaseTransform> tf;
    };

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
        SgExpression&  newexp = sg::deref(si::deepCopy(&oldexp));
        SgExprListExp& args   = sg::deref(sb::buildExprListExp(&newexp));
        SgExpression&  wrpexp = sg::deref(sb::buildFunctionCallExp(&wrapperfn, &args));

        si::replaceExpression( &oldexp, &wrpexp );
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

      bool descendTrue(SgNode& n) { descend(n); return true; }

      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // support nodes
      void handle(SgProject& n)             { descend(n); }
      void handle(SgFileList& n)            { descend(n); }
      void handle(SgSourceFile& n)          { descend(n); }
      void handle(SgInitializedName& n)     { descend(n); }
      void handle(SgFunctionParameterList&) { /* skip; */ }

      void handle(SgStatement& n)           { descend(n); }
      //~ void handle(SgExpression& n)          { descend(n); }
      void handle(SgLambdaCapture& n)       { descend(n); }
      void handle(SgLambdaCaptureList& n)   { descend(n); }

      // picks
      bool tracerFunction(SgFunctionDeclaration& n)
      {
        SgName funname = n.get_name();

        return matchFirstOf
               || setFunctionIf(funname == "traceR",              n, trRead)
               || setFunctionIf(funname == "traceW",              n, trWrite)
               || setFunctionIf(funname == "traceRW",             n, trReadWrite)
               || setFunctionIf(funname == "tracerMallocManaged", n, trCudaMallocManaged)
               || setFunctionIf(funname == "malloc",              n, trMalloc)
               ;
      }

      bool skipInstrumentation(SgFunctionDeclaration&)
      {
        // delay descending into function bodies until the main
        //   instrumentation functions have been identified.
        return !(trReadWrite && trWrite && trRead);
      }

      template <class Action>
      void record(const Action& action)
      {
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

      void handle(SgFunctionDeclaration& n)
      {
        matchFirstOf
        || tracerFunction(n)
        || mallocFunction(n)
        || skipInstrumentation(n)
        || descendTrue(n)
        ;
      }

      void handle(SgFunctionRefExp& n)
      {
        SgFunctionDeclaration*           fundecl = n.getAssociatedFunctionDeclaration();
        if (!fundecl) return;

        SgFunctionDeclaration&           key = su::keyDecl(*fundecl);
        FunctionReplacementMap::iterator pos = replmap.find(&key);

        if (pos == replmap.end()) return;
        ROSE_ASSERT(pos->second);

        record( instrument::repl(n, sg::deref(sb::buildFunctionRefExp(pos->second))) );
      }

      void instrumentRead(SgExpression& n)
      {
        if (n.isUsedAsLValue()) return;

        ROSE_ASSERT(trRead);
        record( instrument::wrap(n, sg::deref(sb::buildFunctionRefExp(trRead))) );
      }
/*
      void handle(SgPointerDerefExp& n)
      {
        descend(n);

        instrumentRead(n);
      }

      void handle(SgPntrArrRefExp& n)
      {
        descend(n);

        instrumentRead(n);
      }
      void handle(SgInitializer& n)
      {
        std::cerr << typeid(n).name() << std::endl;
        descend(n);
      }
*/

      void handle(SgAssignInitializer& n)
      {
        descend(n);

        // instrument read operations:
        //   => all non-temporary values (l-values)
        //      that are not used in a write context.
        if (!n.isLValue() || n.isUsedAsLValue()) return;

        ROSE_ASSERT(trRead);
        record( instrument::wrap(n, instrument::operand, sg::deref(sb::buildFunctionRefExp(trRead))) );
      }

      void handle(SgVarRefExp& n)
      {
        if (!si::isReferenceType(&su::skipTypeModifier(sg::deref(n.get_type())))) return;

        handle(static_cast<SgExpression&>(n));
      }

      void handle(SgExpression& n)
      {
        descend(n);

        // instrument read operations:
        //   => all non-temporary values (l-values)
        //      that are not used in a write context.
        if (!n.isLValue() || n.isUsedAsLValue()) return;

        ROSE_ASSERT(trRead);
        record( instrument::wrap(n, sg::deref(sb::buildFunctionRefExp(trRead))) );
      }

      void handle(SgAssignOp& n)
      {
        descend(n);

        ROSE_ASSERT(trWrite);
        record( instrument::wrap(n, instrument::lhs, sg::deref(sb::buildFunctionRefExp(trWrite))) );
      }

      void executeInstrumentation()
      {
        for (instrument::AnyTransform& t : replacements)
          t.execute();
      }

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


  struct Raja : Rose::PluginAction
  {
    // This is mandatory: providing work in your plugin
    // Do actual work after ParseArgs();
    void process (SgProject* n) ROSE_OVERRIDE
    {
      Explorer expl = sg::traverseChildren(Explorer(), sg::deref(n));

      expl.executeInstrumentation();
    } // end process()
  };
}

//Step 2: Declare a plugin entry with a unique name
//        Register it under a unique action name plus some description
static Rose::PluginRegistry::Add<tracer::Raja>  rajatracerName("rajatracer", "instruments Raja code");
