
#include "rose.h"

#include <limits>

#include <QDialog>
#include <QDebug>

#include "MetricsConfig.h"

#include "ui_MetricsConfig.h"

using namespace std;

// definition of the local proxy class
MetricsConfig::MetricsConfig( const QString& configureId_,
                              MetricsConfig *globalConfig,
                              SgNode *root )
    : /*impl( impl::MetricsConfig::getGlobal( root ) ),*/
      configureId( configureId_ )
{
    if( globalConfig == NULL )
    {
        impl = new impl::MetricsConfig( root );
        globalConfig = this;
    }
    else
    {
        impl = globalConfig->impl;
        impl->registerId( configureId );
    }
}


int MetricsConfig::getMetricsInfoCount()
{
    return impl->getMetricsInfoCount( configureId );
}

bool MetricsConfig::hasMetricsInfo( const QString& name ) const
{
    return impl->hasMetricsInfo( name, configureId );
}

const MetricsInfo& MetricsConfig::getMetricsInfo( const QString& name ) const
{
    return impl->getMetricsInfo( name, configureId );
}

MetricsInfo& MetricsConfig::getMetricsInfo( const QString& name )
{
    return impl->getMetricsInfo( name, configureId );
}

MetricsConfig::iterator MetricsConfig::begin() const
{
    return impl->begin( configureId );
}

MetricsConfig::iterator MetricsConfig::end() const
{
    return impl->end( configureId );
}

void MetricsConfig::setRoot( SgNode *root )
{
    assert( isSgProject( root ) );
    impl->setRoot( root );
}

void MetricsConfig::configureMultiple()
{
    impl->configureMultiple( configureId );

    emit configChanged();
}

void MetricsConfig::configureSingle()
{
    impl->configureSingle( configureId );
    emit configChanged();
}

namespace impl {

    MetricsConfig::MetricsConfig( SgNode *root )
        : currentId( "" )
    {
        setRoot( root );
    }

    MetricsConfig::~MetricsConfig()
    {
        delete dialogUi;
    }

    void MetricsConfig::setRoot( SgNode *root )
    {
        if( root == NULL ) return;
        enabled.clear();

        collectMetricAttributes( root );
        
        setupEnabled( "" );

        int listId( 0 );
        for( MetricsInfoIterator it = begin(); it != end(); ++it, ++listId )
        {
            it->listId = listId;
        }
    }

    void MetricsConfig::configureMultiple( const QString& configureId )
    {
        QDialog dialog;
        dialogUi = new Ui::MetricsConfig();
        dialogUi->setupUi( &dialog );

        currentId = configureId;

        dialogUi->frmSingle->hide();
        for( MetricsInfoIterator it = begin(); it != end(); ++it )
        {
            QListWidgetItem *metricItem = new QListWidgetItem( it->caption );
           
            metricItem->setFlags( Qt::ItemIsSelectable |
                                  Qt::ItemIsEditable |
                                  Qt::ItemIsDragEnabled |
                                  Qt::ItemIsEnabled );

            metricItem->setData( Qt::UserRole, QVariant( it.name() ) );

            if( enabled[configureId][it.name()] )
                dialogUi->lstEnabledMetrics->insertItem( it->listId, metricItem );
            else
                dialogUi->lstDisabledMetrics->addItem( metricItem );

            dialogUi->cmbNormalize->addItem( it->caption, QVariant( it.name() ) );
        }
        
        connect( dialogUi->lstEnabledMetrics , SIGNAL( currentRowChanged( int ) )        ,
                 this                        , SLOT  ( itemChanged( int ) ) );
        connect( dialogUi->lstDisabledMetrics, SIGNAL( currentRowChanged( int ) )        ,
                 this                        , SLOT  ( itemChanged( int ) ) );
        connect( dialogUi->spnMin            , SIGNAL( valueChanged( double ) )          ,
                 this                        , SLOT  ( minChanged( double ) ) );
        connect( dialogUi->spnMax            , SIGNAL( valueChanged( double ) )          ,
                 this                        , SLOT  ( maxChanged( double ) ) );
        connect( dialogUi->cmbNormalize      , SIGNAL( currentIndexChanged( int ) )      ,
                 this                        , SLOT  ( normalizeNameChanged( int ) ) );
        connect( dialogUi->lstEnabledMetrics , SIGNAL( itemChanged( QListWidgetItem * ) ),
                 this                        , SLOT( captionChanged( QListWidgetItem * ) ) );
        connect( dialogUi->lstDisabledMetrics, SIGNAL( itemChanged( QListWidgetItem * ) ),
                 this                        , SLOT( captionChanged( QListWidgetItem * ) ) );
        connect( dialogUi->rdoNormalizeRange , SIGNAL( toggled( bool ) )                 ,
                 this                        , SLOT  ( normalizeByRange( bool ) ) );
        connect( dialogUi->cmdEnable         , SIGNAL( clicked() )                       ,
                 this                        , SLOT  ( enableItem() ) );
        connect( dialogUi->cmdDisable        , SIGNAL( clicked() )                       ,
                 this                        , SLOT( disableItem() ) );

        execDialog( dialog, configureId );

        currentId = "";

        delete dialogUi;
    }

