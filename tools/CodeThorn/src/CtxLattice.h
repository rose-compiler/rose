
#ifndef CTX_LATTICE_H
#define CTX_LATTICE_H 1

/// \author Peter Pirkelbauer

#include "sageGeneric.h"
#include "DFAnalysisBase.h"
#include "Utility.h"

namespace CodeThorn
{

namespace
{
  //~ constexpr bool EXTENSIVE_ASSERTION_CHECKING = true;
  constexpr bool EXTENSIVE_ASSERTION_CHECKING = false;

  template <class P>
  inline
  std::string type_name(P* p)
  {
    if (p == NULL) return "null";

    return typeid(*p).name();
  }

  static inline
  void dbg_rd(std::ostream& os, char sep, Lattice* lat)
  {
    //RDLattice has been removed
    //RDLattice& rdlat = dynamic_cast<RDLattice&>(sg::deref(lat));
    //os << lat << sep << rdlat.size() << std::endl;
    std::cerr<<"Error: dbg_rd: RDLattice has been removed. bailing out."<<std::endl;
    exit(1);
  }

  /// clones a lattice @ref orig.
  template <class CodeThornFactory, class CodeThornLattice>
  inline
  CodeThornLattice*
  cloneLattice(CodeThornFactory& factory, const CodeThornLattice& elem)
  {
    CodeThornLattice& orig  = const_cast<CodeThornLattice&>(elem);
    CodeThornLattice& clone = sg::deref(factory.create());

    clone.combine(orig);

    ROSE_ASSERT(!EXTENSIVE_ASSERTION_CHECKING || orig.approximatedBy(clone));
    ROSE_ASSERT(!EXTENSIVE_ASSERTION_CHECKING || clone.approximatedBy(orig));
    return &clone;
  }


  template <class Map>
  class LatticeCombiner
  {
    public:
      typedef typename Map::value_type entry_t;

      explicit
      LatticeCombiner(Map& lhsLatticemap)
      : lhslattice(lhsLatticemap)
      {}

      void operator()(const unavailable_t&, const entry_t& rhs)
      {
        CodeThorn::PropertyStateFactory& factory = lhslattice.componentFactory();
        Lattice&                         sublat  = sg::deref(rhs.second);

        lhslattice[rhs.first] = cloneLattice(factory, sublat);
      }

      void operator()(const entry_t&, const unavailable_t&)
      {
        // nothing to do
      }

      void operator()(entry_t& lhs, const entry_t& rhs)
      {
        lhs.second->combine(const_cast<Lattice&>(sg::deref(rhs.second)));
      }

    private:
      Map& lhslattice;
  };

  template <class M>
  inline
  LatticeCombiner<M>
  latticeCombiner(M& lhsLatticemap)
  {
    return LatticeCombiner<M>(lhsLatticemap);
  }

  /// functor that determines whether an element exists in some map.
  template <class M>
  class CtxLatticeNotIn
  {
    public:
      typedef typename M::value_type entry_t;

      explicit
      CtxLatticeNotIn(const M& rhsLatticemap)
      : rhslattice(rhsLatticemap)
      {}

      // returns true if this element lattice IS NOT in rhslattice
      //   (used by find_if to find the first element not in rhslattice)
      bool operator()(const entry_t& lhslattice)
      {
        typename M::const_iterator rhspos = rhslattice.find(lhslattice.first);

        return rhspos == rhslattice.end()
               || !lhslattice.second->approximatedBy(sg::deref(rhspos->second));
      }

    private:
      const M& rhslattice;
  };

  // deduces M for class construction
  template <class M>
  static inline
  CtxLatticeNotIn<M>
  ctxLatticeNotIn(const M& rhslattices)
  {
    return CtxLatticeNotIn<M>(rhslattices);
  }

  /// Deletes lattices in maps
  struct LatticeDeleter
  {
    template<class Key>
    void operator()(std::pair<const Key, Lattice*>& pair)
    {
      delete pair.second;
    }
  };

  /// deletes all Lattice* in maps within the range [aa, zz)
  template <class _ForwardIterator>
  void deleteLattices(_ForwardIterator aa, _ForwardIterator zz)
  {
    std::for_each(aa, zz, LatticeDeleter());
  }

  class CtxLatticeStreamer
  {
    public:
      CtxLatticeStreamer(std::ostream& stream, VariableIdMapping* vmap)
      : os(stream), vm(vmap)
      {}

      template <class CallContext>
      void operator()(const std::pair<const CallContext, Lattice*>& entry)
      {
        os << "[" << entry.first << ": ";
        entry.second->toStream(os, vm);
        os << "]";
      }

