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

#include "QRCoords.h"
#include <QRAstInfo.h>

using namespace std;

QRCoordBox::QRCoordBox() {
    m_node = NULL;
    m_childBoxes = NULL;
    m_line0 = m_col0 = m_line1 = m_col1 = 0; 
}
    
    
void QRCoordBox::setNode(SgNode *node) {
    m_node = node;
}

SgNode *QRCoordBox::getNode() {
    return m_node;
}

void QRCoordBox::setCoord(int line0, int col0, int line1, int col1) {
    m_line0 = line0; m_col0 = col0; m_line1 = line1; m_col1 = col1;
}

void QRCoordBox::insertBox(QRCoordBox *box) {
   if (!m_childBoxes)  {
       m_childBoxes = new list<QRCoordBox *>();
   }
   m_childBoxes->push_back(box);
}
    

bool QRCoordBox::isTarget(int line, int col) {
   bool  result = ( ((m_line0 < line) || ((m_line0 == line) && (m_col0 <= col))) &&
             ((line < m_line1) || ((m_line1 == line) && (col <= m_col1))));
   return result;
}

string QRCoordBox::str() {
    static char buffer[100];
    sprintf(buffer, "[%s] (%d, %d) - (%d, %d)", QRAstInfo::get_info(m_node).c_str(), 
	    m_line0, m_col0, m_line1, m_col1);
    return buffer;
}


