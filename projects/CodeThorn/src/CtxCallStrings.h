
#ifndef CTX_CALLSTRINGS_H
#define CTX_CALLSTRINGS_H 1

#include <vector>

#include <rose.h>
#include <Labeler.h>

#include "CtxLattice.h"


namespace CodeThorn
{
#if ONLY_FOR_DOCUMENTATION

//! defines the interface for call contexts
concept CallContext
{
  //! cctor
  CallContext(const CallContex&);

  //! returns true, iff this context is valid for the return label @ref lbl.
  bool isValidReturn(Labeler& labeler, Label lbl) const;

  //! tests if this context could call that context at label @ref callsite.
  //! \param target   a context at callee's definition
  //! \param callsite the function call label at the call-site
  //! \return true, iff this plus @ref callsite and @ref that are in a caller/callee relationship
  bool callerOf(const CallContext& that, Label callsite) const;

  //! returns true if *this equals that.
  bool operator==(const CallContext& that) const;

  //! changes the contexts in @ref src for the function call described by @ref lbl
  //! and stores them into @ref tgt.
  //! \param src contains the lattices/contexts before the invocation @ref lbl
  //! \param tgt lattices/contexts changed according to invocation
  //! \param analysis the analysis (in case it is needed)
  //! \param labeler the labeler (in case it is needed)
  //! \param lbl the call label
  //! \pre  tgt is bot
  //! \post references in @ref src and @ref tgt MUST BE disjoint,
  //!       anything in src will be deleted after callInvoke returns.
  static
  void callInvoke( CtxLattice<CallContext>&  src,
                   CtxLattice<CallContext>&  tgt,
                   CtxAnalysis<CallContext>& analysis,
                   Labeler&                  labeler
                   Label                     lbl
                 );

  //! changes the contexts in @ref src for return from the function call
  //! described by @ref lbl and stores them into @ref tgt.
  //! \param src contains the lattices/contexts before the return @ref lbl
  //! \param tgt lattices/contexts changed according to the return
  //! \param analysis the analysis (in case it is needed)
  //! \param labeler the labeler (in case it is needed)
  //! \param lbl the call label
  //! \pre  tgt is bot
  //! \post references in @ref src and @ref tgt MUST BE disjoint,
  //!       anything in src will be deleted after callReturn returns.
  static
  void callReturn( CtxLattice<CallContext>&  src,
                   CtxLattice<CallContext>&  tgt,
                   CtxAnalysis<CallContext>& analysis,
                   Labeler&                  labeler
                   Label                     lbl
                 );

  //! defines a strict weak ordering on call contexts.
  friend
  operator<(const CallContext& lhs, const CallContext& rhs);
}

#endif /* ONLY_FOR_DOCUMENTATION */

template <class ContextType>
struct CtxAnalysis;

//! A class representing an infinitely long call string.
//! The class is precise, but will NOT WORK for recursive codes.
struct InfiniteContext : private std::vector<Label>
{
    typedef std::vector<Label> context_string;

    using context_string::reverse_iterator;
    using context_string::const_reverse_iterator;
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    bool operator==(const InfiniteContext& that) const;

    bool isValidReturn(Labeler& labeler, Label retlbl) const;
    bool callerOf(const InfiniteContext& target, Label callsite) const;
    void callInvoke(const Labeler&, Label lbl);
    void callReturn(Labeler& labeler, Label lbl);

    static
    void callInvoke( const CtxLattice<InfiniteContext>& src,
                     CtxLattice<InfiniteContext>& tgt,
                     CtxAnalysis<InfiniteContext>&,
                     Labeler& labeler,
                     Label lbl
                   );

    static
    void callReturn( const CtxLattice<InfiniteContext>& src,
                     CtxLattice<InfiniteContext>& tgt,
                     CtxAnalysis<InfiniteContext>&,
                     Labeler& labeler,
                     Label lbl
                   );


    friend
    std::ostream&
    operator<<(std::ostream& os, const InfiniteContext& el);
};


//! a context holds up to @ref MAX_CTX_LENGTH as contexts
//!   when calls return, the contexts is mapped on to all feasible contexts
//!   in the caller.
// \todo the base rep could be replaced by a ring-buffer for efficiency
struct FiniteContext : private std::vector<Label>
{
    typedef std::vector<Label> context_string;

    static const size_t        MAX_CTX_LENGTH = 4;

    using context_string::reverse_iterator;
    using context_string::begin;
    using context_string::end;
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    bool operator==(const FiniteContext& that) const;

    bool isValidReturn(Labeler& labeler, Label retlbl) const;
    bool callerOf(const FiniteContext& target, Label callsite) const;
    void callInvoke(const Labeler&, Label lbl);
    void callReturn(Labeler& labeler, Label lbl);

    static
    void callInvoke( const CtxLattice<FiniteContext>& src,
                     CtxLattice<FiniteContext>& tgt,
                     CtxAnalysis<FiniteContext>&,
                     Labeler& labeler,
                     Label lbl
                   );

    static
    void callReturn( const CtxLattice<FiniteContext>& src,
                     CtxLattice<FiniteContext>& tgt,
                     CtxAnalysis<FiniteContext>& analysis,
                     Labeler& labeler,
                     Label lbl
                   );

    friend
    std::ostream&
    operator<<(std::ostream& os, const FiniteContext& el);
};



bool operator<(const InfiniteContext& lhs, const InfiniteContext& rhs);
bool operator<(const FiniteContext&   lhs, const FiniteContext&   rhs);

std::ostream& operator<<(std::ostream& os, const InfiniteContext& el);
std::ostream& operator<<(std::ostream& os, const FiniteContext& el);

} // namespace CodeThorn

#endif /* CTX_CALLSTRINGS_H */
