#include "rose.h"
#include "RoseCodeEdit.h"

#include <QCodeEdit/qcodeedit.h>
#include <QCodeEdit/qformatscheme.h>
#include <QCodeEdit/qlanguagefactory.h>
#include <QCodeEdit/qlinemarksinfocenter.h>
#include <QCodeEdit/document/qdocumentline.h>
#include <QCodeEdit/widgets/qlinemarkpanel.h>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QAction>
#include <QDialogButtonBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QIcon>
#include <QAction>

#include <QDebug>

#include "SgNodeUtil.h"
#include "SageMimeData.h"

#include "qeditconfig.h"

QLanguageFactory * RoseCodeEdit::m_languages=NULL;

void RoseCodeEdit::init()
{

    QString qxsPath = ":/QCodeEdit/qxs";
    m_formats = new QFormatScheme(qxsPath + "/cpp.qxf", this);

    QDocument::setDefaultFormatScheme(m_formats);
    QLineMarksInfoCenter::instance()->loadMarkTypes(qxsPath + "/marks.qxm");

    if(!m_languages)
    {
        m_languages = new QLanguageFactory(m_formats, NULL);
        m_languages->addDefinitionPath(qxsPath);
    }

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);



    editorWrapper = new QCodeEdit(this ,this);

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

}

QList<int> RoseCodeEdit::getBreakPoints()
{
    QList<int> result;

    int bpId = QLineMarksInfoCenter::instance()->markTypeId("breakpoint");

    QDocument * d = document();

    int line=-1;
    while( (line = d->findNextMark(bpId,line+1)) != -1 )
        result.push_back(line);


    return result;
}


void RoseCodeEdit::enableBreakPointEdit(bool enable)
{
    QList<QPanel*> panels = editorWrapper->panels("Line marks");
    if(panels.isEmpty())
        return;

    QLineMarkPanel  * p = dynamic_cast<QLineMarkPanel*>(panels[0]);
    if(p)
        p->setDisableClicks(!enable);
}

void RoseCodeEdit::markAsError(int line)
{
    QLineMarksInfoCenter * lm = QLineMarksInfoCenter::instance();

    QDocumentLine l = document()->line(line);

    int markId = lm->markTypeId("error");
    if(! l.hasMark(markId))
        l.addMark(markId);

}

void RoseCodeEdit::markAsWarning(int line)
{
    QLineMarksInfoCenter * lm = QLineMarksInfoCenter::instance();

    QDocumentLine l = document()->line(line);

    int markId = lm->markTypeId("warning");
    if(! l.hasMark(markId))
        l.addMark(markId);
}

void RoseCodeEdit::loadCppFile(const QString & filename)
{
    // hack to always have C++ highlighting (problem: include files without ending)
    m_languages->setLanguage(this, filename + ".cpp");
    load(filename);
}

void RoseCodeEdit::gotoPosition(int row, int col)
{
    QDocumentCursor cursor(document(),row,col);
    setCursor(cursor);
}

void RoseCodeEdit::setNode(SgNode * node)
{

    if(node==NULL)
    {
        //remove contents
        load("");
    }

    if(isSgFile(node))
    {
        loadCppFile(node->get_file_info()->get_filenameString().c_str());
        return;
    }

    SgLocatedNode* sgLocNode = isSgLocatedNode(node);
    if(sgLocNode)
    {
        Sg_File_Info* fi = sgLocNode->get_startOfConstruct();

        loadCppFile(fi->get_filenameString().c_str());
        gotoPosition(fi->get_line(), fi->get_col());
    }

}


void RoseCodeEdit::showEditorSettingsDialog()
{
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
        ec->cancel();
}

QAction * RoseCodeEdit::getDisabledActions(const QString & name)
{
    static QAction * actUndo  = new QAction(QIcon(":/undo.png"),QString(),0);
    static QAction * actRedo  = new QAction(QIcon(":/redo.png"),QString(),0);

    static QAction * actCut   = new QAction(QIcon(":/cut.png"),QString(),0);
    static QAction * actCopy  = new QAction(QIcon(":/copy.png"),QString(),0);
    static QAction * actPaste = new QAction(QIcon(":/paste.png"),QString(),0);


    if(name=="undo")  return actUndo;
    if(name=="redo")  return actRedo;
    if(name=="cut")   return actCut;
    if(name=="copy")  return actCopy;
    if(name=="paste") return actPaste;

    return NULL;
}


// ---------------------- Drop Functionality -----------------------------------

void RoseCodeEdit::dragEnterEvent(QDragEnterEvent * ev)
{
    if (ev->mimeData()->hasFormat(SG_NODE_MIMETYPE))
    {
        if( this != ev->source())
        {
            if(getSourceNodes(ev->mimeData()).size() > 0 )
                ev->accept();
            else
                ev->ignore();
        }
    }
}


void RoseCodeEdit::dropEvent(QDropEvent *ev)
{
    if(ev->source()==this)
        return;

    SgNodeVector  nodes = getSourceNodes(ev->mimeData());
    if(nodes.size()==0)
        return;

    setNode(nodes[0]);
}
