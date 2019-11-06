
#include <algorithm>

#include "CtxCallStrings.h"

#include "CtxAnalysis.h"

namespace CodeThorn
{

namespace
{
  //! \brief copies elements from an input range to an output range,
  //!        iff pred(element) is true. The input type and output type
  //!        can differ.
  //! \tparam InputIterator iterator type of the input range
  //! \tparam OutputIterator iterator type of the output range
  //! \tparam Predicate bool Predicate::operator()(InputIterator::reference) returns true,
  //!         if the element should be copied
  //! \tparam Op transformation operation,
  //!         OutputIterator::value_type Op::operator()(InputIterator::reference)
  //! \param  first an InputIterator, beginning of the input range
  //! \param  last  an InputIterator, one past the end element of the input range
  //! \param  sink  an OutputIterator, beginning of the output range
  //! \param  pred  a Predicate
  //! \param  op    an Operation
  //! \details writes op(*first) to the output range if pred(*first) is true.
  //!
  //!         see also: std::transform, std::remove_copy_if
  template <class InputIterator, class OutputIterator, class Predicate, class Op>
  void transform_if(InputIterator first, InputIterator last, OutputIterator sink, Predicate pred, Op op)
  {
    first = std::find_if(first, last, pred);
    while(first != last)
    {
      *sink = op(*first);
      ++sink; ++first;

      first = std::find_if(first, last, pred);
    }
  }

  struct InvokeHandler
  {
      InvokeHandler(PropertyStateFactory& propfactory, Labeler& labels, Label lbl)
      : factory(propfactory), labeler(labels), label(lbl)
      {}

      template <class Ctx>
      std::pair<Ctx, Lattice*>
      operator()(const std::pair<const Ctx, Lattice*>& entry)
      {
        Ctx tmp(entry.first);

        tmp.callInvoke(labeler, label);

        return std::make_pair(tmp, cloneLattice(factory, sg::deref(entry.second)));
      }

    private:
      PropertyStateFactory& factory;
      Labeler&              labeler;
      Label                 label;
  };


  template <class CallContext>
  void defaultCallInvoke( const CtxLattice<CallContext>& src,
                          CtxLattice<CallContext>& tgt,
                          Labeler& labeler,
                          Label lbl
                        )
  {
    InvokeHandler ih(tgt.componentFactory(), labeler, lbl);

    std::transform(src.begin(), src.end(), std::inserter(tgt, tgt.end()), ih);
  }

  bool
  defaultIsValidReturn(Labeler& labeler, Label invlbl, Label retlbl)
  {
    return astNode(labeler, invlbl) == astNode(labeler, retlbl);
  }

  struct InfiniteReturnHandler
  {
      InfiniteReturnHandler(PropertyStateFactory& propfactory, Labeler& labels, Label lbl)
      : factory(propfactory), labeler(labels), label(lbl)
      {}

      template <class Ctx>
      std::pair<Ctx, Lattice*>
      operator()(const std::pair<const Ctx, Lattice*>& entry)
      {
        Ctx retctx(entry.first);

        ROSE_ASSERT(retctx.size());
        retctx.callReturn(labeler, label);

        // \note none of the sub-lattices are "reused"
        return std::make_pair(retctx, cloneLattice(factory, sg::deref(entry.second)));
      }

    private:
      PropertyStateFactory& factory;
      Labeler&              labeler;
      Label                 label;
  };


  //! Compares relevant call string segments, relevant(@ref calltx) < @ref returnctx
  //! \param callctx   the call string at the call site
  //! \param returnctx the already shortened string at the return site
  //! \details
  //!   On a call return, a call string is shortened. If the call string
  //!   was already at full capacity, we map the result on all substrings
  //!   that have the same postfix.
  bool ltProjected(const FiniteContext& callctx, const FiniteContext& returnctx)
  {
    int dif = static_cast<int>(callctx.size()) - returnctx.size();

    if (dif < 0) dif = 0;
    ROSE_ASSERT(dif < 2);

    return std::lexicographical_compare( callctx.rbegin(),   callctx.rend()-dif,
                                         returnctx.rbegin(), returnctx.rend()
                                       );
  }

