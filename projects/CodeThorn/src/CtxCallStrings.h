
#ifndef CTX_CALLSTRINGS_H
#define CTX_CALLSTRINGS_H 1

/// \author Peter Pirkelbauer

#include <vector>
#include <iterator>
//~ #include <deque>
//~ #include <list>

#include <sage3basic.h>
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
  
  /// updates the call context according to the function call invocation at @ref lab
  void callInvoke(const Labeler&, Label lab);

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

/// sets the finite call string max length
/// \brief modifying the call string length while some analysis runs
///        is undefined.
/// \param len len must be >= 2
void setFiniteCallStringMaxLength(size_t len);

/// returns the finite call string max length
size_t getFiniteCallStringMaxLength();

/// Comparator class
struct FiniteCallStringComparator;

/// adds pop_front to a sequence data structure (required from ContextSequence)
template <class _BaseT>
struct ext_sequence : _BaseT
{
  typedef _BaseT base;
  
  static constexpr bool FIXED_LEN_REP = true;
  
  using base::base;
  
  void pop_front() { base::erase(base::begin()); } 
};


extern int callstring_creation_counter;
extern int callstring_deletion_counter;

#if 0
struct CtorDtorCounter
{
  CtorDtorCounter()
  {
    ++callstring_creation_counter;
  }
  
  CtorDtorCounter(const CtorDtorCounter&)
  {
    ++callstring_creation_counter;
  }
  
  CtorDtorCounter(CtorDtorCounter&&)
  {
    ++callstring_creation_counter;
  }
  
  ~CtorDtorCounter()
  {
    ++callstring_deletion_counter;
  }
};

#endif


#if REQUIRES_CONSTANT_SIZE_CALL_STRING_LENGTH

template <class T>
struct SimpleString // : CtorDtorCounter
{
    typedef T                                     value_type;
    typedef const T&                              const_reference;
    typedef const T*                              const_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    
    SimpleString(size_t initsize, T val)
    : len(getFiniteCallStringMaxLength())
    {
      ROSE_ASSERT(initsize == size_t(len));
      
      std::fill_n(data, initsize, val);
    }
    
    SimpleString()                                 = delete;
    SimpleString(const SimpleString& s)            = default;
    SimpleString& operator=(const SimpleString& s) = default;
    ~SimpleString()                                = default;
    
    void pop_front()
    {
      for (int i = 1; i < len; ++i)
        data[i-1] = data[i];
        
      --len;
    }
    
    void push_back(T el)
    {
      ROSE_ASSERT(len == getFiniteCallStringMaxLength()-1);
      
      data[len] = el;  
      ++len;
    }
    
    void pop_back()
    {
      ROSE_ASSERT(len == getFiniteCallStringMaxLength());
      
      --len;
    }
    
    const T& back() const
    {
      return data[len-1];
    }
    
    int size() const
    {
      return len;
    }
    
    const_iterator begin() const
    {
      return data;
    }
    
    const_iterator end() const
    {
      return data+len;
    }
    
    const_reverse_iterator rbegin() const
    {
      return const_reverse_iterator(end());
    }
    
    const_reverse_iterator rend() const
    {
      return const_reverse_iterator(begin());
    }
    
    template <class U>
    friend 
    bool operator==(const SimpleString<U>& lhs, const SimpleString<U>& rhs)
    {
      return (  lhs.len == rhs.len
             && std::equal(lhs.begin(), lhs.end(), rhs.begin())
             );
    }
        
  private:
    int len;
    T   data[getFiniteCallStringMaxLength()];
};

#endif /* REQUIRES_CONSTANT_SIZE_CALL_STRING_LENGTH */

/// This class is a simple implementation of a fixed length call string
///   that is based on an exchangable underlying representation @ref _ImplT/
/// \details
///   this implementation fills the call sequence with empty labels,
///   which simplifies some of the comparison algorithms. 
/// \note 
///   the class has been factored out from FiniteCallString to simplify
///   its replacement with improved versions.
///   (e.g., circular buffer based, an implementation that have the
///          object share its underlying representation to improve
///          memory efficiency). 

