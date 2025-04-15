#include "ClassHierarchyAnalysis.h"
#include "Utility.h"

#include <algorithm>
#include <unordered_set>
#include <cassert>
#include <iterator>

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/join.hpp>

namespace ct = CodeThorn;
namespace adapt = boost::adaptors;


namespace
{
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



const ClassData::SpecialMemberFunctionContainer&
ClassData::specialMemberFunctions() const
{
  return allSpecialMemberFunctions;
}

ClassData::SpecialMemberFunctionContainer&
ClassData::specialMemberFunctions()
{
  return allSpecialMemberFunctions;
}

#if COPY_CTOR_ANALYSIS
ClassData::SpecialMemberFunctionContainer
ClassData::specialMemberFunctions(SpecialMemberFunction::Kind knd) const
{
  msgError() << "SpecialMemberFunctions not implemented" << std::endl;
  return {};
}
#endif /*COPY_CTOR_ANALYSIS*/

}




namespace
{
  using RelationAccessor = std::vector<ct::InheritanceDesc>& (ct::ClassData::*)();
  using RelationAccessorConst = const std::vector<ct::InheritanceDesc>& (ct::ClassData::*)() const;

  struct GetClass
  {
    ct::ClassKeyType operator()(const ct::InheritanceDesc& desc) const
    {
      return desc.getClass();
    }
  };

  struct GetType
  {
    ct::ClassKeyType
    operator()(ct::VariableKeyType id) const
    {
      ct::CompatibilityBridge compat;

      return compat.typeOf(id).classType();
    }
  };

  template <class AnalysisT, class MemFnSelector>
  auto
  inheritanceRelationships(AnalysisT& all, MemFnSelector selector)
  {
    return [selector, &all](auto& elem) // -> someRange
           {
             return (elem.second.*selector)() | adapt::filtered(ct::IsDirect{}) | adapt::transformed(GetClass{});
           };
  }

  auto
  valueTypes(const ct::ClassAnalysis::value_type& clazz) // -> someRange
  {
    return clazz.second.dataMembers() | adapt::transformed(GetType{}) | adapt::filtered(ct::IsNotNull{});
  }

  template <class AnalysisT, class Fn, class Selector, class Set>
  void traversal(AnalysisT& all, Fn fn, ct::ClassKeyType curr, Selector selector, Set& explored)
  {
    if (!explored.insert(curr).second)
      return;

    auto& elem = lookup(all, curr);

    for (ct::ClassKeyType clazz : selector(elem))
    {
      try
      {
        traversal(all, fn, clazz, selector, explored);
      }
      catch (...)
      {
        msgError() << "  .. required from " << ct::typeNameOf(elem.first)
                   << "\n     and dependency " << ct::typeNameOf(clazz) << " - " << typeid(*clazz).name()
                   << "\n     CONSIDER: use CodeThorn::analyzeClassesFromMemoryPool to find all classes."
                   << std::endl;

        throw;
      }
    }

    fn(elem);
  }


  template <class AnalysisT, class Fn, class MemFnSelector>
  void traversal(AnalysisT& all, Fn fn, MemFnSelector selector)
  {
    std::unordered_set<ct::ClassKeyType> explored;

    for (auto& elem : all)
      traversal(all, fn, elem.first, inheritanceRelationships(all, selector), explored);
  }

  template <class Fn>
  void traverseInTopologicalOrder(ct::ClassAnalysis& all, Fn fn)
  {
    std::unordered_set<ct::ClassKeyType> explored;

    auto preOrderClasses =
           [&all](auto& elem) // -> someRange
           {
             RelationAccessorConst ancestors = &ct::ClassData::ancestors;

             return boost::range::join( inheritanceRelationships(all, ancestors)(elem),
                                        valueTypes(elem)
                                      );
           };

    for (auto& elem : all)
      traversal(all, fn, elem.first, preOrderClasses, explored);
  }
}

