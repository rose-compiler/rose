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

#ifndef QRQUERYRANGE_H
#define QRQUERYRANGE_H

#include <config.h>
#include <rose.h>
#include <string>
#include <map>

class QRQueryRange {
public:    
   void clear();
   void insertNode(SgNode *node);
   void insertNode(SgNode *node, std::string str);
   std::set<SgNode *> *getNodes() { return &m_nodes; }
   bool getNodeStr(SgNode *node, std::string &str);
   unsigned countNodeStr();
   unsigned countRange();
    
protected:    
  std::set<SgNode *> m_nodes;
  std::map<SgNode *, std::string> m_nodeStr;   
};

#endif
