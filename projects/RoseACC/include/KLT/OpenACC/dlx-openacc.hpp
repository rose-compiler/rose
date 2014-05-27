
#ifndef __KLT_DLX_OPENACC_HPP__
#define __KLT_DLX_OPENACC_HPP__

#include "DLX/OpenACC/language.hpp"
#include "DLX/klt-annotations.hpp"

#include <vector>

namespace DLX {

/*!
 * \addtogroup grp_klt_dlx_openacc
 * @{
*/

template <>
void KLT_Annotation<DLX::OpenACC::language_t>::parseRegion(std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> > & container);

template <>
void KLT_Annotation<DLX::OpenACC::language_t>::parseData(std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> > & container);

template <>
void KLT_Annotation<DLX::OpenACC::language_t>::parseLoop(std::vector<DLX::KLT_Annotation<DLX::OpenACC::language_t> > & container);

/** @} */

}

#endif /* __KLT_DLX_OPENACC_HPP__ */

