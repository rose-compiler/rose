#ifndef METRICBGDELEGATE_H
#define METRICBGDELEGATE_H

#include <QItemDelegate>
#include <QColor>
#include "MetricsConfig.h"
#include "MetricsInfo.h"

class SgProject;

/**
 * This delegate paints the background of an itemview
 * according to a metric attribute (value is color-coded)
 */
class MetricBgDelegate : public QItemDelegate
{
    Q_OBJECT

    public:
        MetricBgDelegate( QObject * parent =0,
                          const QString& configureId = "MetricsBgDelegate",
                          MetricsConfig *globalConfig = NULL );

        void setupConfigWidget( QWidget *parent =0 );
        void applyConfigWidget( QWidget *parent =0 );

        const QString& configureId() const
        { return configId; }

        const MetricsInfo& metricsInfo() const
        { return metricsInf; }
        
        MetricsInfo& metricsInfo()
        { return metricsInf; }

        QItemDelegate *copy()
        { return new MetricBgDelegate( *this ); }

    protected:
        virtual void paint ( QPainter * painter,
                             const QStyleOptionViewItem & option,
                             const QModelIndex & index ) const;
        
        QString configId;

        const MetricsConfig metricsConfig;
        QString metricsName;
        MetricsInfo metricsInf;

        /// Returns a color for a metric value between [0,1]
        QColor getColor(double val) const;

        
        QGradient gradient;
        QImage colorMap;

    private slots:

        void gradientFinished( int result );

    private:
        MetricBgDelegate( const MetricBgDelegate& o )
            : configId( o.configId ),
              metricsConfig( o.metricsConfig ),
              metricsName( o.metricsName ),
              metricsInf( o.metricsInf ),
              gradient( o.gradient ),
              colorMap( o.colorMap )
        {}
};

#endif
