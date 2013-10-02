/**
 *  \file NameGenerator.cc
 *  \brief Class to generate numbered names.
 */

#if !defined(INC_NAMEGENERATOR_HH)
#define INC_NAMEGENERATOR_HH //!< NameGenerator.hh included.

#include <string>
#include <cstdlib>
#include "rosedll.h"

class ROSE_DLL_API NameGenerator
{
public:
  NameGenerator (size_t first_id = 1);
  NameGenerator (const std::string& pre,
                 size_t first_id = 1,
                 const std::string& suf = std::string (""));
  virtual ~NameGenerator (void) {}

  //! Returns the current ID number.
  size_t id (void) const;

  //! Returns the prefix.
  std::string prefix (void) const;

  //! Returns the suffix.
  std::string suffix (void) const;

  //! Returns the current name, based on the current ID number.
  virtual std::string current (void) const;

  //! Returns the next name, updating the current ID number.
  std::string next (void);

private:
  size_t cur_id_; //!< Current ID number.
  std::string prefix_; //!< Name prefix.
  std::string suffix_; //!< Name suffix.
};

#endif

// eof
