#include "TaskList.h"

#include <QDebug>


#include "Project.h"

// --------------------- Task List Item ------------------

TaskListItem::TaskListItem(Task * t, QListWidget * par)
    : task(t)
{
    Q_ASSERT(task);

    setText(task->getDescription());
    setIcon(QIcon(":/images/gears.gif"));

    par->addItem(this);
}

TaskListItem::~TaskListItem()
{
    delete task;
}



// ------------------------ Task List --------------------

TaskList::TaskList(QWidget * p)
    : QListWidget(p),
       lastClickedItem(NULL)
{
    connect(this, SIGNAL(itemClicked(QListWidgetItem*)),
            this, SLOT(on_itemClicked(QListWidgetItem*)));

}

void TaskList::submitTask(Task * t)
{
    TaskListItem * i = new TaskListItem(t,this);
    i->getTask()->start();
}


void TaskList::on_itemClicked(QListWidgetItem * item)
{
    QWidget *statusWidget = ProjectManager::instance()->taskOutputWidget();
    if(!statusWidget)
    {
        qWarning("TaskList: No task output window set");
        return;
    }

    if(lastClickedItem)
        lastClickedItem->getTask()->getOutputInfo()->hide(statusWidget);


    TaskListItem * curItem = dynamic_cast<TaskListItem*> (item);
    TaskOutputInfo *outInfo = curItem->getTask()->getOutputInfo();

    if(outInfo)
    {
        lastClickedItem=curItem;
        outInfo->showInWidget(statusWidget,this);
    }
    else
        lastClickedItem = NULL;
}




