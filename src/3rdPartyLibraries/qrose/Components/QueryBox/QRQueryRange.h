/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#ifndef QRQUERYRANGE_H
#define QRQUERYRANGE_H

#include <string>
#include <map>
#include <set>

class SgNode;

namespace qrs {
/**
* \ingroup qrose_comps
**/

/// Query Range
class QRQueryRange {
public:
   //@ {
        /// @name Properties

        /// Returns range elements
   std::set<SgNode *> *getNodes() { return &m_nodes; }
        /// Returns true if node contains additional information
   bool getNodeStr(SgNode *node, std::string &str);
        /// Counts the number of nodes that contain additional information
   unsigned countNodeStr();
        /// Coutns the total number of elements in the range
   unsigned countRange();
   //@}


   //@ {
        /// @name Actions

        /// Inserts new node
   void insertNode(SgNode *node);
        /// Inserts new node and additional information
   void insertNode(SgNode *node, std::string str);

        /// Removes all range elements
   void clear();

   //@}
protected:
  std::set<SgNode *> m_nodes;
  std::map<SgNode *, std::string> m_nodeStr;
};

}
#endif
