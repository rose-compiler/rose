#include "rose.h"

#include <cmath>

#include <QListWidget>
#include <QGraphicsRectItem>
#include <QGraphicsTextItem>
#include <QMouseEvent>
#include <QFileInfo>
#include <QAction>
#include <QMenu>

#include <QDebug>

#include "MetricsKiviat.h"
#include "MetricsConfig.h"

#include "SageMimeData.h"


using namespace std;

MetricsKiviat::MetricsKiviat( QWidget *parent, MetricsConfig *global )
   : KiviatView( parent, 0 )/*,
     metricsConfig( new MetricsConfig( "MetricsKiviat", global ) )*/
{
    connect( this, SIGNAL( clickedOnData( int ) ), SLOT( setActiveItem( int ) ) );

    connect( this, SIGNAL( clickedOnData( int ) ), SLOT( updateView( int ) ) );

    //connect( metricsConfig, SIGNAL( configChanged() ), this, SLOT( configChanged() ) );

    legend = scene.addRect( QRect()/*, QPen( Qt::NoPen )*/ );
    //legend->setBrush( QBrush( QColor( 255, 255, 255, 180 ) ) );
    //legend->setZValue( 100 );
}

MetricsKiviat::~MetricsKiviat()
{
    delete legend;
    //delete metricsConfig;
    //qDeleteAll( legendText );
}

void MetricsKiviat::init( SgNode *root )
{
    currentNode = root;

    //setAxisCount( metricsConfig->getMetricsInfoCount() );

    addData( QVector<float>(axisCount, 0.0f ) );

    //configureMetrics( false );
}

/*
void MetricsKiviat::setRoot( SgNode *root )
{
    currentNode = root;
    collector.traverse( root, 0 );

    MetricAttributeCollector::const_iterator it( collector.begin() );

    for(int index( 0 ); it != collector.end(); ++it, ++index )
    {
        //attributes.push_back( info( it->first, true, it->second.first, it->second.second ) );
        //attributes[index].kiviatID = index;
    }
    //setAxisCount( attributes.size() );

    addData( QVector<float>(axisCount, 0.0f ) );

    configureMetrics( false );
}*/


void MetricsKiviat::updateView( int id )
{
    if( id >= 0 && id < nodes.size() )
        updateView( nodes[id] );
}

void MetricsKiviat::updateView( SgNode *astNode )
{
    if( astNode == currentNode || astNode == NULL )
        return;

    drawData( astNode );

    currentNode = astNode;
}

void MetricsKiviat::addNode( SgNode *astNode )
{
    if( nodes.indexOf( astNode ) != -1 || astNode == NULL )
        return;

    int id( nodes.size() );
    nodes.push_back( astNode );

    int h = ( id * 35 ) % 360;
    setDataColor( id, QColor::fromHsv( h, 255, 255 ) );
    addData( QVector<float>( axisCount, 0.0f ) );

    drawData( astNode );

    emit nodeAdded( astNode );

    currentNode = astNode;
}

void MetricsKiviat::delNode( SgNode *astNode )
{
    int idx( nodes.indexOf( astNode ) );

    if( idx == -1 ) return;

    delNode( idx );
}

void MetricsKiviat::delNode( int nodeId )
{
    removeData( nodeId );
    nodes.remove( nodeId );

    drawData( currentNode );
}

void MetricsKiviat::configureMetrics( bool dialog )
{
    if( dialog )
    {
        //MetricsConfig configDiag( attributes );

        //if( configDiag.exec() != QDialog::Accepted ) return;

        //attributes = QVector<info>( configDiag.attributes() );

        /*for( int i = 0; i < configDiag.count(); ++i )
        {
            //int idx = configDiag.getAttributeIndex( i );
            //attributes[idx].kiviatID = i;
        }*/

        //metricsConfig->configureMultiple();

        //setAxisCount( metricsConfig->getMetricsInfoCount() );

        qDebug() << axisCount;
    }

    QString maxLabel( "" );
    /*for( MetricsConfig::iterator it( metricsConfig->begin() );
         it != metricsConfig->end();
         ++it )
    {
        setAxisLabel( it->listId, it->caption );
    }*/
    resizeEvent( NULL );

    drawData( currentNode );
}

