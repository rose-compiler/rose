/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                           *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QRPixmap.h>

using namespace std;

map<const char**, QPixmap *> QRPixmap::m_icons_cache = map<const char**, QPixmap *>();
    
void QRPixmap::clear_pixmap_cache() {
    
    for (map<const char**, QPixmap *>::iterator iter = m_icons_cache.begin();
         iter != m_icons_cache.end(); iter++) 
    {
	delete iter->second;
    }
}
    
QPixmap* QRPixmap::get_pixmap(char** pixdef) {
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
	        
                  