    const MetricsInfo& MetricsConfig::configureSingle( const QString& configureId )
    {
        QDialog dialog;
        dialogUi = new Ui::MetricsConfig();
        dialogUi->setupUi( &dialog );
        
        currentId = configureId;

        dialogUi->frmMultiple->hide();
        
        for( MetricsInfoIterator it = begin(); it != end(); ++it )
        {
            QListWidgetItem *metricItem = new QListWidgetItem( it->caption );

            metricItem->setFlags( Qt::ItemIsSelectable |
                                  Qt::ItemIsEditable |
                                  Qt::ItemIsDragEnabled |
                                  Qt::ItemIsEnabled );
            metricItem->setData( Qt::UserRole, QVariant( it.name() ) );    
            dialogUi->lstMetrics->insertItem( it->listId, metricItem );

            dialogUi->cmbNormalize->addItem( it->caption, QVariant( it.name() ) );
        }

        connect( dialogUi->lstMetrics       , SIGNAL( currentRowChanged( int ) ),
                 this                       , SLOT( itemChanged( int ) ) );
        connect( dialogUi->spnMin           , SIGNAL( valueChanged( double ) ),
                 this                       , SLOT( minChanged( double ) ) );
        connect( dialogUi->spnMax           , SIGNAL( valueChanged( double ) ),
                 this                       , SLOT( maxChanged( double ) ) );
        connect( dialogUi->cmbNormalize     , SIGNAL( currentIndexChanged( int ) ),
                 this                       , SLOT( normalizeNameChanged( int ) ) );
        connect( dialogUi->lstMetrics       , SIGNAL( itemChanged( QListWidgetItem * ) ),
                 this                       , SLOT( captionChanged( QListWidgetItem * ) ) );
        connect( dialogUi->rdoNormalizeRange, SIGNAL( toggled( bool ) ),
                 this                       , SLOT( normalizeByRange( bool ) ) );

        execDialog( dialog, configureId );
        MetricsInfoIterator itrBegin( begin( configureId ) );
        MetricsInfoIterator itrEnd( end( configureId ) );
        assert( itrBegin != itrEnd );
        /*const MetricsInfo& metricsInfo;
        if( begin != end )
            metricsInfo = *begin;*/

        currentId = "";

        delete dialogUi;

        return *itrBegin;
    }

    int MetricsConfig::getMetricsInfoCount( const QString& configureId )
    {
        if( configureId == "" )
            return globalInfo.size();

        int count( 0 );
        for( MetricsInfoIterator it( begin( configureId ) );
             it != end( configureId );
             ++it, ++count );

        return count;
    }

    bool MetricsConfig::hasMetricsInfo( const QString& name, const QString& configureId ) const
    {
        return globalInfo.find( name ) != globalInfo.end();
    }

    const MetricsInfo& MetricsConfig::getMetricsInfo( const QString& name, const QString& configureId ) const
    {
        map_iterator info( globalInfo.find( name ) );
        return *info;
    }

    MetricsInfo& MetricsConfig::getMetricsInfo( const QString& name, const QString& configureId )
    {
        map_iterator info( globalInfo.find( name ) );
        return *info;
    }

    MetricsInfoIterator MetricsConfig::begin( const QString& configureId )
    {
        MetricsInfoContainer::iterator iter( globalInfo.begin() );

        MetricsInfoIterator res( *this, configureId );
        res.iter = iter;
        

        if( iter == globalInfo.end() ) return res;

        if( !enabled[configureId][iter.key()] ) ++res;

        return res;
    }

    MetricsInfoIterator MetricsConfig::end( const QString& configureId )
    {
        MetricsInfoContainer::iterator iter( globalInfo.end() );
        

        MetricsInfoIterator res( *this, configureId );
        res.iter = iter;
        return res;
    }

