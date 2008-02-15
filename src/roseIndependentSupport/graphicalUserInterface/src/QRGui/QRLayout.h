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

#ifndef QRLAYOUT_H
#define QRLAYOUT_H

#include <qdialog.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qframe.h>
#include "QRGui.h"

class QRPanelLayout {
public:
    QRPanelLayout(QWidget *widget);
    QSplitter* frame() {return m_splitter; }
    void sizes(int, ...);
    
protected:
    QBoxLayout *m_layout;
    QSplitter *m_splitter; 
    QWidget *m_widget;
    QRGUI::PanelType m_panel_type;  
};

class QRPanel: public QFrame, public QRPanelLayout {
    
public:
     QRPanel(QWidget *parent, const char *name = 0);
     QRPanel(const char *name = 0);
};

class QRDialog: public QDialog, public QRPanelLayout {
    Q_OBJECT
    
public:
    QRDialog(QWidget *parent = 0, const char *name = 0);
    
public slots:
    void click();
           
};

#endif
