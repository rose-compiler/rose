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


  struct Instrumenter : TraversalBase<Instrumenter>
  {
    void handle(SgNode& n)              { SG_UNEXPECTED_NODE(n); }

    // support nodes
    void handle(SgProject& n)           { descend(n); }
    void handle(SgSourceFile& n)        { descend(n); }

    void handle(SgStatement& n)         { descend(n); }
    void handle(SgExpression& n)        { descend(n); }
    void handle(SgLambdaCapture& n)     { descend(n); }
    void handle(SgLambdaCaptureList& n) { descend(n); }
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
