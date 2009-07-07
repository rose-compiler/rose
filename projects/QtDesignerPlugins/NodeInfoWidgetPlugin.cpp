#include "NodeInfoWidgetPlugin.h"
#include "NodeInfoWidget.h"


#include <QtPlugin>
#include <QDebug>

NodeInfoWidgetPlugin::NodeInfoWidgetPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

NodeInfoWidgetPlugin::~NodeInfoWidgetPlugin()
{

}


QWidget* NodeInfoWidgetPlugin::createWidget(QWidget * par)
{
    NodeInfoWidget * wdg = new NodeInfoWidget(par);
    return wdg;
}

void NodeInfoWidgetPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString NodeInfoWidgetPlugin::domXml() const
{
    return "<widget class=\"NodeInfoWidget\" name=\"nodeInfoWidget\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>200</width>\n"
           "   <height>500</height>\n"
           "  </rect>\n"
           " </property>\n"
           "</widget>\n";
}

