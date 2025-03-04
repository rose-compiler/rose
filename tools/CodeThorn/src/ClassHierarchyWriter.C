#include "ClassHierarchyWriter.h"

//~ #include <sage3basic.h>
//~ #include <sageGeneric.h>
#include <cassert>

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

  void dot_header(std::ostream& os, const std::string& kind, const std::string& s)
  {
    os << kind << ' ' << s << " {" << std::endl;
  }

  void dot_footer(std::ostream& os)
  {
    os << "}" << std::endl;
  }

  void dot_inheritance(std::ostream& os, const ct::ClassAnalysis& all, ct::ClassFilterFn include)
  {
    static constexpr const char* normal_color  = "color=green";
    static constexpr const char* virtual_color = "color=red";

    // set common edge properties
    //   *1) to get the graph layout top-down, the edges are reversed
    os << "edge[dir=back arrowtail=empty]" << std::endl;

    for (const ct::ClassAnalysis::value_type& elem : all)
    {
      if (!include(elem.first)) continue;

      for (const ct::InheritanceDesc& child : elem.second.descendants())
      {
        const bool virt   = child.isVirtual();
        const bool direct = child.isDirect();
        //~ const bool includeInheritance = virt || direct;

        //~ if (includeInheritance && include(child.getClass()))
        if (direct && include(child.getClass()))
        {
          std::string linestyle = virt ? virtual_color : normal_color;

          if (!direct) linestyle += " style=dotted";

          // reversed edges (see *1 above)
          edge(os, elem.first, child.getClass(), "", linestyle);
        }
      }
    }
  }


  void dot_casts(std::ostream& os, const ct::ClassAnalysis& classes, const ct::CastAnalysis& casts, ct::ClassFilterFn include)
  {
    using ClassTypePair     = std::pair<ct::ClassKeyType, ct::TypeKeyType>;
    using ClassCastAnalysis = std::unordered_map<ct::ClassCastDesc, std::vector<ct::CastKeyType> >;

    static constexpr const char* downcast_color  = "color=purple";
    static constexpr const char* crosscast_color = "color=gold";

    ClassCastAnalysis classcasts;

    // common edge properties
    //   dir=back, edges are reversed
    //   constraint=false, these edges do not influence the layout
    os << "edge[dir=back arrowtail=vee style=dashed constraint=false]" << std::endl;

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
        msgWarn() << "cast from non class type to a class type."
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
      const bool             isDownCast  = classes.isBaseOf(src, tgt);
      const bool             isUpCast    = !isDownCast && classes.isBaseOf(tgt, src);
      const bool             isCrossCast = !isDownCast && !isUpCast;

      // exclude up casts
      if (!isDownCast && !isCrossCast)
      {
        assert(isUpCast);
        continue;
      }

      std::string style;
      std::string num;

      style += isDownCast ? downcast_color : crosscast_color;

      //~ if (isDownCast > 1) num = boost::lexical_cast<std::string>(isDownCast);

      // reversed edges
      edge(os, tgt, src, num, style);
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
      const ct::FunctionKeyType      funid = fn.function();
      const ct::VirtualFunctionDesc& other = vfuns.at(funid);

      os << className(other.classId())
         << "::" << funcName(funid)
         << " #" << funid
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
    return true;
    // return vfn.overriders().size() || vfn.overridden().size();
  }

  void writeFunctionsInClass( std::ostream& os,
                              ct::ClassNameFn& className,
                              ct::FuncNameFn& funcName,
                              const ct::VirtualFunctionAnalysis& vfuns,
                              const ct::ClassAnalysis::value_type& clazz,
                              bool withOverridden = false
                            )
  {
    os << "Class " << className(clazz.first) << " " << clazz.second.virtualFunctions().size() << "\n"
       << std::endl;

    for (ct::FunctionKeyType vfnId : clazz.second.virtualFunctions())
    {
      const ct::VirtualFunctionDesc& vfn = vfuns.at(vfnId);

      if (!printsData(vfn, withOverridden))
        continue;

      os << "  Function " << funcName(vfnId)
         << " #" << vfnId
         << (vfn.isPureVirtual() ? " (pure virtual)" : "")
         << std::endl;

      printOverriders(os, className, funcName, vfuns, vfn);

      if (withOverridden)
        printOverridden(os, className, funcName, vfuns, vfn);
    }

    os << std::endl;
  }

  void writeVirtualBaseOrder( std::ostream& os,
                              ct::ClassNameFn& className,
                              const ct::ClassAnalysis::value_type& clazz
                            )
  {
    const ct::ClassData::VirtualBaseOrderContainer& vbases = clazz.second.virtualBaseClassOrder();

    if (vbases.empty())
      return;

    os << "Class " << className(clazz.first) << "\n    "
       << std::flush;

    for (ct::ClassKeyType basecls : vbases)
      os << className(basecls) << ", ";

    os << '\n' << std::endl;
  }


  struct ObjectLayoutElementPrinter : boost::static_visitor<void>
  {
      ObjectLayoutElementPrinter(std::ostream& out, ct::ClassNameFn& classNamer, ct::VarNameFn& varNamer)
      : os(out), className(classNamer), varName(varNamer)
      {}

      virtual std::string escapeName(std::string s) const
      {
        return s;
      }

      virtual void operator()(const ct::Subobject& subobj)
      {
        out() << "subobj " << escapeName(className(subobj.ref))
              << (subobj.isVirtual ? " (virtual)" : "");
      }

      virtual void operator()(const ct::Field& fld)
      {
        out() << "field  " << escapeName(varName(fld.id));
      }

      virtual void operator()(const ct::VTable& vtbl)
      {
        out() << "vtable " << escapeName(className(vtbl.ref))
              << (vtbl.isPrimary ? " (primary)" : "");
      }

      virtual void printEntry(const ct::ObjectLayoutEntry& el)
      {
        boost::apply_visitor(*this, el.element());
      }

      virtual void printClassHeader(const ct::ObjectLayoutAnalysis::value_type& el)
      {
        out() << "class " << escapeName(className(el.first));
      }

      virtual void printClassFooter() {}

      std::ostream& out() { return os; }

    private:
      std::ostream&    os;
      ct::ClassNameFn& className;
      ct::VarNameFn&   varName;
  };

  struct InheritanceEdgeDot : std::tuple<const void*, const void*, bool, bool>
  {
    using base = std::tuple<const void*, const void*, bool, bool>;
    using base::base;

    const void* subobj() const { return std::get<0>(*this); }
    const void* clazz()  const { return std::get<1>(*this); }
    bool isVirtual()     const { return std::get<2>(*this); }
    bool isDirect()      const { return std::get<3>(*this); }
  };


  struct ObjectLayoutElementPrinterDot : ObjectLayoutElementPrinter
  {
      using base = ObjectLayoutElementPrinter;

      ObjectLayoutElementPrinterDot( std::ostream& out,
                                     ct::ClassNameFn& classNamer,
                                     ct::VarNameFn& varNamer,
                                     ct::ClassFilterFn incl
                                   )
      : base(out, classNamer, varNamer), include(incl), edges(), currentClass()
      {}

      void elem_begin(const void* elem) ;
      void elem_end() ;
      void class_begin(const void* elem, bool abstractClass) ;
      void class_end() ;

      template <class ClassElem>
      void elem(const ClassElem& el)
      {
        elem_begin(&el);
        base::operator()(el);
        elem_end();
      }

      void operator()(const ct::Field& el) override     { elem(el); }
      void operator()(const ct::VTable& el) override    { elem(el); }

      void operator()(const ct::Subobject& el) override
      {
        elem(el);

        if (include(el.ref))
          edges.emplace_back(&el, el.ref, el.isVirtual, el.isDirect);
      }

      void printClassHeader(const ct::ObjectLayoutAnalysis::value_type& el) override
      {
        currentClass = el.first;

        class_begin(currentClass, el.second.abstractClass());

        out() << "=";
        base::printClassHeader(el);
        out() << "=";
      }

      void printClassFooter() override
      {
        class_end();

        for (const InheritanceEdgeDot& rel : edges)
        {
          out() << "p" << std::hex << currentClass << ":p" << std::hex << rel.subobj()
                << " -> p" << std::hex << rel.clazz() << ":0"
                << "[color="
                << (rel.isVirtual() ? "red" : "green")
                << (rel.isDirect()  ? "" : " style=dotted")
                << "]"
                << ';' << std::endl;
        }

        edges.clear();
        currentClass = ct::ClassKeyType{};
      }

      std::string escapeName(std::string s) const override
      {
        std::string res;

        res.reserve(s.size());
        for (char c : s)
        {
          if ((c == '<') || (c == '>'))
            res += '\\';

          res += c;
        }

        return res;
      }

    private:
      ct::ClassFilterFn               include;
      std::vector<InheritanceEdgeDot> edges;
      ct::ClassKeyType                currentClass;
  };

  void ObjectLayoutElementPrinterDot::elem_begin(const void* elem)
  {
    out() << "|<p" << std::hex << (elem) << ">";
  }

  void ObjectLayoutElementPrinterDot::elem_end() {}

  void ObjectLayoutElementPrinterDot::class_begin(const void* elem, bool abstractClass)
  {
    out() << "p" << std::hex << (elem) << "["
          << (abstractClass ? "fontcolor=red " : "")
          << "label=\"<0>";
  }

  void ObjectLayoutElementPrinterDot::class_end()
  {
    out() << "\"];" << std::endl;
  }

  struct ObjectLayoutElementPrinterTxt : ObjectLayoutElementPrinter
  {
      using base = ObjectLayoutElementPrinter;
      using base::base;

      void printEntry(const ct::ObjectLayoutEntry& el) override
      {
        out() << el.offset() << " ";

        base::printEntry(el);
        // out() << std::endl;
      }

      void printClassHeader(const ct::ObjectLayoutAnalysis::value_type& el) override
      {
        base::printClassHeader(el);
        if (el.second.abstractClass()) out() << " [abstract]";
        out() << std::endl;
      }
  };


  struct VTableLayoutElementPrinter : boost::static_visitor<void>
  {
      VTableLayoutElementPrinter(std::ostream& out, ct::ClassNameFn& classNamer, ct::FuncNameFn& funcNamer)
      : os(out), className(classNamer), funcName(funcNamer), classKey()
      {}

      virtual std::string escapeName(std::string s) const
      {
        return s;
      }

      virtual void operator()(const ct::VirtualCallOffset& /* callOfs */)
      {
        out() << "callOfs (todo)";
      }

      virtual void operator()(const ct::VirtualBaseOffset& /* baseOfs */)
      {
        out() << "baseOfs (todo)";
      }

      virtual void operator()(const ct::OffsetToTop& /* ofsToTop */)
      {
        out() << "ofsToTop (todo)";
      }

      virtual void operator()(const ct::TypeInfoPointer& tyinf)
      {
        out() << "typeinfo (todo)";
      }

      virtual void operator()(const ct::VirtualFunctionEntry& vfn)
      {
        out() << escapeName(funcName(vfn.function()))
              << " " << vfn.function()
              << (vfn.getClass() != classKey    ? " inh"   : "")
              << (vfn.adjustReturnObj()         ? " covar" : "")
              << (vfn.needsCompilerGeneration() ? " compgen" : "")
              << (vfn.isPureVirtual()           ? " pure"  : "")
              ;
      }

      virtual void printClassHeader(const ct::VTableLayoutAnalysis::value_type& el)
      {
        //~ const ct::VTableLayout::VTableSections& sects = el.second.vtableSections();

        classKey = el.first;

        out() << "class " << escapeName(className(classKey))
              << (el.second.isAbstractClass() ? " abstract " : "")
              //~ << " (" << sects.size() << " sub-vtables)"
              //~ << sects.front().numTotal() << " / " << sects.front().numInherited()
              ;
      }

      virtual void printClassFooter() { classKey = ct::ClassKeyType{}; }

      std::ostream& out() { return os; }

      virtual void printEntry(const ct::VTableLayoutElement& el)
      {
        //~ boost::apply_visitor(*this, el.element());
        boost::apply_visitor(*this, el);
      }

    private:
      std::ostream&    os;
      ct::ClassNameFn& className;
      ct::FuncNameFn&  funcName;
      ct::ClassKeyType classKey;
  };

  struct VTableLayoutElementPrinterTxt : VTableLayoutElementPrinter
  {
      using base = VTableLayoutElementPrinter;
      using base::base;

      void printEntry(const ct::VTableLayoutElement& el) override
      {
        base::printEntry(el);
        out() << std::endl;
      }

      void printClassFooter() override { out() << std::endl; }

      void printClassHeader(const ct::VTableLayoutAnalysis::value_type& el) override
      {
        base::printClassHeader(el);
        out() << std::endl;
      }
  };


  template<class Printer, class LayoutContainer>
  void prnLayout(Printer&& prn, ct::ClassFilterFn include, const LayoutContainer& cont)
  {
    for (const typename LayoutContainer::value_type& entry : cont)
    {
      if (!include(entry.first)) continue;

      prn.printClassHeader(entry);

      for (const auto& elem : entry.second)
        prn.printEntry(elem);

      prn.printClassFooter();
    }
  }


  void prnClassLayout( ObjectLayoutElementPrinter&& printer,
                       ct::ClassFilterFn include,
                       const ct::ObjectLayoutAnalysis& cont
                     )
  {
    prnLayout(printer, include, cont);
  }

  void prnVTableLayout( VTableLayoutElementPrinter&& printer,
                        ct::ClassFilterFn include,
                        const ct::VTableLayoutAnalysis& cont
                      )
  {
    prnLayout(printer, include, cont);
  }
}

