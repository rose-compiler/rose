
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
    auto                      aPrimarySubobj = [&all](const Vec::value_type& cand) -> bool
                                               {
                                                 return (  (!cand.isVirtual())
                                                        && cand.isDirect()
                                                        && all.at(cand.getClass()).hasVirtualTable()
                                                        );
                                               };
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


}
