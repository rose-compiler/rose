/*!
 * \author Peter Pirkelbauer
 * \date 2025-2025
 */

#include "ClassHierarchyExport.h"

//~ #include <unordered_map>

#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>

#include <sageGeneric.h>

#include "ClassHierarchyAnalysis.h"
#include "CallGraphAnalysis.h"
#include "RoseCompatibility.h"

//~ #include "Memoizer.h"

namespace json  = nlohmann;
namespace adapt = boost::adaptors;
namespace ct    = CodeThorn;

namespace
{
  inline
  std::string
  mangleFunction(ct::FunctionKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.uniqueName(key);
  }

  inline
  std::string
  mangleClassType(ct::ClassKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.uniqueName(key);
  }

  inline
  std::string
  mangleVariable(ct::VariableKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.uniqueName(key);
  }

  inline
  std::string
  nameClassType(ct::ClassKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.nameOf(key);
  }

  inline
  std::string
  nameFunction(ct::FunctionKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.nameOf(key);
  }

  inline
  std::string
  nameVariable(ct::VariableKeyType key)
  {
    ct::CompatibilityBridge compat;

    return compat.nameOf(key);
  }

  std::string
  toJsonValue(ct::ClassKeyType key) { return mangleClassType(key); }

  std::string
  toJsonValue(ct::FunctionKeyType key) { return mangleFunction(key); }

  std::string
  toJsonValue(ct::VariableKeyType key) { return mangleVariable(key); }

  // fallback
  template <class T>
  T
  toJsonValue(const T& el) { return el; }

  // Helper for tuple iteration using index_sequence (C++14)
  template <typename Tuple, std::size_t... Is>
  json::json tupleToJson2( json::json res,
                           const std::initializer_list<std::string>& flds,
                           const Tuple& data,
                           std::index_sequence<Is...>
                         )
  {
    auto namesBegin = flds.begin(); // initializer_list does not support at(idx)

    // Fold expression is not available in C++14, so use initializer list trick
    using expander = int[];
    (void)expander{0, (res[std::string(*(namesBegin + Is))] = toJsonValue(std::get<Is>(data)), 0)...};

    return res;
  }


  /// creates a json object by converting the tuple to a json object.
  /// \tparam Fields a list of types that describe the tuple element types
  /// \param  flds   a list of field names for the tuple elements
  /// \param  data   the data described as tuple
  /// \return a nlohmann json object containing the values from \p data
  /// \details
  ///    creates an nlohman json object containing the data in the tuple argument.
  ///    the field names for the data are passed in separately as fieldNames.
  /// \pre
  ///    sizeof(Fields...) == flds.size()
  template <class... Fields>
  json::json
  tupleToJson(const std::initializer_list<std::string>& flds, const std::tuple<Fields...>& data, json::json res = json::json())
  {
    constexpr std::size_t NUM_FIELDS = sizeof...(Fields);

    if (flds.size() != NUM_FIELDS)
      throw std::invalid_argument("Number of field names must match tuple size");

    return tupleToJson2(std::move(res), flds, data, std::make_index_sequence<NUM_FIELDS>{});
  }


#if TOO_MODERN
  template <class... Fields>
  json::json
  tupleToJson(const std::initializer_list<std::string_view>& fieldNames, const std::tuple<Fields...>& data, json::json res = json::json())
  {
    constexpr std::size_t tupleSize = sizeof...(Fields);

    if (fieldNames.size() != tupleSize)
        throw std::invalid_argument("Number of field names must match tuple size");

    auto namesBegin = fieldNames.begin();

    // Use index sequence to iterate over tuple and field names
    auto jsonMaker =
             [namesBegin = fieldNames.begin(), &res]<std::size_t... Is>(std::index_sequence<Is...>)
             {
               ((res[std::string(*(namesBegin + Is))] = std::get<Is>(data)), ...);
             };

    jsonMaker(std::make_index_sequence<tupleSize>{});

    return res;
  }
#endif /*TOO_MODERN*/


  struct JsonCreator
  {
    virtual void elem(json::json)                      = 0;
    virtual void field(const std::string&, json::json) = 0;

    virtual void arrayBegin(const std::string&)        = 0;
    virtual void arrayEnd(const std::string&)          = 0;

    template <class Range>
    void array(const std::string& nm, Range range)
    {
      arrayBegin(nm);

      for (json::json el : range)
        elem(std::move(el));

      arrayEnd(nm);
    }
  };

  struct JsonObjectCreator : JsonCreator
  {
      using base = JsonCreator;

