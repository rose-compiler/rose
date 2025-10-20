#include "ObjectLayoutAnalysis.h"

#include <numeric>
#include "RoseCompatibility.h"

namespace ct = CodeThorn;

namespace
{
  void addDataMembers(ct::ObjectLayout& ol, const ct::ClassAnalysis::value_type& clazz)
  {
    const std::vector<ct::VariableKeyType>& members = clazz.second.dataMembers();

    for (ct::VariableKeyType id : members)
      ol.emplace_back(0, ct::Field{id});
  }

  struct PrimarySubobjectFinder
  {
    auto operator()(const std::vector<ct::InheritanceDesc>::value_type& cand) -> bool
    {
      return (  (!cand.isVirtual())
             && cand.isDirect()
             && all->at(cand.getClass()).hasVirtualTable()
             );
    };

    const ct::ClassAnalysis* all;
  };


  ct::ObjectLayout
  computeObjectLayout( const ct::ClassAnalysis& all,
                       const ct::ObjectLayoutContainer& layouts,
                       const ct::ClassAnalysis::value_type& clazz
                     )
  {
    using Vec = std::vector<ct::InheritanceDesc>;

    static constexpr bool virtually = true;
    static constexpr bool directly  = true;

    ct::ObjectLayout          res;

    res.abstractClass(clazz.second.abstractClass());

    PrimarySubobjectFinder    aPrimarySubobj{&all};
    const Vec&                parents = clazz.second.ancestors();
    const Vec::const_iterator aa      = parents.begin();
    const Vec::const_iterator zz      = parents.end();
    const Vec::const_iterator primary = std::find_if(aa, zz, aPrimarySubobj);

    // 1) add primary subobject or own vtable
    if (primary != zz)
    {
      // if the class has a primary subobject it is ordered first
      res.emplace_back(0, ct::Subobject{primary->getClass(), !virtually, directly});
    }
    else if (clazz.second.hasVirtualTable())
    {
      // otherwise add a vtable, if the class requires one
      res.emplace_back(0, ct::VTable{clazz.first, true});
    }

    // 2) Add all direct, non-virtual parent classes without vtable
    for (Vec::const_iterator it = aa; it != zz; ++it)
    {
      if (it->isVirtual() || (!it->isDirect()) || all.at(it->getClass()).hasVirtualTable())
        continue;

      res.emplace_back(0, ct::Subobject{it->getClass(), !virtually, directly});
    }

    // 3) Add the class' own data members
    addDataMembers(res, clazz);

    // 4) Add all non-primary, non-virtual parent classes with vtable
    //    \note if primary exists, the loop starts with the element after primary
    for (Vec::const_iterator it = (primary == zz ? zz : primary+1); it != zz; ++it)
    {
      if (!aPrimarySubobj(*it))
        continue;

      res.emplace_back(0, ct::Subobject{it->getClass(), !virtually, directly});
    }

    // 5) Add all virtual ancestors
    for (Vec::const_iterator it = aa; it != zz; ++it)
    {
      if (!it->isVirtual())
        continue;

      res.emplace_back(0, ct::Subobject{it->getClass(), virtually, it->isDirect()});
    }

    return res;
  }

  using OverriderInfo = std::unordered_map<ct::FunctionKeyType, ct::VirtualFunctionEntry >;
  using FreshVirtualFunctions = std::unordered_map<ct::FunctionKeyType, bool>;

  using VirtualMemberFnSummaryBase = std::tuple<OverriderInfo, FreshVirtualFunctions>;
  struct VirtualMemberFnSummary : VirtualMemberFnSummaryBase
  {
    using base = VirtualMemberFnSummaryBase;
    using base::base;

    const std::tuple_element<0, base>::type&
    overriderInfo() const { return std::get<0>(*this); }

    const std::tuple_element<1, base>::type&
    freshVirtualFunctions() const { return std::get<1>(*this); }

    std::tuple_element<1, base>::type&
    freshVirtualFunctions() { return std::get<1>(*this); }
  };

