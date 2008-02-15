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

#ifndef QRQUERYOP_H
#define QRQUERYOP_H

#include <config.h>
#include <rose.h>
#include <QRQueryDomain.h>
#include <QRQueryRange.h>

class QRQueryOperator {    
public:
    QRQueryOperator();
    void performQuery(QRQueryDomain *domain, QRQueryRange *range);
    void setProgressFunc(QRQueryProgressFunc func, void *arg);
    
protected:    
    virtual bool query(SgNode *node, string &text) = 0;    
    virtual void startQuery() = 0;
    virtual void terminateQuery() = 0;
    QRQueryProgressFunc m_progress_func;
    void *m_progress_arg;
};

template <class T>
class QRQueryOp: public QRQueryOperator {
public:
    typedef bool (T::*TypeQueryMethodPtr) (SgNode *, string &);    
    QRQueryOp(TypeQueryMethodPtr queryMethod) {
	m_classInstance = NULL;
	m_queryMethod = queryMethod;
    }
        
protected:        
    virtual void startQuery() { m_classInstance = new T; }    
    virtual bool query(SgNode *node, string &text) { 
	ROSE_ASSERT(m_classInstance);
	ROSE_ASSERT(m_queryMethod);	
	return (m_classInstance->*m_queryMethod)(node, text);
    }    
    virtual void terminateQuery() {
	delete m_classInstance;
	m_classInstance = NULL;
    }
	
protected:
   TypeQueryMethodPtr m_queryMethod;
   T *m_classInstance;
};

typedef bool (*TypeQueryFuncPtr) (SgNode *, string &);  

class QRQueryOpFunc: public QRQueryOperator {
public:
    QRQueryOpFunc(TypeQueryFuncPtr queryFunc);    
    
protected:    
    virtual void startQuery();
    virtual bool query(SgNode *node, string &text);    
    virtual void terminateQuery();    
    
protected:
    TypeQueryFuncPtr m_queryFunc;    
};

class QRQueryOpVariant: public QRQueryOperator {
public:
    QRQueryOpVariant(const NodeQuery::VariantVector &vector);    
    
protected:    
    virtual void startQuery();
    virtual bool query(SgNode *node, string &text);    
    virtual void terminateQuery();    
    
protected:
    NodeQuery::VariantVector m_variantVector; 
};

#endif

