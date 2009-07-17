#ifndef QRLISTVIEWITEM_H
#define QRLISTVIEWITEM_H

#include <Q3ListView>
#include <Q3ListViewItem>
#include <QRAttribute.h>

class SgNode;

namespace qrs {

class QRListViewItem : public Q3ListViewItem
{

public:
    QRListViewItem( Q3ListView *parent, const char *name);
    QRListViewItem( Q3ListView *parent, Q3ListViewItem *sibling, const char *name);
    QRListViewItem( Q3ListViewItem *parent, const char *name);
    QRListViewItem( Q3ListViewItem *parent, Q3ListViewItem *sibling, const char *name);

    virtual void paintCell( QPainter *p, const QColorGroup &cg,
                            int column, int width, int alignment);
    int compare ( Q3ListViewItem * i, int col, bool ascending ) const;
    unsigned getId();
    void setId(unsigned id);

    void setNode(SgNode *node);
    SgNode* getNode();

    AstAttribute* getAstAttribute(std::string attrib_name);
    QRAttribute* getQRattrib(int column);
    bool isWritable(int column, QRAttribute *qrattrib = NULL);
    void setCellValue(int column, QRAttribute *qrattrib, std::string str);

protected:
    unsigned m_id;
    SgNode *m_node;
};

}

#endif
