
#include "rose.h"

#include "MetricsConfig.h"
#include "AstFilters.h"

#include "MetricFilter.h"

#include "ui_MetricFilter.h"

MetricFilter::MetricFilter( QWidget *parent, MetricsConfig *globalConfig )
    : QWidget( parent ),
      metricFilterUi( new Ui::MetricFilter() ),
      //metricsConfig( new MetricsConfig( "MetricFilter", globalConfig ) ),
      currentName( "" ),
      currentThreshold( 0 )
{
    metricFilterUi->setupUi( this );

    connect( metricFilterUi->cmbMetric          , SIGNAL( currentIndexChanged( int ) ), this, SLOT( itemChanged( int ) ) );
    connect( metricFilterUi->sldThreshold       , SIGNAL( valueChanged( int ) )       , this, SLOT( thresholdChanged( int ) ) );
    //connect( metricsConfig                      , SIGNAL( configChanged() )           , this, SLOT( updateMetrics() ) );

    updateMetrics();
}

MetricFilter::~MetricFilter()
{
    delete metricFilterUi;
    //delete metricsConfig;
}

void MetricFilter::itemChanged( int id )
{
    if( id == 0 )
    {
        metricFilterUi->frmThreshold->setDisabled( true );
    }
    else
    {
        metricFilterUi->frmThreshold->setEnabled( true );
    }

    currentName = metricFilterUi->cmbMetric->itemData( id ).toString();
    currentId = id;

    filterChanged();
}

void MetricFilter::thresholdChanged( int threshold )
{
    currentThreshold = threshold;

    filterChanged();
}

void MetricFilter::updateMetrics()
{
    /*for( MetricsConfig::iterator i = metricsConfig->begin();
         i != metricsConfig->end();
         ++i )
    {
        int id = metricFilterUi->cmbMetric->findData( i.name() );
        if( id == -1 )
            metricFilterUi->cmbMetric->insertItem( i->listId+1, i->caption, QVariant( i.name() ) );
        else
            metricFilterUi->cmbMetric->setItemText( id, i->caption );
    }*/
}

void MetricFilter::filterChanged()
{
    if( metricFilterUi->cmbMetric->currentText() == "<None>" )
    {
        AstFilterAll all;
        emit filterChanged( &all );
        return;
    }

    //MetricsInfo tmp( metricsConfig->getMetricsInfo( currentName ) );

    //AstFilterMetricAttributeByThreshold filter( currentName, tmp, currentThreshold );
    //emit filterChanged( &filter );
}