      JsonObjectCreator()
      : base(), data(1, json::json())
      {}

      ~JsonObjectCreator()                                    = default;
      JsonObjectCreator(JsonObjectCreator&& other)            = default;
      JsonObjectCreator& operator=(JsonObjectCreator&& other) = default;

      void field(const std::string& nm, json::json val) override final
      {
        data.back()[nm] = std::move(val);
      }

      void arrayBegin(const std::string& nm) override final
      {
        data.push_back(json::json::array());
      }

      void elem(json::json el) override final
      {
        data.back().push_back(std::move(el));
      }

      void arrayEnd(const std::string& nm) override final
      {
        json::json tmp = std::move(data.back());

        data.pop_back();
        data.back()[nm] = std::move(tmp);
      }

      json::json result() &&
      {
        ASSERT_require(data.size() == 1);

        json::json tmp = std::move(data.back());
        return tmp;
      }

    private:
      std::vector<json::json> data;

      JsonObjectCreator(const JsonObjectCreator& other)            = delete;
      JsonObjectCreator& operator=(const JsonObjectCreator& other) = delete;
  };


  struct JsonFileCreator : JsonCreator
  {
      using base = JsonCreator;

      explicit
      JsonFileCreator(std::ostream& os)
      : base(), out(os), firstElement(1, true)
      {}

      ~JsonFileCreator()
      {
        ASSERT_require(firstElement.size() == 1);
        out << '}';
      }

      void elementSeparation()
      {
        if (!firstElement.back()) out << ','; else firstElement.back() = false;
      }

      void field(const std::string& nm, json::json val) override final
      {
        elementSeparation();
        out << "\"" << nm << "\":" << val;
      }

      void arrayBegin(const std::string& nm) override final
      {
        elementSeparation();
        out << "\"" << nm << "\":[";
        firstElement.push_back(true);
      }

      void elem(json::json el) override final
      {
        elementSeparation();
        out << el;
      }

      void arrayEnd(const std::string& nm) override final
      {
        ASSERT_require(firstElement.size() > 1);
        out << ']';
        firstElement.pop_back();
      }

    private:
      std::ostream&     out;
      std::vector<bool> firstElement;

      JsonFileCreator(const JsonFileCreator& other)            = delete;
      JsonFileCreator& operator=(const JsonFileCreator& other) = delete;
      JsonFileCreator(JsonFileCreator&& other)                 = delete;
      JsonFileCreator& operator=(JsonFileCreator&& other)      = delete;
  };

  // general field names
  const char* const FLD_CLASSES_ROOT     = "classes";
  const char* const FLD_KIND             = "kind";       // tag describing one of multiple object choices
  const char* const FLD_KEY              = "id";         // typically mangled name of type or function
  const char* const FLD_NAME             = "name";       // printable name
  const char* const FLD_VTABLE           = "vtable";
  const char* const FLD_SECTIONS         = "sections";
  const char* const FLD_OBJ_LAYOUT       = "layout";
  const char* const FLD_ASSOC_CLASS      = "class";
  const char* const FLD_COVARIANT        = "covariant";  // covariant return
  const char* const FLD_COMPGEN          = "compgen";    // requires compiler generation
  const char* const FLD_ABSTRACT         = "abstract";   // pure virtual
  const char* const FLD_NUM_INHERITED    = "inherited";
  const char* const FLD_NUM_SIZE         = "size";
  const char* const FLD_START_OFS        = "at";
  const char* const FLD_VIRTUAL_BASE     = "virtual";
  const char* const FLD_DIRECT_BASE      = "direct";
  const char* const FLD_BASE_CLASS       = "base";
  const char* const FLD_PRIMARY          = "primary";

  // vtable kinds
  const char* const VTKIND_FUNCTION      = "fun";
  const char* const VTKIND_CALL_OFFSET   = "VirtualCallOffset";
  const char* const VTKIND_BASE_OFFSET   = "VirtualBaseOffset";
  const char* const VTKIND_OFFSET_TO_TOP = "OffsetToTop";
  const char* const VTKIND_TYPEINFO      = "ti";

  // object layout kinds
  const char* const OLKIND_SUBOBJECT     = "sub";
  const char* const OLKIND_FIELD         = "fld";
  const char* const OLKIND_VTABLE        = FLD_VTABLE;


  auto toJson(const ct::VirtualFunctionEntry& vfn) -> json::json
  {
    static const std::initializer_list<std::string> names =
        { FLD_KEY, FLD_ASSOC_CLASS, FLD_COVARIANT, FLD_COMPGEN, FLD_ABSTRACT };

    return tupleToJson(names, vfn, { {FLD_KIND, VTKIND_FUNCTION} });
  }


