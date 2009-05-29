#include "rose.h"
#include "RoseCodeEdit.h"

#include <qformatscheme.h>
#include <qlanguagefactory.h>
#include <qlinemarksinfocenter.h>
#include <qdocumentline.h>

#include <QDebug>


QLanguageFactory * RoseCodeEdit::m_languages=NULL;

void RoseCodeEdit::init()
{

    QString qxsPath = ":/QCodeEdit/qxs";
    m_formats = new QFormatScheme(qxsPath + "/cpp.qxf", this);

    QDocument::setDefaultFormatScheme(m_formats);
    QLineMarksInfoCenter::instance()->loadMarkTypes(qxsPath + "/marks.qxm");

    if(!m_languages)
    {
        m_languages = new QLanguageFactory(m_formats, this);
        m_languages->addDefinitionPath(qxsPath);
    }

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

    SgLocatedNode* sgLocNode = isSgLocatedNode(node);
    if(sgLocNode)
    {
        Sg_File_Info* fi = sgLocNode->get_startOfConstruct();

        loadCppFile(fi->get_filenameString().c_str());
        gotoPosition(fi->get_line(), fi->get_col());
    }
}
