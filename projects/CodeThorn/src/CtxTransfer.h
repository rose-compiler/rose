
#ifndef CTX_TRANSFER_H
#define CTX_TRANSFER_H 1

#include <iterator>
#include <algorithm>

#include <sage3basic.h>
#include <sageGeneric.h>
#include <Labeler.h>

#include "CtxLattice.h"

namespace CodeThorn
{

//
// auxiliary functions

namespace
{
  inline
  bool isCallContextModifier(Labeler& labeler, Label lbl)
  {
    return (  labeler.isFunctionCallLabel(lbl)
           || labeler.isFunctionCallReturnLabel(lbl)
           );
  }

  //
  // auxiliary functors

  //! calls transfer on the TfObject.
  //! \tparam TfObject either a node (source) or an edge
  template <class TfObject>
  struct ComponentTransfer
  {
      ComponentTransfer(DFTransferFunctions& comptrans, TfObject tfobj)
      : component(comptrans), object(tfobj)
      {}

      template <class Key>
      void operator()(std::pair<const Key, Lattice*>& p) const
      {
        component.transfer(object, sg::deref(p.second));
      }

    private:
      DFTransferFunctions& component;
      TfObject             object;
  };

  template <class TfObject>
  inline
  ComponentTransfer<TfObject>
  componentTransfer(DFTransferFunctions& comptrans, TfObject tfobj)
  {
    return ComponentTransfer<TfObject>(comptrans, tfobj);
  }
}


template <class CallContext>
struct CtxAnalysis;

//
// implements call context transfers on top of underlying components

//! Transfer functions for context-sensitive dataflow analysis.
//!
//! \tparam  CallContext an implementation of a CallContext
template <class CallContext>
struct CtxTransfer : DFTransferFunctions
{
    typedef DFTransferFunctions     base;
    typedef CtxLattice<CallContext> ctx_lattice_t;

    //! Initializes a new transfer object.
    //! \param comptrans   the component transfer functions (e.g., for reaching
    //!                    definitions).
    //! \param ctxanalysis the context sensitive analysis class.
    CtxTransfer(DFTransferFunctions& comptrans, CtxAnalysis<CallContext>& ctxanalysis)
    : base(), component(comptrans), analysis(ctxanalysis)
    {}

    //! sets its and its' component's abstraction layer
    void setProgramAbstractionLayer(ProgramAbstractionLayer* pal) ROSE_OVERRIDE
    {
      ROSE_ASSERT(pal);

      base::setProgramAbstractionLayer(pal);

      component.setProgramAbstractionLayer(pal);
    }

    //! actual transfer function.
    //! \details computes call context changes for call and return labels
    //! \note the method does not override
    void transfer(Label lbl, ctx_lattice_t& lat);

    //! handles transfers on edges
    //! \note CURRENTLY ONLY CALLED form post-info processing
    void transfer(Label lbl, Lattice& element) ROSE_OVERRIDE
    {
      ctx_lattice_t& lat = dynamic_cast<ctx_lattice_t&>(element);

      this->transfer(lbl, lat);
      std::for_each(lat.begin(), lat.end(), componentTransfer(component, lbl));
    }

    //! this method is invoked from the solver/worklist algorithm and
    //!   calls the component's transfer function for each context.
    //!   It also calls the concrete transfer function (above) to
    //!   handle context changes.
    void transfer(Edge edge, Lattice& element) ROSE_OVERRIDE;


  private:
    DFTransferFunctions&      component; //!< The component transfer
    CtxAnalysis<CallContext>& analysis;  //!< The analysis objects
};


//
// implementation

template <class CallContext>
void CtxTransfer<CallContext>::transfer(Label lbl, ctx_lattice_t& lat)
{
  CodeThorn::Labeler& labeler = sg::deref(getLabeler());

  // we are only interested in Call context modifications
  if (!isCallContextModifier(labeler, lbl)) return;

  ctx_lattice_t       tmp(lat.componentFactory());

  // mv lattices to tmp, then fill lat
  tmp.swap(lat);

  if (labeler.isFunctionCallLabel(lbl))
    CallContext::callInvoke(tmp, lat, analysis, labeler, lbl);
  else
    CallContext::callReturn(tmp, lat, analysis, labeler, lbl);

  // Sub-lattices in tmp WILL BE DELETED !!!
}


template <class CallContext>
void CtxTransfer<CallContext>::transfer(Edge edge, Lattice& element)
{
  ctx_lattice_t& lat = dynamic_cast<ctx_lattice_t&>(element);

  this->transfer(edge.source(), element);
  std::for_each(lat.begin(), lat.end(), componentTransfer(component, edge));
}

} // namespace CodeThorn

#endif /* CTX_TRANSFER_H */
