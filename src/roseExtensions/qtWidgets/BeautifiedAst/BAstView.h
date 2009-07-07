#ifndef BASTVIEW_H
#define BASTVIEW_H

#include "RoseTreeView.h"
#include <QtDesigner/QDesignerExportWidget>

class SgNode;
class ItemTreeModel;

class AstFilterInterface;

/**
 * A tree showing namespaces,classes,function,loops (not raw SgNodes)
 *
 * <img src="../BeautifiedAst.jpg" alt="Beautified AST Screenshot">
 *
 * There are two usage modes:
 *  - project mode (if node is a SgProject)
 *      - view is sorted after include and source files
 *  - if node is no SgProject: all elements of subtree spanned by this SgNode
 *    are displayed
 *
 *  Filtering is also possible via the AstFilterInterface
 */
class QDESIGNER_WIDGET_EXPORT BAstView : public RoseTreeView
{
    Q_OBJECT

    public:
        BAstView(QWidget * parent=NULL);
        virtual ~BAstView();


    protected:
        void updateModel();
};

#endif
