#ifndef OBJECT_LAYOUT_ANALYSIS_H
#define OBJECT_LAYOUT_ANALYSIS_H 1

#include "RoseCompatibility.h"

// with C++ 17
// #include <variant>
#include <boost/variant.hpp>

#include "ClassHierarchyAnalysis.h"

namespace CodeThorn
{

struct Subobject
{
  ClassKeyType ref;
  bool         isVirtual;
};

struct Field
{
  CodeThorn::VariableId id;
};

struct VTable
{
  ClassKeyType ref;
  bool         isPrimary;
};

/*
struct FlatMemberList : std::vector<Field>
{
    using base = std::vector<Field>;
    using base::base;

    iterator beginVirtualMembers()
    {
      return begin() + firstVirtualEntry;
    }

    const_iterator beginVirtualMembers() const
    {
      return begin() + firstVirtualEntry;
    }

    void firstVirtual(size_t firstVirtualMember) { firstVirtualEntry = firstVirtualMember; }

  private:
    size_t firstVirtualEntry;
};

std::map<ClassTypeKey, FlatMemberList >
flatMemberList(codethorn::VariableIdMapping& vmap, const ClassAnalysis& all);
*/

// in C++17 use STL
// using ObjectLayoutElement = std::variant<Subobject, Field>;
using ObjectLayoutElement = boost::variant<Subobject, Field, VTable>;

struct ObjectLayoutEntry : std::tuple<size_t, ObjectLayoutElement>
{
  using base = std::tuple<size_t, ObjectLayoutElement>;
  using base::base;

  size_t                     offset()  const { return std::get<0>(*this); }
  const ObjectLayoutElement& element() const { return std::get<1>(*this); }
};


struct ObjectLayout : private std::vector<ObjectLayoutEntry>
{
  using base = std::vector<ObjectLayoutEntry>;
  using base::base;

  using base::value_type;
  using base::iterator;
  using base::const_iterator;
  using base::begin;
  using base::end;
  using base::emplace_back;

  //~ size_t object_size()    const;
  //~ size_t number_entries() const;
};

using ObjectLayoutContainer = std::unordered_map<ClassKeyType, ObjectLayout>;

ObjectLayoutContainer
computeObjectLayouts(const ClassAnalysis& all, bool onlyClassesWithVTable = true);


}

#endif
