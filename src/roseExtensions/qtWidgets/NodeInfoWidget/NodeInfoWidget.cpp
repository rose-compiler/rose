#include "sage3basic.h"

#include "NodeInfoWidget.h"
#include "SageMimeData.h"
#include "AsmToSourceMapper.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QApplication>

NodeInfoWidget::NodeInfoWidget(QWidget * par)
        : PropertyTreeWidget(par),curNode(NULL)
{
    setAcceptDrops(true);
    viewport()->setAcceptDrops(true);
}

NodeInfoWidget::~NodeInfoWidget()
{
}


void NodeInfoWidget::printDeclModifier(const QModelIndex & par, const SgDeclarationModifier & declMod)
{
        addEntry(par,"isUnknown",declMod.isUnknown());
        addEntry(par,"isDefault",declMod.isDefault());
        addEntry(par,"isFriend",declMod.isFriend());
        addEntry(par,"isTypedef",declMod.isTypedef());
        addEntry(par,"isExport",declMod.isExport());
        addEntry(par,"isThrow",declMod.isThrow());
}


void NodeInfoWidget::printFuncModifier(const QModelIndex & par,
                                                                                const SgFunctionModifier & funcMod)
{
        addEntry(par,"isUnknown",funcMod.isUnknown());
        addEntry(par,"isDefault",funcMod.isDefault());
        addEntry(par,"isInline",funcMod.isInline());
        addEntry(par,"isVirtual",funcMod.isVirtual());
        addEntry(par,"isPureVirtual",funcMod.isPureVirtual());
        addEntry(par,"isExplicit",funcMod.isExplicit());
        addEntry(par,"isPure",funcMod.isPure());
}

void NodeInfoWidget::printSpecialFuncModifier(const QModelIndex & par,
        const SgSpecialFunctionModifier& funcMod)
{
        addEntry(par,"isUnknown",funcMod.isUnknown());
        addEntry(par,"isDefault",funcMod.isDefault());
        addEntry(par,"isNotSpecial",funcMod.isNotSpecial());
        addEntry(par,"isConstructor",funcMod.isConstructor());
        addEntry(par,"isDestructor",funcMod.isDestructor());
        addEntry(par,"isConversion",funcMod.isConversion());
        addEntry(par,"isOperator",funcMod.isOperator());
}



QString NodeInfoWidget::getTraversalName(SgNode * node)
{
    if(isSgGraphNode(node))
        return("GraphNode - no parent");

    SgNode * par = node->get_parent();
    if(par)
    {
        //TODO is there a better way to get this info?
         unsigned int i;
         for(i=0; i<par->get_numberOfTraversalSuccessors(); i++)
             if(par->get_traversalSuccessorByIndex(i)==node)
                 break;

         if( i >= par->get_numberOfTraversalSuccessors())
             return QString(tr("Unknown"));

         return QString(par->get_traversalSuccessorNamesContainer()[i].c_str());
    }
    else
        return QString();
}


