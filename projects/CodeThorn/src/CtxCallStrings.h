
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

  //! defines a strict weak ordering on call contexts.
  friend
  operator<(const CallContext& lhs, const CallContext& rhs);
}

// + free standing functions

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
void allCallInvoke( CtxLattice<CallContext>&  src,
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
void allCallReturn( CtxLattice<CallContext>&  src,
                    CtxLattice<CallContext>&  tgt,
                    CtxAnalysis<CallContext>& analysis,
                    Labeler&                  labeler
                    Label                     lbl
                  );


#endif /* ONLY_FOR_DOCUMENTATION */

template <class ContextType>
struct CtxAnalysis;

//
// InfiniteCallString

//! A class representing an infinitely long call string.
//! The class is precise, but will NOT WORK for recursive codes.
struct InfiniteCallString : private std::vector<Label>
{
    typedef std::vector<Label> context_string;

    using context_string::reverse_iterator;
    using context_string::const_reverse_iterator;
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    bool operator==(const InfiniteCallString& that) const;

    bool isValidReturn(Labeler& labeler, Label retlbl) const;
    bool callerOf(const InfiniteCallString& target, Label callsite) const;
    void callInvoke(const Labeler&, Label lbl);
    void callReturn(Labeler& labeler, Label lbl);

    friend
    std::ostream&
    operator<<(std::ostream& os, const InfiniteCallString& el);
};

void allCallInvoke( const CtxLattice<InfiniteCallString>& src,
                    CtxLattice<InfiniteCallString>& tgt,
                    CtxAnalysis<InfiniteCallString>&,
                    Labeler& labeler,
                    Label lbl
                  );

void allCallReturn( const CtxLattice<InfiniteCallString>& src,
                    CtxLattice<InfiniteCallString>& tgt,
                    CtxAnalysis<InfiniteCallString>&,
                    Labeler& labeler,
                    Label lbl
                  );

bool operator<(const InfiniteCallString& lhs, const InfiniteCallString& rhs);

std::ostream& operator<<(std::ostream& os, const InfiniteCallString& el);


//
// FiniteCallString


//! a context holds up to @ref MAX_CTX_LENGTH as contexts
//!   when calls return, the contexts is mapped on to all feasible contexts
//!   in the caller.
// \todo the base rep could be replaced by a ring-buffer for efficiency
struct FiniteCallString : private std::vector<Label>
{
    typedef std::vector<Label> context_string;

    static const size_t        MAX_CTX_LENGTH = 4;

    using context_string::reverse_iterator;
    using context_string::begin;
    using context_string::end;
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    //! returns true if *this equals that.
    bool operator==(const FiniteCallString& that) const;

    //! returns true, iff this context is valid for the return label @ref lbl.
    bool isValidReturn(Labeler& labeler, Label retlbl) const;

    //! tests if this context could call the context @ref target at label @ref callsite.
    //! \param target   a context at callee's definition
    //! \param callsite the function call label at the call-site
    //! \return true, iff (this + @ref callsite) and @ref target are in a caller/callee relationship
    bool callerOf(const FiniteCallString& target, Label callsite) const;

    //! adds lbl to this call-string
    //! \post
    //!   size() == min(MAX_CTX_LENGTH, pre.size()+1)
    void callInvoke(const Labeler&, Label lbl);

    //! removes lbl from this call-string.
    //! \note
    //!   does not add required prefix at call site.
    //! \post
    //!   size() == pre.size()-1
    void callReturn(Labeler& labeler, Label lbl);

    friend
    std::ostream&
    operator<<(std::ostream& os, const FiniteCallString& el);
};


bool operator<(const FiniteCallString&   lhs, const FiniteCallString&   rhs);

std::ostream& operator<<(std::ostream& os, const FiniteCallString& el);


void allCallInvoke( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    CtxAnalysis<FiniteCallString>&,
                    Labeler& labeler,
                    Label lbl
                  );

void allCallReturn( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    CtxAnalysis<FiniteCallString>& analysis,
                    Labeler& labeler,
                    Label lbl
                  );

//! auxiliary overload to map analysis in src onto lattice at call site (@ref precall).
void allCallReturn( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    const CtxLattice<FiniteCallString>& precall,
                    Labeler& labeler,
                    Label lbl
                  );

} // namespace CodeThorn

#endif /* CTX_CALLSTRINGS_H */
