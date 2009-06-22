#include "BAstViewPlugin.h"
#include "BAstView.h"

#include <QtPlugin>


BAstViewPlugin::BAstViewPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

BAstViewPlugin::~BAstViewPlugin()
{
}


QWidget* BAstViewPlugin::createWidget(QWidget * par)
{
    BAstView * wdg = new BAstView(par);
    return wdg;
}

void BAstViewPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString BAstViewPlugin::domXml() const
{
    return "<widget class=\"BAstView\" name=\"bAstView\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>250</width>\n"
           "   <height>500</height>\n"
           "  </rect>\n"
           " </property>\n"
           "</widget>\n";
}
