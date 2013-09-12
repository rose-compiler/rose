#ifndef WORKLIST_H
#define WORKLIST_H

/*************************************************************
 * Copyright: (C) 2012 by Markus Schordan                    *
 * Author   : Markus Schordan                                *
 * License  : see file LICENSE in the CodeThorn distribution *
 *************************************************************/

#include <WorkList.h> // from ROSE

namespace CodeThorn {

template <typename Element>
class WorkList: public WorkListNonUnique<Element> {};

} // end of namespace CodeThorn

#endif
