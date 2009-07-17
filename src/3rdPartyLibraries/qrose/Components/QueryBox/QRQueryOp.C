/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>

#include <QRQueryOp.h>

using namespace std;

namespace qrs {

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
       }
       if (m_progress_func) (*m_progress_func)(false, hits, m_progress_arg);
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


QRQueryOpVariant::QRQueryOpVariant(const VariantVector &vector) {
    m_variantVector = vector;
}

void QRQueryOpVariant::startQuery() { }

bool QRQueryOpVariant::query(SgNode *node, string &) {
    VariantT variant = node->variantT();

    for (VariantVector::iterator iter = m_variantVector.begin();
           iter != m_variantVector.end(); iter++)
    {
	if (variant == *iter) return true;
    }

    return false;
}

void QRQueryOpVariant::terminateQuery() { }

}