  VirtualMemberFnSummary
  createVirtualMemberFunctionSummary(const ct::VirtualFunctionAnalysis& vfa, const ct::ClassAnalysis::value_type& clazz)
  {
    FreshVirtualFunctions fresh;
    OverriderInfo         over;

    for (ct::FunctionKeyType fn : clazz.second.virtualFunctions())
    {
      const ct::VirtualFunctionAnalysis::mapped_type& vfn = vfa.at(fn);

      for (const ct::OverrideDesc& ovrdsc : vfn.overridden())
      {
        const bool pureVirt  = vfn.isPureVirtual();
        const bool retvalAdj = ovrdsc.adjustReturnObj();
        const bool needsGen  = false;
        auto       status = over.emplace( ovrdsc.function(),
                                          ct::VirtualFunctionEntry{fn, clazz.first, retvalAdj, pureVirt, needsGen}
                                        );

        // the same overrider pair may exist for multiple inheritance
        if ((!status.second) && (status.first->second.function() != fn))
        {
          msgError() << ovrdsc.function()->get_name() << " overriden by " << fn->get_name()
                     << " in class " << isSgClassDeclaration(clazz.first->get_parent())->get_name()
                     << std::endl;
        }
      }

      auto status = fresh.emplace(fn, vfn.isPureVirtual());

      ROSE_ASSERT(status.second);
    }

    return VirtualMemberFnSummary{std::move(over), std::move(fresh)};
  }

  void
  processInherited( ct::VTableLayout& vt,
                    const ct::VTableLayout& base,
                    const OverriderInfo& overriders,
                    ct::VTableSection& sect
                  )
  {
    using VTableSections = ct::VTableLayout::VTableSections;

    const VTableSections&          baseSects = base.vtableSections();
    VTableSections::const_iterator beg = baseSects.begin();
    VTableSections::const_iterator lim = std::find_if( beg, baseSects.end(),
                                                       [](const ct::VTableSection& baseSec)->bool
                                                       {
                                                         return baseSec.virtualBase();
                                                       }
                                                     );
    auto         sumFn = [](size_t n, const ct::VTableSection& s) -> size_t { return n+s.numTotal(); };
    const size_t numFn = std::accumulate(beg, lim, size_t{0}, sumFn);

    std::transform( base.begin(), base.begin()+numFn,
                    std::back_inserter(vt),
                    [&overriders](const ct::VTableLayoutElement& el) -> ct::VTableLayoutElement
                    {
                      const ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);
                      auto                            pos = overriders.find(vfn.function());

                      ASSERT_not_null(vfn.getClass());
                      ROSE_ASSERT((pos == overriders.end()) || pos->second.getClass());

                      return pos == overriders.end() ? vfn : pos->second;
                    }
                  );

