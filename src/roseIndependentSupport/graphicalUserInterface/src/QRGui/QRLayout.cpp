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

#include "QRLayout.h"
#include <qapplication.h>
#include <qpushbutton.h>
#include <stdarg.h>
#include <qobjectlist.h>

QRPanelLayout::QRPanelLayout(QWidget *widget) {
    m_panel_type = QRGUI::panel_type();
    switch (m_panel_type) {
	case QRGUI::down:
           m_splitter = new QSplitter(QSplitter::Vertical, widget);     
           m_layout = new QBoxLayout(widget, QBoxLayout::Down);           
	   break;
	case QRGUI::right:
           m_splitter = new QSplitter(QSplitter::Horizontal, widget);
           m_layout = new QBoxLayout(widget, QBoxLayout::LeftToRight);          
    }   
    m_widget = widget;  
    m_layout->addWidget(m_splitter);  
}

   
void QRPanelLayout::sizes(int size, ...) {
    va_list ap;
  
    bool is_shown = QRGUI::dialog()->isShown();
    QRGUI::dialog()->show();
  
    va_start(ap, size);
    QValueList<int> rel_sizes;
    int total_rel_size = 0;
    QValueList<int> cur_sizes = m_splitter->sizes();
    
    int n = cur_sizes.count();    
    if (n == 0) return;
    QValueList<int>::Iterator it1 = cur_sizes.begin();
    
    int total_cur_size = 0;
    for (int i = 1; i < n; i++) {
       total_rel_size += size;	
       if (total_rel_size < 0) {
	   total_rel_size = size = 0; 
       } else if (total_rel_size > 100) {
	   total_rel_size -= size; size = 0;
       }
       rel_sizes.push_back(size);
       size = va_arg(ap, int);
       total_cur_size += *it1; it1++;
    }
    va_end(ap);
    rel_sizes.push_back(100 - total_rel_size);
    total_cur_size += *it1;    
    
    it1 = cur_sizes.begin();
    QValueList<int>::Iterator it2 = rel_sizes.begin();
    int count = 0;
    while( it1 != cur_sizes.end() ) {
	int &val = *it1;
	count += val;
	val = (int) ((float) total_cur_size * (float) (*it2 / 100.0));
        ++it1; ++it2;
	if (it1 == cur_sizes.end()) {
	    if (count != total_cur_size) {
		val = total_cur_size - (count - val);
	    }
	}
    }
       
    m_splitter->setSizes(cur_sizes);    
    QRGUI::dialog()->setShown(is_shown);
}

QRPanel::QRPanel(QWidget *parent, const char *name): QFrame(parent, name),
    QRPanelLayout(this)
{
    QRGUI::current_panel(this);
}

QRPanel::QRPanel(const char *name): QFrame(QRGUI::current_panel_frame()), 
    QRPanelLayout(this)
{
    QRGUI::current_panel(this);
}


QRDialog::QRDialog(QWidget *parent, const char *name): QDialog(parent, name, 0), 
                  QRPanelLayout(this) 
{
   // QPushButton *button = new QPushButton(m_splitter);
//    button->connect(button, SIGNAL(clicked()), this, SLOT(click()));
    
    setGeometry(0,0,1000,1000);    
    QRGUI::current_panel(this);    
}

void QRDialog::click() {
}

#include "QRLayout.moc"

