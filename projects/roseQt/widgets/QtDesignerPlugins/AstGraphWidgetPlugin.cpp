#include "AstGraphWidgetPlugin.h"
#include "AstGraphWidget/AstGraphWidget.h"


#include <QtPlugin>
#include <QDebug>

AstGraphWidgetPlugin::AstGraphWidgetPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

AstGraphWidgetPlugin::~AstGraphWidgetPlugin()
{
}


QWidget* AstGraphWidgetPlugin::createWidget(QWidget * par)
{
    AstGraphWidget * wdg = new AstGraphWidget(par);
    return wdg;
}

void AstGraphWidgetPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString AstGraphWidgetPlugin::domXml() const
{
    return "<widget class=\"AstGraphWidget\" name=\"astGraphWidget\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>500</width>\n"
           "   <height>500</height>\n"
           "  </rect>\n"
           " </property>\n"
           "</widget>\n";
}