namespace CodeThorn
{

SpecialMemberFunctionPredicate
isCopyCtor()
{
  return [](const SpecialMemberFunction& smf)->bool
         {
           return smf.isCopyCtor();
         };
}

SpecialMemberFunctionPredicate
isCopyAssign()
{
  return [](const SpecialMemberFunction& smf)->bool
         {
           return smf.isCopyAssign();
         };
}

SpecialMemberFunctionPredicate
isDefaultCtor()
{
  return [](const SpecialMemberFunction& csf)->bool
         {
           return csf.isDefaultCtor();
         };
}

SpecialMemberFunctionPredicate
isDtor()
{
  return [](const SpecialMemberFunction& csf)->bool
         {
           return csf.isDestructor();
         };
}

SpecialMemberFunctionPredicate
isUserDefinedCtor()
{
  return [](const ct::SpecialMemberFunction& smf)->bool
         {
           return (  smf.isConstructor()
                  && (!smf.compilerGenerated())
                  );
         };
}

bool hasSpecialFunction(const SpecialMemberFunctionContainer& specials, SpecialMemberFunctionPredicate pred)
{
  return std::any_of( specials.begin(), specials.end(),
                      pred
                    );
}

bool hasSpecialFunction(const ClassAnalysis& all, ClassKeyType clazz, SpecialMemberFunctionPredicate pred)
{
  return hasSpecialFunction(all.at(clazz).specialMemberFunctions(), std::move(pred));
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

}



namespace
{
  bool inheritsVirtualFunctions(const ct::ClassAnalysis& classes, const ct::ClassAnalysis::value_type& entry)
  {
    const std::vector<ct::InheritanceDesc>&          parents = entry.second.ancestors();
    std::vector<ct::InheritanceDesc>::const_iterator aa = parents.begin();
    std::vector<ct::InheritanceDesc>::const_iterator zz = parents.end();

    while (aa != zz && !classes.at(aa->getClass()).hasVirtualFunctions())
      ++aa;

    return aa != zz;
  }


  void integrateIndirectInheritance(ct::ClassAnalysis& classes, ct::ClassAnalysis::value_type& entry)
  {
    std::vector<ct::InheritanceDesc>  tmp;

    // collect additional ancestors
    for (ct::InheritanceDesc& parent : entry.second.ancestors())
      for (ct::InheritanceDesc ancestor : classes.at(parent.getClass()).ancestors())
      {
        // skip virtual bases (they have already been propagated to derived)
        if (ancestor.isVirtual()) continue;

        ancestor.setDirect(false);
        tmp.push_back(ancestor);
      }

    // add additional ancestors
    for (ct::InheritanceDesc ancestor : tmp)
    {
      classes.addInheritanceEdge(entry, ancestor);
    }
  }

  struct UniqueVirtualInheritancePredicate
  {
    bool operator()(const ct::InheritanceDesc& lhs, const ct::InheritanceDesc& rhs)
    {
      return lhs.getClass() == rhs.getClass();
    }
  };

  struct VirtualInheritanceComparator
  {
    bool operator()(const ct::InheritanceDesc& lhs, const ct::InheritanceDesc& rhs)
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


  void copyVirtualInhertanceToDerived(ct::ClassAnalysis& classes, ct::ClassAnalysis::value_type& entry)
  {
    using iterator = std::vector<ct::InheritanceDesc>::iterator;

    std::vector<ct::InheritanceDesc> tmp;

    // collect addition ancestors
    for (ct::InheritanceDesc& parent : entry.second.ancestors())
    {
      if (parent.isVirtual())
        tmp.push_back(parent);

      for (ct::InheritanceDesc ancestor : classes.at(parent.getClass()).ancestors())
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
                   [&classes, &entry](const ct::InheritanceDesc& ancestor) -> void
                   {
                     if (!ancestor.isDirect())
                       classes.addInheritanceEdge(entry, ancestor);
                   }
                 );
  }

  void computeVirtualBaseInitializationOrder(ct::ClassAnalysis& classes, ct::ClassAnalysis::value_type& entry)
  {
    std::set<ct::ClassKeyType>     alreadySeen;
    std::vector<ct::ClassKeyType>& initorder = entry.second.virtualBaseClassOrder();

    // traverse direct ancestors and collect their initialization orders
    for (ct::InheritanceDesc& parentDesc : entry.second.ancestors() | adapt::filtered(ct::IsDirect{}))
    {
      ct::ClassKeyType                     parent = parentDesc.getClass();
      const std::vector<ct::ClassKeyType>& parentInit = classes.at(parent).virtualBaseClassOrder();

      // add the virtual bases that have not been seen before
      for (ct::ClassKeyType vbase : parentInit)
        if (alreadySeen.insert(vbase).second)
          initorder.push_back(vbase);

      if (parentDesc.isVirtual() && alreadySeen.insert(parent).second)
        initorder.push_back(parent);
    }
  }

