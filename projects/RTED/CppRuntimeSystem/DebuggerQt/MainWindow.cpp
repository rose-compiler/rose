#include <QSettings>
#include <QFileDialog>
#include <QDebug>
#include <QSortFilterProxyModel>
#include <QDialogButtonBox>
#include <QModelIndex>

#include <QListWidgetItem>

#include "MainWindow.h"
#include "qcodeedit.h"

#include "RtedDebug.h"

#include "ui_MainWindow.h"

#include "TypeInfoDisplay.h"
#include "MemoryTypeDisplay.h"
#include "VariablesTypeDisplay.h"
#include "PointerDisplay.h"

#include "ItemTreeModel.h"

#include "ModelRoles.h"


DbgMainWindow::DbgMainWindow(RtedDebug * dbgObj_,
                             QWidget * par)
    : QMainWindow(par),
      dbgObj(dbgObj_),
      singleStep(true),
      typeModel(new ItemTreeModel(this)),
      typeProxyModel(NULL),
      memModel(new ItemTreeModel(this)),
      memProxyModel(NULL),
      stackModel(new ItemTreeModel(this)),
      stackProxyModel(NULL),
      pointerModel(new ItemTreeModel(this)),
      pointerProxyModel(NULL)
{
    ui = new Ui::MainWindow();
    ui->setupUi(this);

    rs = RuntimeSystem::instance();

    ui->editorToolbar->addAction(ui->codeEdit1->action("undo"));
    ui->editorToolbar->addAction(ui->codeEdit1->action("redo"));
    ui->editorToolbar->addSeparator();
    ui->editorToolbar->addAction(ui->codeEdit1->action("cut"));
    ui->editorToolbar->addAction(ui->codeEdit1->action("copy"));
    ui->editorToolbar->addAction(ui->codeEdit1->action("paste"));

    ui->menuEdit->addAction(ui->codeEdit1->action("undo"));
    ui->menuEdit->addAction(ui->codeEdit1->action("redo"));
    ui->menuEdit->addSeparator();
    ui->menuEdit->addAction(ui->codeEdit1->action("cut"));
    ui->menuEdit->addAction(ui->codeEdit1->action("copy"));
    ui->menuEdit->addAction(ui->codeEdit1->action("paste"));

    ui->codeEdit1->enableBreakPointEdit();
    ui->codeEdit2->enableBreakPointEdit();


    rs = RuntimeSystem::instance();

    //restore settings
    QSettings settings;
    settings.beginGroup("WindowState");

    QByteArray d = settings.value("mainwindow").toByteArray();
    if(d.size() > 0)
    {
      restoreState(d);
      qDebug() << "MainWindow settings restored";
    }

    settings.endGroup();
}


DbgMainWindow::~DbgMainWindow()
{
    delete ui;
}


void DbgMainWindow::closeEvent(QCloseEvent * ev)
{
    QSettings settings;
    settings.beginGroup("WindowState");
    settings.setValue("mainwindow",saveState());
    settings.endGroup();

    qDebug() << "MainWindow settings stored";
}


void DbgMainWindow::addMessage(const QString &  msg)
{
    new QListWidgetItem(QIcon(":/AppIcons/info.png"),msg,ui->lstMessages);
    ui->lstMessages->setCurrentRow(ui->lstMessages->count()-1);
}

void DbgMainWindow::addErrorMessage(const QString &  msg)
{
    new QListWidgetItem(QIcon(":/icons/application-exit.png"),msg,ui->lstMessages);
    ui->lstMessages->setCurrentRow(ui->lstMessages->count()-1);
}


void DbgMainWindow::on_actionSave_triggered()
{
    ui->codeEdit1->save();
}

void DbgMainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "", tr("C++ files (*.cpp *.C *.h)"));

    ui->codeEdit1->save(fileName);
}

void DbgMainWindow::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                               "", tr("C++ files (*.cpp *.C *.h)"));
    ui->codeEdit1->loadCppFile(fileName);
}

void DbgMainWindow::on_actionEditorSettings_triggered()
{
    QCodeEditWidget::showEditorSettingsDialog();
}



void DbgMainWindow::on_actionSingleStep_triggered()
{
  singleStep=true;
  breakPoints1[file1] = ui->codeEdit1->getBreakPoints();
  breakPoints2[file2] = ui->codeEdit2->getBreakPoints();

    dbgObj->startRtsi();
}

void DbgMainWindow::on_actionResume_triggered()
{
    singleStep=false;
    breakPoints1[file1] = ui->codeEdit1->getBreakPoints();
    breakPoints2[file2] = ui->codeEdit2->getBreakPoints();

    std::cout << "Saved Breakpoints:" << std::endl;
    foreach(int bp, breakPoints1[file1])
        std::cout << bp << " ";

    qDebug() << "bp2";
    foreach(int bp, breakPoints2[file2])
        std::cout << bp << " ";


    dbgObj->startRtsi();
}





void DbgMainWindow::on_chkShowStack_toggled()
{
    updateMemoryDisplay();
}

