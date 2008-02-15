/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                          *
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

#ifndef QRPIXMAP_H
#define QRPIXMAP_H

#include <qpixmap.h>
#include <map>

/**********************************************************[icons]*/ 
namespace icons {
   #include <icons22/source_c.xpm>
   #include <icons22/func_decl.xpm> 
   #include <icons22/func_defn.xpm>
   #include <icons22/class_decl.xpm>
   #include <icons22/typedf.xpm>
   #include <icons22/project.xpm>
   #include <icons22/var.xpm>
   #include <icons22/method_public_decl.xpm>
   #include <icons22/method_public_defn.xpm>
   #include <icons22/method_npublic_decl.xpm>
   #include <icons22/method_npublic_defn.xpm>
   #include <icons22/empty.xpm>    
} 
/******************************************************************/


class QRPixmap {        
public:        
    static void clear_pixmap_cache();       
    static QPixmap* get_pixmap(char** pixdef);   
    
protected:    
    static std::map<const char **, QPixmap *> m_icons_cache;    
};

#endif
