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

#ifndef QRTOOLBAR_H
#define QRTOOLBAR_H

#include <qhbox.h>
#include <qtoolbutton.h>
#include <string>
#include <map>

typedef struct QRToolBarItem;

class QRToolBar: public QHBox {
    
    Q_OBJECT
    
public:    
    QRToolBar(QWidget *parent = 0, const char *name = 0);
    void insertItem(std::string label, int id, bool toggleButton);
    void insertSeparator();
    
    void setState(int id, bool buttonDown);
    void setEnable(int id, bool enabled);
    bool getState(int id);
    
public slots:
    void button_clicked();

signals:
    void clicked(int id, bool buttonDown);    

protected:
    std::map<QToolButton *, QRToolBarItem> m_items;
    std::map<int, QRToolBarItem> m_ids;
};



#endif
