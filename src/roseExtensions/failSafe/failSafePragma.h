#ifndef __FAIL_SAFE_PRAGMA_H__
#define __FAIL_SAFE_PRAGMA_H__

/**
 * Pragma parsing for FAIL-SAFE project
 * 
 * By Liao, Jan. 27th, 2014
 * 
 */
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <vector>
class SgNode;

namespace FailSafe
{

  // FAIL-SAFE construct name list
  // All directive and clause types are listed so they are accessible uniformly
  enum failsafe_construct_enum
  {
    e_unknow = 0,

    //1. Directives
    e_region, 
    e_status_predicate,
    e_data_predicate,
    e_tolerance,
    e_double_redundancy,
    e_triple_redundancy,
    e_save,

   
   // 2. Clauses
   e_assert, 
   e_region_reference, 
   e_violation,
   e_recover,

   // 3. Values of some clauses
   
   // specifier value
   e_pre, // default for most pragmas
   e_post,
   
   // error types
   // TODO: define a real set of error types
   e_type_ET1,
   e_type_ET2,
   
   // violation types
   e_violation_NaN, 
   e_violation_SECDED,
   e_violation_SEGFAULT,
   e_violation_ANY,
   // TODO mantissa ?
   // TODO : bits
   
   e_not_failsafe // not a FAIL-SAFE construct
  }; // end fail_safe_construct_enum


  //! Help convert FailSafe construct to string 
  std::string toString (failsafe_construct_enum fs_type);
} //end namespace FailSafe

#endif /* __FAIL_SAFE_PRAGMA_H__ */