void NodeInfoWidget::setNode(SgNode * node)
{
    curNode=node;

        clear();
        if(node==NULL)
                return;



        int colorNr=0;

        int generalSection = addSection(tr("General Info"),colorNr++);
        SgNode * par=node->get_parent();
        if(par)
        {
        addEntryToSection(generalSection,"Class Name",node->class_name().c_str());
        addEntryToSection(generalSection,"Desc Name",getTraversalName(node));

            addEntryToSection(generalSection,"Parent Class Name",par->class_name().c_str());
        addEntryToSection(generalSection,"Parent Desc",getTraversalName(par));

        SgNode * parpar = par->get_parent();
        if(parpar)
        {
            addEntryToSection(generalSection,"Grandparent Class Name",parpar->class_name().c_str());
            addEntryToSection(generalSection,"Grandparent Desc",getTraversalName(parpar));
        }
        }


        // Located Node
        SgLocatedNode* sgLocNode = isSgLocatedNode(node);
        if(sgLocNode)
        {
                int locNodeSec= addSection(tr("Located Node"), colorNr);

                Sg_File_Info* fi = sgLocNode->get_file_info();

                QModelIndex idx = addEntryToSection(locNodeSec,"File Information","");

                //Filename
                addEntry(idx,"File",QString(fi->get_filenameString().c_str()));
                //Line
                addEntry(idx,"Line",fi->get_line());
                //Column
                addEntry(idx,"Column",fi->get_col());
                //FileId
                addEntry(idx,"FileId",fi->get_file_id());


                //Transformation
                addEntryToSection(locNodeSec,"IsTransformation",fi->isTransformation());
                //Output in CodeGen
                addEntryToSection(locNodeSec,"IsOutputInCodeGeneration",fi->isOutputInCodeGeneration());


                // Declaration Statement
                SgDeclarationStatement * sgDeclNode =isSgDeclarationStatement(node);
                if(sgDeclNode)
                        addEntryToSection(locNodeSec,"Decl Mangled Name",QString(sgDeclNode->get_mangled_name().getString().c_str()));


                // Expression Type
                SgExpression * sgExprNode = isSgExpression(node);
                if(sgExprNode)
                        addEntryToSection(locNodeSec,"Expression Type",QString(sgExprNode->get_type()->unparseToString().c_str()));
        }
        colorNr++;


        //RTI information from SgNode
    RTIReturnType rti=node->roseRTI();
        int rtiNodeSec= addSection(tr("RTI from SgNode"),colorNr);

    for(RTIReturnType::iterator i=rti.begin(); i<rti.end(); i++)
    {
                if (strlen(i->type) >= 7 &&
                strncmp(i->type, "static ", 7) == 0)
                {
                        continue; // Skip static members
                }

                QString propName (i->name);
                QString propValue(i->value.c_str());


                //  Declaration Modifier Special Treatment
                if(propName=="p_declarationModifier")
                {
                        SgDeclarationStatement * declStatement = isSgDeclarationStatement(node);
                        if(declStatement)
                        {
                                QModelIndex idx = addEntryToSection(rtiNodeSec,"Declaration Modifier","");
                                printDeclModifier(idx,declStatement->get_declarationModifier());
                        }
                        continue;
                }

                if(propName=="p_functionModifier")
                {
                        SgFunctionDeclaration * funcDecl = isSgFunctionDeclaration(node);
                        if(funcDecl)
                        {
                                QModelIndex idx = addEntryToSection(rtiNodeSec,tr("Function Modifier"),"");
                                printFuncModifier(idx,funcDecl->get_functionModifier());
                        }
                        continue;
                }

                if(propName=="p_specialFunctionModifier")
                {
                        SgFunctionDeclaration * funcDecl = isSgFunctionDeclaration(node);
                        if(funcDecl)
                        {
                                QModelIndex idx = addEntryToSection(rtiNodeSec,tr("Special Function Modifier"),"");
                                printSpecialFuncModifier(idx,funcDecl->get_specialFunctionModifier());
                        }
                        continue;
                }
                if( propName == "p_address" )
                {
                    rose_addr_t address( 0 );
                    SgAsmStatement *asmStatement( isSgAsmStatement( node ) );
                    if( asmStatement )
                    {
                        address = asmStatement->get_address();
                    }
                    SgAsmDwarfLine *dwarfLine( isSgAsmDwarfLine( node ) );
                    if( dwarfLine )
                    {
                        address = dwarfLine->get_address();
                    }

                    if( address == 0 )
                    {
                        addEntryToSection(rtiNodeSec, tr("Adress"), propValue);
                    }
                    else
                    {
                        addEntryToSection( rtiNodeSec, tr("Address"),
                                QString("0x%1").arg(address,0,16) );
                    }
                }

                addEntryToSection(rtiNodeSec,propName,propValue);
    }
    colorNr++;

    // Attributes
    if (node->get_attributeMechanism() != NULL)
    {
        int attrNodeSec = addSection(tr("Attributes"),colorNr);

                AstAttributeMechanism::AttributeIdentifiers aidents = node->get_attributeMechanism()->getAttributeIdentifiers();

                QModelIndex metricNodeSection;
                QModelIndex nodeLinkSection;

                for (AstAttributeMechanism::AttributeIdentifiers::iterator it = aidents.begin(); it != aidents.end(); ++it )
                {
                        AstAttribute * attr = node->getAttribute(*it);
                        MetricAttribute * metricAttr = dynamic_cast<MetricAttribute * >(attr);
                        AstNodeLinkAttribute *nodeLink( dynamic_cast<AstNodeLinkAttribute *>( attr ) );
                        if(metricAttr)
                        {
                                if(! metricNodeSection.isValid())
                                        metricNodeSection = addEntryToSection(attrNodeSec,tr("Metric Attributes"),"");

                                addEntry(metricNodeSection,(*it).c_str(), metricAttr->getValue() );
                        }
                        else if( nodeLink )
                        {
                            typedef std::vector<std::pair<SgNode *, SgNode *> >::const_iterator iterator;

                            if( !nodeLinkSection.isValid() )
                                nodeLinkSection = addEntryToSection( attrNodeSec, tr("AST Links"), "" );
                            
                            for( iterator jt( nodeLink->begin() ); jt != nodeLink->end(); ++jt )
                            {
                                QModelIndex fileIdx;
                                fileIdx = ( addEntry( nodeLinkSection, tr("Filename"), it->c_str() ) );
                                SgNode *start( jt->first );
                                SgNode *end( jt->second );

                                if( isSgFile( node ) ) continue;

                                if( dynamic_cast<AstBinaryNodeLink *>( nodeLink ) )
                                {
                                    SgAsmInstruction *startInstr( isSgAsmInstruction( start ) );
                                    SgAsmInstruction *endInstr( isSgAsmInstruction( end ) );
                                    addEntry( fileIdx, "SgNode (Start)", QString("0x%1").arg( uint64_t(start), 0, 16 ) );
                                    addEntry( fileIdx, "SgNode (Stop)", QString("0x%1").arg( uint64_t(end), 0, 16 ) );
                                    if( startInstr )
                                        addEntry( fileIdx, "Start", QString("0x%1").arg( startInstr->get_address(), 0, 16 ) );
                                    else
                                    {
                                        SgAsmFunctionDeclaration *asmFun( isSgAsmFunctionDeclaration( start ) );
                                        addEntry( fileIdx, "Function", QString( asmFun->get_name().c_str() ) );
                                    }
                                    if( endInstr )
                                        addEntry( fileIdx, "End", QString("0x%1").arg( endInstr->get_address(), 0, 16 ) );
                                }
                                else // if( dynamic_cast<AstSourceNodeLink *>( nodeLink ) )
                                {
                                    Sg_File_Info *fileInfo( start->get_file_info() );
                                    addEntry( fileIdx, "SgNode", QString("0x%1").arg( uint64_t(start), 0, 16 ) );
                                    addEntry( fileIdx, "Class Name", start->class_name().c_str() );
                                    addEntry( fileIdx, "Line", fileInfo->get_line() );
                                }
                            }
                        }
                        else
                        {
                                QString name (it->c_str());
                                QString attributeValue (node->getAttribute(*it)->toString().c_str());
                                addEntryToSection(attrNodeSec,name,attributeValue);
                        }
                }
                //expand(metricNodeSection);
                expandAll();
    }
    colorNr++;

}