template <class _ImplT>
struct ContextSequence
{
    typedef _ImplT impl;
  
    typedef typename impl::const_reference        const_reference;
    typedef typename impl::const_reverse_iterator const_reverse_iterator;
    typedef typename impl::const_iterator         const_iterator;
    
    static constexpr bool FIXED_LEN_REP = true;
  
    ContextSequence()
    : data(getFiniteCallStringMaxLength(), Label())
    {
      ROSE_ASSERT(data.size() == getFiniteCallStringMaxLength());
    }
    
    ContextSequence(const ContextSequence&)            = default;
    ContextSequence& operator=(const ContextSequence&) = default;
    ~ContextSequence()                                 = default;
    
    // "inherited" member functions
    const_iterator         begin()   const { return data.begin(); }
    const_iterator         end()     const { return data.end(); }
    const_reverse_iterator rbegin()  const { return data.rbegin(); }
    const_reverse_iterator rend()    const { return data.rend(); }
    size_t                 size()    const { return data.size(); }
    
    /// adds a call label @ref lbl to the end of the sequence.
    /// if the sequence is at its capacity, the oldest call label will be
    /// removed.
    void append(Label lbl)
    {
      ROSE_ASSERT(data.size() == getFiniteCallStringMaxLength());
      
      data.pop_front();
      data.push_back(lbl);
    }
    
    /// removes the most recent call label from the sequence
    void remove()
    {
      ROSE_ASSERT(data.size() == getFiniteCallStringMaxLength());
      data.pop_back();
    }
    
    /// returns the most recent call label
    Label last() const { return data.back(); }
    
    /// a call string ending in Label() is considered empty
    bool empty() const { return data.back() == Label(); }
    
    bool operator==(const ContextSequence& that) const
    {
      return this->data == that.data;
    }
  
  private:
    impl data;

    ContextSequence(ContextSequence&&)            = delete;
    ContextSequence& operator=(ContextSequence&&) = delete;
};


/// \note NOT SAFE IN CONCURRENT ENVIRONMENT
template <class _ImplT>
struct CounterCOW : _ImplT
{
  typedef _ImplT base;
  
  CounterCOW(size_t initsize, typename base::value_type val)
  : base(initsize, val), cnt(1)
  {}
  
  int cnt;
};

/// \note NOT SAFE IN CONCURRENT ENVIRONMENT
/// This class is a simple implementation of a fixed length call string
///   that uses copy on write to cut on unnecessary copies. On one
///   test code (beamline), the number of call strings generated
///   was only 2% compared to value objects (nevertheless was slower..)
/// \details
///   this implementation fills the call sequence with empty labels,
///   which simplifies some of the comparison algorithms. 
/// \note
///    number of allocations (RD on simplified beamline)
///       value strings: 9942160 / 9895731
///       cow strings:    192943 /  190232
///    ? nevertheless, value strings are faster.. ???

template <class _ImplT>
struct ContextSequenceCOW
{    
    typedef CounterCOW<_ImplT> impl;
  
    typedef typename impl::const_reference        const_reference;
    typedef typename impl::const_reverse_iterator const_reverse_iterator;
    typedef typename impl::const_iterator         const_iterator;
    
    static constexpr bool FIXED_LEN_REP = _ImplT::FIXED_LEN_REP;
  
    ContextSequenceCOW()
    : data(new impl(getFiniteCallStringMaxLength(), Label()))
    {
      ROSE_ASSERT(data->cnt > 0);
      ROSE_ASSERT(data->size() == getFiniteCallStringMaxLength());
    }
    
    ContextSequenceCOW(const ContextSequenceCOW& orig)
    : data(orig.data)
    {
      ROSE_ASSERT(data->cnt > 0);
      
      ++data->cnt;
    }
    
