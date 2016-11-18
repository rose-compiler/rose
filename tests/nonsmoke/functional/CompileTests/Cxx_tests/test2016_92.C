// This is a bug that ROSE reported specific to ROSE compiling the ROSE header files.
// These header files are now included in ROSE and so we need to compile them.
// Note that this only appears to be an issue in voost versions 1.57 and later (also 
// confimred in boost version 1.60).

// #include <boost/serialization/type_info_implementation.hpp>
#include <boost/serialization/shared_ptr.hpp>
