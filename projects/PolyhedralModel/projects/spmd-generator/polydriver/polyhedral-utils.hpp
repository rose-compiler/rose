
#ifndef __POLYHEDRAL_UTILS_HPP__
#define __POLYHEDRAL_UTILS_HPP__

#include "rose/rose-utils.hpp"

// Needed printers
std::ostream & operator << (std::ostream & out, SgStatement & arg);

std::ostream & operator << (std::ostream & out, const SgStatement & arg);

#endif /* __POLYHEDRAL_UTILS_HPP__ */

