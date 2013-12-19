
#ifndef __KLT_CORE_UTILS_HPP__
#define __KLT_CORE_UTILS_HPP__

#include <iostream>
#include <fstream>

class SgProject;

namespace KLT {

namespace Core {

/*!
 * \addtogroup grp_klt_core
 * @{
*/

void initAstFromString(std::ifstream & in_file);

void ensure(char c);

/** @} */

}

}

#endif /* __KLT_CORE_UTILS_HPP__ */
