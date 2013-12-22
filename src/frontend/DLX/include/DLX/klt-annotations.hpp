
#ifndef __DLX_KLT_ANNOTATIONS_HPP__
#define __DLX_KLT_ANNOTATIONS_HPP__

#include <vector>

namespace DLX {

/*!
 * \addtogroup grp_dlx_klt_annotations
 * @{
*/

template <class Lang> class KLT_Data {
  public:
    static bool matchLabel();
    static void parse(std::vector<KLT_Data<Lang> > &);
};

template <class Lang> class KLT_Region {
  public:
    static bool matchLabel();
    static void parse(std::vector<KLT_Region<Lang> > &);
};

template <class Lang> class KLT_Loop {
  public:
    static bool matchLabel();
    static void parse(std::vector<KLT_Loop<Lang> > &);
};

/** @} */

}

#endif /* __DLX_KLT_ANNOTATIONS_HPP__ */