    sect.numInherited(numFn);
    sect.numTotal(numFn);
  }

  struct CheckedIter : std::tuple<ct::VTableLayout::const_iterator, size_t>
  {
    using base = std::tuple<ct::VTableLayout::const_iterator, size_t>;
    using base::base;

    ct::VTableLayout::const_iterator iter() { return std::get<0>(*this); }
    size_t size() { return std::get<1>(*this); }
  };

  std::vector<CheckedIter>
  collectVirtualBaseSections( const ct::VTableLayout& vt,
                              const ct::VTableLayout& base,
                              size_t numVirtualFunctions,
                              const ct::ClassAnalysis& all,
                              const ct::VTableLayoutContainer& layouts
                            )
  {
    using result_type = std::vector<CheckedIter>;

    result_type                           res;
    const ct::ClassAnalysis::mapped_type& thisClass = all.at(vt.getClass());

    // add the base-vtable
    res.emplace_back(base.begin(), numVirtualFunctions);

    // add vtables overriding base-vtable
    for (ct::InheritanceDesc inh : thisClass.ancestors())
    {
      if (!inh.isDirect() && !inh.isVirtual()) continue;
      if (!all.isVirtualBaseOf(base.getClass(), inh.getClass())) continue;

      // inh.getClass also inherits from base
      const ct::VTableLayout&  overLayout   = layouts.at(inh.getClass());
      const ct::VTableSection& virtBaseSect = overLayout.virtualBaseSection(base.getClass());

      ROSE_ASSERT(virtBaseSect.numTotal() == numVirtualFunctions);
      res.emplace_back(overLayout.begin() + virtBaseSect.startOffset(), numVirtualFunctions);
    }

    return res;
  }

  void
  processInheritedVirtualBases( ct::VTableLayout& vt,
                                const ct::ClassAnalysis& all,
                                const ct::VTableLayout& base,
                                const ct::VTableLayoutContainer& layouts,
                                const OverriderInfo& overriders,
                                ct::VTableSection& sect,
                                const ct::CompatibilityBridge& compat
                              )
  {
    using VTableSections = ct::VTableLayout::VTableSections;

    // 1) find the virtual part in the inherited vtable
    const VTableSections&          baseSects = base.vtableSections();
    VTableSections::const_iterator beg = baseSects.begin();
    VTableSections::const_iterator lim = std::find_if( beg, baseSects.end(),
                                                       [](const ct::VTableSection& baseSec)->bool
                                                       {
                                                         return baseSec.virtualBase();
                                                       }
                                                     );
    auto         sumFn = [](size_t n, const ct::VTableSection& s) -> size_t { return n+s.numTotal(); };
    const size_t numFn = std::accumulate(beg, lim, size_t{0}, sumFn);

    // 2) compute overrider candidates based on base classes
    std::vector<CheckedIter>       candBaseSects = collectVirtualBaseSections(vt, base, numFn, all, layouts);
    ASSERT_require(!candBaseSects.empty());

    // candidates contain an entry for each slot in the vtable
    std::vector<std::vector<ct::VTableLayoutElement> > candidates;

    candidates.reserve(numFn);

    // 2.a) fill each slot in candidates with virtual function candidates
    for (size_t slot = 0; slot < numFn; ++slot)
    {
      // entries stores all candidates for a vtable slot
      std::vector<ct::VTableLayoutElement> entries;

      entries.reserve(candBaseSects.size());

      // 2.a.i) collect all overriders from the virtual base classes
      for (CheckedIter candBaseBeg : candBaseSects)
      {
        ASSERT_require(slot < candBaseBeg.size());
        const ct::VirtualFunctionEntry& ccc = boost::get<ct::VirtualFunctionEntry>(*(candBaseBeg.iter()+slot));

        ASSERT_not_null(ccc.getClass());
        entries.push_back(ccc);
      }

      ASSERT_require(entries.size() == candBaseSects.size());

      // 2.a.ii) Sort entries so that duplicates (by function) are adjacent.
      const std::vector<ct::VTableLayoutElement>::iterator aaa = entries.begin();
      const std::vector<ct::VTableLayoutElement>::iterator zzz = entries.end();
      auto entryLessThan = [](const ct::VTableLayoutElement& lhs, const ct::VTableLayoutElement& rhs) -> bool
                           {
                             const ct::VirtualFunctionEntry& lhsFn = boost::get<ct::VirtualFunctionEntry>(lhs);
                             const ct::VirtualFunctionEntry& rhsFn = boost::get<ct::VirtualFunctionEntry>(rhs);

                             return lhsFn.function() < rhsFn.function();
                           };

      std::sort(aaa, zzz, entryLessThan);

      // 2.a.iii) Get a range of unique elements.
      auto entryEquals = [](const ct::VTableLayoutElement& lhs, const ct::VTableLayoutElement& rhs) -> bool
                         {
                           const ct::VirtualFunctionEntry& lhsFn = boost::get<ct::VirtualFunctionEntry>(lhs);
                           const ct::VirtualFunctionEntry& rhsFn = boost::get<ct::VirtualFunctionEntry>(rhs);

                           return lhsFn.function() == rhsFn.function();
                         };
      const std::vector<ct::VTableLayoutElement>::iterator unq = std::unique(aaa, zzz, entryEquals);
      const size_t unqLen = std::distance(aaa, unq);
      ASSERT_require(unqLen > 0);

      // 2.a.iii) Remove overriden entries: if one function comes from a virtual base of another,
      //          keep only the most derived.

      // entries_revised collects those functions that are not overriden
      std::vector<ct::VTableLayoutElement> entries_revised;

      for (size_t i = 0; i < unqLen; ++i)
      {
        bool                            isDominated = false;
        const ct::VTableLayoutElement&  ci = entries[i];
        const ct::VirtualFunctionEntry& ciFn = boost::get<ct::VirtualFunctionEntry>(ci);
        ASSERT_not_null(ciFn.getClass());

        for (size_t j = 0; j < unqLen; ++j)
        {
          if (i == j) continue;

          const ct::VTableLayoutElement&  cj = entries[j];
          const ct::VirtualFunctionEntry& cjFn = boost::get<ct::VirtualFunctionEntry>(cj);
          ASSERT_not_null(cjFn.getClass());

          if (all.isVirtualBaseOf(ciFn.getClass(), cjFn.getClass()))
            isDominated = true;
        }

        if (!isDominated)
          entries_revised.push_back(ci);
      }

      ASSERT_require(!entries_revised.empty());
      candidates.emplace_back(std::move(entries_revised));
    }

    // 3) compute final overrider
    std::transform( candidates.begin(), candidates.end(),
                    std::back_inserter(vt),
                    [&overriders,&vt,&compat,&all,numFn]
                    (const std::vector<ct::VTableLayoutElement>& els) -> ct::VTableLayoutElement
                    {
                      ROSE_ASSERT(els.size());

                      ct::VirtualFunctionEntry res{nullptr, nullptr, false, false, false};

                      for (const ct::VTableLayoutElement& el : els)
                      {
                        const ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);
                        ASSERT_not_null(vfn.getClass());

                        auto                            pos = overriders.find(vfn.function());
                        const ct::VirtualFunctionEntry& vtElem = (pos == overriders.end() ? vfn : pos->second);
                        ASSERT_not_null(vtElem.getClass());

                        if (!res.function())
                        {
                          res = vtElem;
                          ASSERT_not_null(res.getClass());
                        }
                        else if (res.function() != vtElem.function())
                        {
                          // \note res.needsCompilerGeneration(true) will be set later.
                          if (!compat.isAutoGeneratable(all, vt.getClass(), res.function()))
                            msgError() << "No unique overrider for virtual inheritance!"
                                       << std::endl;

                          // \todo abort on error
                          // ASSERT_require(compat.isAutoGeneratable(all, vt.getClass(), res.function()));
                        }
                      }

                      // sanity check
                      if (!res.getClass())
                      {
                        msgError() << "n=" << numFn << std::endl;
                        ASSERT_not_null(res.getClass());
                      }

                      return res;
                    }
                  );

    sect.numInherited(numFn);
    sect.numTotal(numFn);
  }


  void
  processNonInherited( ct::VTableLayout& vt,
                       const ct::ClassAnalysis::value_type& clazz,
                       const FreshVirtualFunctions& fresh,
                       ct::VTableSection& sect
                     )
  {
    std::transform( fresh.begin(), fresh.end(),
                    std::back_inserter(vt),
                    [&clazz](const FreshVirtualFunctions::value_type& el) -> ct::VTableLayoutElement
                    {
                      constexpr bool noPtrAdj = false;
                      constexpr bool noCompGen = false;

                      return ct::VirtualFunctionEntry{el.first, clazz.first, noPtrAdj, el.second, noCompGen };
                    }
                  );

    sect.numTotal(sect.numTotal() + fresh.size());
  }

  ct::VTableLayout
  computeVTableLayout( const ct::ClassAnalysis& all,
                       const ct::VTableLayoutContainer& layouts,
                       const ct::ClassAnalysis::value_type& clazz,
                       VirtualMemberFnSummary summary,
                       const ct::CompatibilityBridge& compat
                     )
  {
    using Vec = std::vector<ct::InheritanceDesc>;

    ct::VTableLayout          res{clazz.first};
    PrimarySubobjectFinder    aPrimarySubobj{&all};
    const Vec&                parents = clazz.second.ancestors();
    const Vec::const_iterator aa      = parents.begin();
    const Vec::const_iterator zz      = parents.end();
    const Vec::const_iterator primary = std::find_if(aa, zz, aPrimarySubobj);

    // 1) add primary vtable
    ct::VTableSection&        primarySect = res.createVTableSection();

    // 2) add from primary base class
    if (primary != zz)
    {
      // if the class has a primary subobject it is ordered first
      primarySect.associatedClass(primary->getClass());
      processInherited(res, layouts.at(primary->getClass()), summary.overriderInfo(), primarySect);
    }
    else
    {
      primarySect.associatedClass(clazz.first);
    }

    // 3) add virtual functions not yet in the primary table
    std::for_each( res.begin(), res.end(),
                   [&summary](const ct::VTableLayoutElement& el)->void
                   {
                     const ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);

                     if (!vfn.adjustReturnObj())
                       summary.freshVirtualFunctions().erase(vfn.function());
                   }
                 );

    processNonInherited(res, clazz, summary.freshVirtualFunctions(), primarySect);

    // 4) Add all non-primary, non-virtual vtables from parents
    //    \note if primary exists, the loop starts with the element after primary
    for (Vec::const_iterator it = (primary == zz ? zz : primary+1); it != zz; ++it)
    {
      if (!aPrimarySubobj(*it))
        continue;

      ct::VTableSection& nonprimarySect = res.createVTableSection();

      nonprimarySect.associatedClass(it->getClass());
      processInherited(res, layouts.at(it->getClass()), summary.overriderInfo(), nonprimarySect);
    }

    // 5) Add all vtables from virtual ancestors
    for (Vec::const_iterator it = aa; it != zz; ++it)
    {
      if (!it->isVirtual())
        continue;

      ct::VTableSection& virtualSect = res.createVTableSection();

      virtualSect.virtualBase(true);
      virtualSect.associatedClass(it->getClass());
      processInheritedVirtualBases( res,
                                    all,
                                    layouts.at(it->getClass()),
                                    layouts,
                                    summary.overriderInfo(),
                                    virtualSect,
                                    compat
                                  );
    }

    // 6) - check if there are any abstract functions that cannot be auto generated.
    //    - mark all functions that require generation by the compiler.

    bool isAbstract = false;

    std::for_each( res.begin(), res.end(),
                   [&isAbstract,&clazz,&compat,&all](ct::VTableLayoutElement& el)->void
                   {
                     ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);
                     ASSERT_not_null(vfn.getClass());

                     const bool willGen = (  (clazz.first != vfn.getClass())
                                          && compat.isAutoGeneratable(all, clazz.first, vfn.function())
                                          );

                     if (!isAbstract && vfn.isPureVirtual() && !willGen)
                       isAbstract = true;

                     if (willGen)
                     {
                       vfn.setClass(clazz.first);
                       vfn.needsCompilerGeneration(willGen);
                     }
                   }
                 );

    res.isAbstractClass(isAbstract);
    return res;
  }

  ct::VTableLayout
  computeVTableLayout( const ct::ClassAnalysis& all,
                       const ct::VTableLayoutContainer& layouts,
                       const ct::ClassAnalysis::value_type& clazz,
                       const ct::VirtualFunctionAnalysis& vfa,
                       const ct::CompatibilityBridge& compat
                     )
  {
    return computeVTableLayout( all,
                                layouts,
                                clazz,
                                createVirtualMemberFunctionSummary(vfa, clazz),
                                compat
                              );
  }

  // TClassAnalysis = ClassAnalysis or const ClassAnalysis
  template <class TLayoutAnalysis>
  inline
  auto
  lookupAux(TLayoutAnalysis& m, const typename TLayoutAnalysis::key_type& key, const char* code)
    -> decltype(*m.find(key))
  {
    auto pos = m.find(key);

    if (pos != m.end())
      return *pos;

    msgError() << "\nUnable to find representation for class " << ct::typeNameOf(key)
               << "\n  * Number of known classes (in " << code << "): " << m.size()
               << std::endl;

    if (m.size() == 0)
      msgError() << code << " found 0 classes"
                 << std::endl;

    throw std::out_of_range("Unable to find class in LayoutAnalysis.");
  }
}

