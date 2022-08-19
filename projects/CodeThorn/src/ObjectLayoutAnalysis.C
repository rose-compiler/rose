
#include <numeric>
#include "ObjectLayoutAnalysis.h"
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
  
  using OverriderInfo = std::unordered_map<ct::FunctionKeyType, ct::VTableLayoutEntry >;
  using FreshVirtualFunctions = std::map<ct::FunctionKeyType, bool>;
  
  struct VirtualMemberFnSummary : std::tuple<OverriderInfo, FreshVirtualFunctions>
  {
    using base = std::tuple<OverriderInfo, FreshVirtualFunctions>;
    using base::base;

    const std::tuple_element<0, base>::type& 
    overriderInfo() const { return std::get<0>(*this); }
    
    const std::tuple_element<1, base>::type& 
    freshVirtualFunctions() const { return std::get<1>(*this); }
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
        auto status = over.emplace(ovrdsc.function(), ct::VTableLayoutEntry{fn, vfn.isPureVirtual(), ovrdsc.adjustReturnObj()});
        
        ROSE_ASSERT(status.second);
      }
      
      if (vfn.overridden().empty())
      {
        auto status = fresh.emplace(fn, vfn.isPureVirtual());
        
        ROSE_ASSERT(status.second);
      }
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
                    [&overriders](const ct::VTableLayoutEntry& el) -> ct::VTableLayoutEntry
                    {
                      auto pos = overriders.find(el.function());
                      
                      return pos == overriders.end() ? el : pos->second;
                    }
                  );
    
    sect.numInherited(numFn);
    sect.numTotal(numFn);
  }
  
  
  void
  processNonInherited( ct::VTableLayout& vt, 
                       const ct::ClassAnalysis::value_type& /* clazz */,
                       const FreshVirtualFunctions& fresh,
                       ct::VTableSection& sect
                     )
  {
    std::transform( fresh.begin(), fresh.end(), 
                    std::back_inserter(vt),
                    [](const FreshVirtualFunctions::value_type& el) -> ct::VTableLayoutEntry
                    {
                      return ct::VTableLayoutEntry{el.first, false /* no ptr adj */, el.second};
                    }
                  );
    
    sect.numTotal(sect.numTotal() + fresh.size());
  }
  
  ct::VTableLayout
  computeVTableLayout( const ct::ClassAnalysis& all,
                       const ct::VTableLayoutContainer& layouts,
                       const ct::ClassAnalysis::value_type& clazz,
                       VirtualMemberFnSummary summary
                     )
  {
    using Vec = std::vector<ct::InheritanceDesc>;

    static constexpr bool virtually = true;
    static constexpr bool directly  = true;

    ct::VTableLayout          res;
    PrimarySubobjectFinder    aPrimarySubobj{&all};
    const Vec&                parents = clazz.second.ancestors();
    const Vec::const_iterator aa      = parents.begin();
    const Vec::const_iterator zz      = parents.end();
    const Vec::const_iterator primary = std::find_if(aa, zz, aPrimarySubobj);
    
    // 1) add primary vtable
    ct::VTableSection&           primarySect = res.createVTableSection();

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
    
    // 3) add virtual functions not in the primary table and covariant functions
    //    that require this pointer adjustment.
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
      processInherited(res, layouts.at(it->getClass()), summary.overriderInfo(), virtualSect);
    }
    
    ct::VTableLayout::iterator zzz = res.end();
    ct::VTableLayout::iterator pos = std::find_if( res.begin(), zzz,
                                                   [](const ct::VTableLayoutEntry& e)->bool
                                                   {
                                                     return e.isPureVirtual();
                                                   }
                                                 );
    res.isAbstractClass(pos != zzz);
    return res;
  }
      
  ct::VTableLayout
  computeVTableLayout( const ct::ClassAnalysis& all,
                       const ct::VTableLayoutContainer& layouts,
                       const ct::ClassAnalysis::value_type& clazz,
                       const ct::VirtualFunctionAnalysis& vfa
                     )
  {
    return computeVTableLayout( all, 
                                layouts, 
                                clazz,
                                createVirtualMemberFunctionSummary(vfa, clazz)
                              );
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
            if (!onlyClassesWithVTable || clazz.second.hasVirtualTable())
            {
              res[clazz.first] = computeObjectLayout(all, res, clazz);
            }
          };

  topDownTraversal(all, objectLayoutComputation);
  return res;
}

VTableLayoutContainer
computeVTableLayouts(const ClassAnalysis& all, const VirtualFunctionAnalysis& vfa)
{
  VTableLayoutContainer res;
  
  auto vtableLayoutComputation =
          [&all, &vfa, &res]
          (const ClassAnalysis::value_type& clazz) -> void
          {
            if (!clazz.second.hasVirtualTable()) return;
            
            res[clazz.first] = computeVTableLayout(all, res, clazz, vfa);
          };
  topDownTraversal(all, vtableLayoutComputation);
  return res;
}

}