    void MetricsConfig::collectMetricAttributes( SgNode *astNode )
    {
        if( astNode == NULL )
            return;
        if( astNode->get_attributeMechanism() != NULL)
        {
            AstAttributeMechanism *astAttributes = astNode->get_attributeMechanism();
            for( AstAttributeMechanism::iterator it = astAttributes->begin(); it != astAttributes->end(); ++it )
            {
                MetricAttribute *metrAttr = dynamic_cast<MetricAttribute *>( it->second );
                if( metrAttr )
                {
                    const QString name( it->first.c_str() );
                    const double value( metrAttr->getValue() );

                    //qDebug() << "MetricAttribute:" <<name;

                    map_iterator i = globalInfo.find( name );
                    if( i == globalInfo.end() )
                    {
                        globalInfo.insert( name, MetricsInfo( name, true, value, value ) );
                    }
                    else
                    {
                        if( value < i.value().minValue ) 
                        {
                            //cout << "New minimum: " << value << " old: " << i.value().minValue << endl;
                            i.value().minValue = value; // update min
                            i.value().minNormalize = value; // update min
                        }
                        if( value > i.value().maxValue ) 
                        {
                            //cout << "New maximum: " << value << i.value().maxValue <<        endl;
                            i.value().maxValue = value; // update max
                            i.value().maxNormalize = value; // update max
                        }
                    }
                }
            }
        }

        for( size_t i( 0 ); i < astNode->get_numberOfTraversalSuccessors(); ++i )
        {
            collectMetricAttributes( astNode->get_traversalSuccessorByIndex( i ) );
        }
    }

    void MetricsConfig::setupEnabled( const QString& configureId )
    {
        QMap<QString, QMap<QString, bool> >::iterator it( enabled.find( configureId ) );

        if( it != enabled.end() ) 
        {
            return;
        }

        QMap<QString, bool> tmp;
        for( map_iterator i = globalInfo.begin(); i != globalInfo.end(); ++i )
        {
            tmp.insert( i.key(), true );
        }
        enabled.insert( configureId, tmp );

        //QMap<QString, bool> test = enabled[""];
    }

    void MetricsConfig::execDialog( QDialog& dialog, const QString& configureId )
    {
        MetricsInfoContainer globalBak( globalInfo );
        QMap<QString, QMap<QString, bool> > enabledBak( enabled );

        if( dialog.exec() != QDialog::Accepted )
        {
            globalInfo = globalBak;
            enabled = enabledBak;
            return;
        }

        if( !dialogUi->frmSingle->isHidden() )
        {
            const QString name( dialogUi->lstMetrics->currentItem()->data( Qt::UserRole ).toString() );
            QMap<QString, bool>& isEnabled( enabled[configureId] );

            if( configureId != "" )
            {
                std::fill( isEnabled.begin(), isEnabled.end(), false );
                isEnabled[name] = true;
            }
        }

        for( int id( 0 ); id < dialogUi->lstEnabledMetrics->count(); ++id )
        {
            const QString name = dialogUi->lstEnabledMetrics->item( id )->data( Qt::UserRole ).toString();

            getMetricsInfo( name, configureId ).listId = id;
        }
    }

    // slots for the list widgets

    void MetricsConfig::itemChanged( int rowId )
    {
        QListWidget *list = dynamic_cast<QListWidget *>( sender() );
        if( list == NULL ) return;

        if( rowId == -1 ) return;

        /*const QString*/ currentMetric = list->item( rowId )->data( Qt::UserRole ).toString();

        const MetricsInfo& info( getMetricsInfo( currentMetric, currentId ) );

        switch( info.normalizeByRange )
        {
            case true:
                dialogUi->rdoNormalizeRange->setChecked( true );
                dialogUi->rdoNormalizeValue->setChecked( false );
                break;
            case false:
                dialogUi->rdoNormalizeRange->setChecked( false );
                dialogUi->rdoNormalizeValue->setChecked( true );
                break;
        }

        dialogUi->spnMin->setMinimum( -numeric_limits<double>::max() );//info.minValue );
        dialogUi->spnMin->setMaximum( info.maxNormalize-numeric_limits<double>::min() );
        dialogUi->spnMax->setMinimum( info.minNormalize+numeric_limits<double>::min() );
        dialogUi->spnMax->setMaximum( numeric_limits<double>::max() );//info.maxNormalize );

        dialogUi->spnMin->setValue( info.minNormalize );
        dialogUi->spnMax->setValue( info.maxNormalize );

        int normalizeId = dialogUi->cmbNormalize->findData( info.normalizeMetricName );
        if( normalizeId == -1 ) normalizeId = 0;
        dialogUi->cmbNormalize->setCurrentIndex( normalizeId );
    }

    void MetricsConfig::minChanged( double min )
    {
        getMetricsInfo( currentMetric, currentId ).minNormalize = min;
        dialogUi->spnMax->setMinimum( min + numeric_limits<double>::min() );
    }

    void MetricsConfig::maxChanged( double max )
    {
        getMetricsInfo( currentMetric, currentId ).maxNormalize = max;
        dialogUi->spnMin->setMaximum( max - numeric_limits<double>::min() );
    }

