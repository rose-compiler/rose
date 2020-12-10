

#ifndef _F2CXX_ANALYZER_HPP

#define _F2CXX_ANALYZER_HPP 1

#include <set>

#include "rose.h"

namespace f2cxx
{
  enum ParamAttr
  {
    none        = (0),
    param       = (1 << 0),  // parameter
    data        = (1 << 1),  // 3D-data
    dataparam   = (data | param),
    space_lower = (1 << 2),  // marks parameters that are part of space bounds
    space_upper = (1 << 3),
    loop_lower  = (1 << 4),  // marks parameters that are part of loop bounds
    loop_upper  = (1 << 5),
    normal_expr = (1 << 6),  // unrelated expression
    induction   = (1 << 7),  // marks induction variables (curr. only loop variables)
    amrex_loop  = (1 << 8)   // marks loops that require translations
  };

  struct AnalyzerAttribute : AstAttribute
  {
      // implicit
      AnalyzerAttribute(ParamAttr attr = none)
      : AstAttribute(), val(attr), data_upper(), data_lower(),
        iter_upper(), iter_lower(), transl(NULL), data_bounds(NULL)
      {}

      /** returns a set with associated declarations
       *  \todo basic correctness is a 1:1 mapping, thus only one entry
       *        is expected at this time. */
      std::set<SgInitializedName*>& associates(ParamAttr relation);

      /** records the role @p attr of an variable/parameter */
      AnalyzerAttribute& role(ParamAttr attr);

      /** tests whether the role @p attr is set */
      bool hasRole(ParamAttr attr) const;

      /** records a relationship described by @p attr with @p other */
      AnalyzerAttribute& associate(ParamAttr attr, SgInitializedName* other);

      /** records an indexing relationship at dimension @p dim with @p idx */
      AnalyzerAttribute& access_dim(size_t dim, SgInitializedName* idx);

      /** returns the array access of this induction variable. */
      std::pair<size_t, SgInitializedName*> array_access() const;

      /** removes all stored array accesses */
      void clear_array_access();

      /** returns the role */
      ParamAttr flags() const { return val; }

      /** sets a node prototype for the C++ code. */
      AnalyzerAttribute& setTranslated(SgLocatedNode& cppnode)
      {
        transl = &cppnode;
        return *this;
      }

      /** Retrieves the C++ prototype. */
      SgLocatedNode* translated() const { return transl; }

      /** sets a node prototype for the C++ code. */
      AnalyzerAttribute& setBounds(SgInitializedName& cppnode)
      {
        data_bounds = &cppnode;
        return *this;
      }

      /** Retrieves the C++ prototype for the bounds. */
      SgInitializedName* bounds() const { return data_bounds; }

    private:
      /** stores the discovered role in the fortran code */
      ParamAttr                    val;

      /** stores all space upper bounds associated with this datablock */
      std::set<SgInitializedName*> data_upper;

      /** stores all space lower bounds associated with this datablock */
      std::set<SgInitializedName*> data_lower;

      /** stores all loop upper bounds associated with this datablock */
      std::set<SgInitializedName*> iter_upper;

      /** stores all loop lower bounds associated with this datablock */
      std::set<SgInitializedName*> iter_lower;

      /** stores all datablocks associated with this bound value */
      std::set<SgInitializedName*> datablocks;

      /** stores all variables used for indexing into a dimension of this datablock */
      std::map<size_t, std::set<SgInitializedName*> > indexvars;

      /** the translated node in C++ */
      SgLocatedNode*               transl;
      SgInitializedName*           data_bounds;
  };

  /** retrieves attribute on n
   *  \details
   *    if the attribute is not yet set, it will be created. */
  AnalyzerAttribute& getAttribute(SgNode& n);

  struct Analyzer
  {
    /** @brief
     *    Analyzes the procedure parameters and its uses in order to
     *    determine whether they need to be translated to special Amrex
     *    types.
     *  @details
     *     Supported type migration:
     *     * any three dimensional array in Fortran will be translated
     *       to an amrex::Box
     *     * a combination of four 3-element arrays will be translated
     *       into amrex::FArrayBox. The four arrays describe:
     *       - lower bounds on a 3D array
     *       - upper bounds on a 3D array
     *       - lower bounds on an iteration space over a 3D array
     *       - upper bounds on an iteration space over a 3D array
     */
    void operator()(SgProcedureHeaderStatement* n);
  };

  void print_param_tags(SgProcedureHeaderStatement* proc);
}

#endif /* _F2CXX_ANALYZER_HPP */
