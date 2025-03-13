#include "ClassHierarchyAnalysis.h"

#include <algorithm>
#include <unordered_set>
#include <cassert>
#include <iterator>


namespace ct = CodeThorn;

namespace
{
  /// pseudo type to indicate that an element is not in a sequence
  struct unavailable_t {};

  template <class First, class Second>
  auto key(const std::pair<First, Second>& keyval) -> const First&
  {
    return keyval.first;
  }

  template <class... Elems>
  auto key(const std::tuple<Elems...>& keydata) -> decltype( std::get<0>(keydata) )
  {
    return std::get<0>(keydata);
  }

  auto key(ct::FunctionKeyType keydata) -> ct::FunctionKeyType
  {
    return keydata;
  }

  /// \brief  traverses two ordered associative sequences in order of their elements.
  ///         The elements in the sequences must be convertible. A merge object
  ///         is called with sequence elements in order of their keys in [aa1, zz1) and [aa2, zz2).
  /// \tparam _Iterator1 an iterator of an ordered associative container
  /// \tparam _Iterator2 an iterator of an ordered associative container
  /// \tparam BinaryOperator a merge object that provides three operator()
  ///         functions.
  ///         - void operator()(_Iterator1::value_type, unavailable_t);
  ///           called when an element is in sequence 1 but not in sequence 2.
  ///         - void operator()(unavailable_t, _Iterator2::value_type);
  ///           called when an element is in sequence 2 but not in sequence 1.
  ///         - void operator()(_Iterator1::value_type, _Iterator2::value_type);
  ///           called when an element is in both sequences.
  /// \tparam Comparator compares elements in sequences.
  ///         called using both (_Iterator1::key_type, _Iterator2::key_type)
  //          and (_Iterator2::key_type, _Iterator1::key_type).
  template <class _Iterator1, class _Iterator2, class BinaryOperator, class Comparator>
  BinaryOperator
  merge_keys( _Iterator1 aa1, _Iterator1 zz1,
              _Iterator2 aa2, _Iterator2 zz2,
              BinaryOperator binop,
              Comparator comp
            )
  {
    static constexpr unavailable_t unavail;

    while (aa1 != zz1 && aa2 != zz2)
    {
      if (comp(key(*aa1), key(*aa2)))
      {
        binop(*aa1, unavail);
        ++aa1;
      }
      else if (comp(key(*aa2), key(*aa1)))
      {
        binop(unavail, *aa2);
        ++aa2;
      }
      else
      {
        binop(*aa1, *aa2);
        ++aa1; ++aa2;
      }
    }

    while (aa1 != zz1)
    {
      binop(*aa1, unavail);
      ++aa1;
    }

    while (aa2 != zz2)
    {
      binop(unavail, *aa2);
      ++aa2;
    }

    return binop;
  }

  [[noreturn]]
  void reportAndThrowError(ct::ClassKeyType key, const ct::ClassAnalysis& m)
  {
    msgError() << m.classInfo(key) << std::endl;

    if (m.size() == 0)
      msgError() << "\nNote, the class hierarchy database is empty." << std::endl;

    throw std::out_of_range("Unable to find key in ClassAnalysis.");
  }

  [[noreturn]]
  void reportAndThrowError(ct::FunctionKeyType key, const ct::VirtualFunctionAnalysis& m)
  {
    ct::CompatibilityBridge cb;

    msgError() << "Function " << cb.nameOf(key) << " not found in VirtualFunctionAnalysis"
               << std::endl;

    if (m.size() == 0)
      msgError() << "\nNote, the virtual hierarchy database is empty." << std::endl;

    throw std::out_of_range("Unable to find key in VirtualFunctionAnalysis.");
  }


  // AnalysisMap = is either a const or non-const analysis (i.e., ClassAnalysis, VirtualFunctionAnalysis)
  template <class AnalysisMap>
  inline
  auto
  lookup(AnalysisMap& m, const typename AnalysisMap::key_type& key) -> decltype(*m.find(key))
  {
    auto pos = m.find(key);

    if (pos != m.end())
      return *pos;

    reportAndThrowError(key, m);
  }
}