void DbgMainWindow::on_chkShowHeap_toggled()
{
    updateMemoryDisplay();
}




void DbgMainWindow::updateAllRsData(bool showAlways)
{

    file1 = rs->getCodePosition().getFile().c_str();
    file2 = rs->getCodePosition().getTransformedFile().c_str();


    int row1 = rs->getCodePosition().getLineInOrigFile();
    int row2 = rs->getCodePosition().getLineInTransformedFile();

    // skip stepping over transformed code
    std::string filename1 = file1.toStdString();
    //std::cout << "++++++++++++ file1 :" << filename1 << std::endl;
    QString text = QString("Looking at: ");
    if (file1=="0") {
      singleStep=false;
      text.append(" program transformation. Skipping line ... ");
      text.append(QString("%1").arg(row2));
    } else {
      singleStep=true;
      text.append(file1+" : ");
      text.append(QString("%1,").arg(row1));
      text.append(QString("%1").arg(row2));
    }
    addMessage(text);

    if(!singleStep && !showAlways &&
       !breakPoints1[file1].contains(row1) &&
       !breakPoints2[file2].contains(row2))
    {
        dbgObj->startRtsi();
        return;
    }


    ui->codeEdit1->loadCppFile(file1);
    ui->codeEdit2->loadCppFile(file2);

    foreach(int bp, breakPoints1[file1])
        ui->codeEdit1->markAsBreakpoint(bp);

    foreach(int bp, breakPoints2[file2])
        ui->codeEdit2->markAsBreakpoint(bp);


    ui->codeEdit1->gotoPosition(row1,0);
    ui->codeEdit2->gotoPosition(row2,0);

    ui->codeEdit1->markAsWarning(row1);
    ui->codeEdit2->markAsWarning(row2);



    updateTypeDisplay();
    updateMemoryDisplay();
    updateStackDisplay();
    updatePointerDisplay();

}
void DbgMainWindow::updateTypeDisplay()
{
    ItemTreeNode * typeRoot = RsTypeDisplay::build(rs->getTypeSystem());
    typeModel->setRoot(typeRoot);

    if(typeProxyModel)
        delete typeProxyModel;

    typeProxyModel = new QSortFilterProxyModel(this);
    connect(ui->txtTypeSystemFilter,SIGNAL(textChanged(const QString&)),
            typeProxyModel, SLOT(setFilterWildcard(const QString&)));

    typeProxyModel->setSourceModel(typeModel);

    ui->treeTypeSystem->setModel(typeProxyModel);
}


void DbgMainWindow::on_treeMemorySystem_clicked(const QModelIndex & ind)
{
    MemoryType * mt = qvariant_cast<MemoryType*>( ind.model()->data(ind,MemoryTypeRole));
    ui->memGraphicsView->setMemoryType(*mt);
}


void DbgMainWindow::updateMemoryDisplay()
{
    bool showHeap = ui->chkShowHeap->isChecked();
    bool showStack = ui->chkShowStack->isChecked();

    ItemTreeNode * memRoot = MemoryTypeDisplay::build(rs->getMemManager(),showHeap,showStack);
    memModel->setRoot(memRoot);

    if(memProxyModel)
        delete memProxyModel;

    memProxyModel = new QSortFilterProxyModel(this);
    connect(ui->txtMemFilter,SIGNAL(textChanged(const QString&)),
            memProxyModel, SLOT(setFilterWildcard(const QString&)));

    memProxyModel->setSourceModel(memModel);

    ui->treeMemorySystem->setModel(memProxyModel);


    // GraphicsView
    MemoryManager * mm = RuntimeSystem::instance()->getMemManager();
    if (!mm->getAllocationSet().empty())
    {
        ui->memGraphicsView->setMemoryType(mm->getAllocationSet().begin()->second);
    }
}

void DbgMainWindow::updateStackDisplay()
{
    ItemTreeNode * stackRoot = VariablesTypeDisplay::build(rs->getStackManager());
    stackModel->setRoot(stackRoot);

    if(stackProxyModel)
        delete stackProxyModel;

    stackProxyModel = new QSortFilterProxyModel(this);
    connect(ui->txtStackFilter,SIGNAL(textChanged(const QString&)),
            stackProxyModel, SLOT(setFilterWildcard(const QString&)));

    stackProxyModel->setSourceModel(stackModel);

    ui->treeStack->setModel(stackProxyModel);
}


void DbgMainWindow::updatePointerDisplay()
{
    ItemTreeNode * pointerRoot = PointerDisplay::build(rs->getPointerManager());
    pointerModel->setRoot(pointerRoot);

    if(pointerProxyModel)
        delete pointerProxyModel;

    pointerProxyModel = new QSortFilterProxyModel(this);
    connect(ui->txtPointerFilter,SIGNAL(textChanged(const QString&)),
            pointerProxyModel, SLOT(setFilterWildcard(const QString&)));

    pointerProxyModel->setSourceModel(pointerModel);

    ui->treePointer->setModel(pointerProxyModel);
}
