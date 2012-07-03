// -*- mode:C++; tab-width:8; indent-tabs-mode:nil -*-
/*!
 *  \defgroup ROSEHPCT_UTIL ROSE-HPCT Utilities
 *  \ingroup ROSEHPCT
 *
 *  This submodule, imported from other projects, implements a number
 *  of miscellaneous utility functions for compile-time type and type
 *  trait analysis and manipulation, file and pathname handling, a
 *  generic tree data structure, and simple templated operations
 *  (algorithms) over iterators, among others.
 *
 *  $Id: general.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

/*!
 *  \file rosehpct/util/general.hh
 *  \brief Miscellaneous utilities.
 *  \ingroup ROSEHPCT_UTIL
 *
 *  $Id: general.hh,v 1.1 2008/01/08 02:56:43 dquinlan Exp $
 */

#if !defined(INC_UTIL_GENERAL_HH)
//! util/general.hh included
#define INC_UTIL_GENERAL_HH

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdlib>

/*!
 *  \brief Defines a number of miscellaneous utility routines,
 *  imported from other projects.
 */
namespace GenUtil
{
  //! \addtogroup ROSEHPCT_UTIL
  //@{

  //! Generates a string representing some number of tabs
  std::string tabs (size_t num, size_t width = 4);

  //! Generates a hex address string representation of a pointer
  std::string toAddr (const void* p);

  //! Generates a string representation of a file location
  std::string toFileLoc (const std::string& filename,
                         size_t first_line, size_t last_line = 0);

  //! \name Returns the base filename for a pathname
  /*@{*/
  std::string getBaseFilename (const std::string& full_pathname);
  std::string getBaseFilename (const char* full_pathname);
  /*@}*/

  //! \name Returns the directory path for a pathname
  /*@{*/
  std::string getDirname (const std::string& full_pathname);
  std::string getDirname (const char* full_pathname);
  /*@}*/

  //! \name Loose pathname comparison.
  /*! Splits the input path names into directory names and
   *  base filenames, and checks that both components match
   *  lexically.
   */
  /*@{*/
  bool isSamePathname (const std::string& source, const std::string& target);
  bool isSamePathname (const char* source, const char* target);
  /*@}*/

  //! Convert to a string.
  template < class T >
  std::string toString (T i)
  {
    std::ostringstream ostr;
    ostr << i;
    return ostr.str ();
  }

  //! Returns the maximum element in a non-empty vector.
  template < class T >
  T max (const std::vector < T > &V)
  {
    ROSE_ASSERT (!V.empty ());

    T cur_max = V[0];             /* current maximum value */
    size_t i;
    for (i = 1; i < V.size (); i++)
      if (V[i] > cur_max)
        cur_max = V[i];
    return cur_max;
  }

  //! Returns the sum of a vector
  template < class T >
  T sum (const std::vector <T> & V)
  {
    if (V.empty ())
      return T (0);

    T s (0);
    for (typename std::vector<T>::const_iterator i = V.begin();
         i != V.end();
         ++i)
      {
        s += (*i);
      }
    return s;
  }

  //! Overloaded conversion-to-float routine for C++ string type
  inline
  double atof (const std::string& s)
  {
    return ::atof (s.c_str ());
  }

  //! Overloaded conversion-to-long routine for C++ string type
  inline
  long atol (const std::string& s)
  {
    return ::atol (s.c_str ());
  }

  //! Extract just the keys in a map
  template <typename KeyType, typename ValueType>
  std::vector<KeyType>
  getKeys (const std::map<KeyType, ValueType>& m)
  {
    std::vector<KeyType> v;
    for (typename std::map<KeyType, ValueType>::const_iterator i = m.begin ();
         i != m.end ();
         ++i)
      {
        v.push_back (i->first);
      }
    return v;
  }

  //@}
} /* namespace */


#endif

/* eof */
