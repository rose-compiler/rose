#include "rose.h"

#include <QFileInfo>
#include <QColorDialog>

#include "MetricsKiviat.h"
#include "MetricsConfig.h"

#include "Project.h"

#include "KiviatInfo.h"
#include "ui_KiviatInfo.h"

#include <cassert>

KiviatInfo::KiviatInfo( QWidget *parent, MetricsKiviat *metricsKiviat )
    : QWidget( parent ),
      ui( new Ui::KiviatInfo() )
{
    ui->setupUi( this );

    connect( ui->cmdChangeColor  , SIGNAL( clicked( bool ) )         , this, SLOT( changeColor() ) );
    connect( ui->cmdRemoveDataSet, SIGNAL( clicked( bool ) )         , this, SLOT( delNode() ) );
    connect( ui->lstDataSets     , SIGNAL( currentRowChanged( int ) ), this, SLOT( itemChanged( int ) ) );

    setKiviat( metricsKiviat );
}

KiviatInfo::~KiviatInfo()
{
    delete ui;
}

void KiviatInfo::setKiviat( MetricsKiviat *metricsKiviat )
{
    if( metricsKiviat == NULL ) return;

    metrics = metricsKiviat;

    connect( ui->cmdConfigMetrics, SIGNAL( clicked      ( bool ) )   , metrics, SLOT( configureMetrics() ) );
    connect( metrics             , SIGNAL( clicked      ( SgNode *) ), this   , SLOT( update ( SgNode *) ) );
    connect( metrics             , SIGNAL( nodeAdded    ( SgNode *) ), this   , SLOT( addNode( SgNode *) ) );
    //connect( metrics->metricsConfig, SIGNAL( configChanged() )         , this   , SLOT( configChanged() ) );

    for( MetricsKiviat::nodes_iterator node = metrics->nodesBegin();
         node != metrics->nodesEnd();
         ++node )
    {
        addNode( *node );
    }

    assert( metrics );
}

void KiviatInfo::update( SgNode *node )
{
    int id = metrics->getNodeId( node );

    if( id == -1 )
    {
        return;
    }

    QVector<float> kiviatAxis( metrics->getData( id ) );

    ui->treeDataSetInfo->clear();

    QList< QTreeWidgetItem *> items;

    //MetricsConfig globalConfig( "", ProjectManager::instance()->getMetricsConfig( node ) );
    /*MetricsConfig metricsConfig( "MetricsKiviat" );

    for( MetricsConfig::iterator it( metricsConfig.begin() );
         it != metricsConfig.end();
         ++it )
    {
        int i = it->listId;

        QTreeWidgetItem *newItem = new QTreeWidgetItem( (QTreeWidgetItem *)NULL, QStringList( it->caption ) << QString("%1").arg( kiviatAxis[i] ) );

        if( it->normalizeByRange )
        {
            new QTreeWidgetItem( newItem, QStringList( "Minimum:" ) << QString("%1").arg( it->minNormalize ) );
            new QTreeWidgetItem( newItem, QStringList( "Maximum:" ) << QString("%1").arg( it->maxNormalize ) );
        }
        else
        {
            new QTreeWidgetItem( newItem, QStringList( "Normalize by" ) << it->normalizeMetricName );//QString("%1").arg( info.minNormalize ) );
        }

        items.append( newItem );
    }

    ui->treeDataSetInfo->insertTopLevelItems( 0, items );

    ui->lstDataSets->setCurrentRow( id );*/
}

void KiviatInfo::addNode( SgNode *node )
{
    QString caption;
    SgLocatedNode *locNode = dynamic_cast<SgLocatedNode *>( node );
    if( locNode )
    {
        Sg_File_Info *fileInfo = locNode->get_file_info();
        caption = QString( "%1@%2:%3" ).arg( getSgVariant( node->variantT() ).c_str() )
                                       .arg( QFileInfo( fileInfo->get_filenameString().c_str() ).fileName() )
                                       .arg( fileInfo->get_line() );
    }
    else
    {
        caption = node->class_name().c_str();
    }
    ui->lstDataSets->addItem( caption );

    ui->lstDataSets->setCurrentRow( ui->lstDataSets->count()-1 );
}

void KiviatInfo::delNode()
{
    assert( metrics );

    if( ui->lstDataSets->count() )
    {
        int id = ui->lstDataSets->currentRow();
        if( id == -1 ) id = 0;

        metrics->delNode( id );
        delete ui->lstDataSets->item( id );

        ui->lstDataSets->setCurrentRow( 0 );
    }
}

void KiviatInfo::changeColor()
{
    assert( metrics );

    if( ui->lstDataSets->count() )
    {
        int id = ui->lstDataSets->currentRow();
        if( id == -1 ) id = 0;

        QColor oldColor = metrics->getDataColor( id );
        QColor newColor = QColorDialog::getColor( oldColor );

        metrics->setDataColor( id, newColor );
        metrics->configureMetrics( false );
    }
}

void KiviatInfo::itemChanged( int id )
{
    if( id != -1 )
        emit clicked( metrics->getNodeFromId( id ) );
}

void KiviatInfo::configChanged()
{
}