namespace CodeThorn
{

void classHierarchyDot( std::ostream& os,
                        ClassNameFn& nameOf,
                        ClassFilterFn include,
                        const ClassAnalysis& classes,
                        const CastAnalysis& casts
                      )
{
  static constexpr const char* class_color = "color=blue";

  dot_header(os, "digraph", "\"Thorn 2 - Class Relationships and Casts\"");

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

void virtualFunctionsTxt( std::ostream& os,
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


void virtualBaseClassInitOrderTxt( std::ostream& os,
                                   ClassNameFn& className,
                                   ClassFilterFn include,
                                   const ClassAnalysis& classes
                                 )
{
  topDownTraversal( classes,
                    [&os, &className, &include]
                    (const ClassAnalysis::value_type& elem) -> void
                    {
                      if (!include(elem.first)) return;

                      writeVirtualBaseOrder( os,
                                             className,
                                             elem
                                           );
                    }
                  );
}



void classLayoutTxt( std::ostream& os,
                     ClassNameFn& className,
                     VarNameFn& varName,
                     ClassFilterFn include,
                     const ObjectLayoutAnalysis& classLayout
                   )
{
  prnClassLayout(ObjectLayoutElementPrinterTxt{os, className, varName}, include, classLayout);
}


void classLayoutDot( std::ostream& os,
                     ClassNameFn& className,
                     VarNameFn& varName,
                     ClassFilterFn include,
                     const ObjectLayoutAnalysis& classLayout
                   )
{
  dot_header(os, "digraph", "\"thorn2 - Class Layout\"");
  os << "rankdir = LR;\n"
     << "node [shape=record];\n";

  prnClassLayout(ObjectLayoutElementPrinterDot{os, className, varName, include}, include, classLayout);
  dot_footer(os);
}

void vtableLayoutTxt( std::ostream& os,
                      ClassNameFn& className,
                      FuncNameFn& funcName,
                      ClassFilterFn include,
                      const VTableLayoutAnalysis& vtableLayout
                    )
{
  prnVTableLayout(VTableLayoutElementPrinterTxt{os, className, funcName}, include, vtableLayout);
}


}
