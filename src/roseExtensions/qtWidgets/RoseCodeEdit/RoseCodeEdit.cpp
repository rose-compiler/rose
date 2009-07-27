#include "rose.h"

#include "RoseCodeEdit.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QIcon>
#include <QAction>

#include "SgNodeUtil.h"
#include "SageMimeData.h"

#include <QDebug>



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
