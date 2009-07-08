
#ifndef METRICS_CONFIG_H
#define METRICS_CONFIG_H

#include <QObject>
#include <QMap>
#include <QDialog>

#include "MetricsInfo.h"

class SgNode;
//namespace RoseHPCT { class MetricAttr; }
class QListWidget;
class QListWidgetItem;

namespace Ui { class MetricsConfig; }

// implementation of global MetricsConfig
// not to be used directly
namespace impl {

    typedef QMap<QString, MetricsInfo> MetricsInfoContainer;

    // forward declaration of the global MetricsConfig Class
    class MetricsConfig;
    // forward declaration of custom iterator
    class MetricsInfoIterator;

} // end namespace impl


/**
 * Provides configuration dialog for ranges and normalization of metric attributes
 *
 * Declaration of Metrics Config Class for local configurations
 *  - uses \b always one instance of the global Configuration
 *  - en-/disable is the only thing atm which is unique to each local instance
 *    this is managed by the global configuration
 *
 * to create a new local config call the constructor with a unique ID\n
 *
 * this class acts as a proxy to the global configuration
 */
class MetricsConfig
    : public QObject
{
    Q_OBJECT

    public:
        typedef impl::MetricsInfoContainer MetricsInfoContainer;
        typedef impl::MetricsInfoIterator  iterator;

        // Constructor which register a unique Identification ID to the
        // static Global impl::MetricsConfig Object
        //
        // With this configureId the Global Object is able to distinguish several
        // configuration modules
        explicit MetricsConfig( const QString& configureId,
                                MetricsConfig *globalConfig,
                                SgNode *root = NULL );

        MetricsConfig( const MetricsConfig& o )
            : impl( o.impl ),
              configureId( o.configureId )
        {}

        // returns the selected Attribute
              int          getMetricsInfoCount() const;

              bool         hasMetricsInfo( const QString& name ) const;
        const MetricsInfo& getMetricsInfo( const QString& name ) const;
              MetricsInfo& getMetricsInfo( const QString& name );

        // iterator access:
        //   - bi directional iterators
        //   - only iterate over MetricInfo items which are enabled in the dialog
        //     ( all items are enabled by default )
        iterator begin() const;
        iterator end() const;

        // recollect metrics from root (can be subtree)
        //
        // changes are made to global config!
        void setRoot( SgNode *root );

    public slots:
        // create dialog window to configure, and select Metric Attributes
        // sets attributes in MetricsInfo to enabled/disabled
        void configureMultiple();
        void configureSingle();

    signals:
        void configChanged();

    private:
        /*explicit MetricsConfig( impl::MetricsConfig *global )
            : impl( global ),
              configureId( "" )
        {}*/
        impl::MetricsConfig *impl;
        QString configureId;
};

namespace impl {

    class MetricsConfig
        : public QObject
    {
        Q_OBJECT

        public:
            // ctor to collect all information about Metric Attributes
            MetricsConfig( SgNode *root = NULL );
            ~MetricsConfig();

            void registerId( const QString& configureId )
            {
                setupEnabled( configureId );
            }

            // returns the selected Attribute
            int getMetricsInfoCount          ( const QString& configureId = "" );

            bool               hasMetricsInfo( const QString& name, const QString& configureId = "" ) const;
            const MetricsInfo& getMetricsInfo( const QString& name, const QString& configureId = "" ) const;
            MetricsInfo& getMetricsInfo      ( const QString& name, const QString& configureId = "" );

            MetricsInfoIterator begin( const QString& configureId = "" );
            MetricsInfoIterator end  ( const QString& configureId = "" );

            //static MetricsConfig *getGlobal( SgNode *root = NULL );

            // recollect metrics from root (can be subtree)
            void setRoot( SgNode *root );

            // create dialog window to configure, and select Metric Attributes
            // sets attributes in MetricsInfo to enabled/disabled
            void configureMultiple( const QString& configureId = "" );
            const MetricsInfo& configureSingle  ( const QString& configureId = "" );

        private slots:
            void itemChanged( int );
            void minChanged( double );
            void maxChanged( double );
            void captionChanged( QListWidgetItem * );
            void normalizeNameChanged( int );
            void normalizeByRange( bool );
            void enableItem();
            void disableItem();

        private:
            typedef MetricsInfoContainer::iterator                map_iterator;

            void collectMetricAttributes( SgNode *astNode );

            void setupEnabled( const QString& configureId );

            void addLocalInfo( const QString& configureId, const QString& metricName );

            void execDialog( QDialog& dialog, const QString& configureId );

            MetricsInfoContainer                globalInfo;
            QMap<QString, QMap<QString, bool> > enabled;
            QString                             currentId;
            QString                             currentMetric;
            Ui::MetricsConfig                  *dialogUi;

            //static MetricsConfig *globalConfig;

            friend class MetricsInfoIterator;
    };

    class MetricsInfoIterator
    {
        public:
            MetricsInfoIterator( const MetricsInfoIterator& other );

                  MetricsInfoIterator& operator= ( const MetricsInfoIterator& other );
                  bool                 operator!=( const MetricsInfoIterator& other );
                  bool                 operator==( const MetricsInfoIterator& other );

                  MetricsInfoIterator  operator++( int );
                  MetricsInfoIterator  operator--( int );

                  MetricsInfoIterator& operator++( );
                  MetricsInfoIterator& operator--( );

                  MetricsInfo&         operator* ();
            const MetricsInfo&         operator* ()                                   const;
                  MetricsInfo*         operator->();
            const MetricsInfo*         operator->()                                   const;

            // conveninience Functions
            const QString&             name()                                         const;
                  QString              name();
            const MetricsInfo&         info()                                         const;
                  MetricsInfo&         info();

        private:
            typedef MetricsInfoContainer::iterator map_iterator;

            MetricsInfoIterator( MetricsConfig& config, const QString& configureId_ );

            map_iterator iter;

            MetricsInfoContainer& globalInfo;
            QMap<QString, bool>&  enabled;
            const QString&        configureId;

            friend class MetricsConfig;
    };

}

#endif
