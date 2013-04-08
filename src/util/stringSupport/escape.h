#ifndef ROSE_ESCAPE_H
#define ROSE_ESCAPE_H

#include <string>
#include "rosedll.h"

ROSE_UTIL_API std::string escapeString(const std::string& s);
ROSE_UTIL_API std::string unescapeString(const std::string& s);

#endif // ROSE_ESCAPE_H
