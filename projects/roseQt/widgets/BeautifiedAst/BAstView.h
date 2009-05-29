#ifndef BASTVIEW_H
#define BASTVIEW_H

#include <QTreeView>
#include <QtDesigner/QDesignerExportWidget>

class SgNode;
class BAstModel;

class AstFilterInterface;

/**
 * Displays a tree of  Classes, Namespaces and Functions
 * There are two modes:
 *  - project mode (if node is a SgProject)
 *      - view is sorted after include and source files
 *  - if node is no SgProject: all elements of subtree spanned by this SgNode
 *    are displayed
 */
class QDESIGNER_WIDGET_EXPORT BAstView : public QTreeView
{
    Q_OBJECT

    public:
        BAstView(QWidget * parent=NULL);
        virtual ~BAstView();

    public slots:
        void setNode(SgNode * node);
        void setFilter(AstFilterInterface * filter);
    signals:
        void clicked(SgNode * node);
        void clicked(const QString & file, int startRow, int startCol,
                                           int endRow, int endCol);

    protected slots:
        void viewClicked(const QModelIndex & ind);


    protected:
        BAstModel * model;

};

#endif