void MetricsKiviat::resizeEvent( QResizeEvent *ev )
{
    KiviatView::resizeEvent( ev );
}

void MetricsKiviat::mousePressEvent( QMouseEvent *ev )
{
   KiviatView::mousePressEvent( ev );

   QGraphicsTextItem * item = dynamic_cast<QGraphicsTextItem*> ( itemAt( ev->pos() ) );

   if(item==NULL)
      return;

   for( int i = 1; i < legendText.size(); ++i )
   {
       if( legendText[i] == item )
       {
           if( i-1 < nodes.size() )
           {
               drawData( nodes[i-1] );
           }
           else
               drawData( currentNode );
           break;
       }
   }
}

void MetricsKiviat::drawData( SgNode *astNode )
{
    int nodesId( nodes.indexOf( astNode ) );
    int id( nodesId );
    QString maxLabel( "" );

    if( id == -1 )
    {
        id = nodes.size();
        setDataColor( id, Qt::blue );
    }

    setActiveItem( id );

#if 0
    for( MetricsConfig::iterator it( metricsConfig->begin() );
         it != metricsConfig->end();
         ++it )
    {
        int axisId = it->listId;
        const QString& metricName( it.name() );

        setDataPoint( nodes.size(), axisId, 0.0f );

        if( it->caption.length() > maxLabel.length() )
            maxLabel = it->caption;

        double newValue = it->eval( astNode, metricName );

        setDataPoint( id, axisId, (float)newValue );
        setToolTip( id, axisId, QString( "Relative Value: %1\n").arg( newValue ) +
                                //QString( "Absolute Value: %1\n").arg( value ) +
                                QString( "Minimum Value : %1\n").arg( it->minValue ) +
                                QString( "Maximum Value : %1").arg( it->maxValue ) );
    }

    qDeleteAll( legendText );
    legendText.clear();
    qDeleteAll( legendColor );
    legendColor.clear();

    QGraphicsTextItem *text;
    QFont highlightFont( labelFont );
    highlightFont.setBold( true );
    highlightFont.setItalic( true );

    int height( labelFont.pointSize() + labelFont.pointSize()*0.8 );
    if( nodesId == -1 )
    {
        height = height * (nodes.size() + 1 );
    }
    else
    {
        height = height * nodes.size();
    }

    height += 2 * labelFont.pointSize();

    QFontMetrics metric( labelFont );

    const QPointF newPos( AXIS_LENGTH + metric.width( maxLabel ) + 25.0f,
                          -height/2 );
    legend->setPos( newPos );

    QFont legendFont( labelFont );
    legendFont.setPointSize( labelFont.pointSize() + 2 );
    legendFont.setUnderline( true );
    //legendFont.setBold( true );
    text = new QGraphicsTextItem( QString( "Legend:" ), legend );
    //text->setPos( 0.0f, -( labelFont.pointSize() * 2 ) );
    text->setFont( legendFont );

    legendText.push_back( text );

    qreal legendWidth( 0 );

    int nodeId( 0 );
    foreach( SgNode *node, nodes )
    {
        SgLocatedNode *locNode = dynamic_cast<SgLocatedNode *>( node );
        if( locNode )
        {
            Sg_File_Info *fileInfo = locNode->get_file_info();

            text = new QGraphicsTextItem( QString( "%1@%2:%3" ).arg( getSgVariant( node->variantT() ).c_str() )
                                                               .arg( QFileInfo( fileInfo->get_filenameString().c_str() ).fileName() )
                                                               .arg( fileInfo->get_line() ), legend );
        }
        else
            text = new QGraphicsTextItem( node->class_name().c_str(), legend );

        text->setPos( 40.0f, nodeId * ( labelFont.pointSize() + labelFont.pointSize()*0.8 ) + 3 * legendFont.pointSize());

        if( nodeId == id )
        {
            //text->setDefaultTextColor( Qt::gray );
            text->setFont( highlightFont );
        }
        else
            text->setFont( labelFont );

        if( text->boundingRect().width() > legendWidth )
            legendWidth = text->boundingRect().width();

        legendText.push_back( text );

        QGraphicsRectItem *rect = new QGraphicsRectItem( QRectF( 0.0f, 0.0f, 25.0f, labelFont.pointSize() ), legend );
        rect->setPos( 10.0f, nodeId * ( labelFont.pointSize() + labelFont.pointSize()* 0.8 ) + 3.7 * legendFont.pointSize() );
        rect->setPen( QPen( Qt::NoPen ) );
        rect->setBrush( getDataColor( nodeId ) );
        legendColor.push_back( rect );

        nodeId++;

    }
    if( nodesId == -1 )
    {
        SgLocatedNode *locNode = isSgLocatedNode( astNode );
        if( locNode )
        {
            Sg_File_Info *fileInfo = locNode->get_file_info();

            text = new QGraphicsTextItem( QString( "%1@%2:%3" ).arg( getSgVariant( astNode->variantT() ).c_str() )
                                                               .arg( QFileInfo( fileInfo->get_filenameString().c_str() ).fileName() )
                                                               .arg( fileInfo->get_line() ), legend );
        }
        else
            text = new QGraphicsTextItem( astNode->class_name().c_str(), legend );

        text->setFont( highlightFont );

        text->setPos( 40.0f, nodeId * ( labelFont.pointSize() + labelFont.pointSize()*0.8 ) + 3 * legendFont.pointSize() );
        //text->setDefaultTextColor( Qt::gray );

        if( text->boundingRect().width() > legendWidth )
            legendWidth = text->boundingRect().width();

        QGraphicsRectItem *rect = new QGraphicsRectItem( QRectF( 0.0f, 0.0f, 25.0f, labelFont.pointSize() ), legend );
        rect->setPos( 10.0f, nodeId * ( labelFont.pointSize() + labelFont.pointSize()* 0.8 ) + 3.7 * legendFont.pointSize() );
        rect->setPen( QPen( Qt::NoPen ) );
        rect->setBrush( Qt::blue );
        legendColor.push_back( rect );

        legendText.push_back( text );

        nodeId++;
    }

    legend->setRect( QRectF( 0.0f, 0.0f, legendWidth + 60, nodeId * ( labelFont.pointSize() + labelFont.pointSize()* 0.8 ) + 3.7 * legendFont.pointSize() ) );

    scene.setSceneRect( QRectF() );

    resizeEvent( NULL );
    emit clicked( astNode );
#endif
}

void MetricsKiviat::contextMenuEvent( QContextMenuEvent *ev, const QPoint &pos )
{
    QList<QAction *> actions;

    QObject *parent( new QObject() );
    QAction *configMetrics( new QAction( "configure Metrics" , parent ) );

    actions << configMetrics;

    QAction *res( QMenu::exec( actions, mapToGlobal( pos ) ) );

    if( res == configMetrics )
    {
        configureMetrics();
    }
    else
    {
        qDebug() << "Unknown action ...";
    }
}

void MetricsKiviat::dropEvent( QDropEvent *ev )
{
    if( ev->source()==this )
        return;

    SgNode *node( getGeneralNode( ev->mimeData() ) );

    addNode( node );
}

void MetricsKiviat::dragEnterEvent( QDragEnterEvent *ev )
{
    if (ev->mimeData()->hasFormat(SG_NODE_MIMETYPE))
    {
        if( this != ev->source())
                ev->accept();
    }
    else
        ev->ignore();
}

void MetricsKiviat::dragMoveEvent( QDragMoveEvent *ev )
{
    QWidget::dragMoveEvent( ev );
}
