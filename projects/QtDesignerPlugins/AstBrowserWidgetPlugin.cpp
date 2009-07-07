#include "AstBrowserWidgetPlugin.h"
#include "AstBrowserWidget.h"

#include <QtPlugin>

AstBrowserWidgetPlugin::AstBrowserWidgetPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

AstBrowserWidgetPlugin::~AstBrowserWidgetPlugin()
{
}


QWidget* AstBrowserWidgetPlugin::createWidget(QWidget * par)
{
    AstBrowserWidget * wdg = new AstBrowserWidget(par);
    return wdg;
}

void AstBrowserWidgetPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString AstBrowserWidgetPlugin::domXml() const
{
    return "<widget class=\"AstBrowserWidget\" name=\"astBrowserWidget\">\n"
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
