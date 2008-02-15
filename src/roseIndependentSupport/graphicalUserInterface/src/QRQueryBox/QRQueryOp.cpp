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

#include <QRQueryOp.h>

QRQueryOperator::QRQueryOperator() {
    m_progress_func = NULL;
    m_progress_arg = NULL;
}

void QRQueryOperator::setProgressFunc(QRQueryProgressFunc func, void *arg) {
    m_progress_func = func;
    m_progress_arg = arg;
}

void QRQueryOperator::performQuery(QRQueryDomain *domain, QRQueryRange *range) {
   startQuery();
   range->clear();
   set<SgNode *> *nodes = domain->getNodes();
   string str;
   unsigned hits = 0;
   if (m_progress_func) (*m_progress_func)(true, nodes->size(), m_progress_arg);
   for (set<SgNode *>::iterator iter = nodes->begin(); iter != nodes->end(); iter++) {
       SgNode *node = *iter;
       str.clear();
       if (query(node, str)) {
	   if (str.empty()) {
	       range->insertNode(node);
	   } else {
	       range->insertNode(node, str);
	   }
	   hits++;
           if (m_progress_func) (*m_progress_func)(false, hits, m_progress_arg);
       }
   }    
   terminateQuery();   
}

QRQueryOpFunc::QRQueryOpFunc(TypeQueryFuncPtr queryFunc) {
    m_queryFunc = queryFunc;
}

void QRQueryOpFunc::startQuery() { }
bool QRQueryOpFunc::query(SgNode *node, string &text) {
   ROSE_ASSERT(m_queryFunc);    
   return (*m_queryFunc) (node, text);
}
void QRQueryOpFunc::terminateQuery() { }


QRQueryOpVariant::QRQueryOpVariant(const NodeQuery::VariantVector &vector) {
    m_variantVector = vector;
}

void QRQueryOpVariant::startQuery() { }

bool QRQueryOpVariant::query(SgNode *node, string &) {
    VariantT variant = node->variantT();
    
    for (NodeQuery::VariantVector::iterator iter = m_variantVector.begin();
           iter != m_variantVector.end(); iter++)
    {
	if (variant == *iter) return true;
    }
    
    return false;    
}
    
void QRQueryOpVariant::terminateQuery() { }





