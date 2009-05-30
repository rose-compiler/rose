#include "rose.h"

#include "CompassMainWindow.h"
#include "ui_CompassMainWindow.h"

#include "compassViolation.h"
#include "compassInterface.h"


#include "CheckerTableModel.h"
#include "ViolationsTableModel.h"

#include <QDebug>
#include <QProgressDialog>

#include <QMenu>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHeaderView>

#include <qlinemarksinfocenter.h>
//#include <qeditconfig.h>



CompassMainWindow::CompassMainWindow(CompassInterface * ci, QWidget * par)
    : QMainWindow(par),
      compInterface(ci),
      checkerTableModel(NULL),
      checkerFilterModel(NULL),
      violationsTableModel(NULL),
      violationsFilterModel(NULL)
{
    Q_ASSERT(compInterface);

    ui = new Ui::CompassMainWindow();
    ui->setupUi(this);

    updateCheckersTable();

    //Clear Entries in Help Widget
    ui->lblCheckerName->setText("");
    ui->lblCheckerShortDesc->setText("");
    ui->txtCheckerLongDesc->setPlainText("");


    editorWrapper = new QCodeEdit(ui->codeEdit ,this);

    editorWrapper
        ->addPanel("Line Mark Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F6"));

    editorWrapper
        ->addPanel("Line Number Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F11"));

    editorWrapper
        ->addPanel("Fold Panel", QCodeEdit::West, true)
        ->setShortcut(QKeySequence("F9"));

    //editorWrapper
    //    ->addPanel("Line Change Panel", QCodeEdit::West, true);

    editorWrapper
        ->addPanel("Status Panel", QCodeEdit::South, true);

    editorWrapper
        ->addPanel("Search Replace Panel", QCodeEdit::South);

    ui->editorToolbar->addAction(ui->codeEdit->action("undo"));
    ui->editorToolbar->addAction(ui->codeEdit->action("redo"));
    ui->editorToolbar->addSeparator();
    ui->editorToolbar->addAction(ui->codeEdit->action("cut"));
    ui->editorToolbar->addAction(ui->codeEdit->action("copy"));
    ui->editorToolbar->addAction(ui->codeEdit->action("paste"));

    ui->menuEdit->addAction(ui->codeEdit->action("undo"));
    ui->menuEdit->addAction(ui->codeEdit->action("redo"));
    ui->menuEdit->addSeparator();
    ui->menuEdit->addAction(ui->codeEdit->action("cut"));
    ui->menuEdit->addAction(ui->codeEdit->action("copy"));
    ui->menuEdit->addAction(ui->codeEdit->action("paste"));

    on_actionSelectAll_triggered();
}

CompassMainWindow::~CompassMainWindow()
{
    delete ui;
}



void CompassMainWindow::on_actionSelectAll_triggered()
{
    Q_ASSERT(checkerTableModel);
    for(int i=0; i<checkerTableModel->rowCount(); i++ )
    {
        QModelIndex ind = checkerTableModel->index(i,0);
        checkerTableModel->setData(ind,Qt::Checked,Qt::CheckStateRole);
    }
}

void CompassMainWindow::on_actionUnselectAll_triggered()
{
    Q_ASSERT(checkerTableModel);
    for(int i=0; i<checkerTableModel->rowCount(); i++ )
    {
        QModelIndex ind = checkerTableModel->index(i,0);
        checkerTableModel->setData(ind,Qt::Unchecked,Qt::CheckStateRole);
    }
}

void CompassMainWindow::on_actionInvertSelection_triggered()
{
    Q_ASSERT(checkerTableModel);
    for(int i=0; i<checkerTableModel->rowCount(); i++ )
    {
        QModelIndex ind = checkerTableModel->index(i,0);
        QVariant cur = checkerTableModel->data(ind,Qt::CheckStateRole);
        QVariant newState = (cur==Qt::Checked) ? Qt::Unchecked : Qt::Checked;
        checkerTableModel->setData(ind,newState,Qt::CheckStateRole);
    }
}


void CompassMainWindow::on_tblCheckers_customContextMenuRequested (const QPoint & p)
{
    QList<QAction*> actions;
    actions << ui->actionSelectAll << ui->actionUnselectAll << ui->actionInvertSelection;

    QMenu::exec(actions,p,NULL);

}




void CompassMainWindow::on_actionEditorSettings_triggered()
{
    //TODO problem with include of qeditconfig.h (ui file etc..)
    /*
    QDialog settingsDlg;
    QEditConfig * ec = new QEditConfig(&settingsDlg);

    QDialogButtonBox * buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok
                                                      | QDialogButtonBox::Cancel,
                                                      Qt::Horizontal,
                                                      &settingsDlg);

    connect(buttonBox, SIGNAL(accepted()), &settingsDlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &settingsDlg, SLOT(reject()));

    QVBoxLayout * layout = new QVBoxLayout(&settingsDlg);
    layout->addWidget(ec);
    layout->addWidget(buttonBox);

    int res = settingsDlg.exec();
    if( res == QDialog::Accepted)
        ec->apply();
    else
    ec->cancel();*/

}

void CompassMainWindow::on_actionReset_triggered()
{
    compInterface->reset();
    updateCheckersTable();
}

