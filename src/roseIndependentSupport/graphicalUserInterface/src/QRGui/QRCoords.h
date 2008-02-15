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

#ifndef QRCOORDS_H
#define QRCOORDS_H

#include <config.h>
#include <rose.h>
#include <list>
#include <string>

class QRCoordBox {
public:    
    QRCoordBox();
    void setNode(SgNode *node);
    SgNode *getNode();  
    void setCoord(int line0, int col0, int line1, int col1);
    bool insideBox(int line0, int col0, int line1, int col1);
    int line0() { return m_line0; }
    int col0() { return m_col0; }
    int line1() { return m_line1; }
    int col1() { return m_col1; }
    void insertBox(QRCoordBox *box);
    bool isTarget(int line, int col);
    std::list<QRCoordBox *> *children() { return m_childBoxes; }
    std::string str();
    
protected:
    std::list<QRCoordBox *> *m_childBoxes;    
    SgNode *m_node;
    int m_line0;
    int m_col0;
    int m_line1;
    int m_col1;
};

 #endif

