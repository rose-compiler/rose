#ifndef CTX_ANALYSIS_H
#define CTX_ANALYSIS_H 1

//! \author Peter Pirkelbauer

#include "CtxLattice.h"
#include "CtxTransfer.h"
#include "CtxAttribute.h"

namespace CodeThorn
{

//! implements the Decorator pattern to enhance the
//!   PropertyStateFactory with context specific functionality
template <class CallContext>
struct CtxPropertyStateFactory : PropertyStateFactory
{
    typedef CallContext           context_t;
    typedef CtxLattice<context_t> context_lattice_t;

    explicit
    CtxPropertyStateFactory(PropertyStateFactory& compFac)
    : compFactory(compFac)
    {}

    context_lattice_t* create() ROSE_OVERRIDE
    {
      return new context_lattice_t(compFactory);
    }

    PropertyStateFactory& componentFactory()
    {
      return compFactory;
    }

  private:
    PropertyStateFactory& compFactory;
};


//! analysis class that wraps a context-sensitive analysis around
//!   a non-context-sensitive forward analysis.
template <class CallContext>
struct CtxAnalysis : DFAnalysisBase
{
    typedef DFAnalysisBase        base;
    typedef CallContext           context_t;
    typedef CtxLattice<context_t> context_lattice_t;

    CtxAnalysis(PropertyStateFactory& compFactory, DFTransferFunctions& compTransfer)
    : base(), ctxFactory(compFactory), ctxTransfer(compTransfer, *this)
    {
      _transferFunctions = &ctxTransfer;
      _transferFunctions->setInitialElementFactory(&ctxFactory);
    }

    CtxAnalysis(PropertyStateFactory& compFactory, DFTransferFunctions& compTransfer, const context_lattice_t& init)
    : base(), ctxFactory(compFactory), ctxTransfer(compTransfer, *this, init)
    {
      _transferFunctions = &ctxTransfer;
      _transferFunctions->setInitialElementFactory(&ctxFactory);
    }

/*
    void initializeExtremalValue(Lattice* element) ROSE_OVERRIDE
    {
      ROSE_ASSERT(element);

      // the initial extremalValue will be set to an empty lattice
      context_lattice_t& lat = dynamic_cast<context_lattice_t&>(*element);
      Lattice*           sub = lat.componentFactory().create();

      ROSE_ASSERT(lat.isBot());
      // subAnalysis.initializeExtremalValue(sub);
      lat[context_t()] = sub;
    }


    void initializeTransferFunctions() ROSE_OVERRIDE
    {
      base::initializeTransferFunctions();

      subAnalysis.initializeTransferFunctions();
    }

    Lattice* initializeGlobalVariables(SgProject* root) ROSE_OVERRIDE
    {
      // subAnalysis.initializeGlobalVariables(root);
      return base::initializeGlobalVariables(root);
    }
*/

/*
    // shadows non-virtual function in base class
    void initialize(SgProject* root, bool variableIdForEachArrayElement = false) ROSE_OVERRIDE
    void initialize(SgProject* root, bool createCFG=true, ProgramAbstractionLayer* programAbstractionLayer=nullptr, bool variableIdForEachArrayElement = false)
    {
      base::initialize(root, variableIdForEachArrayElement);

      // \todo do we need to initialize component?
      subAnalysis.initialize(root, variableIdForEachArrayElement);
    }
*/

    const CtxLattice<CallContext>&
    getCtxLattice(Label lbl)
    {
      return dynamic_cast<CtxLattice<CallContext>&>(SG_DEREF(getPreInfo(lbl)));
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(const SgStatement& stmt)
    {
      SgStatement& call    = const_cast<SgStatement&>(stmt);
      Labeler&     labeler = *getLabeler();

      //~ std::cerr << call.unparseToString() << std::endl;
      return getCtxLattice( labeler.functionCallLabel(&call) );
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(const SgCallExpression& exp)
    {
      return getCallSiteLattice(sg::ancestor<SgStatement>(exp));
    }

    /// retrieves the lattice from the call site
    const CtxLattice<CallContext>&
    getCallSiteLattice(Label lblret)
    {
      Labeler& labeler = *getLabeler();
      ROSE_ASSERT(labeler.isFunctionCallReturnLabel(lblret) && (lblret.getId() > 0));
      
      Label    lblcall(lblret.getId()-1);
      
      return getCtxLattice(lblcall);
      //~ SgStatement* call = SG_ASSERT_TYPE(SgStatement, astNode(labeler, lblret));
      //~ return getCallSiteLattice(*call);
    }


    CtxPropertyStateFactory<context_t>& factory()  { return ctxFactory;  }
    CtxTransfer<context_t>&             transfer() { return ctxTransfer; }

  protected:
    CtxAttribute<CallContext>*
    createDFAstAttribute(Lattice* elem) ROSE_OVERRIDE
    {
      context_lattice_t* lat = dynamic_cast<context_lattice_t*>(elem);

      return new CtxAttribute<CallContext>(sg::deref(lat));
    }

  private:
    CtxPropertyStateFactory<context_t> ctxFactory;
    CtxTransfer<context_t>             ctxTransfer;
};

} // namespace CodeThorn

#endif /* CTX_ANALYSIS_H */