void CompassMainWindow::on_actionRunTests_triggered()
{
    CompassCheckers_v checkers = compInterface->getCompassCheckers();

    compInterface->getResult()->reset();

    QProgressDialog dlg(tr("Running Compass checkers..."),"Cancel",0,checkers.size(),this);
    dlg.setWindowModality(Qt::WindowModal);
    dlg.show();

    int row=0;
    for( CompassCheckers_v::iterator itr = checkers.begin(); itr != checkers.end(); ++itr,++row )
    {
        dlg.setValue(row);
        if(dlg.wasCanceled())
            break;

        dlg.setLabelText(QString("Running %1").arg( (*itr)->getName().c_str()));

        bool enable =  compInterface->getCompassChecker(row)->isEnabled();
        (*itr)->process(enable,*compInterface);
    }

    updateCheckersTable();
}




void CompassMainWindow::on_actionSave_triggered()
{
    ui->codeEdit->save();
}

void CompassMainWindow::on_actionSaveAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                               "", tr("C++ files (*.cpp *.C *.h)"));

    ui->codeEdit->save(fileName);
}






void CompassMainWindow::updateCheckersTable()
{
    if(checkerTableModel)
        delete checkerTableModel;

    if(checkerFilterModel)
        delete checkerFilterModel;

    checkerTableModel = new CheckerTableModel(compInterface,this);
    checkerFilterModel = new CheckerFilterModel(this);

    checkerFilterModel->setSourceModel(checkerTableModel);

    connect(ui->chkShowPassed,  SIGNAL(toggled(bool)),
            checkerFilterModel, SLOT(showPassedTests(bool)));
    connect(ui->chkShowFailed,  SIGNAL(toggled(bool)),
            checkerFilterModel, SLOT(showFailedTests(bool)));
    connect(ui->chkShowWithViolations,  SIGNAL(toggled(bool)),
            checkerFilterModel, SLOT(showTestsWithViolations(bool)));
    connect(ui->chkShowUnselected,  SIGNAL(toggled(bool)),
            checkerFilterModel, SLOT(showUnselectedTests(bool)));


    connect(ui->txtFilter,SIGNAL(textChanged(const QString&)),
            checkerFilterModel,SLOT(setFilterWildcard(const QString &)));

    ui->tblCheckers->setModel(checkerFilterModel);
    ui->tblCheckers->horizontalHeader()->resizeSection(1,0);
    ui->tblCheckers->horizontalHeader()->resizeSection(2,0);
    ui->tblCheckers->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);
}

void CompassMainWindow::on_tblCheckers_clicked(const QModelIndex & proxyInd)
{
    QModelIndex ind = checkerFilterModel->mapToSource(proxyInd);

    if(violationsTableModel)   delete violationsTableModel;
    if(violationsFilterModel)  delete violationsFilterModel;

    const CompassChecker * checker = checkerTableModel->getCompassChecker(ind);

    Q_ASSERT(checker);

    //Update Violations
    violationsTableModel  = new ViolationsTableModel(checker,this);
    violationsFilterModel = new QSortFilterProxyModel(this);

    violationsFilterModel->setSourceModel(violationsTableModel);

    connect(ui->txtVioFileFilter,SIGNAL(textChanged(const QString&)),
            violationsFilterModel,SLOT(setFilterWildcard(const QString &)));


    ui->tblViolations->setModel(violationsFilterModel);


    ui->tblViolations->horizontalHeader()->resizeSection(0,0);
    ui->tblViolations->horizontalHeader()->resizeSection(1,0);
    ui->tblViolations->horizontalHeader()->resizeSection(2,0);
    ui->tblViolations->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);


    //Update Help
    ui->lblCheckerName->setText(checker->getName().c_str());
    ui->lblCheckerShortDesc->setText(checker->getShortDescription().c_str());
    ui->txtCheckerLongDesc->setPlainText(checker->getLongDescription().c_str());
}

void CompassMainWindow::on_tblViolations_clicked(const QModelIndex & proxyInd)
{
    QModelIndex ind = violationsFilterModel->mapToSource(proxyInd);

    const CompassViolation * curViolation = violationsTableModel->getViolation(ind);

    //may happen when instead of violation the error msg of tester is displayed
    if(curViolation==NULL)
        return;


    QString filename (curViolation->getFilename().c_str());


    QLineMarksInfoCenter * lm = QLineMarksInfoCenter::instance();
    //lm->clear();

    const CompassChecker * checker = violationsTableModel->getCurChecker();


    if(filename == "compilerGenerated")
        return;


    ui->codeEdit->loadCppFile(filename);
    ui->codeEdit->gotoPosition(curViolation->getLineStart()-1,curViolation->getColStart());

    for(int i=0; i< checker->getNumViolations(); i++)
    {
        const CompassViolation * vio = checker->getViolation(i);


        if(vio->getFilename().c_str() != filename)
            continue;

        for(int line= vio->getLineStart(); line <= vio->getLineEnd(); line++)
            ui->codeEdit->markAsWarning(line-1);


        if( vio->getLineEnd() < vio->getLineStart())
            ui->codeEdit->markAsWarning(vio->getLineStart() -1);

    }

    /*    QLineMarksInfoCenter * lm = QLineMarksInfoCenter::instance();
    QLineMark mark("",0,lm->markTypeId("warning"));
    for(int i=0; i< checker->getNumViolations(); i++)
    {
        const CompassViolation * vio = checker->getViolation(i);


        for(int line= vio->getLineStart(); line <= vio->getLineEnd(); line++)
        {
            mark.file = vio->getFilename().c_str();
            mark.line=line;
            lm->addLineMark(mark);
            //ui->codeEdit->markAsWarning(line-1);
        }

        if( vio->getLineEnd() < vio->getLineStart())
        {
            mark.file = vio->getFilename().c_str();
            mark.line=vio->getLineStart();
            lm->addLineMark(mark);
            //ui->codeEdit->markAsWarning(vio->getLineStart() -1);
        }

    }*/

}



