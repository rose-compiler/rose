/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 *   Class: QRCoordBox                                                     *
 ***************************************************************************/

#ifndef QRCOORDS_H
#define QRCOORDS_H

#include <list>
#include <string>

class SgNode;

namespace qrs {

/**
* \ingroup qrose_comps
*/

/// Source coordinates of an IR node
class QRCoordBox {
public:
    QRCoordBox();


    /// Sets IR node
    void setNode(SgNode *node);
    /// Returns IR node
    SgNode *getNode();
    /// Sets coordinate
    void setCoord(int line0, int col0, int line1, int col1);
    /// Returns true if given coordinates enclose this region
    bool insideBox(int line0, int col0, int line1, int col1);
    /// Returns y0 coordinate
    int line0() { return m_line0; }
    /// Returns x0 coordinate
    int col0() { return m_col0; }
    /// Returns y1 coordinate
    int line1() { return m_line1; }
    /// Returns x1 coordinate
    int col1() { return m_col1; }
    /// Inserts a source-coordinate to this node
    void insertBox(QRCoordBox *box);
    /// Whether this box (or its children) include this coordinate
    bool isTarget(int line, int col);
    /// Returns all enclosed boxes
    const std::list<QRCoordBox *> *children() const { return &m_childBoxes; }

    std::string str();

protected:
    std::list<QRCoordBox *> m_childBoxes;
    SgNode *m_node;
    int m_line0;
    int m_col0;
    int m_line1;
    int m_col1;
};

}
 #endif

