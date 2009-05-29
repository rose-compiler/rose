
#ifndef METRIC_FILTER_H
#define METRIC_FILTER_H

#include <QWidget>

class SgNode;
class AstFilterInterface;
class MetricsConfig;
namespace Ui { class MetricFilter; }

class MetricFilter
    : public QWidget
{
    Q_OBJECT

    public:
        MetricFilter( QWidget *parent = NULL );

        virtual ~MetricFilter();

    signals:
        void filterChanged( AstFilterInterface *filter );

    public slots:

    private slots:
        void itemChanged( int );
        void thresholdChanged( int threshold );
        void updateMetrics();

    private:
        void filterChanged( );

        const QString metricConfigureId;

        Ui::MetricFilter *metricFilterUi;
        MetricsConfig *metricsConfig;

        int currentId;
        QString currentName;
        int currentThreshold;
};

#endif
