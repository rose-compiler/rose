
#ifndef __KLT_UTILS_HPP__
#define __KLT_UTILS_HPP__

#include <iostream>
#include <fstream>

class SgProject;

namespace KLT {

/*!
 * \addtogroup grp_klt
 * @{
*/

void initAstFromString(std::ifstream & in_file);

void ensure(char c);

/** @} */

}

#endif /* __KLT_UTILS_HPP__ */