namespace CodeThorn
{

ClassAnalysisInfo ClassAnalysis::classInfo(ClassKeyType classKey) const
{
  return { this, classKey };
}

std::ostream& operator<<(std::ostream& os, ClassAnalysisInfo cai)
{
  const ClassAnalysis* analysis = cai.analysis();
  ClassKeyType         classkey = cai.key();

  if (analysis == nullptr)
    return os << "ClassAnalysisInfo::analysis returns nullptr. Use a valid object."
              << std::endl;

  auto pos = analysis->find(cai.key());
  if (pos != analysis->end())
    return os << "class " << typeNameOf(classkey) << " exists in class hierarchy analysis. OK."
              << std::endl;

  os << "class " << typeNameOf(classkey) << " was not found in the class hierarchy analysis.\n";

  if (!analysis->containsAllClasses())
    os << "  - The class hierarchy analysis was built incrementally.\n"
       << "    Possibly, " << typeNameOf(classkey) << " has not been seen."
       << std::endl;

  if (auto opt = missingDiagnostics(classkey))
    os << "\n    " << *opt
       << std::endl;

  return os;
}

ClassAnalysis::mapped_type&
ClassAnalysis::at(const ClassAnalysis::key_type& k)
{
  return lookup(*this, k).second;
}

const ClassAnalysis::mapped_type&
ClassAnalysis::at(const ClassAnalysis::key_type& k) const
{
  return lookup(*this, k).second;
}

const ClassAnalysis::mapped_type&
ClassAnalysis::at(const SgClassDefinition& clsdef) const
{
  return lookup(*this, &clsdef).second;
}


void
ClassAnalysis::addInheritanceEdge(value_type& descendantEntry, ClassKeyType ancestorKey, bool virtualEdge, bool directEdge)
{
  ClassData&   ancestor = this->at(ancestorKey);
  ClassKeyType descendantKey = descendantEntry.first;
  ClassData&   descendant = descendantEntry.second;

  descendant.ancestors().emplace_back(ancestorKey,   virtualEdge, directEdge);
  ancestor.descendants().emplace_back(descendantKey, virtualEdge, directEdge);
}

void
ClassAnalysis::addInheritanceEdge(value_type& descendant, const InheritanceDesc& ancestor)
{
  addInheritanceEdge(descendant, ancestor.getClass(), ancestor.isVirtual(), ancestor.isDirect());
}

bool
ClassAnalysis::isBaseOf(ClassKeyType ancestorKey, ClassKeyType descendantKey) const
{
  using container = ClassData::AncestorContainer;

  const container&          ancestors = this->at(descendantKey).ancestors();
  container::const_iterator zz = ancestors.end();

  return zz != std::find_if( ancestors.begin(), zz,
                             [ancestorKey](const InheritanceDesc& desc) -> bool
                             {
                               return desc.getClass() == ancestorKey;
                             }
                           );
}

bool
ClassAnalysis::isVirtualBaseOf(ClassKeyType ancestorKey, ClassKeyType descendantKey) const
{
  using container = ClassData::AncestorContainer;

  const container&          ancestors = this->at(descendantKey).ancestors();
  container::const_iterator zz = ancestors.end();

  return zz != std::find_if( ancestors.begin(), zz,
                             [ancestorKey](const InheritanceDesc& desc) -> bool
                             {
                               return desc.isVirtual() && desc.getClass() == ancestorKey;
                             }
                           );
}

std::vector<InheritanceDesc>
ClassAnalysis::concreteDescendants(ClassKeyType classKey) const
{
  using container = ClassData::AncestorContainer;

  CompatibilityBridge          cb;
  std::vector<InheritanceDesc> res;
  const container&             descendants = this->at(classKey).descendants();

  std::copy_if( descendants.begin(), descendants.end(),
                std::back_inserter(res),
                [&cb](const InheritanceDesc& desc) -> bool
                {
                  return !cb.isAbstract(desc.getClass());
                }
              );

  std::sort( res.begin(), res.end(),
             [](const InheritanceDesc& lhs, const InheritanceDesc& rhs) -> bool
             {
               if (lhs.isDirect() != rhs.isDirect())
                 return lhs.isDirect();

               if (lhs.isVirtual() != rhs.isVirtual())
                 return !lhs.isVirtual();

               return lhs < rhs;
             }
           );

  return res;
}




namespace
{
  using RelationAccessor = std::vector<InheritanceDesc>& (ClassData::*)();
  using RelationAccessorConst = const std::vector<InheritanceDesc>& (ClassData::*)() const;

