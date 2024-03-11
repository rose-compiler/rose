#ifndef ROSE_sageRti_H
#define ROSE_sageRti_H

#include <string>
#include <vector>

// MS 2002: global type
/*! \brief Access to C++ Run Time Information (RTI)

    I'm unclear as to where this is used (Markus put it here, I think).

    \internal This is not a part of the public interface to SAGE III.

 */
class RTIMemberData
   {
  // DQ (12/31/2005): Note that we can't use "using namespace std;"
  // to simplify this code, or so it seems.
     public:
          // The first two of these fields are constants, and so this avoids the constructor
          const char* type;
          const char* name;
          std::string value;

          RTIMemberData(): type(nullptr), name(nullptr), value() {}

          RTIMemberData(const char* type0, const char* name0, const std::string& value0)
             : type(type0), name(name0), value(value0)
             {}
   };

typedef std::vector<RTIMemberData> RTIReturnType;

#endif
