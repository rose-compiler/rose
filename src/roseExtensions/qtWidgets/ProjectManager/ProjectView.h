
#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include "RoseTreeView.h"

class SgNode;
class SgSourceFile;
class SgBinaryFile;

class ItemTreeModel;
class ItemTreeNode;
class ProjectManagerRootNode;
class TaskList;

class QMouseEvent;

class ProjectView : public RoseTreeView
{
    Q_OBJECT
    public:
        ProjectView( QWidget * par = NULL );
        virtual ~ProjectView();

        /// Sets widget where the task should be displayed
        void setTaskListWidget(TaskList * l);
    protected:

        virtual void mousePressEvent(QMouseEvent *ev);

        virtual void updateModel();

        void displayContextMenu(const QPoint & pos, ItemTreeNode * node);

        ProjectManagerRootNode * pmRootNode;
};

#endif
