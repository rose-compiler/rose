/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <QRPixmap.h>

using namespace std;

namespace qrs {

map<const char**, QPixmap *> QRPixmap::m_icons_cache = map<const char**, QPixmap *>();

void QRPixmap::clearCache() {

    for (map<const char**, QPixmap *>::iterator iter = m_icons_cache.begin();
         iter != m_icons_cache.end(); iter++)
    {
	delete iter->second;
    }
}

/**
QRose and Qt use the $(QBASE_DOC)/qpixmap.html>QPixmap</a> class to deal with images and icons in widgets, such as buttons. All QRose icons are stored in xpm format in the include/icons directory. You can use QPixmap::getPixmap to convert an xpm file into QPixmap object the first time it is accessed, and stored for subsequent accesses. All instances of QPixmap stored are automatically destroyed at the end of the program cycle.


   \par Example:
   \code
   int main(int argc, char *argv[]) {
     QRGUI::init(argc, argv);
     QRDialog *d = QRGUI::getDialog(0);
     QRTestWidget *w = new QRTestWidget(d);

     w->setCaption(QRPixmap::getPixmap(RoseIcons::no), "quit");

     QRGUI::exec();
     return 0;
   }
   \endcode
**/
QPixmap* QRPixmap::getPixmap(const char** pixdef) {
    map<const char**, QPixmap *>::iterator iter = m_icons_cache.find((const char **) pixdef);
    QPixmap *pixmap;

    if (iter == m_icons_cache.end()) {
	pixmap = new QPixmap((const char **) pixdef);
	m_icons_cache[(const char **) pixdef] = pixmap;
	return pixmap;
    } else {
	return iter->second;
    }
}
}

