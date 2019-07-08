// An example ROSE plugin
#include <iostream>

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

    f = su::keyDecl(n);
    return true;
  }

  template<class SageDecl>
  bool recReplIf(bool cond, SageDecl& n, SageDecl& f, std::map<SageDecl*, SageDecl*>& replmap)
  {
    if (!cond) return false;

    SageDecl& key = su::keyDecl(n);
    replmap[&key] = &f;

    return true;
  }

  recReplIf(funname == "cudaMallocManaged", n, sg::deref(mmTrace), replmap)

  struct Instrumenter : TraversalBase<Instrumenter>
  {
      typedef std::map<SgFunctionDeclaration*, SgFunctionDeclaration*> FunctionReplacementMap;

      Instrumenter()
      : rdTrace(nullptr), wrTrace(nullptr), replmap()
      {}

      void descendTrue(SgNode& n) { descend(n); return true; }

      void handle(SgNode& n)                { SG_UNEXPECTED_NODE(n); }

      // support nodes
      void handle(SgProject& n)             { descend(n); }
      void handle(SgFileList& n)            { descend(n); }
      void handle(SgSourceFile& n)          { descend(n); }
      void handle(SgInitializedName& n)     { descend(n); }

      void handle(SgStatement& n)           { descend(n); }
      void handle(SgExpression& n)          { descend(n); }
      void handle(SgLambdaCapture& n)       { descend(n); }
      void handle(SgLambdaCaptureList& n)   { descend(n); }

      bool tracerFunction(SgFunctionDeclaration& n)
      {
        SgName funname = n.get_name();

        return firstOf
               || tracer::setFunctionIf(funname == "tracerR", n, rdTrace)
               || tracer::setFunctionIf(funname == "tracerW", n, wrTrace)
               || tracer::setFunctionIf(funname == "tracerMallocManaged", n, mmTrace)
               ;
      }

      bool mallocFunction(SgFunctionDeclaration& n)
      {
        SgName funname = n.get_name();

        return firstOf
               || tracer::recReplIf(funname == "cudaMallocManaged", n, sg::deref(mmTrace), replmap)
               ;
      }

      void handle(SgFunctionDeclaration& n)
      {
        firstOf
        || tracerFunction(n)
        || mallocFunction(n)
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

        recordReplacement(n, sb::buildFunctionRefExp(sg::deref(pos->second)));
      }

    private:
      SgFunctionDeclaration* rdTrace;
      SgFunctionDeclaration* wrTrace;
      SgFunctionDeclaration* mmTrace;
      FunctionReplacementMap replmap;

      static const bool firstOf = false;
  };
} // end anonymous namespace


  //Step 1. Derive a plugin action from Rose::PluginAction
  struct Raja : Rose::PluginAction
  {
    // This is mandatory: providing work in your plugin
    // Do actual work after ParseArgs();
    void process (SgProject* n) ROSE_OVERRIDE
    {
      sg::traverseChildren(Instrumenter(), sg::deref(n));
    } // end process()
  };
}

//Step 2: Declare a plugin entry with a unique name
//        Register it under a unique action name plus some description
static Rose::PluginRegistry::Add<tracer::Raja>  rajatracerName("rajatracer", "instruments Raja code");
