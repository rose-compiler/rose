
#include <algorithm>
#include <unordered_set>
#include <cassert>

#include "ClassHierarchyAnalysis.h"

namespace CodeThorn
{

std::string typeNameOfClassKeyType(ClassKeyType def)
{
  return typeNameOf(def);
}


void
ClassAnalysis::addInheritanceEdge(value_type& descendant, ClassKeyType ancestorKey, bool virtualEdge, bool directEdge, bool throughVirtual)
{
  ClassKeyType descendantKey = descendant.first;
  value_type&  ancestor = lookup(*this, ancestorKey);

  descendant.second.parents().emplace_back(ancestorKey,   virtualEdge, directEdge, throughVirtual);
  ancestor.second.children() .emplace_back(descendantKey, virtualEdge, directEdge, throughVirtual);

  //~ std::cerr << typeNameOfClassKeyType(descendantKey) << " from " << typeNameOfClassKeyType(ancestorKey)
            //~ << " [" << (directEdge ? "d":"") << (virtualEdge ? "v":"") << "]"
            //~ << std::endl;
}

void
ClassAnalysis::addInheritanceEdge(value_type& descendant, const InheritanceDesc& ancestor)
{
  addInheritanceEdge(descendant, ancestor.getClass(), ancestor.isVirtual(), ancestor.isDirect(), ancestor.throughVirtual());
}

bool
ClassAnalysis::isBaseDerived(ClassKeyType ancestorKey, ClassKeyType descendantKey) const
{
  using container = ClassData::AncestorContainer;

  const container&          ancestors = lookup(*this, descendantKey).second.parents();
  container::const_iterator zz = ancestors.end();

  return zz != std::find_if( ancestors.begin(), zz,
                             [ancestorKey](const InheritanceDesc& desc) -> bool
                             {
                               return desc.getClass() == ancestorKey;
                             }
                           );
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
  RelationAccessor ancestors = &ClassData::parents;

  traversal(all, fn, ancestors);
}

void topDownTraversal(const ClassAnalysis& all, ClassAnalysisConstFn fn)
{
  RelationAccessorConst ancestors = &ClassData::parents;

  traversal(all, fn, ancestors);
}

void bottomUpTraversal(ClassAnalysis& all, ClassAnalysisFn fn)
{
  RelationAccessor descendants = &ClassData::children;

  traversal(all, fn, descendants);
}

void bottomUpTraversal(const ClassAnalysis& all, ClassAnalysisConstFn fn)
{
  RelationAccessorConst descendants = &ClassData::children;

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

bool hasVirtualFunctions(const ClassAnalysis::value_type& clazz)
{
  return clazz.second.virtualMethods() || clazz.second.inheritsVirtualMethods();
}

bool isVirtualInheritance(const InheritanceDesc& desc)
{
  return desc.isVirtual();
}

bool hasVirtualInheritance(const ClassAnalysis::value_type& clazz)
{
  const ClassData::AncestorContainer&          ancestors = clazz.second.parents();
  ClassData::AncestorContainer::const_iterator aa = ancestors.begin();
  ClassData::AncestorContainer::const_iterator zz = ancestors.end();

  return zz != std::find_if(aa, zz, isVirtualInheritance);
}

bool hasVirtualTable(const ClassAnalysis::value_type& clazz)
{
  return hasVirtualFunctions(clazz) || hasVirtualInheritance(clazz);
}


namespace
{
  // does a class define a virtual method?
  void analyzeVirtualMethod(ClassAnalysis::value_type& rep)
  {
    rep.second.virtualMethods(classHasVirtualMethods(rep.first));
  }

  bool hasVirtualMethods(const ClassAnalysis::value_type& clazz)
  {
    const ClassData& cd = clazz.second;

    return cd.virtualMethods() || cd.inheritsVirtualMethods();
  }

  bool inheritsVirtualMethods(const ClassAnalysis& all, const ClassAnalysis::value_type& entry)
  {
    const std::vector<InheritanceDesc>&          parents = entry.second.parents();
    std::vector<InheritanceDesc>::const_iterator aa = parents.begin();
    std::vector<InheritanceDesc>::const_iterator zz = parents.end();

    while (aa != zz && !hasVirtualMethods(lookup(all, aa->getClass())))
      ++aa;

    return aa != zz;
  }


  void integrateIndirectInheritance(ClassAnalysis& all, ClassAnalysis::value_type& entry)
  {
    std::vector<InheritanceDesc>  tmp;

    // collect addition ancestors
    for (InheritanceDesc& parent : entry.second.parents())
      for (InheritanceDesc ancestor : lookup(all, parent.getClass()).second.parents())
      {
        // skip virtual bases (they have already been propagated to derived)
        if (ancestor.isVirtual()) continue;

        ancestor.setDirect(false);
        tmp.push_back(ancestor);
      }

    // add additional ancestors
    for (InheritanceDesc ancestor : tmp)
    {
      all.addInheritanceEdge(entry, ancestor);
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


  void copyVirtualInhertanceToDerived(ClassAnalysis& all, ClassAnalysis::value_type& entry)
  {
    using iterator = std::vector<InheritanceDesc>::iterator;

    std::vector<InheritanceDesc> tmp;

    // collect addition ancestors
    for (InheritanceDesc& parent : entry.second.parents())
    {
      if (parent.isVirtual())
        tmp.push_back(parent);

      for (InheritanceDesc ancestor : lookup(all, parent.getClass()).second.parents())
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
                   [&all, &entry](const InheritanceDesc& ancestor) -> void
                   {
                     if (!ancestor.isDirect())
                       all.addInheritanceEdge(entry, ancestor);
                   }
                 );
  }

  void analyzeClasses(ClassAnalysis& all)
  {
    logTrace() << all.size() << std::endl;

    auto propagateVirtualInheritance =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             copyVirtualInhertanceToDerived(all, rep);
           };
    topDownTraversal(all, propagateVirtualInheritance);

    auto flattenInheritance =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             integrateIndirectInheritance(all, rep);
           };
    topDownTraversal(all, flattenInheritance);

    unorderedTraversal(all, analyzeVirtualMethod);

    auto analyzeInheritedVirtualMethod =
           [&all](ClassAnalysis::value_type& rep) -> void
           {
             rep.second.inheritsVirtualMethods(inheritsVirtualMethods(all, rep));
           };
    topDownTraversal(all, analyzeInheritedVirtualMethod);
  }


  void inheritanceRelations(ClassAnalysis& classes)
  {
    for (ClassAnalysis::value_type& elem : classes)
    {
      inheritanceEdges( elem.first,
                        [&elem, &classes](ClassKeyType child, ClassKeyType parent, bool virt, bool direct) -> void
                        {
                          assert(elem.first == child);

                          classes.addInheritanceEdge(elem, parent, virt, direct);
                        }
                      );
    }
  }
}

AnalysesTuple
extractFromProject(SgProject* n)
{
  ClassAnalysis classes;
  CastAnalysis  casts;

  extractFromProject(classes, casts, n);
  inheritanceRelations(classes);
  analyzeClasses(classes);

  return AnalysesTuple{std::move(classes), std::move(casts)};
}

}
