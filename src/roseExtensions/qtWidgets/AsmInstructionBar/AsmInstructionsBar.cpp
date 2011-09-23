
#include "sage3basic.h"
#include "RoseBin_FlowAnalysis.h"
#include <QMouseEvent>
#include <QWheelEvent>
#include <QGraphicsRectItem>
#include <QGraphicsPolygonItem>
#include <QPoint>
#include <QPolygon>

#include <iostream>
#include <QDebug>

#include "ItemModelHelper.h"
#include "SgNodeUtil.h"
#include "SageMimeData.h"

#include "AsmInstructionsBar.h"

#include <QSizePolicy>

using namespace std;

AsmInstructionsBar::AsmInstructionsBar( QWidget *parent )
   : QGraphicsView( parent ),
     indicatorBottom( NULL ),
     indicatorMiddle( NULL ),
     indicatorTop( NULL ),
     base_width( 20 ),
     height( base_width * 15 ),
     pressed( false )
{
   setScene( &scene );
   //setDragMode( QGraphicsView::NoDrag );
   setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
   setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );

   setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
   setFixedHeight(base_width);

   setAcceptDrops( true );
   viewport()->setAcceptDrops( true );
}

AsmInstructionsBar::~AsmInstructionsBar()
{}

int AsmInstructionsBar::getNumberOfBlocks()
{
    return blockList.size();
}

void AsmInstructionsBar::setRoot( SgNode *root_ )
{
   cout << "setting up AsmInstructionsBar" << endl;

   if( isSourceNode( root_ ) ) return;

   qDeleteAll( blockList );
   blockList.clear();

   if( indicatorBottom )
   {
       delete indicatorBottom;
       indicatorBottom = NULL;
   }
   if( indicatorMiddle )
   {
       delete indicatorMiddle;
       indicatorMiddle = NULL;
   }
   if( indicatorTop )
   {
       delete indicatorTop;
       indicatorTop = NULL;
   }

   currentBlock = NULL;

   root = root_;

   if( !root ) return;

   typedef std::vector<SgAsmNode *>::const_iterator iterator;

   std::vector<SgAsmNode *> stmts;

   FindAsmStatementsHeaderVisitor visStat;
   AstQueryNamespace::querySubTree( root, std::bind2nd( visStat, &stmts ) );

   uint64_t offset( 0 );
   int id( 0 );
   for( iterator i = stmts.begin(); i != stmts.end(); ++i )
   {
       SgAsmNode *node( *i );

       uint64_t width( base_width );
       QPen   foreground( Qt::NoPen );
       QBrush background;

       if( isSgAsmBlock( node ) )
       {
           continue;
       }
       else if( isSgAsmFunction( node ) )
       {
           background = QBrush( QColor( 255, 216, 0 ) );
       }
       else if( isSgAsmInstruction( node ) )
       {
           SgAsmInstruction* instr( isSgAsmInstruction( node ) );
           background = QBrush( QColor( 128, 108, 0 ) );

           width *= instr->get_raw_bytes().size();
       }
       else if( isSgAsmElfSymbol( node ) )
       {
           background = QBrush( QColor( 0, 0, 255 ) );
       }
       else if( isSgAsmElfSection( node ) )
       {
           background = QBrush( QColor( 0, 132, 255 ) );
       }
       else if( isSgAsmElfSectionTableEntry( node ) )
       {
           background = QBrush( QColor( 0, 66, 128 ) );
       }
       else if( isSgAsmElfSegmentTableEntry( node ) )
       {
           background = QBrush( QColor( 64, 97, 128 ) );
       }
       else
       {
           // not interested in that node type ...
           continue;
       }

       blockList.push_back(
           scene.addRect( static_cast<float>( offset ), 0.0f,
                          static_cast<float>( width ) , height,
                          foreground, background )
           );
       blockList.back()->setData( SgNodeRole , QVariant::fromValue<SgNode *>( node ) );
       blockList.back()->setData( Qt::UserRole , id );

       offset += width;
       ++id;
   }

   currentBlock = blockList[0];

   if( indicatorMiddle )
        indicatorMiddle->scale( 0.1f, 0.1f );

    foreach( QGraphicsRectItem *block, blockList )
    {
        block->scale( 0.1f, 0.1f );
    }

   setIndicator();


   cout << "end setup AsmInstructionsBar" << endl;
}