  auto toJson(const ct::VTableSection& vtsection) -> json::json
  {
    static const std::initializer_list<std::string> names =
        { FLD_ASSOC_CLASS, FLD_NUM_INHERITED, FLD_NUM_SIZE, FLD_START_OFS, FLD_VIRTUAL_BASE };

    return tupleToJson(names, vtsection);
  }


  auto toJson(const ct::Field& fld) -> json::json
  {
    // explicit to add in clear name
    return { {FLD_KIND, OLKIND_FIELD}
           , {FLD_KEY,  toJsonValue(fld.id())}
           , {FLD_NAME, nameVariable(fld.id())}
           };
  }

  auto toJson(const ct::VTable& vtable) -> json::json
  {
    static const std::initializer_list<std::string> names =
        { FLD_ASSOC_CLASS, FLD_PRIMARY };

    return tupleToJson(names, vtable, {{FLD_KIND, OLKIND_VTABLE}});
  }

  auto toJson(const ct::Subobject& sub) -> json::json
  {
    static const std::initializer_list<std::string> names =
        { FLD_BASE_CLASS, FLD_VIRTUAL_BASE, FLD_DIRECT_BASE };

    return tupleToJson(names, sub, {{ FLD_KIND, OLKIND_SUBOBJECT }});
  }

  auto toJson(const ct::VirtualCallOffset&) -> json::json
  {
    return {{ FLD_KIND, VTKIND_CALL_OFFSET }};
  }

  auto toJson(const ct::VirtualBaseOffset&) -> json::json
  {
    return {{ FLD_KIND, VTKIND_BASE_OFFSET }};
  }

  auto toJson(const ct::OffsetToTop&) -> json::json
  {
    return {{ FLD_KIND, VTKIND_OFFSET_TO_TOP }};
  }

  auto toJson(const ct::TypeInfoPointer&) -> json::json
  {
    return {{ FLD_KIND, VTKIND_TYPEINFO }};
  }


  struct TaggedVariantConverterImpl : boost::static_visitor<json::json>
  {
    template <class T>
    json::json operator()(const T& el) const
    {
      return toJson(el);
    }
  };

  struct TaggedVariantConverter
  {
    template <class T>
    json::json operator()(const T& el) const
    {
      return boost::apply_visitor(TaggedVariantConverterImpl{}, el);
    }
  };


  json::json
  vtableEntries(const ct::VTableLayoutContainer::value_type& vtable)
  {
    auto tfrm = adapt::transform(vtable.second, TaggedVariantConverter{});

    return std::vector<json::json>(tfrm.begin(), tfrm.end());
  }

  struct VTableSectionConverter
  {
    json::json operator()(const ct::VTableSection& sec) const
    {
      return toJson(sec);
    }
  };

  json::json
  vtableSections(const ct::VTableLayoutContainer::value_type& vtable)
  {
    auto tfrm = adapt::transform(vtable.second.vtableSections(), VTableSectionConverter{});

    return std::vector<json::json>(tfrm.begin(), tfrm.end());
  }


  struct ObjectLayoutEntryConverter
  {
    json::json operator()(const ct::ObjectLayoutEntry& el) const
    {
      return TaggedVariantConverter{}(el.element());
    }
  };

  json::json
  objectLayout(const ct::ObjectLayoutContainer::value_type& layout)
  {
    auto tfrm = adapt::transform(layout.second, ObjectLayoutEntryConverter{});
    std::vector<json::json> res;

    for (json::json el : tfrm)
      res.push_back(std::move(el));

    return res;
  }

  void
  convertToJson( JsonCreator& output,
                 const ct::ObjectLayoutContainer& classLayout,
                 const ct::VTableLayoutContainer& vtableLayout,
                 ct::ClassPredicate pred
               )
  {
    output.arrayBegin(FLD_CLASSES_ROOT);

    //~ for (const ct::VTableLayoutContainer::value_type& el : vtl)
    for (const ct::ObjectLayoutContainer::value_type& layout : classLayout)
    {
      ct::ClassKeyType key = layout.first;

      if (!pred(key)) continue;

      json::json obj = { {FLD_KEY,        mangleClassType(key)}
                       , {FLD_NAME,       nameClassType(key)}
                       , {FLD_OBJ_LAYOUT, objectLayout(layout)}
                       , {FLD_ABSTRACT,   layout.second.abstractClass()}
                       };

      auto vtpos = vtableLayout.find(key);
      if (vtpos != vtableLayout.end())
      {
        obj[FLD_VTABLE]   = vtableEntries(*vtpos);
        obj[FLD_SECTIONS] = vtableSections(*vtpos);
      }

      output.elem(obj);
    }

    output.arrayEnd(FLD_CLASSES_ROOT);
  }
}