// ------------------------- Drag and Drop Functions --------------------------


void NodeInfoWidget::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() == Qt::LeftButton)
        dragStartPosition = ev->pos();

    QTreeView::mousePressEvent(ev);
}

void NodeInfoWidget::mouseMoveEvent(QMouseEvent *ev)
{
    if (!(ev->buttons() & Qt::LeftButton))
        return QTreeView::mouseMoveEvent(ev);;
    if ((ev->pos() - dragStartPosition).manhattanLength() < QApplication::startDragDistance())
        return QTreeView::mouseMoveEvent(ev);;

    // write node into into QByteArray
    QByteArray d;
    QDataStream s(&d, QIODevice::Unbuffered | QIODevice::ReadWrite);
    s.writeRawData((const char*) & curNode, sizeof(SgNode*));


    // create drag object, set pixmap and data
    QDrag *drag = new QDrag(viewport());
    QMimeData *mimeData = new QMimeData();

    mimeData->setData(SG_NODE_MIMETYPE, d);
    drag->setMimeData(mimeData);
    drag->exec();


    QTreeView::mouseMoveEvent(ev);
}

void NodeInfoWidget::dragEnterEvent(QDragEnterEvent * ev)
{
    if (ev->mimeData()->hasFormat(SG_NODE_MIMETYPE))
    {
        if(this != ev->source())
            ev->acceptProposedAction();
    }
}

void NodeInfoWidget::dragMoveEvent(QDragMoveEvent * ev)
{
    QWidget::dragMoveEvent(ev);
}


void NodeInfoWidget::dropEvent(QDropEvent *ev)
{
    if(ev->source()==this)
        return;

    QByteArray d = ev->mimeData()->data(SG_NODE_MIMETYPE);
    QDataStream s (d);

    while(! s.atEnd())
    {
        SgNode * node = 0;

        int bytesRead = s.readRawData((char*)&node,sizeof(SgNode*));
        Q_ASSERT(bytesRead == sizeof(SgNode*));

        setNode(node);
    }
    ev->acceptProposedAction();
}