    ContextSequenceCOW& operator=(const ContextSequenceCOW& orig) 
    {
      ROSE_ASSERT(data->cnt > 0 && orig.data->cnt > 0);
      
      ContextSequenceCOW tmp(*this); // <-- move 
      
      --data->cnt;
      
      data = orig.data;
      ++data->cnt;
      
      return *this;
    }
    
    ~ContextSequenceCOW()
    {
      ROSE_ASSERT(data->cnt > 0);
      
      if ((--data->cnt) == 0) delete data;
    }
    
    // "inherited" member functions
    const_iterator         begin()   const { return data->begin(); }
    const_iterator         end()     const { return data->end(); }
    const_reverse_iterator rbegin()  const { return data->rbegin(); }
    const_reverse_iterator rend()    const { return data->rend(); }
    size_t                 size()    const { return data->size(); }
    
    /// adds a call label @ref lbl to the end of the sequence.
    /// if the sequence is at its capacity, the oldest call label will be
    /// removed.
    void append(Label lbl)
    {
      privatizeIfShared();
      
      ROSE_ASSERT(data->size() == getFiniteCallStringMaxLength());
      
      data->pop_front();
      data->push_back(lbl);
    }
    
    /// removes the most recent call label from the sequence
    void remove()
    {
      privatizeIfShared();
      
      ROSE_ASSERT(data->size() == getFiniteCallStringMaxLength());
      data->pop_back();
    }
    
    /// returns the most recent call label
    Label last() const { return data->back(); }
    
    /// a call string ending in Label() is considered empty
    bool empty() const { return data->back() == Label(); }
    
    bool operator==(const ContextSequenceCOW& that) const
    {
      return (  this->data == that.data
             || (*this->data) == (*that.data)
             );
    }
  
  private:
    void privatizeIfShared()
    {
      if (data->cnt == 1) return;
      ROSE_ASSERT(data->cnt > 1);
      
      --data->cnt;             // unlink
      data = new impl(*data);  // copy
      data->cnt = 1;           // set initial cnt
    }
  
    impl* data;

    ContextSequenceCOW(ContextSequenceCOW&&)            = delete;
    ContextSequenceCOW& operator=(ContextSequenceCOW&&) = delete;
};

/// a context holds up to @ref getFiniteCallStringMaxLength() as contexts
///   when calls return, the contexts is mapped on to all feasible contexts
///   in the caller.
// \todo the base rep could be replaced by a ring-buffer for efficiency
struct FiniteCallString 
{
    // pick the underlying sequence representation
    //~ typedef SimpleString<Label>                       sequence; // 0.2% (1s/475s)faster than vector (on some whole application)
    typedef ext_sequence< std::vector<Label> >        sequence; // seems slightly faster than alternatives below
    //~ typedef ext_sequence< std::basic_string<Label> >  sequence;
    //~ typedef std::deque<Label>                         sequence;
    //~ typedef std::list<Label>                          sequence;
    
    //~ typedef ContextSequence< sequence >               context_string;
    typedef ContextSequenceCOW< sequence >            context_string; // COW wrapper makes things slightly slower...
    
    // string comparison reverses "normal" sort to place NO_LABEL first.
    //   (required by callsite merging in FiniteReturnHandler)
    typedef FiniteCallStringComparator                comparator;
    
    // "inherited" types
    typedef context_string::const_reverse_iterator    const_reverse_iterator;
    typedef context_string::const_iterator            const_iterator;
    
    // true, if the underlying representation is fixed length.
    static constexpr bool FIXED_LEN_REP = context_string::FIXED_LEN_REP;
    
    // "inherited" functions 
    const_iterator         begin()  const { return rep.begin(); }
    const_iterator         end()    const { return rep.end(); }
    const_reverse_iterator rbegin() const { return rep.rbegin(); }
    const_reverse_iterator rend()   const { return rep.rend(); }
    bool                   empty()  const { return rep.empty(); }  
    Label                  last()   const { return rep.last(); }  
    size_t                 size()   const { return rep.size(); }  

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
    ///   size() == min(getFiniteCallStringMaxLength(), pre.size()+1)
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
    
  private:
     context_string rep;
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
