
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
    ROSE_ASSERT(Labeler::NO_LABEL != invlbl && Labeler::NO_LABEL != retlbl);

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


  //! Compares relevant call string segments, relevant(@ref callctx) < @ref returnctx
  //! \param callctx   the call string at the call site
  //! \param returnctx the already shortened string at the return site
  //! \details
  //!   On a call return, a call string is shortened. If the call string
  //!   was already at full capacity, we map the result on all substrings
  //!   that have the same postfix.
  bool ltProjected(const FiniteCallString& callctx, const FiniteCallString& returnctx)
  {
    int dif = static_cast<int>(callctx.size()) - returnctx.size();

    // if the call context is shorter, we still want to include it in the
    //   set of candidates.
    if (dif < 0) dif = 0;

    //~ ROSE_ASSERT(dif <= 1);
    // the length difference can be greater than one
    // (i.e., in recursive functions whose call context is initially very short.
    // \todo \pp discuss with Markus whether we could have an additional
    //           pseudo label min that we can use for filling the initial
    //           call string.
    return std::lexicographical_compare( callctx.rbegin(),   callctx.rend()-dif,
                                         returnctx.rbegin(), returnctx.rend()
                                       );
  }

  //! extracts the call string from a lattice element and forwards it for comparison.
  //! \note ltProjectedFull is not overloaded to syntactically simplify taking its address
  bool ltProjectedFull(const FiniteCallString& lhs, const std::pair<const FiniteCallString, Lattice*>& rhs)
  {
    return ltProjected(lhs, rhs.first);
  }

  //! clones a lattice element
  struct LatticeCloner
  {
      LatticeCloner(PropertyStateFactory& propfact, Lattice& lat)
      : factory(propfact), lattice(lat)
      {}

      template <class CallContext>
      std::pair<const CallContext, Lattice*>
      operator()(const std::pair<const CallContext, Lattice*>& callsite)
      {
        return std::make_pair(callsite.first, cloneLattice(factory, lattice));
      }

    private:
      PropertyStateFactory& factory;
      Lattice&              lattice;
  };

  //! true if these string lengths indicate a caller/callee relationship
  //! on FiniteCallString
  bool callerCalleeLengths(size_t caller, size_t callee, size_t MAXLEN)
  {
    //~ std::cerr << caller << " / " << callee << " : " << MAXLEN << std::endl;

    return (  ((caller == MAXLEN) && (callee == MAXLEN))
           || ((caller <  MAXLEN) && (callee == caller+1))
           );
  }

  //! tests if this is a prefix to target
  //! \details
  //!   tests a call string (sub-) range and determines
  //!   whether target could be called from this.
  bool callerCalleePrefix(const FiniteCallString& caller, const FiniteCallString& callee)
  {
    static const size_t MAX_OVERLAP = FiniteCallString::MAX_CTX_LENGTH-1;

    // if all labels in the common subrange match
    const size_t len     = caller.size();
    const size_t overlap = std::min(len, MAX_OVERLAP);
    const size_t ofs     = len-overlap;

    ROSE_ASSERT(overlap+1 == callee.size());
    return std::equal(caller.begin() + ofs, caller.end(), callee.begin());
  }

  struct IsCallerCallee
  {
    bool
    operator()(const std::pair<const FiniteCallString, Lattice*>& callsite)
    {
      const FiniteCallString& caller    = callsite.first;

      bool res1 = callerCalleeLengths(caller.size(), retctx.size(), FiniteCallString::MAX_CTX_LENGTH);
      bool res2 = res1 && callerCalleePrefix(caller, retctx);

      return res2;
    }

    FiniteCallString retctx;
  };

  //! A functor that is invoked for every lattice flowing over a return edge.
  //! - Every valid return context is mapped onto every feasible context in the caller
  //! - A context is feasible if it has postfix(precall-context) == prefix(return-context)
  struct FiniteReturnHandler
  {
      FiniteReturnHandler( const CtxLattice<FiniteCallString>& prelat,
                           CtxLattice<FiniteCallString>& tgtlat,
                           PropertyStateFactory& propfactory,
                           Labeler& labels,
                           Label lbl
                         )
      : pre(prelat), tgt(tgtlat), factory(propfactory), labeler(labels), label(lbl)
      {}

      void operator()(const std::pair<const FiniteCallString, Lattice*>& entry)
      {
        typedef CtxLattice<FiniteCallString>::const_iterator const_iterator;

        if (!entry.first.isValidReturn(labeler, label))
        {
          //~ std::cerr << "** INVALID RETURN *" << std::endl;
          return;
        }

        FiniteCallString retctx(entry.first);

        retctx.callReturn(labeler, label);

        const_iterator prelow = pre.lower_bound(retctx);
        const_iterator prepos = std::upper_bound( prelow, pre.end(),
                                                  retctx,
                                                  ltProjectedFull
                                                );

        transform_if( prelow, prepos,
                      std::inserter(tgt, tgt.end()),
                      IsCallerCallee{entry.first},
                      LatticeCloner(factory, sg::deref(entry.second))
                    );
      }

    private:
      const CtxLattice<FiniteCallString>& pre;
      CtxLattice<FiniteCallString>&       tgt;
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
// InfiniteCallString

bool InfiniteCallString::isValidReturn(Labeler& labeler, Label retlbl) const
{
  // see comment in FiniteCallString::isValidReturn
  return size() && defaultIsValidReturn(labeler, back(), retlbl);
}


void InfiniteCallString::callInvoke(const Labeler&, Label lbl)
{
  push_back(lbl);
}


bool InfiniteCallString::callerOf(const InfiniteCallString& target, Label callsite) const
{
  ROSE_ASSERT(target.size());

  // target is invoked from this, if
  // (1) the target's last label is callsite
  // (2) this' call string is one label shorter than target's call string
  // (3) if all labels in the common subrange match
  return (  target.back() == callsite
         && (this->size() + 1) == target.size()
         && std::equal(begin(), end(), target.begin())
         );
}

bool InfiniteCallString::operator==(const InfiniteCallString& that) const
{
  const context_string& self = *this;

  return self == that;
}


void InfiniteCallString::callReturn(Labeler& labeler, Label lbl)
{
  ROSE_ASSERT(isValidReturn(labeler, lbl));
  pop_back();
}


void allCallInvoke( const CtxLattice<InfiniteCallString>& src,
                    CtxLattice<InfiniteCallString>& tgt,
                    CtxAnalysis<InfiniteCallString>& /* not used */,
                    Labeler& labeler,
                    Label lbl
               )
{
  defaultCallInvoke(src, tgt, labeler, lbl);
}

void allCallReturn( const CtxLattice<InfiniteCallString>& src,
                    CtxLattice<InfiniteCallString>& tgt,
                    CtxAnalysis<InfiniteCallString>& /* not used */,
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

bool operator<(const InfiniteCallString& lhs, const InfiniteCallString& rhs)
{
  typedef InfiniteCallString::const_reverse_iterator rev_iterator;
  typedef std::pair<rev_iterator, rev_iterator>      rev_terator_pair;

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


std::ostream& operator<<(std::ostream& os, const InfiniteCallString& el)
{
  for (size_t i = 0; i < el.size(); ++i)
    os << (i == 0 ? "'" : ", ") << el.at(i);

  os << ".";
  return os;
}


//
// FiniteCallString

bool FiniteCallString::isValidReturn(Labeler& labeler, Label retlbl) const
{
  // an empty call-string cannot flow over a return label
  // \example
  //   int main(int argc, ...) {
  //     if (argc==1) return 0; /* produces empty call string for 2 */
  //     if (argc<2) return main(argc-1, ...);
  //     return 0;
  //   }
  // \todo use pseudo context to model call to entry functions.
  //       -> saves periodic calls to size.
  return size() && defaultIsValidReturn(labeler, back(), retlbl);
}

void FiniteCallString::callInvoke(const Labeler&, Label lbl)
{
  if (size() == MAX_CTX_LENGTH)
    erase(begin());

  ROSE_ASSERT(size() < MAX_CTX_LENGTH);
  push_back(lbl);
}

void FiniteCallString::callReturn(Labeler& labeler, CodeThorn::Label lbl)
{
  ROSE_ASSERT(isValidReturn(labeler, lbl));
  pop_back();
}


bool FiniteCallString::callerOf(const FiniteCallString& target, Label callsite) const
{
  ROSE_ASSERT(MAX_CTX_LENGTH > 0 && target.size());

  // target is invoked from this, if
  // (1) the target's last label is callsite
  // (2) the lengths of the call string match caller/callee lengths
  // (3) if all labels in the common subrange match
  return (  (target.back() == callsite)
         && callerCalleeLengths(size(), target.size(), MAX_CTX_LENGTH)
         && callerCalleePrefix(*this, target)
         );
}

bool FiniteCallString::operator==(const FiniteCallString& that) const
{
  const context_string& self = *this;

  return self == that;
}

void allCallInvoke( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    CtxAnalysis<FiniteCallString>& /* not used */,
                    Labeler& labeler,
                    Label lbl
                  )
{
  defaultCallInvoke(src, tgt, labeler, lbl);
}

void allCallReturn( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    const CtxLattice<FiniteCallString>& pre,
                    Labeler& labeler,
                    Label lbl
                  )
{
  PropertyStateFactory&               fact = tgt.componentFactory();

  std::for_each( src.begin(), src.end(),
                 FiniteReturnHandler(pre, tgt, fact, labeler, lbl)
               );
}

void allCallReturn( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    CtxAnalysis<FiniteCallString>& analysis,
                    Labeler& labeler,
                    Label lbl
                  )
{
  allCallReturn(src, tgt, analysis.getCallSiteLattice(lbl), labeler, lbl);
}

//! full comparison of the call string
bool operator<(const FiniteCallString& lhs, const FiniteCallString& rhs)
{
  return std::lexicographical_compare(lhs.rbegin(), lhs.rend(), rhs.rbegin(), rhs.rend());
}

std::ostream& operator<<(std::ostream& os, const FiniteCallString& el)
{
  for (size_t i = 0; i < el.size(); ++i)
    os << (i == 0 ? "'" : ", ") << el.at(i);

  os << ".";
  return os;
}



} // namespace CodeThorn


