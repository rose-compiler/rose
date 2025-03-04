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

  struct VirtualMemberFnSummary : std::tuple<OverriderInfo, FreshVirtualFunctions>
  {
    using base = std::tuple<OverriderInfo, FreshVirtualFunctions>;
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

    // 1) find the non-virtual part in the inherited vtable
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
    std::vector<CheckedIter>             candBaseSects = collectVirtualBaseSections(vt, base, numFn, all, layouts);
    std::vector<std::vector<ct::VTableLayoutElement> > candidates;

    candidates.reserve(numFn);

    ROSE_ASSERT(candBaseSects.size());
    for (size_t slot = 0; slot < numFn; ++slot)
    {
      std::vector<ct::VTableLayoutElement> entries;

      entries.reserve(candBaseSects.size());

      for (CheckedIter candBaseBeg : candBaseSects)
      {
        ROSE_ASSERT(slot < candBaseBeg.size());
        const ct::VirtualFunctionEntry& ccc = boost::get<ct::VirtualFunctionEntry>(*(candBaseBeg.iter()+slot));

        ROSE_ASSERT(ccc.getClass());
        entries.push_back(ccc);

        const ct::VirtualFunctionEntry& chk = boost::get<ct::VirtualFunctionEntry>(entries.back());
        ROSE_ASSERT(chk.getClass());
      }

      ROSE_ASSERT(entries.size());

      const std::vector<ct::VTableLayoutElement>::iterator aaa = entries.begin();
      const std::vector<ct::VTableLayoutElement>::iterator zzz = entries.end();
      auto entryLessThan = [](const ct::VTableLayoutElement& lhs, const ct::VTableLayoutElement& rhs) -> bool
                           {
                             const ct::VirtualFunctionEntry& lhsFn = boost::get<ct::VirtualFunctionEntry>(lhs);
                             const ct::VirtualFunctionEntry& rhsFn = boost::get<ct::VirtualFunctionEntry>(rhs);

                             return lhsFn.function() < rhsFn.function();
                           };

      std::sort(aaa, zzz, entryLessThan);

      auto entryEquals = [](const ct::VTableLayoutElement& lhs, const ct::VTableLayoutElement& rhs) -> bool
                         {
                           const ct::VirtualFunctionEntry& lhsFn = boost::get<ct::VirtualFunctionEntry>(lhs);
                           const ct::VirtualFunctionEntry& rhsFn = boost::get<ct::VirtualFunctionEntry>(rhs);

                           return lhsFn.function() == rhsFn.function();
                         };
      const std::vector<ct::VTableLayoutElement>::iterator unq = std::unique(aaa, zzz, entryEquals);
      const size_t unqLen = std::distance(aaa, unq);
      ROSE_ASSERT(unqLen > 0);

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

      ROSE_ASSERT(entries_revised.size());
      candidates.emplace_back(std::move(entries_revised));
      ROSE_ASSERT(candidates.back().size());
    }

    // 3) compute final overrider
    std::transform( candidates.begin(), candidates.end(),
                    std::back_inserter(vt),
                    [&overriders,&vt,&compat,numFn]
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
                          if (!compat.isAutoGeneratable(vt.getClass(), res.function()))
                            msgError() << "No unique overrider for virtual inheritance!"
                                       << std::endl;
                        }
                      }

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

    //~ static constexpr bool virtually = true;
    //~ static constexpr bool directly  = true;

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

    //~ auto nullCheck = [](const ct::VTableLayoutElement& el)->void
                     //~ {
                       //~ const ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);
                       //~ ASSERT_not_null(vfn.getClass());
                     //~ };

    //~ std::for_each(res.begin(), res.end(), nullCheck);

    // 3) add virtual functions not yet in the primary table
    //    no longer: (and covariant functions that require this pointer adjustment.)
    std::for_each( res.begin(), res.end(),
                   [&summary](const ct::VTableLayoutElement& el)->void
                   {
                     const ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);

                     if (!vfn.adjustReturnObj())
                       summary.freshVirtualFunctions().erase(vfn.function());
                   }
                 );

    processNonInherited(res, clazz, summary.freshVirtualFunctions(), primarySect);

    //~ std::for_each(res.begin(), res.end(), nullCheck);


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

    //~ std::for_each(res.begin(), res.end(), nullCheck);

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

    //~ std::for_each(res.begin(), res.end(), nullCheck);
    bool isAbstract = false;

    std::for_each( res.begin(), res.end(),
                   [&isAbstract,&clazz,&compat](ct::VTableLayoutElement& el)->void
                   {
                     ct::VirtualFunctionEntry& vfn = boost::get<ct::VirtualFunctionEntry>(el);
                     ASSERT_not_null(vfn.getClass());

                     const bool willGen = (  (clazz.first != vfn.getClass())
                                          && compat.isAutoGeneratable(clazz.first, vfn.function())
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
  ROSE_ASSERT(pos != lim);
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