  //! extracts the call string from a lattice element and forwards it for comparison.
  bool ltProjectedFull(const FiniteContext& lhs, const std::pair<const FiniteContext, Lattice*>& rhs)
  {
    return ltProjected(lhs, rhs.first);
  }

  //! clones a lattice element
  struct LatticeCloner
  {
      LatticeCloner(PropertyStateFactory& propfact, Lattice& lat)
      : factory(propfact), lattice(lat)
      {}

      template <class Ctx>
      std::pair<const Ctx, Lattice*>
      operator()(const std::pair<const Ctx, Lattice*>& entry)
      {
        return std::make_pair(entry.first, cloneLattice(factory, lattice));
      }

    private:
      PropertyStateFactory& factory;
      Lattice&              lattice;
  };

  //! A functor that is invoked for every lattice flowing over a return edge.
  //! - Every valid return context is mapped onto every feasible context in the caller
  //! - A context is feasible if it has postfix(precall-context) == prefix(return-context)
  struct FiniteReturnHandler
  {
      FiniteReturnHandler( const CtxLattice<FiniteContext>& prelat,
                           CtxLattice<FiniteContext>& tgtlat,
                           PropertyStateFactory& propfactory,
                           Labeler& labels,
                           Label lbl
                         )
      : pre(prelat), tgt(tgtlat), factory(propfactory), labeler(labels), label(lbl)
      {}

      void operator()(const std::pair<const FiniteContext, Lattice*>& entry)
      {
        typedef CtxLattice<FiniteContext>::const_iterator const_iterator;

        if (!entry.first.isValidReturn(labeler, label)) return;

        FiniteContext retctx(entry.first);

        retctx.callReturn(labeler, label);

        const_iterator prelow = pre.lower_bound(retctx);
        const_iterator prepos = std::upper_bound( prelow, pre.end(),
                                                  retctx,
                                                  ltProjectedFull
                                                );

        std::transform( prelow, prepos,
                        std::inserter(tgt, tgt.end()),
                        LatticeCloner(factory, sg::deref(entry.second))
                      );
      }

    private:
      const CtxLattice<FiniteContext>& pre;
      CtxLattice<FiniteContext>&       tgt;
      PropertyStateFactory&            factory;
      Labeler&                         labeler;
      Label                            label;
  };


  struct IsValidReturn
  {
      IsValidReturn(Labeler& labels, Label lbl)
      : labeler(labels), label(lbl)
      {}

      template <class Pair>
      bool operator()(Pair& p)
      {
        return p.first.isValidReturn(labeler, label);
      }

