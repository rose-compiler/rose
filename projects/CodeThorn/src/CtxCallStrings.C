#include <algorithm>

#include "sage3basic.h"

#include "CtxCallStrings.h"

#include "CtxAnalysis.h"

namespace CodeThorn
{

namespace
{
  size_t finiteCallStringMaxLen = 4;
  
  /// REVERSING the ordering, sorts Label() first;
  ///   which is a requirement for the use of lower_bound and upper_bound 
  ///   in FiniteReturnHandler.
  struct FiniteLabelComparator
  {
    bool operator()(Label lhs, Label rhs)
    {
      return lhs.getId() > rhs.getId();
    }
  };
  
  /// \brief copies elements from an input range to an output range,
  ///        iff pred(element) is true. The input type and output type
  ///        can differ.
  /// \tparam InputIterator iterator type of the input range
  /// \tparam OutputIterator iterator type of the output range
  /// \tparam Predicate bool Predicate::operator()(InputIterator::reference) returns true,
  ///         if the element should be copied
  /// \tparam Op transformation operation,
  ///         OutputIterator::value_type Op::operator()(InputIterator::reference)
  /// \param  first an InputIterator, beginning of the input range
  /// \param  last  an InputIterator, one past the end element of the input range
  /// \param  sink  an OutputIterator, beginning of the output range
  /// \param  pred  a Predicate
  /// \param  op    an Operation
  /// \details writes op(*first) to the output range if pred(*first) is true.
  ///
  ///         see also: std::transform, std::remove_copy_if
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

        return std::make_pair(tmp, cloneLattice(factory, SG_DEREF(entry.second)));
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

    return labeler.getNode(invlbl) == labeler.getNode(retlbl);
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


  /// clones a lattice element
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

  /// true if these string lengths indicate a caller/callee relationship
  /// on FiniteCallString
  inline
  bool callerCalleeLengths(bool fixedLen, size_t caller, size_t callee, size_t MAXLEN)
  {
    return (  fixedLen
           || ((caller == MAXLEN) && (callee == MAXLEN))
           || ((caller <  MAXLEN) && (callee == caller+1))
           );
  }
  
  /// extracts the call string from a lattice element and forwards it for comparison.
  /// returns retctx < callctx.first
  // \todo can this code be unified with callerCalleePrefix below?
  bool cmpCalleeCallerCtx(const FiniteCallString& retctx, const std::pair<const FiniteCallString, Lattice*>& callctx)
  {
    FiniteCallString::const_reverse_iterator callzz = callctx.first.rend();
    
    return std::lexicographical_compare( retctx.rbegin(), retctx.rend(), 
                                         callctx.first.rbegin(), --callzz,
                                         FiniteLabelComparator()
                                       );
  }

  /// tests if this is a prefix to target
  /// \details
  ///   tests a call string (sub-) range and determines
  ///   whether target could be called from this.
  bool callerCalleePrefix(const FiniteCallString& caller, const FiniteCallString& callee)
  {
    if (FiniteCallString::FIXED_LEN_REP)
    {
      const size_t ofs = 1;
      const bool   res = std::equal( std::next(caller.begin(), ofs), caller.end(), 
                                     callee.begin());
                                     
      return res;
    }    
    
    const size_t MAX_OVERLAP = getFiniteCallStringMaxLength()-1;

    // if all labels in the common subrange match
    const size_t len         = caller.size();
    const size_t overlap     = std::min(len, MAX_OVERLAP);
    const size_t ofs         = len-overlap;
    
    ROSE_ASSERT(overlap+1 == callee.size());
    return std::equal(std::next(caller.begin(), ofs), caller.end(), callee.begin()); 
  }

#if OBSOLETE_CODE
  struct IsCallerCallee
  {
    bool
    operator()(const std::pair<const FiniteCallString, Lattice*>& callsite)
    {
      const FiniteCallString& caller = callsite.first;
      
      return callerCalleePrefix(caller, retctx);  
    }

    FiniteCallString retctx;
  };
#endif /* OBSOLETE_CODE */

  /// A functor that is invoked for every lattice flowing over a return edge.
  /// - Every valid return context is mapped onto every feasible context in the caller
  /// - A context is feasible if it has postfix(precall-context) == prefix(return-context)
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