    private:
      std::ostream&      os;
      VariableIdMapping* vm;
  };
}


template <class CallContext>
using CtxLatticeMap = std::map<CallContext, Lattice*, typename CallContext::comparator>;

template <class CallContext>
using KeySelectAndCompare = std::function<bool(typename CtxLatticeMap<CallContext>::value_type const&,
                                               typename CtxLatticeMap<CallContext>::value_type const&)>;


template <class CallContext>
KeySelectAndCompare<CallContext>
lessThanComparatorKeys()
{
  return []( typename CtxLatticeMap<CallContext>::value_type const& lhs,
             typename CtxLatticeMap<CallContext>::value_type const& rhs
           ) -> bool
         {
           return typename CallContext::comparator()(lhs.first, rhs.first);
         };
}


/// A CtxLattice holds information organized by call contexts.
/// Each lattice element (aka call context) has a component lattice
/// that stores the information of some specific analysis.
template <class CallContext>
class CtxLattice : public Lattice, private CtxLatticeMap<CallContext>
{
  public:
    using base        = Lattice;
    using context_t   = CallContext;
    using context_map = CtxLatticeMap<CallContext>;

    using typename context_map::value_type;
    using typename context_map::iterator;
    using typename context_map::const_iterator;
    using typename context_map::const_reverse_iterator;
    using context_map::begin;
    using context_map::end;
    using context_map::rbegin;
    using context_map::rend;
    using context_map::clear;
    using context_map::insert;
    using context_map::lower_bound;
    using context_map::find;
    using context_map::size;
    using context_map::operator[];

    explicit
    CtxLattice(PropertyStateFactory& compfac)
    : base(), context_map(), compPropertyFactory(compfac)
    {}

    ~CtxLattice()
    {
      deleteLattices(begin(), end());
    }

    void swap(CtxLattice<context_t>& that)
    {
      context_map::swap(that);
    }

    bool isBot() const override { return context_map::size() == 0; }

    bool approximatedBy(Lattice& other) const override
    {
      const CtxLattice<context_t>& that = dynamic_cast<CtxLattice<context_t>& >(other);

      // this \ other == {}
      // this is approximated by other, iff this w/o other yields the empty set.
      return std::find_if(begin(), end(), ctxLatticeNotIn(that)) == end();
    }

    void combine(Lattice& other) override
    {
      const CtxLattice<context_t>& that = dynamic_cast<CtxLattice<context_t>& >(other);

      mergeOrderedSequences( begin(), end(),
                             that.begin(), that.end(),
                             latticeCombiner(*this),
                             lessThanComparatorKeys<CallContext>()
                           );

      ASSERT_require(this->size() >= that.size());
      ASSERT_require(!EXTENSIVE_ASSERTION_CHECKING || other.approximatedBy(*this));
    }

    PropertyStateFactory& componentFactory() const
    {
      return compPropertyFactory;
    }

    void toStream(std::ostream& os, VariableIdMapping* vm) override
    {
      if (isBot()) { os << " bot "; return; }

      os << "{";
      std::for_each(begin(), end(), CtxLatticeStreamer(os, vm));
      os << "}";
    }

    bool callLosesPrecision(const_iterator pos) const
    {
      return equalPostfixB(pos, std::prev(pos)) || equalPostfixE(pos, std::next(pos));
    }

    Lattice* combineAll() const
    {
      Lattice* res = componentFactory().create();

      for (const value_type& elem : *this)
        res->combine(const_cast<Lattice&>(SG_DEREF(elem.second)));

      ROSE_ASSERT(!isBot() || isBot());
      return res;
    }

  private:
    bool equalPostfix(const_iterator lhs, const_iterator rhs) const
    {
      return lhs->first.mergedAfterCall(rhs->first);
    }

    bool equalPostfixB(const_iterator lhs, const_iterator rhs) const
    {
      if (lhs == begin()) return false;

      return equalPostfix(lhs, rhs);
    }

    bool equalPostfixE(const_iterator lhs, const_iterator rhs) const
    {
      if (rhs == end()) return false;

      return equalPostfix(lhs, rhs);
    }

    PropertyStateFactory& compPropertyFactory;

    // CtxLattice(const CtxLattice&) = delete;
    // CtxLattice(CtxLattice&&) = delete;
};

template <class OutputStream, class CallContext>
void dbgPrintCtx(OutputStream& logger, Labeler& labeler, CtxLattice<CallContext>& lat)
{
  typedef CallContext call_string_t;

  for (auto el : lat)
  {
    call_string_t ctx = el.first;

    for (auto lbl : ctx)
    {
      std::string code = lbl != Label() ? SG_DEREF(labeler.getNode(lbl)).unparseToString()
                                        : std::string();

      logger << lbl << " : " << code << std::endl;
    }

    logger << "***\n";
  }
}

} // namespace CodeThorn

#endif /* CTX_LATTICE_H */
