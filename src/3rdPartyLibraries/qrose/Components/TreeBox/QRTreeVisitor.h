#ifndef QRTREEVISITOR_H
#define QRTREEVISITOR_H


//#include <rose.h>
#include <QRListViewItem.h>

#include <map>

class SgNode;

namespace qrs {

class QRTreeBox;


typedef struct QRTreeVisitorInfo  {
    friend class QRTreeBox;
    friend class QRTreeVisitor;

public:
   QRTreeVisitorInfo() { level = 0; }

private:
   int level;
} QRTreeVisitorInfo;

class QRTreeVisitor: public AstTopDownProcessing<QRTreeVisitorInfo> {

public:
    QRTreeVisitor(QRTreeBox *tree);
    void setArguments(bool cacheView, Q3ListView *lv,
        std::map<QRListViewItem *, SgNode *> *viewItems,
        std::map<SgNode *, QRListViewItem *> *nodeItems,
        std::map<std::string, unsigned> *capturedNames,
        std::map<unsigned, std::string> *capturedIndices,
        int detailLevel, bool cacheShowChildren);

    void performTraversal(SgNode *node);

protected:
    void prepareTraversal();
    QRTreeVisitorInfo virtual evaluateInheritedAttribute
          (SgNode *node, QRTreeVisitorInfo info);
    void registerNode (QRListViewItem *li, SgNode *node);
    void processNormalView(SgNode *node, QRTreeVisitorInfo &info);
    void processCacheView(SgNode *node, QRTreeVisitorInfo &info);
    std::string getNodeDesc(SgNode *node);

protected:
    QRTreeBox *m_tree;
    unsigned m_id_counter;
    std::map<SgNode *, unsigned> m_ids;
    std::map<SgNode *, QRListViewItem *> m_lastChild;

    bool m_cacheView;
    Q3ListView *m_lv;
    std::map<QRListViewItem *, SgNode *> *m_viewItems;
    std::map<SgNode *, QRListViewItem *> *m_nodeItems;
    std::map<std::string, unsigned> *m_capturedNames;
    std::map<unsigned, std::string> *m_capturedIndices;
    bool m_cacheShowChildren;
    int m_detailLevel;
};

}

#endif
