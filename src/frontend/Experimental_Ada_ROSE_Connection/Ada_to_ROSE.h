
#ifndef _ADA_TO_ROSE
#define _ADA_TO_ROSE 1

#include <map>
#include <vector>
#include <utility>

#include "Diagnostics.h"
#include "AdaUtil.h"
#include "a_nodes.h"

namespace Ada_ROSE_Translation
{
  static constexpr bool PRODUCTION_CODE = false;

  //
  // node mapping accessors, storage, and retrieval

  /// returns a mapping from an Asis Element_ID to an Asis struct
  std::map<int, Element_Struct*>& asisIdMap();

  template <class KeyNode, class SageNode>
  using map_t = std::map<KeyNode, SageNode>;

  /// returns a mapping from Unit_ID to constructed root node in AST
  //~ map_t<int, SgDeclarationStatement*>& asisUnits();

  /// returns a mapping from Element_ID to SgInitializedName
  map_t<int, SgInitializedName*>& asisVars();

  /// returns a mapping from Element_ID to Exception declaration
  /// \todo revise with representation of Exceptions
  map_t<int, SgInitializedName*>& asisExcps();

  /// returns a mapping from Declaration_ID to SgDeclarationStatement
  map_t<int, SgDeclarationStatement*>& asisDecls();

  /// returns a mapping from Element_ID to ROSE type declaration
  map_t<int, SgDeclarationStatement*>& asisTypes();

  /// returns a mapping from an Element_ID to a loop statement
  /// \todo this should be localized in the AstContext class
  map_t<int, SgStatement*>& asisLoops();

  /// returns a mapping from string to builtin type nodes
  map_t<std::string, SgType*>& adaTypes();


  /// records a node (value) @ref val with key @ref key in map @ref m.
  /// \pre key is not in the map yet
  template <class KeyT, class DclT, class ValT>
  inline
  void
  recordNode(map_t<KeyT, DclT*>& m, KeyT key, ValT& val)
  {
    ROSE_ASSERT(m.find(key) == m.end());

    m[key] = &val;
  }

  /// use for non-defining/defining decls and
  /// other nodes that do not have a single defining mapping.
  template <class KeyT, class DclT, class ValT>
  inline
  void
  recordNonUniqueNode(map_t<KeyT, DclT*>& m, KeyT key, ValT& val)
  {
    if (m.find(key) != m.end()) return;

    recordNode(m, key, val);
  }

  /// retrieves a node from map @ref m with key @ref key.
  template <class KeyT, class DclT>
  inline
  DclT&
  lookupNode(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    typename map_t<KeyT, DclT*>::const_iterator pos = m.find(key);

    ROSE_ASSERT(pos != m.end());
    return *(pos->second);
  }

  /// retrieves a node from map @ref m with key @ref key, under the
  ///   assumption that it is of real type TgtT.
  template <class TgtT, class KeyT, class DclT>
  inline
  TgtT&
  lookupNodeAs(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    DclT& node = lookupNode(m, key);

    return SG_ASSERT_TYPE(TgtT, node);
  }

  /// retrieves a node from map @ref m with key @ref key if key exists.
  /// returns nullptr otherwise.
  template <class KeyT, class DclT>
  inline
  DclT*
  findNode(const map_t<KeyT, DclT*>& m, KeyT key)
  {
    typename map_t<KeyT, DclT*>::const_iterator pos = m.find(key);

    if (pos == m.end())
      return nullptr;

    return pos->second;
  }

  /// base case when a declaration is not in the map
  template <class KeyT, class DclT>
  inline
  DclT*
  findFirst(const map_t<KeyT, DclT*>&)
  {
    return nullptr;
  }

  /// tries a number of keys to find a declaration from map @ref m
  /// returns nullptr if none of the keys can be found.
  template <class KeyT, class DclT, class Key0T, class... KeysT>
  inline
  DclT*
  findFirst(const map_t<KeyT, DclT*>& m, Key0T key0, KeysT... keys)
  {
    DclT* dcl = findNode(m, key0);

    return dcl ? dcl : findFirst(m, keys...);
  }


  //
  // retrieve from ASIS map

  template <class ElemT>
  inline
  ElemT*
  retrieveAsOpt(std::map<int, Element_Struct*>& map, int key)
  {
    //~ logInfo() << "key: " << key << std::endl;
    std::map<int, Element_Struct*>::iterator pos = map.find(key);

    if (pos != map.end())
      return reinterpret_cast<ElemT*>((*pos).second);

    return nullptr;
  }

  template <class ElemT>
  inline
  ElemT&
  retrieveAs(std::map<int, Element_Struct*>& map, int key)
  {
    return SG_DEREF(retrieveAsOpt<ElemT>(map, key));
  }


  //
  // auxiliary functions and types

  /// attaches the source location information from \ref elem to
  ///   the AST node \ref n.
  void attachSourceLocation(SgLocatedNode& n, Element_Struct& elem);

  /// \brief resolves all goto statements to labels
  ///        at the end of procedures or functions.
  /// \todo fold asisLoop handling into this class
  struct LabelManager
  {
      LabelManager() = default;

      /// patch gotos with target (a label statement)
      ///   at the end of a procudure / function.
      ~LabelManager();

