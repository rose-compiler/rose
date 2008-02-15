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

#include <QRQueryRange.h>

using namespace std;

void QRQueryRange::clear() {
   m_nodes.clear();
   m_nodeStr.clear();
}

void QRQueryRange::insertNode(SgNode *node) {
    m_nodes.insert(node);
}

void QRQueryRange::insertNode(SgNode *node, string str) {
    insertNode(node);
    m_nodeStr[node] = str;
}
    
bool QRQueryRange::getNodeStr(SgNode *node, std::string &str) {
   map<SgNode *, string>::iterator iter = m_nodeStr.find(node);
   
   if (iter == m_nodeStr.end()) {
      return false; 
   } else {       
      str = iter->second;
      return true;
   }
}
       
unsigned QRQueryRange::countNodeStr() {
    return m_nodeStr.size();
}

unsigned QRQueryRange::countRange() {
   return m_nodes.size();
}
       
