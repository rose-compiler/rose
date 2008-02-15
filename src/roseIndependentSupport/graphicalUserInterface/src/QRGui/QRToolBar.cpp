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

#include "QRToolBar.h"
#include <qtoolbutton.h>
#include <qlabel.h>

using namespace std;

struct QRToolBarItem {
    int id;
    QToolButton *button;
};

QRToolBar::QRToolBar(QWidget *parent, const char *name): QHBox(parent, name) { 
   setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
   setFrameStyle(QFrame::MenuBarPanel + QFrame::Sunken);
}
    
void QRToolBar::insertItem(string label, int id, bool toggleButton) {
   QToolButton *button;
   button = new QToolButton(this);
   button->setFixedSize(60,30);
   button->setTextLabel(label);
   button->setTextPosition(QToolButton::BesideIcon);
   button->setUsesTextLabel(true);
   button->setToggleButton(toggleButton);
   button->setAutoRaise(true);
   
   QRToolBarItem item;
   item.id = id;
   item.button = button;
   
   m_items[button] = item;
   m_ids[id] = item;
   
   connect(button, SIGNAL(clicked()), this, SLOT(button_clicked()));
   
}

void QRToolBar::insertSeparator() {
   QLabel *label = new QLabel(this);
   label->setFixedSize(20,0);
   label->setMargin(0);
}
     

void QRToolBar::button_clicked() {
    QToolButton *button = (QToolButton *) sender();
    QRToolBarItem &item = m_items[button];
    emit clicked(item.id, button->state() == QButton::On);
}

void QRToolBar::setState(int id, bool buttonDown) {
    QRToolBarItem &item = m_ids[id];    
    QToolButton *button = item.button;
    button->setOn((buttonDown? QButton::On : QButton::Off));
    emit clicked(item.id, buttonDown);
}
 
void QRToolBar::setEnable(int id, bool enabled) {
    QRToolBarItem &item = m_ids[id];
    QToolButton *button = item.button;
    
    button->setEnabled(enabled);
}
    
bool QRToolBar::getState(int id) {
   QRToolBarItem &item = m_ids[id];
   QToolButton *button = item.button;
    
   return button->isOn();
}

#include "QRToolBar.moc"