    void MetricsConfig::captionChanged( QListWidgetItem *item )
    {

        MetricsInfo& info( getMetricsInfo( currentMetric, currentId ) );

        const QString caption( item->data( Qt::DisplayRole ).toString() );

        int itemId = dialogUi->cmbNormalize->findText( info.caption );

        info.caption = caption;

        dialogUi->cmbNormalize->setItemText( itemId, caption );
    }

    void MetricsConfig::normalizeNameChanged( int itemId )
    {
        getMetricsInfo( currentMetric, currentId ).normalizeMetricName = dialogUi->cmbNormalize->itemData( itemId ).toString();
    }

    void MetricsConfig::normalizeByRange( bool b )
    {
        getMetricsInfo( currentMetric, currentId ).normalizeByRange = b;
    }

    void MetricsConfig::enableItem()
    {
        QList<QListWidgetItem *> selectedItems( dialogUi->lstDisabledMetrics->selectedItems() );

        foreach( QListWidgetItem *item, selectedItems )
        {
            const QString name( item->data( Qt::UserRole ).toString() );
            enabled[currentId][name] = true;
            dialogUi->lstEnabledMetrics->addItem( item->clone() );
            delete item;
        }
        
        if( dialogUi->lstEnabledMetrics->count() > 0 ) dialogUi->grpNormalize->setEnabled( true );
    }

    void MetricsConfig::disableItem()
    {
        QList<QListWidgetItem *> selectedItems( dialogUi->lstEnabledMetrics->selectedItems() );
        
        foreach( QListWidgetItem *item, selectedItems )
        {
            const QString name( item->data( Qt::UserRole ).toString() );
            enabled[currentId][name] = false;
            getMetricsInfo( name, currentId ).listId = -1;
            dialogUi->lstDisabledMetrics->addItem( item->clone() );
            delete item;
        }

        if( dialogUi->lstEnabledMetrics->count() == 0 ) dialogUi->grpNormalize->setEnabled( false );
    }

    //MetricsConfig *MetricsConfig::globalConfig = NULL;

    /*MetricsConfig *MetricsConfig::getGlobal( SgNode *root )
    {
        if( globalConfig == NULL )
            globalConfig = new MetricsConfig( root );

        return globalConfig;
    }*/

    MetricsInfoIterator::MetricsInfoIterator( const MetricsInfoIterator& other )
        : iter( other.iter ),
          globalInfo( other.globalInfo ),
          enabled( other.enabled ),
          configureId( other.configureId )
    {}

    MetricsInfoIterator& MetricsInfoIterator::operator= ( const MetricsInfoIterator& other )
    {
        iter = other.iter;
        globalInfo = other.globalInfo;
        enabled = other.enabled;
        //configureId = other.configureId;
        
        return *this;
    }

    bool MetricsInfoIterator::operator!=( const MetricsInfoIterator& other )
    {
        return iter != other.iter;
    }

    bool MetricsInfoIterator::operator==( const MetricsInfoIterator& other )
    {
        return iter == other.iter;
    }

    MetricsInfo& MetricsInfoIterator::operator* ()
    {
        return *iter;
    }

    const MetricsInfo& MetricsInfoIterator::operator* () const
    {
        return *iter;
    }

    MetricsInfo *MetricsInfoIterator::operator->()
    {
        return &(*iter);
    }

    const MetricsInfo *MetricsInfoIterator::operator->() const
    {

        return &(*iter);
    }

    MetricsInfoIterator MetricsInfoIterator::operator++( int )
    {
        MetricsInfoIterator tmp( *this );
        ++(*this);
        return tmp;
    }

    MetricsInfoIterator& MetricsInfoIterator::operator++()
    {
        ++iter;
        if( iter == globalInfo.end() || enabled[iter.key()] )
            return *this;
        else
        {
            return ++(*this);
        }
    }

    MetricsInfoIterator MetricsInfoIterator::operator--( int )
    {
        MetricsInfoIterator tmp( *this );
        --(*this);
        return tmp;
    }

    MetricsInfoIterator& MetricsInfoIterator::operator--()
    {
        --iter;
        if( iter == globalInfo.begin() || enabled[iter.key()] )
            return *this;
        else
            return --(*this);
    }

    const QString& MetricsInfoIterator:: name() const
    {
        return iter.key();
    }


    QString MetricsInfoIterator::name()
    {
        return iter.key();
    }

    const MetricsInfo& MetricsInfoIterator::info() const
    {
        return this->operator*();
    }

    MetricsInfo& MetricsInfoIterator::info()
    {
        return this->operator*();
    }

    MetricsInfoIterator::MetricsInfoIterator( impl::MetricsConfig& config, const QString& configureId_ )
        : globalInfo( config.globalInfo ),
          enabled( config.enabled[configureId_] ),
          configureId( configureId_ )
    {}

} // end namespace impl
