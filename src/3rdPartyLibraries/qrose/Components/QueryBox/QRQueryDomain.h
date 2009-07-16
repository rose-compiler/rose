/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRQUERYDOMAIN_H
#define QRQUERYDOMAIN_H

class SgNode;
#include <set>

namespace qrs {

/**
* \ingroup qrose_comps
*/

/// Query Domain
class QRQueryDomain {
public:
    /// Domain expansion type
    typedef enum {
         /// Items in domain are not expanded
         no_expansion,
         /// Domain includes initial items and its direct children
         immediate_children,
         /// Domain includes initial items and all its children
         all_children} TypeDomainExpansion;
    ///

    //@ {
           /// @name Initialization

           /// Creates a domain object
    QRQueryDomain();
    //@}

    //@ {
           /// @name Expansion

           /// Sets initial domain element and expansion type
    void expand(SgNode *domain, TypeDomainExpansion expansionMode);

           /// Sets initial domain elements and expansion type
    void expand(std::set<SgNode *> *domain, TypeDomainExpansion expansionMode);
     //@ }

    //@ {
           /// @name Properties

           /// Returns domain nodes
    std::set<SgNode *>* getNodes() { return &m_domain; }
           /// Returns number of domain elements
    unsigned countDomain();
    //@}

            /// Removes all domain elements
    void clear();

protected:
    std::set<SgNode *> m_domain;
};

}
#endif

