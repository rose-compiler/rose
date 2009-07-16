/***************************************************************************
 *   Copyright (C) 2005 by Jose G de Figueiredo Coutinho                   *
 *   jgfc@doc.ic.ac.uk                                                     *
 ***************************************************************************/

#include <QRCoords.h>
#include <QRMain.h>
#include <QRAstInfo.h>
#include <rose.h>

using namespace std;

namespace qrs {

QRCoordBox::QRCoordBox() {
    m_node = NULL;
    m_line0 = m_col0 = m_line1 = m_col1 = 0;
}


void QRCoordBox::setNode(SgNode *node) {
    m_node = node;
}

SgNode *QRCoordBox::getNode() {
    return m_node;
}

void QRCoordBox::setCoord(int line0, int col0, int line1, int col1) {
    m_line0 = line0; m_col0 = col0; m_line1 = line1; m_col1 = col1;
}

void QRCoordBox::insertBox(QRCoordBox *box) {
   m_childBoxes.push_back(box);
}


bool QRCoordBox::isTarget(int line, int col) {
   bool  result = ( ((m_line0 < line) || ((m_line0 == line) && (m_col0 <= col))) &&
             ((line < m_line1) || ((m_line1 == line) && (col <= m_col1))));
   return result;
}

string QRCoordBox::str() {
    string className = m_node->sage_class_name();
    string info = QRAstInfo::getInfo(m_node);
    if (className == info) {
       return QROSE::format("%s", className.c_str());
    } else {
       return QROSE::format("%s [%s]", className.c_str(), info.c_str());
    }
}

}