        if (!entry.first.isValidReturn(labeler, label)) return;
        
        FiniteCallString retctx(entry.first);

        retctx.callReturn(labeler, label);
        
        const_iterator   prelow = pre.lower_bound(retctx);
        
        // \todo make cmpCalleeCallerCtx obsolete and use 
        //       a derivative of callerCalleePrefix instead (like CtxSolver0).
        const_iterator   prepos = std::upper_bound(prelow, pre.end(), retctx, cmpCalleeCallerCtx);
        
        // the use of IsCallerCall is obsolete (transform_if -> transform)
        //   b/c the condition must hold for all elements in range [prelow, prepos).
        std::transform( prelow, prepos,
                        std::inserter(tgt, tgt.end()),
                        //~ IsCallerCallee{entry.first},
                        LatticeCloner(factory, sg::deref(entry.second))
                      );
      }

    private:
      const CtxLattice<FiniteCallString>& pre;
      CtxLattice<FiniteCallString>&       tgt;
      PropertyStateFactory&               factory;
      Labeler&                            labeler;
      Label                               label;
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

template <class Iterator>
void
dbgPrintContexts(Iterator aa, Iterator zz)
{
  while (aa != zz)
  {
    std::cerr << "cr: " << aa->first << std::endl;
    ++aa;
  }
  
  std::cerr << "cr: ----" << std::endl; 
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
              
  //~ dbgPrintContexts(tgt.begin(), tgt.end());
}

bool 
InfiniteCallStringComparator::operator()(const InfiniteCallString& lhs, const InfiniteCallString& rhs) const
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


template <class CallContext>
std::ostream& prnctx(std::ostream& os, const CallContext& callctx)
{
  int cnt = 0;
   
  for (auto lbl : callctx)
    os << (cnt++ ? ", " : "'") << lbl;

  os << (cnt ? "'." : ".");
  return os;
}

std::ostream& operator<<(std::ostream& os, const InfiniteCallString& el)
{
  return prnctx(os, el);
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
    
  return (  (!empty()) 
         && defaultIsValidReturn(labeler, last(), retlbl)
         );
}

void FiniteCallString::callInvoke(const Labeler&, Label lbl)
{
  rep.append(lbl);
}

void FiniteCallString::callReturn(Labeler& labeler, CodeThorn::Label lbl)
{
  ROSE_ASSERT(isValidReturn(labeler, lbl));
  rep.remove();
}


bool FiniteCallString::callerOf(const FiniteCallString& target, Label callsite) const
{
  ROSE_ASSERT(target.size());
  
  constexpr bool fixedLen = FiniteCallString::FIXED_LEN_REP;

  // target is invoked from this, if
  // (1) the target's last label is callsite
  // (2) the lengths of the call string match caller/callee lengths 
  //     NOTE: holds by construction if fixedLen is used.
  // (3) if all labels in the common subrange match
  return (  (!target.empty())
         && (target.last() == callsite)
         && callerCalleeLengths(fixedLen, size(), target.size(), getFiniteCallStringMaxLength())
         && callerCalleePrefix(*this, target)
         );
}


bool FiniteCallString::operator==(const FiniteCallString& that) const
{
  return rep == that.rep;
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
  PropertyStateFactory& fact = tgt.componentFactory();

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
  
  //~ dbgPrintContexts(tgt.begin(), tgt.end());
}

/// full comparison of the call string
bool 
FiniteCallStringComparator::operator()(const FiniteCallString& lhs, const FiniteCallString& rhs) const
{
  return std::lexicographical_compare( lhs.rbegin(), lhs.rend(), 
                                       rhs.rbegin(), rhs.rend(),
                                       FiniteLabelComparator()
                                     );
}

std::ostream& operator<<(std::ostream& os, const FiniteCallString& el)
{
  return prnctx(os, el);
}

/// sets the finite call string max length
void setFiniteCallStringMaxLength(size_t len)
{
  ROSE_ASSERT(len >= 1);
  
  finiteCallStringMaxLen = len;
}

/// returns the finite call string max length
size_t getFiniteCallStringMaxLength()
{
  return finiteCallStringMaxLen;
}


int callstring_creation_counter = 0;
int callstring_deletion_counter = 0;

} // namespace CodeThorn


