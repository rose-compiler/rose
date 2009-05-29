
#ifndef KIVIAT_INFO_H
#define KIVIAT_INFO_H

#include <QWidget>

namespace Ui { class KiviatInfo; }
class MetricsKiviat;
class SgNode;

class KiviatInfo
    : public QWidget
{
    Q_OBJECT

    public:
        KiviatInfo( QWidget *parent, MetricsKiviat *metricsKiviat = NULL );

        virtual ~KiviatInfo();

        void setKiviat( MetricsKiviat *metricsKiviat );

    signals:
        void clicked( SgNode *node );

    protected slots:
        void update( SgNode *node );
        void addNode( SgNode *node );
        void delNode();

        void changeColor();

        void itemChanged( int );

    protected:

    private slots:
        void configChanged();

    private:

        Ui::KiviatInfo *ui;
        MetricsKiviat *metrics;
};

#endif
