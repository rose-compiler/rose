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
#ifndef QRCALLBACK_H
#define QRCALLBACK_H

#include <qobject.h>
#include <map>

class QRCallback: public QObject {
    Q_OBJECT
    
    typedef void (*TypeFunc1) (QObject *, void *);
    typedef enum { func1 } FuncType;
    
public:    
    QRCallback();
    void link(QObject *obj, const char *signal, TypeFunc1 func, void *arg = NULL);    
    
protected slots:
    void linkSlot1();

protected:
    std::map<QObject *, std::pair<void *, void *> > m_callbacks;
};

#endif