  void analyzeClassRelationships(ct::ClassAnalysis& all)
  {
    SAWYER_MESG(msgTrace())
              << "analyzing classes " << all.size() << std::endl;

    auto propagateVirtualInheritance =
           [&all](ct::ClassAnalysis::value_type& rep) -> void
           {
             copyVirtualInhertanceToDerived(all, rep);
           };
    topDownTraversal(all, propagateVirtualInheritance);

    auto computeVirtualBaseClassInitializationOrder =
           [&all](ct::ClassAnalysis::value_type& rep) -> void
           {
             computeVirtualBaseInitializationOrder(all, rep);
           };
    topDownTraversal(all, computeVirtualBaseClassInitializationOrder);

    auto flattenInheritance =
           [&all](ct::ClassAnalysis::value_type& rep) -> void
           {
             integrateIndirectInheritance(all, rep);
           };
    topDownTraversal(all, flattenInheritance);

    auto analyzeInheritedVirtualMethod =
           [&all](ct::ClassAnalysis::value_type& rep) -> void
           {
             rep.second.inheritsVirtualFunctions(inheritsVirtualFunctions(all, rep));
           };
    topDownTraversal(all, analyzeInheritedVirtualMethod);
  }

  std::function<bool(const ct::InheritanceDesc&)>
  baseHasSpecialFunction(const ct::ClassAnalysis& all, ct::SpecialMemberFunctionPredicate pred)
  {
    return [&all, p = std::move(pred)](const ct::InheritanceDesc& baseClass)->bool
           {
             return hasSpecialFunction(all, baseClass.getClass(), p);
           };
  }

  ct::SpecialMemberFunctionPredicate
  isCopyCtorConstRef()
  {
    return [](const ct::SpecialMemberFunction& smf)->bool
           {
             return smf.isCopyCtor() && smf.copyCallableWithConstRefArgument();
           };
  }

  ct::SpecialMemberFunctionPredicate
  isCopyAssignConstRef()
  {
    return [](const ct::SpecialMemberFunction& smf)->bool
           {
             return smf.isCopyAssign() && smf.copyCallableWithConstRefArgument();
           };
  }


  using VariablePredicate = std::function<bool(ct::VariableKeyType)>;

  VariablePredicate
  isDefaultConstructible(const ct::ClassAnalysis& all)
  {
    return [&all](ct::VariableKeyType var)->bool
           {
             // \todo support array types
             ct::CompatibilityBridge compat;
             ct::DataMemberType      desc = compat.typeOf(var);

             if (desc.hasReference())
               return desc.hasInitializer();

             ct::ClassKeyType        clazz = desc.classType();

             return !clazz || hasSpecialFunction(all, clazz, ct::isDefaultCtor());
           };
  }

  VariablePredicate
  isCopyConstructible(const ct::ClassAnalysis& all)
  {
    return [&all](ct::VariableKeyType var)->bool
           {
             // \todo support array types
             ct::CompatibilityBridge compat;
             ct::DataMemberType      desc = compat.typeOf(var);

             //~ if (desc.hasReference())
               //~ return false;

             ct::ClassKeyType        clazz = desc.classType();

             return !clazz || hasSpecialFunction(all, clazz, ct::isCopyCtor());
           };
  }

  VariablePredicate
  isCopyConstructibleConstRef(const ct::ClassAnalysis& all)
  {
    return [&all](ct::VariableKeyType var)->bool
           {
             // \todo support array types
             ct::CompatibilityBridge compat;
             ct::DataMemberType      desc = compat.typeOf(var);

             //~ if (desc.hasReference())
               //~ return false;

             ct::ClassKeyType        clazz = desc.classType();

             return !clazz || hasSpecialFunction(all, clazz, isCopyCtorConstRef());
           };
  }


  VariablePredicate
  isCopyAssignable(const ct::ClassAnalysis& all)
  {
    return [&all](ct::VariableKeyType var)->bool
           {
             // \todo support array types
             ct::CompatibilityBridge compat;
             ct::DataMemberType      desc = compat.typeOf(var);

             if (desc.hasReference() || desc.isConst())
               return false;

             ct::ClassKeyType        clazz = desc.classType();

             return !clazz || hasSpecialFunction(all, clazz, ct::isCopyAssign());
           };
  }

