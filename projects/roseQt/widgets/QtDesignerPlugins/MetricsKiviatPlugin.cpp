
#include "MetricsKiviatPlugin.h"
#include "MetricsKiviat/MetricsKiviat.h"

#include <QtPlugin>


MetricsKiviatPlugin::MetricsKiviatPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

MetricsKiviatPlugin::~MetricsKiviatPlugin()
{
}


QWidget* MetricsKiviatPlugin::createWidget(QWidget * par)
{
    MetricsKiviat * wdg = new MetricsKiviat(par);

    /*
    wdg->setAxisCount( 8 );

    // setup some example data
    QVector<float> data;
    data << 0.1 << 0.2 << 0.3 << 0.4
         << 0.0 << 0.0 << 0.0 << 0.0;
    wdg->addData(data,Qt::blue);

    data.clear();
    data << 0 << 0 << 0<< 0 <<
           0.1 << 0.5 << 0.8 << 0.4;
    wdg->addData(data,Qt::yellow);
    */

    return wdg;
}

void MetricsKiviatPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString MetricsKiviatPlugin::domXml() const
{
    return "<widget class=\"MetricsKiviat\" name=\"metricsKiviat\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>250</width>\n"
           "   <height>250</height>\n"
           "  </rect>\n"
           " </property>\n"
           "</widget>\n";
}