    private:
      Labeler& labeler;
      Label    label;
  };
}


//
// InfiniteContext

bool InfiniteContext::isValidReturn(Labeler& labeler, Label retlbl) const
{
  return defaultIsValidReturn(labeler, back(), retlbl);
}


void InfiniteContext::callInvoke(const Labeler&, Label lbl)
{
  push_back(lbl);
}


bool InfiniteContext::callerOf(const InfiniteContext& target, Label callsite) const
{
  // target is invoked from this, if 
  // (1) the target's last label is callsite
  // (2) this' call string is one label shorter than target's call string
  // (3) if all labels in the common subrange match 
  return (  target.back() == callsite
         && (this->size() + 1) == target.size()
         && std::equal(begin(), end(), target.begin())
         );
}

bool InfiniteContext::operator==(const InfiniteContext& that) const
{
  const context_string& self = *this;
  
  return self == that;
}


void InfiniteContext::callReturn(Labeler& labeler, Label lbl)
{
  ROSE_ASSERT(isValidReturn(labeler, lbl));
  pop_back();
}


// static
void InfiniteContext::callInvoke( const CtxLattice<InfiniteContext>& src,
                                  CtxLattice<InfiniteContext>& tgt,
                                  CtxAnalysis<InfiniteContext>&,
                                  Labeler& labeler,
                                  Label lbl
                                )
{
  defaultCallInvoke(src, tgt, labeler, lbl);
}

// static
void InfiniteContext::callReturn( const CtxLattice<InfiniteContext>& src,
                                  CtxLattice<InfiniteContext>& tgt,
                                  CtxAnalysis<InfiniteContext>&,
                                  Labeler& labeler,
                                  Label lbl
                                )
{
  transform_if( src.begin(),
                src.end(),
                std::inserter(tgt, tgt.end()),
                IsValidReturn(labeler, lbl),
                InfiniteReturnHandler(tgt.componentFactory(), labeler, lbl)
              );
}

bool operator<(const InfiniteContext& lhs, const InfiniteContext& rhs)
{
  typedef InfiniteContext::const_reverse_iterator rev_iterator;
  typedef std::pair<rev_iterator, rev_iterator>   rev_terator_pair;

  const size_t     LHS    = lhs.size();
  const size_t     RHS    = rhs.size();

  if (LHS < RHS) return true;
  if (LHS > RHS) return false;

  rev_iterator     lhs_aa = lhs.rbegin();
  rev_iterator     lhs_zz = lhs_aa + LHS;
  rev_iterator     rhs_aa = rhs.rbegin();
  rev_terator_pair lrpos  = std::mismatch(lhs_aa, lhs_zz, rhs_aa);

  // unless there is a complete match, compare the first different element.
  return lrpos.first != lhs_zz && *lrpos.first < *lrpos.second;
}


std::ostream& operator<<(std::ostream& os, const InfiniteContext& el)
{
  for (size_t i = 0; i < el.size(); ++i)
    os << (i == 0 ? "'" : ", ") << el.at(i);

  os << ".";
  return os;
}


//
// FiniteContext

bool FiniteContext::isValidReturn(Labeler& labeler, Label retlbl) const
{
  return defaultIsValidReturn(labeler, back(), retlbl);
}

void FiniteContext::callInvoke(const Labeler&, Label lbl)
{
  if (size() == MAX_CTX_LENGTH)
    erase(begin());

  ROSE_ASSERT(size() < MAX_CTX_LENGTH);
  push_back(lbl);
}

void FiniteContext::callReturn(Labeler& labeler, CodeThorn::Label lbl)
{
  ROSE_ASSERT(isValidReturn(labeler, lbl));
  pop_back();
}

bool FiniteContext::callerOf(const FiniteContext& target, Label callsite) const
{
  static const size_t MAX_OVERLAP = MAX_CTX_LENGTH-1;
  
  ROSE_ASSERT(MAX_CTX_LENGTH > 0);
  
  // target is invoked from this, if 
  // (1) the target's last label is callsite
  if (target.back() != callsite) return false;
  
  // (2) if all labels in the common subrange match
  const size_t len         = size();
  const size_t overlap     = std::min(len, MAX_OVERLAP);
  const size_t ofs         = len-overlap;
   
  return std::equal(begin() + ofs, end(), target.begin());
}

bool FiniteContext::operator==(const FiniteContext& that) const
{
  const context_string& self = *this;
  
  return self == that;
}

// static
void FiniteContext::callInvoke( const CtxLattice<FiniteContext>& src,
                                CtxLattice<FiniteContext>& tgt,
                                CtxAnalysis<FiniteContext>&,
                                Labeler& labeler,
                                Label lbl
                              )
{
  defaultCallInvoke(src, tgt, labeler, lbl);
}

// static
void FiniteContext::callReturn( const CtxLattice<FiniteContext>& src,
                                CtxLattice<FiniteContext>& tgt,
                                CtxAnalysis<FiniteContext>& analysis,
                                Labeler& labeler,
                                Label lbl
                              )
{
  const CtxLattice<FiniteContext>& pre  = analysis.getCallSiteLattice(lbl);
  PropertyStateFactory&            fact = tgt.componentFactory();

  std::for_each( src.begin(), src.end(),
                 FiniteReturnHandler(pre, tgt, fact, labeler, lbl)
               );
}

//! full comparison of the call string
bool operator<(const FiniteContext& lhs, const FiniteContext& rhs)
{
  return std::lexicographical_compare(lhs.rbegin(), lhs.rend(), rhs.rbegin(), rhs.rend());
}

std::ostream& operator<<(std::ostream& os, const FiniteContext& el)
{
  for (size_t i = 0; i < el.size(); ++i)
    os << (i == 0 ? "'" : ", ") << el.at(i);

  os << ".";
  return os;
}



} // namespace CodeThorn