  VariablePredicate
  isCopyAssignableConstRef(const ct::ClassAnalysis& all)
  {
    return [&all](ct::VariableKeyType var)->bool
           {
             // \todo support array types
             ct::CompatibilityBridge compat;
             ct::DataMemberType      desc = compat.typeOf(var);

             if (desc.hasReference() || desc.isConst())
               return false;

             ct::ClassKeyType        clazz = desc.classType();

             return !clazz || hasSpecialFunction(all, clazz, isCopyAssignConstRef());
           };
  }


  void processDefaultCtor( const ct::ClassAnalysis& all,
                           const ct::ClassAnalysis::value_type& rep,
                           ct::SpecialMemberFunctionContainer& funcs
                         )
  {
    // https://en.cppreference.com/w/cpp/language/default_constructor [3/21/25]
    // declared constructors
    // (1) If there is no user-declared constructor or constructor template
    //     [note, the analysis works on instantiated classes, so we can ignore the constructor template]
    //     for a class type, the compiler will implicitly declare a default constructor as an inline
    //     public member of its class.
    // (2) The implicitly-declared (or defaulted on its first declaration) default constructor has an
    //     exception specification as described in dynamic exception specification (until C++17)
    //     noexcept specification (since C++17). [note, seems not relevant here].
    //
    // deleted constructors
    //   The implicitly-declared or explicitly-defaulted default constructor for class T is defined as
    //   deleted if any of the following conditions is satisfied:
    //   * T has a non-static data member of reference type without a default initializer.
    //   * T is a non-union class and(since C++26) has a non-variant non-static non-const-default-constructible
    //     data member of const-qualified type (or possibly multidimensional array thereof) without
    //     a default member initializer.
    //   - T is a union and all of its variant members are of const-qualified type (or possibly
    //     multidimensional array thereof).
    //   - T is a non-union class and all members of any anonymous union member are of const-qualified type
    //     (or possibly multidimensional array thereof). (until C++26)
    //
    //   Given a class type M, T has a potentially constructed subobject obj of type M
    //   (or possibly multidimensional array thereof), and any of the following conditions is satisfied:
    //   M has a destructor that is deleted or inaccessible from the default constructor, and either obj
    //   is non-variant or obj has a default member initializer (since C++26).
    //   All following conditions are satisfied:
    //   - obj is not a non-static data member with a default initializer.
    //   - obj is not a variant member of a union where another non-static data member has a default
    //     initializer (until C++26).
    //   - The overload resolution as applied to find M's default constructor does not result
    //     in a usable candidate, or in the case of obj being a variant member, selects a
    //     non-trivial function (until C++26).
    //   - If no user-defined constructors are present and the implicitly-declared default constructor
    //     is not trivial, the user may still inhibit the automatic generation of an implicitly-defined
    //     default constructor by the compiler with the keyword delete.

    const bool hasUserDefinedCtor = hasSpecialFunction(funcs, ct::isUserDefinedCtor());

    if (hasUserDefinedCtor)
      return;

    // check if the frontend already generated a default constructor
    const bool hasCompilerGeneratedDefaultCtor = hasSpecialFunction(funcs, ct::isDefaultCtor());

    if (hasCompilerGeneratedDefaultCtor)
      return;

    // check ancestors
    const ct::ClassData::AncestorContainer& bases = rep.second.ancestors();
    const bool basesHaveDefaultCtor = std::all_of( bases.begin(), bases.end(),
                                                   baseHasSpecialFunction(all, ct::isDefaultCtor())
                                                 );

    if (!basesHaveDefaultCtor)
      return;

    // check data members
    const ct::ClassData::DataMemberContainer& data = rep.second.dataMembers();
    const bool dataIsDefaultConstructible = std::all_of( data.begin(), data.end(),
                                                         isDefaultConstructible(all)
                                                       );

    if (!dataIsDefaultConstructible)
      return;

    funcs.emplace_back(nullptr, ct::SpecialMemberFunction::dctor, true, false, true);
  }

  void processDefaultDtor( const ct::ClassAnalysis& /*all*/,
                           const ct::ClassAnalysis::value_type& /*rep*/,
                           ct::SpecialMemberFunctionContainer& funcs
                         )
  {
    // check if the dtor is already present, either user-defined or compiler generated
    const bool hasDtor = ct::hasSpecialFunction(funcs, ct::isDtor());

    if (hasDtor)
      return;

    funcs.emplace_back(nullptr, ct::SpecialMemberFunction::dtor, true, false, true);
  }


