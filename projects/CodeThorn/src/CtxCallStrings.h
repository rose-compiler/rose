
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

  //! returns true, iff this context is valid for the return label @ref lbl
  bool isValidReturn(Labeler& labeler, Label lbl) const;

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

  //! defines a strict weak ordering on call contexts
  friend
  operator<(const CallContext& lhs, const CallContext& rhs);
}

#endif /* ONLY_FOR_DOCUMENTATION */

template <class ContextType>
struct CtxAnalysis;

//! a class representing an infinite long call string. The class
//! is precise, but will NOT WORK for analyzing codes with recursion.
struct InfiniteContext : private std::vector<Label>
{
    typedef std::vector<Label> context_string;

    using context_string::reverse_iterator;
    using context_string::const_reverse_iterator;
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    bool isValidReturn(Labeler& labeler, Label retlbl) const;

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
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    bool isValidReturn(Labeler& labeler, Label retlbl) const;

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
