
#ifndef CTX_CALLSTRINGS_H
#define CTX_CALLSTRINGS_H 1

/// \author Peter Pirkelbauer

#include <vector>

#include <rose.h>
#include <Labeler.h>

#include "CtxLattice.h"


namespace CodeThorn
{
#if ONLY_FOR_DOCUMENTATION

/// defines the interface for call contexts
concept CallContext
{
  /// cctor
  CallContext(const CallContex&);

  /// returns true, iff this context is valid for the return label @ref lbl.
  bool isValidReturn(Labeler& labeler, Label lbl) const;

  /// tests if this context could call that context at label @ref callsite.
  /// \param target   a context at callee's definition
  /// \param callsite the function call label at the call-site
  /// \return true, iff this plus @ref callsite and @ref that are in a caller/callee relationship
  bool callerOf(const CallContext& that, Label callsite) const;

  /// returns true if *this equals that.
  bool operator==(const CallContext& that) const;

  /// defines a strict weak ordering on call contexts.
  friend
  operator<(const CallContext& lhs, const CallContext& rhs);
}

// + free standing functions

/// changes the contexts in @ref src for the function call described by @ref lbl
/// and stores them into @ref tgt.
/// \param src contains the lattices/contexts before the invocation @ref lbl
/// \param tgt lattices/contexts changed according to invocation
/// \param analysis the analysis (in case it is needed)
/// \param labeler the labeler (in case it is needed)
/// \param lbl the call label
/// \pre  tgt is bot
/// \post references in @ref src and @ref tgt MUST BE disjoint,
///       anything in src will be deleted after callInvoke returns.
void allCallInvoke( CtxLattice<CallContext>&  src,
                    CtxLattice<CallContext>&  tgt,
                    CtxAnalysis<CallContext>& analysis,
                    Labeler&                  labeler
                    Label                     lbl
                  );

/// changes the contexts in @ref src for return from the function call
/// described by @ref lbl and stores them into @ref tgt.
/// \param src contains the lattices/contexts before the return @ref lbl
/// \param tgt lattices/contexts changed according to the return
/// \param analysis the analysis (in case it is needed)
/// \param labeler the labeler (in case it is needed)
/// \param lbl the call label
/// \pre  tgt is bot
/// \post references in @ref src and @ref tgt MUST BE disjoint,
///       anything in src will be deleted after callReturn returns.
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

struct InfiniteCallStringComparator;

/// A class representing an infinitely long call string.
/// The class is precise, but will NOT WORK for recursive codes.
struct InfiniteCallString : private std::vector<Label>
{
    typedef InfiniteCallStringComparator comparator;
    typedef std::vector<Label>           context_string;

    using context_string::const_iterator;
    using context_string::reverse_iterator;
    using context_string::const_reverse_iterator;
    using context_string::begin;
    using context_string::end;
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

struct InfiniteCallStringComparator
{
  bool operator()(const InfiniteCallString& lhs, const InfiniteCallString& rhs) const;
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

std::ostream& operator<<(std::ostream& os, const InfiniteCallString& el);


//
// FiniteCallString

/// maximal logical length of a finite call string
static constexpr size_t CTX_CALL_STRING_MAX_LENGTH = 4;

/// Comparator class
struct FiniteCallStringComparator;


/// This class is a simple implementation of a fixed length call string
///   Keeping the interface, we may want to replace it with a rotating
///   buffer at some point.
/// \details
///   this implementation fills the call sequence with empty labels,
///   which simplifies 
/// \note 
///   the class has been factored out from FiniteCallString to simplify
///   its replacement with improved versions
///   (e.g., circular buffer based, an implementation that have the
///          object share its underlying representation to improve
///          memory efficiency). 
struct ContextSequence : private std::vector<Label>
{
  typedef std::vector<Label> base;
  
  ContextSequence()
  : base(CTX_CALL_STRING_MAX_LENGTH, Label())
  {
    ROSE_ASSERT(base::size() == CTX_CALL_STRING_MAX_LENGTH);
  }
  
  ContextSequence(const ContextSequence&)            = default;
  ContextSequence& operator=(const ContextSequence&) = default;
  ContextSequence(ContextSequence&&)                 = delete;
  ContextSequence& operator=(ContextSequence&&)      = delete;
  
  using base::const_reference;
  using base::reverse_iterator;
  using base::const_iterator;
  using base::begin;
  using base::end;
  using base::rbegin;
  using base::rend;
  using base::size;  
  
  /// adds a call label @ref lbl to the end of the sequence.
  /// if the sequence is at its capacity, the oldest call label will be
  /// removed.
  void append(Label lbl)
  {
    ROSE_ASSERT(base::size() == CTX_CALL_STRING_MAX_LENGTH);
    erase(begin());
      
    base::push_back(lbl);
  }
  
  /// removes the most recent call label from the sequence
  void remove()
  {
    ROSE_ASSERT(base::size() == CTX_CALL_STRING_MAX_LENGTH);
    base::pop_back();
  }
  
  /// returns the most recent call label
  Label last() const { return base::back(); }
  
  /// a call string ending in Label() is considered empty
  bool empty() const { return base::back() == Label(); }
  
  bool operator==(const ContextSequence& that) const
  {
    const base& self = *this;

    return self == that;
  }
};

/// a context holds up to @ref CTX_CALL_STRING_MAX_LENGTH as contexts
///   when calls return, the contexts is mapped on to all feasible contexts
///   in the caller.
// \todo the base rep could be replaced by a ring-buffer for efficiency
struct FiniteCallString : ContextSequence
{
    typedef FiniteCallStringComparator comparator;
    typedef ContextSequence            context_string;

    using context_string::reverse_iterator;
    using context_string::const_iterator;
    using context_string::begin;
    using context_string::end;
    using context_string::rbegin;
    using context_string::rend;
    using context_string::size;  // dbg

    /// returns true if *this equals that.
    bool operator==(const FiniteCallString& that) const;

    /// returns true, iff this context is valid for the return label @ref lbl.
    bool isValidReturn(Labeler& labeler, Label retlbl) const;

    /// tests if this context could call the context @ref target at label @ref callsite.
    /// \param target   a context at callee's definition
    /// \param callsite the function call label at the call-site
    /// \return true, iff (this + @ref callsite) and @ref target are in a caller/callee relationship
    bool callerOf(const FiniteCallString& target, Label callsite) const;

    /// adds lbl to this call-string
    /// \post
    ///   size() == min(CTX_CALL_STRING_MAX_LENGTH, pre.size()+1)
    void callInvoke(const Labeler&, Label lbl);

    /// removes lbl from this call-string.
    /// \note
    ///   does not add required prefix at call site.
    /// \post
    ///   size() == pre.size()-1
    void callReturn(Labeler& labeler, Label lbl);

    friend
    std::ostream&
    operator<<(std::ostream& os, const FiniteCallString& el);
};

struct FiniteCallStringComparator
{
  bool operator()(const FiniteCallString& lhs, const FiniteCallString& rhs) const;
};


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

/// auxiliary overload to map analysis in src onto lattice at call site (@ref precall).
void allCallReturn( const CtxLattice<FiniteCallString>& src,
                    CtxLattice<FiniteCallString>& tgt,
                    const CtxLattice<FiniteCallString>& precall,
                    Labeler& labeler,
                    Label lbl
                  );

} // namespace CodeThorn

#endif /* CTX_CALLSTRINGS_H */
