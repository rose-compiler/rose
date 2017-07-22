#ifndef FORTRAN_BUILDER_INTERFACE_H
#define FORTRAN_BUILDER_INTERFACE_H

//-----------------------------------------------------------------------------------
// Abstract builder class for Fortran.
//-----------------------------------------------------------------------------------

namespace Fortran {

class FortranBuilderInterface
  {
    public:

    virtual std::string getCurrentFilename() = 0;

  };

} // namespace Fortran

// endif for FORTRAN_BUILDER_INTERFACE_H
#endif

