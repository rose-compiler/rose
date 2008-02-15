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

#include <QRCallback.h>

using namespace std;

QRCallback::QRCallback() {
    
    
}

void QRCallback::link(QObject *obj, const char *signal, TypeFunc1 func, void *arg) {   
   connect(obj,signal,(QObject *) this,SLOT(linkSlot1()));  
   m_callbacks[obj] = pair<void *, void *> ((void *) func, arg);
   
}

void QRCallback::linkSlot1() {
   map<QObject *, pair<void *,void *> >::iterator iter = m_callbacks.find((QObject *) sender());
   if (iter != m_callbacks.end()) {
      TypeFunc1 func1 = (TypeFunc1) iter->second.first;
      (*func1) (iter->first, iter->second.second);
  }
}

#include <QRCallback.moc>