  // copy constructor
  void processDefaultCopyCtor( const ct::ClassAnalysis& all,
                               const ct::ClassAnalysis::value_type& rep,
                               ct::SpecialMemberFunctionContainer& funcs
                             )
  {
    // - no other user-defined (or compiler-generated) copy-constructor (cctor)
    // - all bases have a usable copy-constructor
    // - all data member can be copy-constructed
    const bool hasCopyCtor = ct::hasSpecialFunction(funcs, ct::isCopyCtor());

    if (hasCopyCtor)
      return;

    // check ancestors
    const ct::ClassData::AncestorContainer& bases = rep.second.ancestors();
    const bool basesSupportConstRefCCtor = std::all_of( bases.begin(), bases.end(),
                                                        baseHasSpecialFunction(all, isCopyCtorConstRef())
                                                      );

    const bool basesHaveCopyCtor = (  basesSupportConstRefCCtor
                                   || std::all_of( bases.begin(), bases.end(),
                                                   baseHasSpecialFunction(all, ct::isCopyCtor())
                                                 )
                                   );


    if (!basesHaveCopyCtor)
      return;

    // check data members
    const ct::ClassData::DataMemberContainer& data = rep.second.dataMembers();
    const bool dataSupportConstRefCCtor = std::all_of( data.begin(), data.end(),
                                                       isCopyConstructibleConstRef(all)
                                                     );
    const bool dataIsCopyConstructible = (  dataSupportConstRefCCtor
                                         || std::all_of( data.begin(), data.end(),
                                                         isCopyConstructible(all)
                                                       )
                                         );

    if (!dataIsCopyConstructible)
      return;

    const bool withConstRef = basesSupportConstRefCCtor & dataSupportConstRefCCtor;

    funcs.emplace_back(nullptr, ct::SpecialMemberFunction::cctor, true, withConstRef, true);
  }



  // copy constructor
  void processDefaultCopyAssign( const ct::ClassAnalysis& all,
                                 const ct::ClassAnalysis::value_type& rep,
                                 ct::SpecialMemberFunctionContainer& funcs
                               )
  {
    // If no user-defined [or compiler generated] copy assignment operators are provided for
    // a class type, the compiler will always declare one as an inline public member of the class.
    // This implicitly-declared copy assignment operator has the form T& T::operator=(const T&)
    // if all of the following is true:
    // - each direct base B of T has a copy assignment operator whose parameters are B or
    //   const B& or const volatile B&;
    // - each non-static data member M of T of class type or array of class type has a
    //   copy assignment operator whose parameters are M or const M& or const volatile M&.
    // [note compilers also seem to accept T& T::operator=(const T) as a vaild cassign.]
    // Otherwise the implicitly-declared copy assignment operator is declared as T& T::operator=(T&).
    //   [note, we do not generate T& T::operator=(T&).]
    const bool hasCopyAssign = ct::hasSpecialFunction(funcs, ct::isCopyAssign());

    if (hasCopyAssign)
      return;

    // check ancestors
    const ct::ClassData::AncestorContainer& bases = rep.second.ancestors();
    const bool basesSupportCopyAssignConstRef = std::all_of( bases.begin(), bases.end(),
                                                             baseHasSpecialFunction(all, isCopyAssignConstRef())
                                                           );
    const bool basesHaveCopyAssign = (  basesSupportCopyAssignConstRef
                                     || std::all_of( bases.begin(), bases.end(),
                                                     baseHasSpecialFunction(all, ct::isCopyAssign())
                                                   )
                                     );

    if (!basesHaveCopyAssign)
      return;

    // check data members
    const ct::ClassData::DataMemberContainer& data = rep.second.dataMembers();

    const bool dataSupportCopyAssignConstRef = std::all_of( data.begin(), data.end(),
                                                            isCopyAssignableConstRef(all)
                                                          );


    const bool dataAreCopyAssignable = (  dataSupportCopyAssignConstRef
                                       || std::all_of( data.begin(), data.end(),
                                                       isCopyAssignable(all)
                                                     )
                                       );

    if (!dataAreCopyAssignable)
      return;

    const bool withConstRef = basesSupportCopyAssignConstRef & dataSupportCopyAssignConstRef;

    funcs.emplace_back(nullptr, ct::SpecialMemberFunction::cassign, true, withConstRef, true);
  }



#if COPY_CTOR_ANALYSIS
  // move constructor
  void processDefaultMCtor(ct::ClassAnalysis& all, ct::ClassAnalysis::value_type& rep, ct::SpecialMemberFunctionContainer& funcs)
  {
    const bool hasUserDefinedCtor = std::any_of( funcs.begin(), funcs.end(),
                                                 ct::isUserDefinedCtor()
                                               );

    if (hasUserDefinedCtor)
      return;

    // check ancestors
    const auto bases             = rep.second.ancestors() | adapt::filtered(DirectBase{});
    const bool basesHaveCopyCtor = std::all_of( bases.begin(), bases.end(),
                                                baseHashasCompilerSupported(all, isCopyCtor());
                                              );

    if (!basesHaveCopyCtor)
      return;

    // check data members
    const ct::ClassData::DataMemberContainer& data = rep.second.dataMembers();
    const bool dataHaveCopyCtor = std::all_of( data.begin(), data.end(),
                                               dataHashasCompilerSupported(all, isCopyCtor());
                                             );

    if (!dataHaveCopyCtor)
      return;

    funcs.emplace_back(nullptr, SpecialMemberFunction::cctor, true);
  }


