/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   Class: QRAstInfo                                                      *
 ***************************************************************************/

#ifndef QRASTINFO_H
#define QRASTINFO_H

#include <QPixmap>

class SgNode;

namespace qrs {
/**
* \ingroup qrose_comps
*/

/// AST Information, such as icons and IR node description
class QRAstInfo {
public:
    /// Returns description of an IR node
    static std::string getInfo(SgNode *node);
    /// Returns true for important IR nodes
    static bool isRelevant(SgNode *node);
    /// Returns an icon (QPixmap) for an IR node
    static QPixmap* getPixmap(SgNode *node);
    /// Returns true for nodes that are known to fail an assert when unparsed
    static bool isUnparseBlackList(SgNode *node);

protected:
    static std::string abstract(std::string str);
};

}

#endif