  template <class AnalysisT, class Fn, class MemFnSelector, class Set>
  void traversal(AnalysisT& all, Fn fn, ClassKeyType curr, MemFnSelector selector, Set& explored)
  {
    if (!explored.insert(curr).second)
      return;

    auto& elem = lookup(all, curr);

    for (const InheritanceDesc& desc : (elem.second.*selector)())
    {
      if (desc.isDirect())
        traversal(all, fn, desc.getClass(), selector, explored);
    }

    fn(elem);
  }

  template <class AnalysisT, class Fn, class MemFnSelector>
  void traversal(AnalysisT& all, Fn fn, MemFnSelector selector)
  {
    std::unordered_set<ClassKeyType> explored;

    for (auto& elem : all)
      traversal(all, fn, elem.first, selector, explored);
  }
}


void topDownTraversal(ClassAnalysis& all, ClassAnalysisFn fn)
{
  RelationAccessor ancestors = &ClassData::ancestors;

  traversal(all, fn, ancestors);
}

void topDownTraversal(const ClassAnalysis& all, ClassAnalysisConstFn fn)
{
  RelationAccessorConst ancestors = &ClassData::ancestors;

  traversal(all, fn, ancestors);
}

void bottomUpTraversal(ClassAnalysis& all, ClassAnalysisFn fn)
{
  RelationAccessor descendants = &ClassData::descendants;

  traversal(all, fn, descendants);
}

void bottomUpTraversal(const ClassAnalysis& all, ClassAnalysisConstFn fn)
{
  RelationAccessorConst descendants = &ClassData::descendants;

  traversal(all, fn, descendants);
}

void unorderedTraversal(ClassAnalysis& all, ClassAnalysisFn fn)
{
  for (ClassAnalysis::value_type& elem : all)
    fn(elem);
}

void unorderedTraversal(const ClassAnalysis& all, ClassAnalysisConstFn fn)
{
  for (const ClassAnalysis::value_type& elem : all)
    fn(elem);
}

bool ClassData::hasVirtualFunctions() const
{
  return declaresVirtualFunctions() || inheritsVirtualFunctions();
}

bool ClassData::hasVirtualInheritance() const
{
  ClassData::AncestorContainer::const_iterator aa = ancestors().begin();
  ClassData::AncestorContainer::const_iterator zz = ancestors().end();
  auto isVirtualInheritance = [](const InheritanceDesc& desc) -> bool
                              {
                                return desc.isVirtual();
                              };

  return zz != std::find_if(aa, zz, isVirtualInheritance);
}

bool ClassData::hasVirtualTable() const
{
  return hasVirtualFunctions() || hasVirtualInheritance();
}




namespace
{
  bool inheritsVirtualFunctions(const ClassAnalysis& classes, const ClassAnalysis::value_type& entry)
  {
    const std::vector<InheritanceDesc>&          parents = entry.second.ancestors();
    std::vector<InheritanceDesc>::const_iterator aa = parents.begin();
    std::vector<InheritanceDesc>::const_iterator zz = parents.end();

    while (aa != zz && !classes.at(aa->getClass()).hasVirtualFunctions())
      ++aa;

    return aa != zz;
  }