  void processDefaultCAssign(ct::ClassAnalysis& all, ct::ClassAnalysis::value_type& rep, SpecialMemberFunctionContainer& funcs)
  {
    // If no user-defined copy assignment operators are provided for a class type,
    //   the compiler will always declare one as an inline public member of the class.
    //   This implicitly-declared copy assignment operator has the form T& T::operator=(const T&)
    //   if all of the following is true:
    //   - each direct base B of T has a copy assignment operator
    //     whose parameters are B or const B& or const volatile B&;
    //   - each non-static data member M of T of class type or array of class type
    //     has a copy assignment operator whose parameters are
    //     M or const M& or const volatile M&.
    //   Otherwise the implicitly-declared copy assignment operator is declared as T& T::operator=(T&).

    const bool hasUserDefinedDtor = std::any_of( funcs.begin(), funcs.end(),
                                                 isDtor()
                                               );

    if (hasUserDefinedDtor)
      return;

    funcs.emplace_back(nullptr, ct::SpecialMemberFunction::mctor, true);
  }
#endif /*COPY_CTOR_ANALYSIS*/


  void analyzeAutoGeneratedMemberFunctions(ct::ClassAnalysis& all, ct::ClassAnalysis::value_type& rep)
  {
    ct::CompatibilityBridge            compat;
    ct::SpecialMemberFunctionContainer availFunctions = compat.specialMemberFunctions(rep.first);

    processDefaultCtor(all, rep, availFunctions);
    processDefaultDtor(all, rep, availFunctions);
    processDefaultCopyCtor(all, rep, availFunctions);
    processDefaultCopyAssign(all, rep, availFunctions);

#if COPY_CTOR_ANALYSIS
    processDefaultMCtor(availFunctions);
#endif /*COPY_CTOR_ANALYSIS*/

    rep.second.specialMemberFunctions() = std::move(availFunctions);
  }

  void analyzeSpecialMemberFunctions(ct::ClassAnalysis& all)
  {
    SAWYER_MESG(msgTrace())
              << "analyzing special member functions " << std::endl;

    auto analyzeSpecialMembersFuncs =
           [&all](ct::ClassAnalysis::value_type& rep) -> void
           {
             analyzeAutoGeneratedMemberFunctions(all, rep);
           };

    traverseInTopologicalOrder(all, analyzeSpecialMembersFuncs);
  }


  void inheritanceRelations(ct::ClassAnalysis& classes)
  {
    for (ct::ClassAnalysis::value_type& elem : classes)
    {
      auto addEdge =
             [&elem, &classes](ct::ClassKeyType child, ct::ClassKeyType parent, bool virt) -> void
             {
               assert(elem.first == child);

               classes.addInheritanceEdge(elem, parent, virt, true /* direct ancestor */);
             };

      ct::inheritanceEdges(elem.first, addEdge);
    }
  }

