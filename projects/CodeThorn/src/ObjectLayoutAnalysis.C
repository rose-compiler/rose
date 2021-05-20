
#include "ObjectLayoutAnalysis.h"
#include "RoseCompatibility.h"

namespace ct = CodeThorn;

namespace
{
  void addDataMembers(ct::RoseCompatibilityBridge& ctx, ct::ObjectLayout& ol, const ct::ClassAnalysis::value_type& clazz)
  {
    std::vector<ct::VariableId> members = getDataMembers(ctx, clazz.first);

    for (ct::VariableId id : members)
      ol.emplace_back(0, ct::Field{id});
  }

  ct::ObjectLayout
  computeObjectLayout( ct::RoseCompatibilityBridge& ctx,
                       const ct::ClassAnalysis& all,
                       const ct::ObjectLayoutContainer& layouts,
                       const ct::ClassAnalysis::value_type& clazz
                     )
  {
    using Vec = std::vector<ct::InheritanceDesc>;

    ct::ObjectLayout          res;
    bool                      primary = true;
    const Vec&                parents = clazz.second.parents();
    const Vec::const_iterator zz = parents.end();

    if (ct::hasVirtualTable(clazz))
    {
      res.emplace_back(0, ct::VTable{clazz.first, true});
    }

    // compute entries for direct, non-virtual ancestors
    for (Vec::const_iterator aa = parents.begin(); aa != zz; ++aa)
    {
      if (aa->isVirtual() || !aa->isDirect())
        continue;

      if (!primary && ct::hasVirtualTable(lookup(all, aa->getClass())))
      {
        res.emplace_back(0, ct::VTable{aa->getClass(), false});
      }

      res.emplace_back(0, ct::Subobject{aa->getClass(), false});

      if (primary)
      {
        primary = false;
        addDataMembers(ctx, res, clazz);
      }
    }

    if (primary)
    {
      addDataMembers(ctx, res, clazz);
    }

    // compute entries for all virtual ancestors
    for (Vec::const_iterator aa = parents.begin(); aa != zz; ++aa)
    {
      if (!aa->isVirtual())
        continue;

      res.emplace_back(0, ct::Subobject{aa->getClass(), true});
    }

    return res;
  }
}

namespace CodeThorn
{

ObjectLayoutContainer
computeObjectLayouts(RoseCompatibilityBridge& ctx, const ClassAnalysis& all, bool onlyClassesWithVTable)
{
  ObjectLayoutContainer res;

  auto objectLayoutComputation =
          [&ctx, &all, &res, onlyClassesWithVTable]
          (const ClassAnalysis::value_type& clazz) -> void
          {
            if (!onlyClassesWithVTable || hasVirtualTable(clazz))
            {
              res[clazz.first] = computeObjectLayout(ctx, all, res, clazz);
            }
          };

  topDownTraversal(all, objectLayoutComputation);
  return res;
}

struct ObjectLayoutElementPrinter : boost::static_visitor<void>
{
    explicit
    ObjectLayoutElementPrinter(const ObjectLayoutElement& el, RoseCompatibilityBridge* ctx = nullptr)
    : entry(el), astctx(ctx), os(nullptr)
    {}

    void operator()(const Subobject& subobj) const
    {
      (*os) << "subobj " << typeNameOfClassKeyType(subobj.ref)
            << (subobj.isVirtual ? " [virtual]" : "");
    }

    void operator()(const Field& fld) const
    {
      ct::VariableIdMapping* vmap = astctx ? &astctx->vmap() : nullptr;

      (*os) << "field  "
            << (astctx ? fld.id.toString(vmap) : std::string{})
            << " " << fld.id.toString();
    }

    void operator()(const VTable& vtbl) const
    {
      (*os) << "vtable " << typeNameOfClassKeyType(vtbl.ref)
            << (vtbl.isPrimary ? " [primary]" : "");
    }

    void stream(std::ostream& out) const { os = &out; }

    const ObjectLayoutElement& obj() const { return entry; }

  private:
    const ObjectLayoutElement& entry;
    RoseCompatibilityBridge*         astctx;
    mutable std::ostream*      os;
};

std::ostream& operator<<(std::ostream& os, const ObjectLayoutElementPrinter& prn)
{
  prn.stream(os);

  boost::apply_visitor(prn, prn.obj());
  return os;
}

std::ostream& operator<<(std::ostream& os, const ObjectLayoutEntry& entry)
{
  return os << std::get<0>(entry) << " " << ObjectLayoutElementPrinter{std::get<1>(entry)};
}

std::ostream& operator<<(std::ostream& os, const ObjectLayoutContainer& cont)
{
  for (const ObjectLayoutContainer::value_type& entry : cont)
  {
    os << '\n' << typeNameOfClassKeyType(entry.first) << std::endl;

    for (const ObjectLayoutEntry& elem : entry.second)
    {
      os << elem << std::endl;
    }
  }

  return os;
}

std::ostream& operator<<(std::ostream& os, const ObjectLayoutPrinter& prn)
{
  const ObjectLayoutContainer& cont = std::get<1>(prn);

  for (const ObjectLayoutContainer::value_type& entry : cont)
  {
    os << '\n' << typeNameOfClassKeyType(entry.first) << std::endl;

    for (const ObjectLayoutEntry& elem : entry.second)
    {
      os << std::get<0>(elem) << " " << ObjectLayoutElementPrinter{std::get<1>(elem), &std::get<0>(prn)}
         << std::endl;
    }
  }

  return os;
}

}