namespace CodeThorn
{
  ClassPredicate
  anyClass()
  {
    return [](ClassKeyType) -> bool { return true; };
  }

  ClassPredicate
  anyClassWithVtable(const VTableLayoutContainer& vtbl)
  {
    return [&vtbl](ClassKeyType key) -> bool
           {
             return vtbl.find(key) != vtbl.end();
           };
  }

  json::json
  toJson( const ObjectLayoutContainer& classLayout,
          const VTableLayoutContainer& vtableLayout,
          ClassPredicate               pred
        )
  {
    JsonObjectCreator creator;

    convertToJson(creator, classLayout, vtableLayout, pred);
    return std::move(creator).result();
  }

  void
  writeJson( std::ostream&                os,
             const ObjectLayoutContainer& classLayout,
             const VTableLayoutContainer& vtableLayout,
             ClassPredicate               pred
           )
  {
    JsonFileCreator writer(os);

    os << '{';
    convertToJson(writer, classLayout, vtableLayout, pred);
    os << '}';
  }

  json::json classHierarchySchema()
  {
    return  R"(
            { "type": "object",
              "required": ["classes"],
              "properties": {
                "classes": {
                  "type": "array",
                  "items": {
                    "type": "object",
                    "required": ["layout", "id", "abstract"],
                    "properties": {
                      "abstract": {
                        "type": "boolean"
                      },
                      "id": {
                        "type": "string"
                      },
                      "layout": {
                        "type": "array",
                        "items": {
                          "type": "object",
                          "required": ["kind"],
                          "properties": {
                            "kind": {
                              "type": "string",
                              "desc": "object kind, one of {sub,fld,vtable}"
                            },
                            "base": {
                              "type": "string",
                              "desc": "base type. set if kind==sub"
                            },
                            "class": {
                              "type": "string",
                              "desc": "type associated with vtable. set if kind==vtable"
                            },
                            "name": {
                              "type": "string",
                              "desc": "source name. set if kind==fld"
                            },
                            "direct": {
                              "type": "boolean",
                              "desc": "true iff this is a direct base. set if kind==sub"
                            },
                            "primary": {
                              "type": "boolean",
                              "desc": "true iff this is the primary vtable. set if kind==vtable"
                            },
                            "virtual": {
                              "type": "boolean",
                              "desc": "true iff this is a virtual base class. set if kind==sub"
                            }
                          }
                        }
                      },
                      "name": {
                        "type": "string"
                      },
                      "sections": {
                        "desc": "meta data describing elements in vtable array"
                        "type": "array",
                        "items": {
                          "type": "object",
                          "required": ["at", "class", "inherited", "size", "virtual"],
                          "properties": {
                            "at": {
                              "type": "number",
                              "desc": "start index of section in vtable"
                            },
                            "class": {
                              "type": "string",
                              "desc": "type associated with section"
                            },
                            "inherited": {
                              "type": "number",
                              "desc": "number of inherited virtual functions"
                            },
                            "size": {
                              "type": "number",
                              "desc": "total size of section in vtable"
                            },
                            "virtual": {
                              "type": "boolean",
                              "desc": "true iff this is a virtually inherited section"
                            }
                          }
                        }
                      },
                      "vtable": {
                        "type": "array",
                        "items": {
                          "type": "object",
                          "required": ["kind"],
                          "properties": {
                            "abstract": {
                              "type": "boolean",
                              "desc": "true iff function is pure virtual. set if kind==fun"
                            },
                            "class": {
                              "type": "string",
                              "desc": "class id where function was defined. set if kind==fun"
                            },
                            "compgen": {
                              "type": "string",
                              "desc": "true iff function needs to be compiler generated. set if kind==fun"
                            },
                            "covariant": {
                              "type": "boolean",
                              "desc": "true iff function is covariant to function in base. set if kind==fun"
                            },
                            "id": {
                              "type": "string",
                              "desc": "function ID. set if kind==fun"
                            },
                            "kind": {
                              "type": "string",
                              "desc": "object kind, one of {fun}"
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
          )"_json;
  }

} // end of namespace CodeThorn
