/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRQUERYOP_H
#define QRQUERYOP_H

#include <rose.h>
#include <QRQueryDomain.h>
#include <QRQueryRange.h>

namespace qrs {
/**
* \ingroup qrose_comps
*/

/// Query operator base class
class QRQueryOperator {
public:
    /// Event-handler for displaying progress
    typedef void (*QRQueryProgressFunc) (bool,unsigned,void*);

    QRQueryOperator();

    /// Performs query given @a domain and places results in @a range
    void performQuery(QRQueryDomain *domain, QRQueryRange *range);
      /**
         @brief Sets progress event-handler

         The event handler(@a func) supplied is of type QRQueryProgressFunc, which
         contains three arguments:  @a isInit (boolean), @a count(unsigned) and a
         user-supplied argument (@a arg). This
         event-handler is called first with @a isInit set to 'true' and @a count with
         the total number of domain elements. Afterwards, the event-handler is called for
         every node queried with @a isInit set to 'false' and @a count set to the number
         of nodes queried at that point.

    **/
    void setProgressFunc(QRQueryProgressFunc func, void *arg);

protected:

         /// Called before querying nodes
    virtual void startQuery() = 0;

         /// Node query method (returns true if it matches query). Argument @a text contains  additional information (optional).
    virtual bool query(SgNode *node, std::string &text) = 0;
         /// Called after querying nodes
    virtual void terminateQuery() = 0;
    QRQueryProgressFunc m_progress_func;

    void *m_progress_arg;
};

/**
* \ingroup qrose_comps
**/

/// Query operator based on an a class method
template <class T>
class QRQueryOp: public QRQueryOperator {
public:

    /// Arbitrary class method type
    typedef bool (T::*TypeQueryMethodPtr) (SgNode *, std::string &);

    /// Constructor receives as argument a method pointer of class T
    QRQueryOp(TypeQueryMethodPtr queryMethod) {
	m_classInstance = NULL;
	m_queryMethod = queryMethod;
    }

protected:
    virtual void startQuery() { m_classInstance = new T; }
    virtual bool query(SgNode *node, std::string &text) {
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


/**
* \ingroup qrose_comps
**/

/// Query operator based on a function
class QRQueryOpFunc: public QRQueryOperator {
public:
    /// Function type
    typedef bool (*TypeQueryFuncPtr) (SgNode *, std::string &);
     /// Constructor receives as argument a query node function
    QRQueryOpFunc(TypeQueryFuncPtr queryFunc);

protected:
    virtual void startQuery();
    virtual bool query(SgNode *node, std::string &text);
    virtual void terminateQuery();

protected:
    TypeQueryFuncPtr m_queryFunc;
};

/**
* \ingroup qrose_comps
**/

/// Query operator based on a variant vector
class QRQueryOpVariant: public QRQueryOperator {
public:

    /// Constructor receives a variant vector.
    QRQueryOpVariant(const VariantVector &vector);

protected:
    virtual void startQuery();
    virtual bool query(SgNode *node, std::string &text);
    virtual void terminateQuery();

protected:
    VariantVector m_variantVector;
};

}
#endif