  void integrateIndirectInheritance(ClassAnalysis& classes, ClassAnalysis::value_type& entry)
  {
    std::vector<InheritanceDesc>  tmp;

    // collect additional ancestors
    for (InheritanceDesc& parent : entry.second.ancestors())
      for (InheritanceDesc ancestor : classes.at(parent.getClass()).ancestors())
      {
        // skip virtual bases (they have already been propagated to derived)
        if (ancestor.isVirtual()) continue;

        ancestor.setDirect(false);
        tmp.push_back(ancestor);
      }

    // add additional ancestors
    for (InheritanceDesc ancestor : tmp)
    {
      classes.addInheritanceEdge(entry, ancestor);
    }
  }

  struct UniqueVirtualInheritancePredicate
  {
    bool operator()(const InheritanceDesc& lhs, const InheritanceDesc& rhs)
    {
      return lhs.getClass() == rhs.getClass();
    }
  };

  struct VirtualInheritanceComparator
  {
    bool operator()(const InheritanceDesc& lhs, const InheritanceDesc& rhs)
    {
      // only virtual inheritance is considered
      assert(lhs.isVirtual() && rhs.isVirtual());

      // at most one can be a direct base
      assert((rhs.isDirect() ^ rhs.isDirect()) <= 1);

      if (lhs.getClass() < rhs.getClass())
       return true;

      if (lhs.getClass() > rhs.getClass())
       return false;

      return lhs.isDirect();
    }
  };


  void copyVirtualInhertanceToDerived(ClassAnalysis& classes, ClassAnalysis::value_type& entry)
  {
    using iterator = std::vector<InheritanceDesc>::iterator;

    std::vector<InheritanceDesc> tmp;

    // collect addition ancestors
    for (InheritanceDesc& parent : entry.second.ancestors())
    {
      if (parent.isVirtual())
        tmp.push_back(parent);

      for (InheritanceDesc ancestor : classes.at(parent.getClass()).ancestors())
      {
        if (!ancestor.isVirtual()) continue;

        ancestor.setDirect(false);
        tmp.push_back(ancestor);
      }
    }

    std::sort(tmp.begin(), tmp.end(), VirtualInheritanceComparator{});

    iterator zz = std::unique(tmp.begin(), tmp.end(), UniqueVirtualInheritancePredicate{});

    // add additional ancestors
    std::for_each( tmp.begin(), zz,
                   [&classes, &entry](const InheritanceDesc& ancestor) -> void
                   {
                     if (!ancestor.isDirect())
                       classes.addInheritanceEdge(entry, ancestor);
                   }
                 );
  }

  void computeVirtualBaseInitializationOrder(ClassAnalysis& classes, ClassAnalysis::value_type& entry)
  {
    std::set<ClassKeyType>     alreadySeen;
    std::vector<ClassKeyType>& initorder = entry.second.virtualBaseClassOrder();

    // traverse direct ancestors and collect their initialization orders
    for (InheritanceDesc& parentDesc : entry.second.ancestors())
    {
      if (!parentDesc.isDirect()) continue;

      ClassKeyType                     parent = parentDesc.getClass();
      const std::vector<ClassKeyType>& parentInit = classes.at(parent).virtualBaseClassOrder();

      // add the virtual bases that have not been seen before
      for (ClassKeyType vbase : parentInit)
        if (alreadySeen.insert(vbase).second)
          initorder.push_back(vbase);

      if (parentDesc.isVirtual() && alreadySeen.insert(parent).second)
        initorder.push_back(parent);
    }
  }

  void analyzeClassRelationships(ClassAnalysis& all)
  {
    SAWYER_MESG(msgTrace())
              << all.size() << std::endl;

    auto propagateVirtualInheritance =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             copyVirtualInhertanceToDerived(all, rep);
           };
    topDownTraversal(all, propagateVirtualInheritance);

    auto computeVirtualBaseClassInitializationOrder =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             computeVirtualBaseInitializationOrder(all, rep);
           };
    topDownTraversal(all, computeVirtualBaseClassInitializationOrder);

