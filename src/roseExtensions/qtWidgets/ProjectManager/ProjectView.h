
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

/**
 * \brief Display of ProjectManager Status
 *
 *  \image html ProjectView.jpg
 *
 *  - Shows the state of the ProjectManager, user can add/remove files to project, edit command line options etc.
 *  - BAstNodes integrated to show an outline of the loaded source files
 */
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
