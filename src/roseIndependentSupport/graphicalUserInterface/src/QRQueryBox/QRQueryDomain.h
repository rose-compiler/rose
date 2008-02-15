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

#ifndef QRQUERYDOMAIN_H
#define QRQUERYDOMAIN_H

#include <config.h>
#include <rose.h>
#include <set>

typedef void (*QRQueryProgressFunc) (bool,unsigned,void*);

class QRQueryDomain {
public:
    typedef enum {no_expansion = 0, immediate_children, all_children} TypeDomainExpansion;
    QRQueryDomain();
    void expand(std::set<SgNode *> *domain, TypeDomainExpansion expansionMode);
    void expand(SgNode *domain, TypeDomainExpansion expansionMode);
    void clear();
    std::set<SgNode *>* getNodes() { return &m_domain; }
    unsigned countDomain();
       
protected:
    std::set<SgNode *> m_domain;
};

#endif