    auto flattenInheritance =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             integrateIndirectInheritance(all, rep);
           };
    topDownTraversal(all, flattenInheritance);

    auto analyzeInheritedVirtualMethod =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             rep.second.inheritsVirtualFunctions(inheritsVirtualFunctions(all, rep));
           };
    topDownTraversal(all, analyzeInheritedVirtualMethod);
  }


  void inheritanceRelations(ClassAnalysis& classes)
  {
    for (ClassAnalysis::value_type& elem : classes)
    {
      inheritanceEdges( elem.first,
                        [&elem, &classes](ClassKeyType child, ClassKeyType parent, bool virt) -> void
                        {
                          assert(elem.first == child);

                          classes.addInheritanceEdge(elem, parent, virt, true /* direct ancestor */);
                        }
                      );
    }
  }

  // returns true iff lhs < rhs
  struct VFNameTypeOrder
  {
    bool operator()(FunctionKeyType lhs, FunctionKeyType rhs) const
    {
      static constexpr bool firstDecisiveComparison = false;

      int res = 0;

      firstDecisiveComparison
      || (res = compat.compareNames(lhs, rhs))
      || (res = compat.compareTypes(lhs, rhs))
      ;

      return res < 0;
    }

    template <class TupleWithFunctionKeyType>
    bool operator()(const TupleWithFunctionKeyType& lhs, const TupleWithFunctionKeyType& rhs) const
    {
      return (*this)(std::get<0>(lhs), std::get<0>(rhs));
    }

    const CompatibilityBridge& compat;
  };

/*
  VirtualFunctionContainer
  extractVirtualFunctionsFromClass(const RoseCompatibilityBridge& rcb, ClassKeyType clkey)
  {
    using BasicVirtualFunctions = std::vector<std::tuple<FunctionId, bool> >;

    VirtualFunctionContainer res;
    BasicVirtualFunctions    funcs = rcb.getVirtualFunctions(clkey);

    res.reserve(funcs.size());
    std::sort(funcs.begin(), funcs.end(), VFNameTypeOrder{ rcb });

    //~ std::move(funcs.begin(), funcs.end(), std::back_inserter(virtualFuncs));
    for (const std::tuple<FunctionId, bool>& func : funcs)
      res.emplace_back(func);

    return res;
  }
*/
}

struct ComputeVFunctionRelation
{
  void operator()(FunctionKeyType drv, FunctionKeyType bas) const
  {
    using ReturnTypeRelation = CompatibilityBridge::ReturnTypeRelation;

    const ReturnTypeRelation rel = compat.haveSameOrCovariantReturn(classes, bas, drv);
    ASSERT_require(rel != CompatibilityBridge::unrelated);

    if (rel == CompatibilityBridge::unrelated)
    {
      msgError() << "oops, covariant return assertion failed: "
                 << compat.nameOf(bas) << " <> " << compat.nameOf(drv)
                 << std::endl;
      return;
    }

    //~ std::cerr << compat.nameOf(drv) << " overrides " << compat.nameOf(bas)
              //~ << std::endl;

    const bool               covariant = rel == CompatibilityBridge::covariant;

    vfunAnalysis.at(drv).overridden().emplace_back(bas, covariant);
    vfunAnalysis.at(bas).overriders().emplace_back(drv, covariant);
  }

  void operator()(FunctionKeyType, unavailable_t) const {}

  void operator()(unavailable_t, FunctionKeyType) const {}

  // data members
  const CompatibilityBridge& compat;
  const ClassAnalysis&       classes;
  VirtualFunctionAnalysis&   vfunAnalysis;
  ClassKeyType               ancestor;
  ClassKeyType               descendant;
};

using SortedVirtualMemberFunctions = std::unordered_map<ClassKeyType, ClassData::VirtualFunctionContainer>;

