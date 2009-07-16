/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <QRQueryDomain.h>
#include <vector>
#include <rose.h>
using namespace std;

namespace qrs {

class DomainVisitor: public AstSimpleProcessing {

public:
    DomainVisitor(set<SgNode *> *domain, QRQueryDomain::TypeDomainExpansion expansionMode) {
	m_root = NULL; m_domain = domain; m_expansionMode = expansionMode;
	for (int i = 0; i < V_SgNumVariants; i++) {
	    m_vectorMask.push_back((VariantT) i);
	}
    }
    void setRoot(SgNode *node) { m_root = node; }

protected:
    void visit(SgNode *node) {
	switch(m_expansionMode) {
	    case QRQueryDomain::immediate_children:
		if (node == m_root) {
		    addTypes(node);
		    addSymbols(node);
  	        }
   	       if ((node != m_root) && (node->get_parent() != m_root) &&
		   (!isSgMemberFunctionDeclaration (node) ||
		    (m_root != isSgMemberFunctionDeclaration(node)->get_scope ()->get_parent ()))) return;
	        m_domain->insert(node);
		break;
	    case QRQueryDomain::all_children:
		// add additional nodes that don't get visited from the traversal
                addTypes(node);
	        addSymbols(node);
	        m_domain->insert(node);
	    break;
	    default: /* no expansion: do nothing */ break;
	}
    }

    void addTypes(SgNode *node) {
       vector<SgNode *> child_nodes = NodeQuery::querySolverGrammarElementFromVariantVector(isSgNode(node), m_vectorMask);
       for (vector<SgNode *>::iterator iter = child_nodes.begin();
              iter != child_nodes.end(); iter++)
       {
	   m_domain->insert(*iter);
       }
   }

    void addSymbols(SgNode *node) {
    if (isSgScopeStatement(node)) {
	   SgSymbolTable *symTable = ((SgScopeStatement *) node)->get_symbol_table();
	   ROSE_ASSERT(symTable);
	   set<SgNode *> symbols = symTable->get_symbols();
 	   for (set<SgNode *>::iterator iter = symbols.begin(); iter != symbols.end(); iter++) {
	      SgSymbol *symbol = isSgSymbol(*iter);
  		  if (symbol) m_domain->insert(symbol);
	   }
	}
    }

protected:
    SgNode *m_root;
    set<SgNode *> *m_domain;
    QRQueryDomain::TypeDomainExpansion m_expansionMode;
    VariantVector m_vectorMask;
};


QRQueryDomain::QRQueryDomain() {
}

unsigned QRQueryDomain::countDomain() {
    return m_domain.size();
}


void QRQueryDomain::expand(std::set<SgNode *> *domain, TypeDomainExpansion expansionMode) {
    m_domain.clear();

    if (expansionMode == no_expansion) {
	m_domain = *domain;
    } else {
        set<SgNode *> &roots = *domain;
        DomainVisitor visitor(&m_domain, expansionMode);
        for (set<SgNode *>::iterator iter = roots.begin(); iter != roots.end(); iter++) {
	   SgNode *root = *iter;
   	   visitor.setRoot(root); visitor.traverse(root,preorder);
        }
    }
}

void QRQueryDomain::expand(SgNode *domain, TypeDomainExpansion expansionMode) {
    set<SgNode *> nodes;
    nodes.insert(domain);
    expand(&nodes, expansionMode);
}


void QRQueryDomain::clear() {
    m_domain.clear();
}

}