      /// records a new labeled statement @ref lblstmt with key @ref id.
      void label(Element_ID id, SgLabelStatement& lblstmt);

      /// records a new goto statement @ref gotostmt with label key @ref id.
      void gotojmp(Element_ID id, SgGotoStatement& gotostmt);

    private:
      typedef std::map<Element_ID, SgLabelStatement*>               LabelContainer;
      typedef std::vector<std::pair<SgGotoStatement*, Element_ID> > GotoContainer;

      LabelContainer labels;
      GotoContainer  gotos;

      LabelManager(const LabelManager&)            = delete;
      LabelManager(LabelManager&&)                 = delete;
      LabelManager& operator=(const LabelManager&) = delete;
      LabelManager& operator=(LabelManager&&)      = delete;
  };


  /// The context class for translation from Asis to ROSE
  struct AstContext
  {
      explicit
      AstContext(SgScopeStatement& s)
      : the_scope(&s), all_labels(nullptr)
      {}

      AstContext()                             = default;
      AstContext(AstContext&&)                 = default;
      AstContext& operator=(AstContext&&)      = default;
      AstContext(const AstContext&)            = default;
      AstContext& operator=(const AstContext&) = default;

      /// returns the current scope
      SgScopeStatement& scope()  const { return *the_scope; }

      /// returns the current label manager
      LabelManager&     labels() const { return SG_DEREF(all_labels); }

      // no-parent-check: sets scope without parent check
      //   e.g., when the parent node is built after the fact (e.g., if statements)
      AstContext scope_npc(SgScopeStatement& s) const;

      // sets scope and checks that the parent is set
      AstContext scope(SgScopeStatement& s) const;

      // sets a new label manager
      AstContext labels(LabelManager& lm) const;

    private:
      SgScopeStatement* the_scope;
      LabelManager*     all_labels;
  };


  /// A range abstraction for a contiguous sequence
  template <class T>
  struct Range : std::pair<T, T>
  {
    Range(T lhs, T rhs)
    : std::pair<T, T>(lhs, rhs)
    {}

    bool empty() const { return this->first == this->second; }
    int  size()  const { return this->second - this->first; }
  };

  /// A range of Asis Units
  struct UnitIdRange : Range<Unit_ID_Ptr>
  {
    typedef Unit_Struct value_type;

    UnitIdRange(Unit_ID_Ptr lhs, Unit_ID_Ptr rhs)
    : Range<Unit_ID_Ptr>(lhs, rhs)
    {}
  };

  /// A range of Asis Elements
  struct ElemIdRange : Range<Element_ID_Ptr>
  {
    typedef Element_Struct value_type;

    ElemIdRange(Element_ID_Ptr lhs, Element_ID_Ptr rhs)
    : Range<Element_ID_Ptr>(lhs, rhs)
    {}
  };

  /// Type mapping for range element types
  template <class T>
  struct range_types {};

  template <>
  struct range_types<Element_ID_List>
  {
    typedef ElemIdRange type;
  };

  template <>
  struct range_types<Unit_ID_Array_Struct>
  {
    typedef UnitIdRange type;
  };


  /// calls \ref functor for each element in the range [\ref first, \ref limit)
  /// e.g., functor(first)
  template <class P, class F>
  inline
  void traverse(P* first, P* limit, F functor)
  {
    while (first != limit)
    {
      functor(*first);

      first = first->Next;
    }
  }


  /// traverses a list of pointers to Elements (or Units) in the range [\ref first, \ref limit),
  ///   looks up the associated Asis struct, and passes it as argument to \ref func.
  ///   e.g., func(*retrieveAs<Element_struct>(map, *first))
  /// \returns a copy of \ref func after all elements have been traversed
  template <class ElemT, class Ptr, class AsisMapT, class F>
  inline
  F
  traverseIDs(Ptr first, Ptr limit, AsisMapT& map, F func)
  {
    while (first != limit)
    {
      ElemT* el = retrieveAsOpt<ElemT>(map, *first);

      if (el)
      {
        func(*el);
      }
      else
      {
        logWarn() << "asis-element of type " << typeid(ElemT).name()
                  << " not available -- asismap[" << *first << "]=nullptr"
                  << std::endl;
      }

      //~ func(retrieveAs<ElemT>(map, *first));
      ++first;
    }

    return func;
  }

  /// traverses all IDs in the range \ref range and calls functor with the associated
  ///   struct of each element.
  /// \returns a copy of \ref func after all elements have been traversed
  template <class Range, class AsisMapT, class F>
  inline
  F
  traverseIDs(Range range, AsisMapT& map, F functor)
  {
    return traverseIDs<typename Range::value_type>(range.first, range.second, map, functor);
  }

  /// creates a range for a contiguous sequence of IDs
  template <class Lst>
  inline
  typename range_types<Lst>::type
  idRange(Lst lst)
  {
    typedef typename range_types<Lst>::type RangeType;

    return RangeType(lst.IDs, lst.IDs + lst.Length);
  }


  namespace
  {
    /// \brief tests whether the link in an Asis node is valid (set)
    ///        or incomplete.
    /// \note  function should become obsolete eventually.
    inline
    bool isInvaldId(int id) { return id == -1; }
  }


}

#endif /* _ADA_TO_ROSE */