void computeOverriders( const CompatibilityBridge& compat,
                        const ClassAnalysis& classes,
                        VirtualFunctionAnalysis& vfunAnalysis,
                        const ClassAnalysis::value_type& entry,
                        bool /*normalizedSignature*/,
                        SortedVirtualMemberFunctions& sortedVFunMap
                      )
{
  using VirtualFunctionContainer = ClassData::VirtualFunctionContainer;

  // create a new entry
  VirtualFunctionContainer& vfunSorted = sortedVFunMap[entry.first];
  ASSERT_require(vfunSorted.empty());

  vfunSorted = entry.second.virtualFunctions();
  std::sort(vfunSorted.begin(), vfunSorted.end(), VFNameTypeOrder{ compat });

  std::for_each( vfunSorted.begin(), vfunSorted.end(),
                 [&vfunAnalysis, &entry, &compat](FunctionKeyType id) -> void
                 {
                   vfunAnalysis.emplace(id, VirtualFunctionDesc{entry.first, compat.isPureVirtual(id)});
                 }
               );

  for (const InheritanceDesc& parentDesc : entry.second.ancestors())
  {
    VirtualFunctionContainer& parentVFunSorted = sortedVFunMap.at(parentDesc.getClass());

    merge_keys( vfunSorted.begin(), vfunSorted.end(),
                parentVFunSorted.begin(), parentVFunSorted.end(),
                ComputeVFunctionRelation{compat, classes, vfunAnalysis, entry.first, parentDesc.getClass()},
                VFNameTypeOrder{compat}
              );
  }
}

VirtualFunctionAnalysis
analyzeVirtualFunctions(const CompatibilityBridge& compat, const ClassAnalysis& all, bool normalizedSignature)
{
  VirtualFunctionAnalysis      res;
  SortedVirtualMemberFunctions tmpSorted;

  topDownTraversal( all,
                    [&compat, &all, &res, &tmpSorted, &normalizedSignature]
                    (const ClassAnalysis::value_type& clrep) -> void
                    {
                      computeOverriders(compat, all, res, clrep, normalizedSignature, tmpSorted);
                    }
                  );

  return res;
}

VirtualFunctionAnalysis
analyzeVirtualFunctions(const ClassAnalysis& all, bool normalizedSignature)
{
  return analyzeVirtualFunctions(CompatibilityBridge{}, all, normalizedSignature);
}


AnalysesTuple
analyzeClassesAndCasts(const CompatibilityBridge& compat, ASTRootType n)
{
  ClassAnalysis classes{true /* full view of translation unit */};
  CastAnalysis  casts;

  compat.extractFromProject(classes, casts, n);
  inheritanceRelations(classes);
  analyzeClassRelationships(classes);

  return AnalysesTuple{std::move(classes), std::move(casts)};
}

AnalysesTuple
analyzeClassesAndCasts(ASTRootType n)
{
  return analyzeClassesAndCasts(CompatibilityBridge{}, n);
}

ClassAnalysis
analyzeClasses(const CompatibilityBridge& compat, ASTRootType n)
{
  return std::move(analyzeClassesAndCasts(compat, n).classAnalysis());
}

ClassAnalysis
analyzeClasses(ASTRootType n)
{
  return analyzeClasses(CompatibilityBridge{}, n);
}

ClassAnalysis
analyzeClassesFromMemoryPool()
{
  ClassAnalysis classes{true /* full view of translation unit */};

  CompatibilityBridge{}.extractFromMemoryPool(classes);
  inheritanceRelations(classes);
  analyzeClassRelationships(classes);

  return classes;
}

ClassAnalysis analyzeClass(ClassKeyType n)
{
  CompatibilityBridge compat;
  ClassAnalysis       classes{false /* incomplete view */};

  // collect all classes reachable upwards from n
  compat.extractClassAndBaseClasses(classes, n);

  inheritanceRelations(classes);
  analyzeClassRelationships(classes);

  return classes;
}

VirtualFunctionAnalysis::mapped_type&
VirtualFunctionAnalysis::at(const VirtualFunctionAnalysis::key_type& k)
{
  return lookup(*this, k).second;
}

const VirtualFunctionAnalysis::mapped_type&
VirtualFunctionAnalysis::at(const VirtualFunctionAnalysis::key_type& k) const
{
  return lookup(*this, k).second;
}

FunctionPredicate
VirtualFunctionAnalysis::virtualFunctionTest() const
{
  return [self = *this](FunctionKeyType key) -> bool
         {
           return self.find(key) != self.end();
         };
}

}

