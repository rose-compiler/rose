// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file FuncSig.h
 *  \brief Declares a class for the functions of a C API.
 */

#if !defined (INC_C_API_FUNCSIG_H)
#define INC_C_API_FUNCSIG_H

#include <string>
#include <vector>
#include <map>
#include <C-API/SigBase.h>

namespace C_API
{
    //! Stores an API-specific function signature.
  class FuncSig : public SigBase
  {
  public:
    //! Stores a list of arguments.
    typedef std::vector<std::string> Args_t;

    //! Maps argument names to their position in the argument list.
    typedef std::map<std::string, size_t> ArgsMap_t;

    //! Constructor: Build a descriptor.
    FuncSig (const std::string& name, const Args_t& args);

    //! Copy constructor.
    FuncSig (const FuncSig& f);

    //! Get the number of arguments this function accepts.
    size_t getNumArgs (void) const;

    //! Returns 'true' iff this function accepts any arguments.
    bool hasArgs (void) const;

    //! Retrieves the name of the i-th argument, where 'i' is a 1-based index.
    const std::string& getArg (size_t i) const;

    /*!
     *  \brief Retrieves the position of the argument whose name is
     *  'name', or 0 if the argument can't be found.
     */
    size_t getArg (const std::string& name) const;

    //! \name Provides an iterator interface to the arguments.
    //@{
    typedef Args_t::const_iterator const_iterator;
    const_iterator begin (void) const;
    const_iterator end (void) const;
    //@}

  private:
    Args_t args_; //!< Function's expected arguments, by name.
    ArgsMap_t args_map_; //!< Argument lookup table, sync'd with 'args_'.

    /*!
     *  \brief Invert 'args_' as 'args_map_': If 'args[i] == s', then
     *  initialize 'args_map_' such that 'args_map_[s] == i'.
     */
    void buildMap (void);
  }; // class FuncSig
} // namespace C_API

#endif // !defined (INC_C_API_FUNCSIG_H)

// eof