void AsmInstructionsBar::setNode( SgNode *node )
{
    if( isSourceNode( node ) )
    {
        SgNodeVector binaryNodes( getLinkedBinaryNodes( node ) );
        SgNodeVector::iterator i( binaryNodes.begin() );
        for( ; i != binaryNodes.end(); ++i )
        {
            if( isAncestor( root, *i ) )
            {
                node = *i;
                break;
            }
        }
        if( i == binaryNodes.end() ) return;
    }

    int id( 0 );
    foreach( QGraphicsRectItem *block, blockList )
    {
        SgNode *tmp = qvariant_cast<SgNode *>( block->data( SgNodeRole ) );

        if( tmp == node )
        {
            currentBlock = block;
            setIndicator();

            emit nodeActivated( node );
            emit clicked( id );
            return;
        }
        ++id;
    }
}

void AsmInstructionsBar::setNode( int id )
{
    if( id < 0 || id >= blockList.size() )
        return;

    currentBlock = blockList[id];
    setIndicator();

    emit nodeActivated( qvariant_cast<SgNode *>( currentBlock->data( SgNodeRole ) ) );
    emit clicked( id );
}

void AsmInstructionsBar::setIndicator()
{
   const float currentWidth( currentBlock->rect().width() );
   const float currentX    ( currentBlock->rect().x() );

   if( indicatorBottom )
   {
       delete indicatorBottom;
       indicatorBottom = NULL;
   }
   if( indicatorMiddle )
   {
       delete indicatorMiddle;
       indicatorMiddle = NULL;
   }
   if( indicatorTop )
   {
       delete indicatorTop;
        indicatorTop = NULL;
   }

   QPolygonF Bottom;
   QPolygonF Top;

   Top    << QPointF( -5.0f, -10.0f )
          << QPointF(  5.0f, -10.0f )
          << QPointF(  0.0f,  0.0f  );

   Bottom << QPointF( -5.0f,   0.0f )
          << QPointF(  5.0f,   0.0f )
          << QPointF(  0.0f, -10.0f );

   indicatorBottom = scene.addPolygon( Bottom, QPen(), QBrush( QColor( 0, 0, 0 ) ) );

   QPen indPen( QBrush( Qt::gray ), 2.0f );
   indPen.setCosmetic( true );
   indicatorMiddle = scene.addRect   ( currentX, 0.0f,
                                       currentWidth, height,
                                       indPen,
                                       QBrush( QColor( 255, 255, 255, 150 ) ) );

   indicatorTop    = scene.addPolygon( Top   , QPen(), QBrush( QColor( 0, 0, 0 ) ) );


   QPointF pos( indicatorBottom->mapFromItem( currentBlock, QPointF( currentX, 0.0f ) ) );

   //indicatorBottom->setTransform( currentBlock->transform() );
   indicatorMiddle->setTransform( currentBlock->transform() );

   QPointF posLeft ( indicatorBottom->mapFromItem( indicatorMiddle, indicatorMiddle->rect().bottomLeft() ) );
   QPointF posRight( indicatorBottom->mapFromItem( indicatorMiddle, indicatorMiddle->rect().bottomRight() ) );
   QPointF top     ( indicatorBottom->mapFromItem( indicatorMiddle, indicatorMiddle->rect().topLeft() ) );

   float width( ( posRight.x() - posLeft.x() ) * 0.5 );
   float currentHeight( ( posLeft.y() - top.y() ) );

   QPointF center( posLeft.x() + width, currentHeight * 0.5 );

   indicatorBottom->setPos( center.x(), currentHeight +10.0f );//currentX + ( currentWidth * 0.5f ), height+10.0f );
   indicatorTop->setPos   ( center.x(), 0.0f );//trans   ( currentX + ( currentWidth * 0.5f ), 0.0f );
   centerOn( center );


   centerOn( center );
}

