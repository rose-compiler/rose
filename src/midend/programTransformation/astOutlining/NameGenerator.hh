/**
 *  \file NameGenerator.cc
 *  \brief Class to generate numbered names.
 *
 *  Updated Aug 19, 2021. Support per file counters instead a single counter
 */

#if !defined(INC_NAMEGENERATOR_HH)
#define INC_NAMEGENERATOR_HH //!< NameGenerator.hh included.

#include <string>
#include <cstdlib>
#include "rosedll.h"
#include <map> 

class ROSE_DLL_API NameGenerator
{
public:
  NameGenerator (size_t first_id = 1);
  NameGenerator (const std::string& pre,
                 size_t first_id = 1,
                 const std::string& suf = std::string (""));
  virtual ~NameGenerator (void) {}

  //! Returns the current ID number.
  size_t id (std::string full_file_name= "") ;

  //! Returns the prefix.
  std::string prefix (void) const;

  //! Returns the suffix.
  std::string suffix (void) const;

  //! Returns the current name, based on the current ID number.
  virtual std::string current (std::string full_file_name= "") ;

  //! Returns the next name, updating the current ID number.
  // Per file based unique ID
  std::string next (std::string full_file_name= "");

private:
  size_t first_id_;
//  size_t cur_id_; //!< Current ID number.
//  If file_name is not provided, we use cur_id_[""]
//  otherwise use cur_id[file_name]
  std::map<std::string, size_t> cur_id_; 
  std::string prefix_; //!< Name prefix.
  std::string suffix_; //!< Name suffix.
};

#endif

// eof
