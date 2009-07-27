

#ifndef ROSECODEEDIT_H
#define ROSECODEEDIT_H

#include "QCodeEditWidget.h"

class SgNode;


class QDESIGNER_WIDGET_EXPORT RoseCodeEdit : public QCodeEditWidget
{
    public:
        RoseCodeEdit(QWidget *p = 0)                   : QCodeEditWidget(p)         {}
        RoseCodeEdit(bool actions, QWidget *p = 0)     : QCodeEditWidget(actions,p) {}
        RoseCodeEdit(const QString& s, QWidget *p = 0) : QCodeEditWidget(s,p)       {}
        RoseCodeEdit(const QString& s, bool actions, QWidget *p = 0)                {}

        virtual ~RoseCodeEdit() {}
    public slots:
        /// If the specified sgNode is a SgLocatedNode, the right file is opened
        /// and the cursor is set to start position of this SgNode
        /// If node is SgFile the file is opened and cursor is set to beginning of file
        void setNode(SgNode * sgNode);
        // same as setNode, for convenience with the link mechanism
        void gotoNode( SgNode *sgNode )
        { setNode( sgNode ); }


    protected:
        virtual void dragEnterEvent(QDragEnterEvent * ev);
        virtual void dropEvent(QDropEvent * ev);


};



#endif