  // returns true iff lhs < rhs
  struct VFNameTypeOrder
  {
    bool operator()(ct::FunctionKeyType lhs, ct::FunctionKeyType rhs) const
    {
      static constexpr bool firstDecisiveComparison = false;

      int res = 0;

      firstDecisiveComparison
      || (res = compat.compareNames(lhs, rhs))
      || (res = compat.compareTypes(lhs, rhs))
      ;

      return res < 0;
    }
/*
    template <class TupleWithFunctionKeyType>
    bool operator()(const TupleWithFunctionKeyType& lhs, const TupleWithFunctionKeyType& rhs) const
    {
      return (*this)(std::get<0>(lhs), std::get<0>(rhs));
    }
*/
    const ct::CompatibilityBridge& compat;
  };

  struct ComputeVFunctionRelation
  {
    void operator()(ct::FunctionKeyType drv, ct::FunctionKeyType bas) const
    {
      using ReturnTypeRelation = ct::CompatibilityBridge::TypeRelation;

      const ReturnTypeRelation rel = compat.haveSameOrCovariantReturn(classes, bas, drv);

      if (rel == ct::CompatibilityBridge::unrelated)
      {
        msgError() << "oops, covariant return assertion failed: "
                   << compat.nameOf(bas) << " <> " << compat.nameOf(drv)
                   << std::endl;
        return;
      }

      ASSERT_require(rel != ct::CompatibilityBridge::unrelated);
      //~ std::cerr << compat.nameOf(drv) << " overrides " << compat.nameOf(bas)
                //~ << std::endl;

      const bool               covariant = rel == ct::CompatibilityBridge::covariant;

      vfunAnalysis.at(drv).overridden().emplace_back(bas, covariant);
      vfunAnalysis.at(bas).overriders().emplace_back(drv, covariant);
    }

    void operator()(ct::FunctionKeyType, unavailable_t) const {}

    void operator()(unavailable_t, ct::FunctionKeyType) const {}

    // data members
    const ct::CompatibilityBridge& compat;
    const ct::ClassAnalysis&       classes;
    ct::VirtualFunctionAnalysis&   vfunAnalysis;
    ct::ClassKeyType               ancestor;
    ct::ClassKeyType               descendant;
  };

  using SortedVirtualMemberFunctions = std::unordered_map<ct::ClassKeyType, ct::ClassData::VirtualFunctionContainer>;

  void computeOverriders( const ct::CompatibilityBridge& compat,
                          const ct::ClassAnalysis& classes,
                          ct::VirtualFunctionAnalysis& vfunAnalysis,
                          const ct::ClassAnalysis::value_type& entry,
                          bool /*normalizedSignature*/,
                          SortedVirtualMemberFunctions& sortedVFunMap
                        )
  {
    using VirtualFunctionContainer = ct::ClassData::VirtualFunctionContainer;

    // create a new entry
    VirtualFunctionContainer& vfunSorted = sortedVFunMap[entry.first];
    ASSERT_require(vfunSorted.empty());

    vfunSorted = entry.second.virtualFunctions();
    std::sort(vfunSorted.begin(), vfunSorted.end(), VFNameTypeOrder{ compat });

    std::for_each( vfunSorted.begin(), vfunSorted.end(),
                   [&vfunAnalysis, &entry, &compat](ct::FunctionKeyType id) -> void
                   {
                     vfunAnalysis.emplace(id, ct::VirtualFunctionDesc{entry.first, compat.isPureVirtual(id)});
                   }
                 );

    for (const ct::InheritanceDesc& parentDesc : entry.second.ancestors())
    {
      VirtualFunctionContainer& parentVFunSorted = sortedVFunMap.at(parentDesc.getClass());

      mergeOrderedSequences( vfunSorted.begin(), vfunSorted.end(),
                             parentVFunSorted.begin(), parentVFunSorted.end(),
                             ComputeVFunctionRelation{compat, classes, vfunAnalysis, entry.first, parentDesc.getClass()},
                             VFNameTypeOrder{compat}
                           );
    }
  }
}

namespace CodeThorn
{

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
  analyzeSpecialMemberFunctions(classes);

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
  ct::CompatibilityBridge compat;
  ClassAnalysis classes{true /* full view of translation unit */};

  compat.extractFromMemoryPool(classes);
  inheritanceRelations(classes);
  analyzeClassRelationships(classes);
  // analyzeSupportedMemberFunctions(classes);

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

