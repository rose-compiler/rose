#include "PropertyTreeWidgetPlugin.h"
#include "PropertyTreeWidget/PropertyTreeWidget.h"


#include <QtPlugin>
#include <QDebug>

PropertyTreeWidgetPlugin::PropertyTreeWidgetPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

PropertyTreeWidgetPlugin::~PropertyTreeWidgetPlugin()
{
}


QWidget* PropertyTreeWidgetPlugin::createWidget(QWidget * par)
{
    PropertyTreeWidget * wdg = new PropertyTreeWidget(par);
    Q_ASSERT(wdg);
    return wdg;
}

void PropertyTreeWidgetPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString PropertyTreeWidgetPlugin::domXml() const
{
    return "<widget class=\"PropertyTreeWidget\" name=\"propertyTreeWidget\">\n"
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
