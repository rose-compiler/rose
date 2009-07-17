/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <rose.h>

#include <QRQueryRange.h>

using namespace std;

namespace qrs {

void QRQueryRange::clear() {
   m_nodes.clear();
   m_nodeStr.clear();
}

void QRQueryRange::insertNode(SgNode *node) {
    m_nodes.insert(node);
}

void QRQueryRange::insertNode(SgNode *node, string str) {
    insertNode(node);
    m_nodeStr[node] = str;
}

bool QRQueryRange::getNodeStr(SgNode *node, std::string &str) {
   map<SgNode *, string>::iterator iter = m_nodeStr.find(node);

   if (iter == m_nodeStr.end()) {
      return false;
   } else {
      str = iter->second;
      return true;
   }
}

unsigned QRQueryRange::countNodeStr() {
    return m_nodeStr.size();
}

unsigned QRQueryRange::countRange() {
   return m_nodes.size();
}

}
