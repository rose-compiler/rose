

#ifndef _F2CXX_ANALYZER_HPP

#define _F2CXX_ANALYZER_HPP 1

#include <set>

#include "rose.h"

namespace f2cxx
{
  enum ParamAttr
  {
    none        = (0),
    param       = (1 << 0),
    data        = (1 << 1),
    space_lower = (1 << 2),
    space_upper = (1 << 3),
    loop_lower  = (1 << 4),
    loop_upper  = (1 << 5),
    normal_expr = (1 << 6),

    auxiliary   = (space_lower | space_upper | loop_lower | loop_upper)
  };

  struct AnalyzerAttribute : AstAttribute
  {
      // implicit
      AnalyzerAttribute(ParamAttr attr = none)
      : AstAttribute(), val(attr), data_upper(), data_lower(),
        iter_upper(), iter_lower(), transl(NULL)
      {}

      /// returns a set with associated declarations
      /// \todo basic correctness is a 1:1 mapping, thus only one entry
      ///       is expected at this time.
      std::set<SgInitializedName*>& associates(ParamAttr relation);

      /// records the role of an variable/parameter
      AnalyzerAttribute& role(ParamAttr attr);

      /// tests whether the role attr is set
      bool hasRole(ParamAttr attr);

      /// records a relationship with other
      AnalyzerAttribute& associate(ParamAttr attr, SgInitializedName* other);

      /// returns the role
      ParamAttr flags() const { return val; }

      void setTranslated(SgLocatedNode& cppexpr) { transl = &cppexpr; }

      SgLocatedNode* translated() { return transl; }

    private:
      /// stores the discovered role in the fortran code
      ParamAttr                       val;

      /// stores all space upper bounds associated with this datablock
      std::set<SgInitializedName*> data_upper;

      /// stores all space lower bounds associated with this datablock
      std::set<SgInitializedName*> data_lower;

      /// stores all loop upper bounds associated with this datablock
      std::set<SgInitializedName*> iter_upper;

      /// stores all loop lower bounds associated with this datablock
      std::set<SgInitializedName*> iter_lower;

      /// stores all datablocks assoicated with this bound value
      std::set<SgInitializedName*> datablocks;

      /// the translated node in C++
      SgLocatedNode*               transl;
  };

  /// retrieves attribute on n
  /// \details
  ///   if the attribute is not yet set, it will be created.
  AnalyzerAttribute& getAttribute(SgNode& n);

  struct Analyzer
  {
    /// \brief
    ///   Analyzes the procedure parameters and its uses in order to
    ///   determine whether they need to be translated to special Amrex
    ///   types.
    /// \details
    ///    Supported type migration:
    ///    * any three dimensional array in Fortran will be translated
    ///      to an amrex::Box
    ///    * a combination of four 3-element arrays will be translated
    ///      into amrex::FArrayBox. The four arrays describe:
    ///      - lower bounds on a 3D array
    ///      - upper bounds on a 3D array
    ///      - lower bounds on an iteration space over a 3D array
    ///      - upper bounds on an iteration space over a 3D array
    void operator()(SgProcedureHeaderStatement* n);

    /// \brief
    ///   translates fortran parameter type into C++ Amrex Type
    // SgType& translate(SgType&);

    /// \brief
    ///   translates fortran expressions into C++ expressions
    ///   under consideration of the type analysis
    // SgExpression& translate(SgExpression&);
  };

  void print_param_tags(SgProcedureHeaderStatement* proc);
}

#endif /* _F2CXX_ANALYZER_HPP */
