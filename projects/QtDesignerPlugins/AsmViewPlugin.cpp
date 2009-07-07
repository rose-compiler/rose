#include "AsmViewPlugin.h"
#include "AsmView.h"

#include <QtPlugin>

AsmViewPlugin::AsmViewPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

AsmViewPlugin::~AsmViewPlugin()
{
}


QWidget* AsmViewPlugin::createWidget(QWidget * par)
{
    AsmView * wdg = new AsmView(par);
    return wdg;
}

void AsmViewPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString AsmViewPlugin::domXml() const
{
    return "<widget class=\"AsmView\" name=\"asmView\">\n"
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
