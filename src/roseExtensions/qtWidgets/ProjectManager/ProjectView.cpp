#include "rose.h"
#include "ProjectView.h"

#include "Project.h"

#include "ItemTreeModel.h"

#include <QMouseEvent>

#include <QMenu>
#include <QFileDialog>
#include <QDebug>


ProjectView::ProjectView(QWidget * par)
    : RoseTreeView(par),
      pmRootNode(NULL)
{
    ProjectManager * pm = ProjectManager::instance();
    pmRootNode = pm->getModelRootNode();

    // here setFixedRoot is called, because pmRootNode should
    // not be deleted by the model
    model->setFixedRoot(pmRootNode);

    expandAll();

    setSgDropEnabled(false);
}

ProjectView::~ProjectView()
{
}

void ProjectView::updateModel()
{

}

void ProjectView::mousePressEvent(QMouseEvent *ev)
{
    RoseTreeView::mousePressEvent(ev);

    if(ev->button()== Qt::RightButton)
    {
        QModelIndex clickedInd = indexAt(ev->pos());
        ItemTreeNode * itemTreeNode = static_cast<ItemTreeNode*>(clickedInd.internalPointer () );

        if(!itemTreeNode)
            return;

        displayContextMenu(mapToGlobal(ev->pos()),itemTreeNode);
    }
}

void ProjectView::displayContextMenu(const QPoint & pos, ItemTreeNode * node)
{
    ProjectNode *    project = dynamic_cast<ProjectNode*> (node);
    SourceFileNode * source  = dynamic_cast<SourceFileNode*> (node);
    BinaryFileNode * binary  = dynamic_cast<BinaryFileNode*> (node);
    BinaryFileHeaderNode * binaryHeader = dynamic_cast<BinaryFileHeaderNode*>(node);
    SourceFileHeaderNode * sourceHeader = dynamic_cast<SourceFileHeaderNode*>(node);

    QAction * actCmdLineDlg = 0;
    QAction * actAddFile    = 0; // add a source or binary file to project, src or bin
    QAction * actAddBinFile = 0; // add a binary file to project ( via BinaryFileHeaderNode)
    QAction * actAddSrcFile = 0; // add a source file to project ( via SourceFileHeaderNode)
    QAction * actRemoveSrc  = 0; // removes source file
    QAction * actRemoveBin  = 0; // removes binary file
    QAction * actRebuildSrc = 0; // rebuild source file
    QAction * actRebuildBin = 0; // rebuild binary file


    // ------ buildup menu ------------
    QMenu * menu = new QMenu(this);
    if(project)
    {
        actAddFile = new QAction(tr("Add File"), menu);
        actCmdLineDlg = new QAction(tr("Edit ROSE CommandLine"),menu);
        menu->addAction(actAddFile);
        menu->addAction(actCmdLineDlg);
    }
    else if(source)
    {
        actRemoveSrc  = new QAction(tr("Remove"),menu);
        actRebuildSrc = new QAction(tr("Rebuild"),menu);
        menu->addAction(actRemoveSrc);
        menu->addAction(actRebuildSrc);
    }
    else if(binary)
    {
        actRemoveBin  = new QAction(tr("Remove"),menu);
        actRebuildBin = new QAction(tr("Rebuild"),menu);
        menu->addAction(actRemoveBin);
        menu->addAction(actRebuildBin);
    }
    else if (binaryHeader)
    {
        actAddBinFile = new QAction(tr("Add binary file"),menu);
        menu->addAction(actAddBinFile);
    }
    else if (sourceHeader)
    {
        actAddSrcFile = new QAction(tr("Add source file"),menu);
        menu->addAction(actAddSrcFile);
    }

    if(menu->actions().isEmpty())
        return;

    QAction * res = menu->exec(pos);
    if(!res)
        return;

    // ----- evaluate response ---------
    if(res == actAddFile)
    {
        //Add file
        QString filter(QObject::tr("Source Files (*.c *.cpp *.C *.f );;Binary Files (*)"));
        QString fileName = QFileDialog::getOpenFileName(NULL, QObject::tr("Add File"), QString(), filter);

        if(! fileName.isEmpty())
            project->addFile(fileName);
    }
    else if (res == actAddSrcFile)
    {
        QString filter(QObject::tr("Source Files (*.c *.cpp *.C *.f )"));
        QString fileName = QFileDialog::getOpenFileName(NULL, QObject::tr("Add Source File"), QString(), filter);

        ProjectNode * p = dynamic_cast<ProjectNode*>(sourceHeader->getParent());
        if(! fileName.isEmpty())
            p->addFile(fileName);

    }
    else if (res == actAddBinFile)
    {
        QString filter(QObject::tr("Binary Files (*)"));
        QString fileName = QFileDialog::getOpenFileName(NULL, QObject::tr("Add Binary File"), QString(), filter);

        ProjectNode * p = dynamic_cast<ProjectNode*>(binaryHeader->getParent());
        if(! fileName.isEmpty())
            p->addFile(fileName);
    }
    else if (res == actRemoveBin)
        binary->getParent()->removeChild(binary->getRow());
    else if (res == actRemoveSrc)
        source->getParent()->removeChild(source->getRow());
    else if (res == actRebuildBin)
        binary->rebuild();
    else if (res == actRebuildSrc)
        source->rebuild();
    else if ( res == actCmdLineDlg)
        project->showCmdLineDialog();

    delete menu;
}



void ProjectView::setTaskListWidget(TaskList * l)
{
}
