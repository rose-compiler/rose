
//~ #include <sage3basic.h>
//~ #include <sageGeneric.h>
#include <cassert>

#include "RoseCompatibility.h"
#include "ClassHierarchyWriter.h"

namespace ct = CodeThorn;

namespace CodeThorn
{
  struct ClassCastDesc : std::tuple<ClassKeyType, ClassKeyType>
  {
    using base = std::tuple<ClassKeyType, ClassKeyType>;
    using base::base;
  };
}

namespace std
{
  template<> struct hash<ct::ClassCastDesc>
  {
    std::size_t operator()(const ct::ClassCastDesc& dsc) const noexcept
    {
      std::size_t h1 = std::hash<const void*>{}(std::get<0>(dsc));
      std::size_t h2 = std::hash<const void*>{}(std::get<1>(dsc));

      return h1 ^ (h2 >> 4);
    }
  };
}



namespace
{
  void edge(std::ostream& os, const void* src, const void* tgt, std::string lbl, std::string attr)
  {
    // if (tgt == nullptr) lbl = "null";

    os << "  p" << size_t(src) << " -> p" << size_t(tgt)
       << "[ taillabel = \"" << lbl << "\" " << attr << "];" << std::endl;
  }

  void node(std::ostream& os, const void* n, std::string lbl, std::string attr)
  {
    os << "  p" << size_t(n) << "[ label = \"" << lbl << "\" " << attr << "];" << std::endl;
  }

  void dot_header(std::ostream& os, std::string s = "Acuity")
  {
    os << "digraph " << s << " {" << std::endl;
  }

  void dot_footer(std::ostream& os)
  {
    os << "}" << std::endl;
  }

  void dot_inheritance(std::ostream& os, const ct::ClassAnalysis& all, ct::ClassFilterFn include)
  {
    static constexpr const char* normal_color  = "color=green";
    static constexpr const char* virtual_color = "color=red";

    for (const ct::ClassAnalysis::value_type& elem : all)
    {
      if (!include(elem.first)) continue;

      for (const ct::InheritanceDesc& child : elem.second.descendants())
      {
        const bool virt   = child.isVirtual();
        const bool direct = child.isDirect();
        const bool includeInheritance = virt || direct;

        if (includeInheritance && include(child.getClass()))
        {
          std::string linestyle = virt ? virtual_color : normal_color;

          if (!direct) linestyle += " style=dotted";

          edge(os, child.getClass(), elem.first, "", linestyle);
        }
      }
    }
  }


  void dot_casts(std::ostream& os, const ct::ClassAnalysis& classes, const ct::CastAnalysis& casts, ct::ClassFilterFn include)
  {
    using ClassTypePair     = std::pair<ct::ClassKeyType, ct::TypeKeyType>;
    using ClassCastAnalysis = std::unordered_map<ct::ClassCastDesc, std::vector<ct::CastKeyType> >;

    static constexpr const char* downcast_color  = "color=indigo";
    static constexpr const char* crosscast_color = "color=gold";
    static constexpr const char* cast_style      = "arrowhead=vee, style=dashed";

    ClassCastAnalysis classcasts;

    for (const ct::CastAnalysis::value_type& elem : casts)
    {
      ct::TypeKeyType sgsrc = std::get<0>(elem.first);
      ct::TypeKeyType sgtgt = std::get<1>(elem.first);
      ClassTypePair   srcty = ct::getClassCastInfo(sgsrc);
      ClassTypePair   tgtty = ct::getClassCastInfo(sgtgt);

      if (srcty.first == tgtty.first)
        continue;

      if (!tgtty.first || !include(tgtty.first))
        continue;

      if (!srcty.first)
      {
        logWarn() << "cast from non class type to a class type."
                  << std::endl;
        continue;
      }

      if (!include(srcty.first))
        continue;

      std::vector<ct::CastKeyType>& allcasts = classcasts[ct::ClassCastDesc{srcty.first, tgtty.first}];

      allcasts.insert(allcasts.end(), elem.second.begin(), elem.second.end());
    }

    for (const ClassCastAnalysis::value_type& elem : classcasts)
    {
      const ct::ClassKeyType src         = std::get<0>(elem.first);
      const ct::ClassKeyType tgt         = std::get<1>(elem.first);
      const bool             isDownCast  = classes.areBaseDerived(src, tgt);
      const bool             isUpCast    = !isDownCast && classes.areBaseDerived(tgt, src);
      const bool             isCrossCast = !isDownCast && !isUpCast;

      // exclude up casts
      if (!isDownCast && !isCrossCast)
      {
        assert(isUpCast);
        continue;
      }

      std::string style = cast_style;
      std::string num;

      style += " ";
      style += isDownCast ? downcast_color : crosscast_color;

      //~ if (isDownCast > 1) num = boost::lexical_cast<std::string>(isDownCast);

      edge(os, src, tgt, num, style);

      if (0)
      {
        logTrace() << '\n'
                   << ct::typeNameOf(src) << " -> " << ct::typeNameOf(tgt)
                   << (isUpCast    ? "  up"    : "")
                   << (isDownCast  ? "  down"  : "")
                   << (isCrossCast ? "  cross" : "")
                   << std::endl;

        for (ct::CastKeyType cast : elem.second)
        {
          logTrace() << ct::CastWriterDbg{cast}
                     << std::endl;
        }
      }
    }
  }