/*void AsmInstructionsBar::resizeEvent( QResizeEvent * ev)
{
   QGraphicsView::resizeEvent( ev );
}*/

void AsmInstructionsBar::mousePressEvent( QMouseEvent *e )
{
    switch( e->button() )
    {
        case Qt::LeftButton:
            updatePosition( e->pos() );
            pressed = true;
            break;
        default:
            break;
    }

}

void AsmInstructionsBar::mouseReleaseEvent( QMouseEvent *e )
{
    switch( e->button() )
    {
        case Qt::LeftButton:
            updatePosition( e->pos() );
            pressed = false;
            break;
        default:
            break;
    }

}
void AsmInstructionsBar::mouseMoveEvent( QMouseEvent *e )
{
    if( pressed )
    {
        updatePosition( e->pos() );
    }

    QGraphicsView::mouseMoveEvent( e );
}

void AsmInstructionsBar::updatePosition( const QPoint& pos )
{
    QGraphicsRectItem *clickedBlock;

    clickedBlock =
            dynamic_cast<QGraphicsRectItem *>( itemAt( pos ) );

    if( clickedBlock == NULL ) return;
    if( clickedBlock == indicatorMiddle ) return;

    currentBlock = clickedBlock;
    setIndicator();

    SgNode *node = qvariant_cast<SgNode *>( clickedBlock->data( SgNodeRole ) );
    int id = clickedBlock->data( Qt::UserRole ).toInt();

    emit nodeActivated( node );
    emit clicked( id );
}

void AsmInstructionsBar::wheelEvent( QWheelEvent *e )
{
    float scaleFactor( 1.0f );

    if( e->delta() > 0 )
    {
        // zoom in
        scaleFactor = 1.2f ;
    }
    else
    {
        // zoom out
        scaleFactor = 0.8f ;
    }

    if( indicatorMiddle )
        indicatorMiddle->scale( scaleFactor, 1.0f );

    foreach( QGraphicsRectItem *block, blockList )
    {
        block->scale( scaleFactor, 1.0f );
    }

    setIndicator();
}

void AsmInstructionsBar::dropEvent( QDropEvent *ev )
{
    if( ev->source()==this )
        return;

    SgNodeVector binNodes( getBinaryNodes( ev->mimeData() ) );

    if( binNodes.size() == 1 )
    {
        setRoot( binNodes[0] );
        return;
    }
}

void AsmInstructionsBar::dragEnterEvent( QDragEnterEvent *ev )
{
    if (ev->mimeData()->hasFormat(SG_NODE_BINARY_MIMETYPE))
    {
        if( this != ev->source())
                ev->accept();
    }
    else
        ev->ignore();
}

void AsmInstructionsBar::dragMoveEvent( QDragMoveEvent *ev )
{
    QWidget::dragMoveEvent( ev );
    /*QGraphicsView::dragMoveEvent( ev );

    if (ev->mimeData()->hasFormat(SG_NODE_MIMETYPE))
    {
        if( this != ev->source())
        {
            QByteArray d = ev->mimeData()->data(SG_NODE_MIMETYPE);
            QDataStream s (d);

            SgNode * node = 0;

            int bytesRead = s.readRawData((char*)&node,sizeof(SgNode*));
            Q_ASSERT( bytesRead == sizeof(SgNode*));
            Q_ASSERT( s.atEnd() );

            if( isBinaryNode( node ) )
                ev->accept();
            else
                ev->ignore();
        }
    }*/
}
