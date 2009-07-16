/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRPIXMAP_H
#define QRPIXMAP_H

#include <map>
#include <QPixmap>

namespace qrs {

/**
* \ingroup qrose_framework
*/

/// Pixmap cache for converting and storing XPM pixmap files
class QRPixmap {
public:
    /// Removes all pixmaps stored in cache
    static void clearCache();
    /// Returns a $(QBASE_DOC)/qpixmap.html>QPixmap</a> object from an XPM data-structure
    static QPixmap* getPixmap(const char** pixdef);

protected:
    static std::map<const char **, QPixmap *> m_icons_cache;
};

}
#endif