namespace CodeThorn
{

ObjectLayoutContainer
computeObjectLayouts(const ClassAnalysis& all, bool onlyClassesWithVTable)
{
  ObjectLayoutContainer res;

  auto objectLayoutComputation =
          [&all, &res, onlyClassesWithVTable]
          (const ClassAnalysis::value_type& clazz) -> void
          {
            if (onlyClassesWithVTable && !clazz.second.hasVirtualTable()) return;

            res.emplace(clazz.first, computeObjectLayout(all, res, clazz));
          };

  topDownTraversal(all, objectLayoutComputation);
  return res;
}



VTableLayoutAnalysis
computeVTableLayouts(const ClassAnalysis& all, const VirtualFunctionAnalysis& vfa, const CompatibilityBridge& compat)
{
  VTableLayoutContainer   res;

  auto vtableLayoutComputation =
          [&all, &vfa, &compat, &res]
          (const ClassAnalysis::value_type& clazz) -> void
          {
            if (!clazz.second.hasVirtualTable()) return;

            res.emplace(clazz.first, computeVTableLayout(all, res, clazz, vfa, compat));
          };
  topDownTraversal(all, vtableLayoutComputation);
  return res;
}

VTableLayoutAnalysis
computeVTableLayouts(const ClassAnalysis& all, const VirtualFunctionAnalysis& vfa)
{
  CompatibilityBridge compat;

  return computeVTableLayouts(all, vfa, compat);
}

//
// VTableLayout

const VTableSection&
VTableLayout::virtualBaseSection(ClassKeyType key) const
{
  VTableSections::const_iterator pos = sections.begin();
  VTableSections::const_iterator lim = sections.end();
  auto pred = [key](const VTableSection& sec) { return sec.virtualBase() && sec.associatedClass() == key; };

  pos = std::find_if(pos, lim, pred);
  ASSERT_require(pos != lim);
  return *pos;
}

VTableSection&
VTableLayout::createVTableSection()
{
  sections.emplace_back(nullptr, 0, 0, size(), false);
  return sections.back();
}

VTableLayoutElement&
VTableLayout::at(const VTableSection& sec, std::size_t i)
{
  return this->base::at(sec.startOffset() + i);
}

const VTableLayoutElement&
VTableLayout::at(const VTableSection& sec, std::size_t i) const
{
  return this->base::at(sec.startOffset() + i);
}



//
// VTableLayoutAnalysis

const VTableLayoutAnalysis::mapped_type&
VTableLayoutAnalysis::at(const VTableLayoutAnalysis::key_type& k) const
{
  return lookupAux(*this, k, "VTableLayoutAnalysis").second;
}

VTableLayoutAnalysis::mapped_type&
VTableLayoutAnalysis::at(const VTableLayoutAnalysis::key_type& k)
{
  return lookupAux(*this, k, "VTableLayoutAnalysis").second;
}

//
// ObjectLayoutAnalysis

const ObjectLayoutAnalysis::mapped_type&
ObjectLayoutAnalysis::at(const ObjectLayoutAnalysis::key_type& k) const
{
  return lookupAux(*this, k, "ObjectLayoutAnalysis").second;
}

ObjectLayoutAnalysis::mapped_type&
ObjectLayoutAnalysis::at(const ObjectLayoutAnalysis::key_type& k)
{
  return lookupAux(*this, k, "ObjectLayoutAnalysis").second;
}

}
