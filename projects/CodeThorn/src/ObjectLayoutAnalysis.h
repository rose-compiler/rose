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
  bool         isDirect;
};

struct Field
{
  CodeThorn::VariableKeyType id;
};

struct VTable
{
  ClassKeyType ref;
  bool         isPrimary;
};


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


class ObjectLayout : private std::vector<ObjectLayoutEntry>
{
  public:
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

/// computes a sample object layout table for classes in \ref all
/// \param all                   the result of the class hierarchy analysis
/// \param onlyClassesWithVTable if true, only classes the layout is only computed for classes with a vtable
/// \details
///    The generated object layout assumes an object model that is vtable based.
///    The generated layout is similar but not exactly the same as the IA64 Itanium object model.
ObjectLayoutContainer
computeObjectLayouts(const ClassAnalysis& all, bool onlyClassesWithVTable = true);


struct VTableLayoutEntry : std::tuple<FunctionKeyType, bool, bool>
{
  using base = std::tuple<FunctionKeyType, bool, bool>;
  using base::base;

  /// returns the function representation
  std::tuple_element<0, base>::type 
  function() const { return std::get<0>(*this); }
  
  /// returns if the returned object requires a this pointer adjustment compared
  ///   to the original entry in the vtable.
  /// \todo seem comment in OverriderDesc
  std::tuple_element<1, base>::type 
  adjustReturnObj() const { return std::get<1>(*this); }  
  
  std::tuple_element<2, base>::type 
  isPureVirtual() const { return std::get<2>(*this); }  
};

/*
struct VTableSection 
{
  ClassKeyType ref;
  size_t       numInherited;
  size_t       numTotal;
  bool         isVirtual;
};
*/

struct VTableSection : std::tuple<ClassKeyType, std::uint32_t, std::uint32_t, bool>
{
  using base = std::tuple<ClassKeyType, std::uint32_t, std::uint32_t, bool>;
  using base::base;
  
  std::tuple_element<0, base>::type associatedClass() const { return std::get<0>(*this); }
  std::tuple_element<1, base>::type numInherited()    const { return std::get<1>(*this); }
  std::tuple_element<2, base>::type numTotal()        const { return std::get<2>(*this); }
  std::tuple_element<3, base>::type virtualBase()     const { return std::get<3>(*this); }
  
  void associatedClass(std::tuple_element<0, base>::type val) { std::get<0>(*this) = val; }
  void numInherited(std::tuple_element<1, base>::type val)    { std::get<1>(*this) = val; }
  void numTotal(std::tuple_element<2, base>::type val)        { std::get<2>(*this) = val; }
  void virtualBase(std::tuple_element<3, base>::type val)     { std::get<3>(*this) = val; }
};


class VTableLayout : private std::vector<VTableLayoutEntry>
{
  public:
    using base = std::vector<VTableLayoutEntry>;
    using base::base;
    
    using VTableSections = std::vector<VTableSection>;

    using base::value_type;
    using base::iterator;
    using base::const_iterator;
    using base::begin;
    using base::insert;
    using base::end;
    using base::emplace_back;
    using base::push_back;
    
          VTableSections& vtableSections()       { return sections; }
    const VTableSections& vtableSections() const { return sections; }
    
    VTableSection& createVTableSection() 
    { 
      sections.emplace_back(nullptr, 0, 0, false); 
      return sections.back();
    }
    
    bool isAbstractClass() const { return isAbstract; }
    void isAbstractClass(bool v) { isAbstract = v; }
    
  private:
    VTableSections sections;
    bool           isAbstract;
};


using VTableLayoutContainer = std::unordered_map<ClassKeyType, VTableLayout>;

/// computes a sample post-object construction virtual function table for classes in \ref all
/// \param all the result of the class hierarchy analysis
/// \param vfa result of virtual function analysis
/// \details
///    The generated layout is similar but not exactly the same as the IA64 Itanium object model.
///    virtual function tables for constructors are different, and their layout generation unsupported at this time.
VTableLayoutContainer
computeVTableLayouts(const ClassAnalysis& all, const VirtualFunctionAnalysis& vfa);


}

#endif
