// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
/**
 *  \file SigBase.h
 *  \brief Declares a base class for the signatures of API-entities.
 */

#if !defined (INC_C_API_SIGBASE_H)
#define INC_C_API_SIGBASE_H //!< SigBase.h included.

#include <string>

namespace C_API
{
  //! Base-class for an API-specific signature entity.
  class SigBase
  {
  public:
    //! Returns the entity's name.
    const std::string& getName (void) const;

  protected:
    SigBase (void);
    SigBase (const std::string& name);
    virtual ~SigBase (void);

    //! Sets the entity's name.
    void setName (const std::string& name);

  private:
    std::string name_; //!< Entity name.
  }; // class SigBase
} // namespace C_API

#endif // !defined (INC_C_API_SIGBASE_H)

// eof
