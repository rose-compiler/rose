#include "RoseCodeEditPlugin.h"

#include "AstGraphWidgetPlugin.h"
#include "RoseCodeEdit.h"


#include <QtPlugin>
#include <QDebug>

RoseCodeEditPlugin::RoseCodeEditPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

RoseCodeEditPlugin::~RoseCodeEditPlugin()
{
}


QWidget* RoseCodeEditPlugin::createWidget(QWidget * par)
{
    RoseCodeEdit * wdg = new RoseCodeEdit(par);
    return wdg;
}

void RoseCodeEditPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString RoseCodeEditPlugin::domXml() const
{
    return "<widget class=\"RoseCodeEdit\" name=\"roseCodeEdit\">\n"
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