  void printFunctions( std::ostream& os,
                       const std::string& desc,
                       ct::ClassNameFn& className,
                       ct::FuncNameFn& funcName,
                       const ct::VirtualFunctionAnalysis& vfuns,
                       const ct::OverrideContainer& functions,
                       bool skipEmpty = true
                     )
  {
    if (skipEmpty && functions.size() == 0)
      return;

    os << "    " << desc << ": ";

    for (const ct::OverrideDesc& fn: functions)
    {
      const ct::FunctionId           funid = fn.functionId();
      const ct::VirtualFunctionDesc& other = vfuns.at(funid);

      os << className(other.classId())
         << "::" << funcName(funid)
         << " #" << funid.getIdCode()
         << (fn.covariantReturn() ? " (covariant)" : "")
         << ", ";
    }

    os << std::endl;
  }

  void printOverriders( std::ostream& os,
                        ct::ClassNameFn& className,
                        ct::FuncNameFn& funcName,
                        const ct::VirtualFunctionAnalysis& vfuns,
                        const ct::VirtualFunctionDesc& vfn
                      )
  {
    printFunctions(os, "Overriden by", className, funcName, vfuns, vfn.overriders(), false /* do not skip empty */);
  }

  void printOverridden( std::ostream& os,
                        ct::ClassNameFn& className,
                        ct::FuncNameFn& funcName,
                        const ct::VirtualFunctionAnalysis& vfuns,
                        const ct::VirtualFunctionDesc& vfn
                      )
  {
    printFunctions(os, "Overrides", className, funcName, vfuns, vfn.overridden());
  }

  bool printsData(const ct::VirtualFunctionDesc& vfn, bool withOverridden)
  {
    return vfn.overriders().size() || vfn.overridden().size();
  }

  void writeFunctionsInClass( std::ostream& os,
                              ct::ClassNameFn& className,
                              ct::FuncNameFn& funcName,
                              const ct::VirtualFunctionAnalysis& vfuns,
                              const ct::ClassAnalysis::value_type& clazz,
                              bool withOverridden = false
                            )
  {
    os << "Class " << className(clazz.first) << "\n"
       << std::endl;

    for (ct::FunctionId vfnId : clazz.second.virtualFunctions())
    {
      const ct::VirtualFunctionDesc& vfn = vfuns.at(vfnId);

      if (!printsData(vfn, withOverridden))
        continue;

      os << "  Function " << funcName(vfnId)
         << " #" << vfnId.getIdCode()
         << (vfn.isPureVirtual() ? " (pure virtual)" : "")
         << std::endl;

      printOverriders(os, className, funcName, vfuns, vfn);

      if (withOverridden)
        printOverridden(os, className, funcName, vfuns, vfn);
    }

    os << std::endl;
  }
}

namespace CodeThorn
{

void writeClassDotFile( std::ostream& os,
                        ClassNameFn& nameOf,
                        ClassFilterFn include,
                        const ClassAnalysis& classes,
                        const CastAnalysis& casts
                      )
{
  static constexpr const char* class_color = "color=blue";

  dot_header(os);

  auto dotClasses = [&os, &nameOf, include](const ClassAnalysis::value_type& elem) -> void
                    {
                      if (include(elem.first))
                        node(os, elem.first, nameOf(elem.first), class_color);
                    };

  //~ topDownTraversal(classes, dotClasses);
  bottomUpTraversal(classes, dotClasses);

  //~ dot_classes(os, all, nameOf, include);
  dot_inheritance(os, classes, include);
  dot_casts(os, classes, casts, include);
  dot_footer(os);
}

void writeVirtualFunctions( std::ostream& os,
                            ClassNameFn& className,
                            FuncNameFn& funcName,
                            ClassFilterFn include,
                            const ClassAnalysis& classes,
                            const VirtualFunctionAnalysis& vfuns,
                            bool overridden
                          )
{
  topDownTraversal( classes,
                    [&os, &className, &funcName, &vfuns, &include, overridden]
                    (const ClassAnalysis::value_type& elem) -> void
                    {
                      if (!include(elem.first)) return;

                      writeFunctionsInClass( os,
                                             className,
                                             funcName,
                                             vfuns,
                                             elem,
                                             overridden
                                           );
                    }
                  );
}


}
