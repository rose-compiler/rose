
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

  void dot_inheritance(std::ostream& os, const ct::ClassAnalysis& all, ct::FilterFn include)
  {
    static constexpr const char* normal_color  = "color=green";
    static constexpr const char* virtual_color = "color=red";

    for (const ct::ClassAnalysis::value_type& elem : all)
    {
      if (!include(elem.first)) continue;

      for (const ct::InheritanceDesc& child : elem.second.children())
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


  void dot_casts(std::ostream& os, const ct::ClassAnalysis& classes, const ct::CastAnalysis& casts, ct::FilterFn include)
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
      const bool             isDownCast  = classes.isBaseDerived(src, tgt);
      const bool             isUpCast    = !isDownCast && classes.isBaseDerived(tgt, src);
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
        std::cerr << '\n'
                  << ct::typeNameOfClassKeyType(src) << " -> " << ct::typeNameOfClassKeyType(tgt)
                  << (isUpCast    ? "  up"    : "")
                  << (isDownCast  ? "  down"  : "")
                  << (isCrossCast ? "  cross" : "")
                  << std::endl;

        for (ct::CastKeyType cast : elem.second)
        {
          std::cerr << "  " << ct::unparseToString(cast)
                    << " [" << ct::typeOf(cast) << "] "
                    << ct::FileInfo{cast};

          std::cerr << std::endl;
        }
      }
    }
  }
}

namespace CodeThorn
{

void write(std::ostream& os, NameFn nameOf, FilterFn include, const ClassAnalysis& classes, const CastAnalysis& casts)
{
  static constexpr const char* class_color = "color=blue";

  dot_header(os);

  auto dotClasses = [&os, nameOf, include](const ClassAnalysis::value_type& elem) -> void
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

}
