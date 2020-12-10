#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#ifndef USE_ROSE_INTEL_PIN_SUPPORT
#error "ROSE not configured to use INTEL PIN support (use --with-IntelPin=<path>)."
#endif

// Required macros for Intel Pin tools (their header files require these) to compile
#define TARGET_LINUX
#define TARGET_IA32E
#define HOST_IA32E
#define USING_XED

// This macro will remove the "using namespace LEVEL_PINCLIENT;" from pin.H"
#define REQUIRE_PIN_TO_USE_NAMESPACES
#include "pin.H"

std::string output (LEVEL_CORE::IMG_TYPE image_type);
std::string output (LEVEL_CORE::SEC_TYPE section_type);

#endif
