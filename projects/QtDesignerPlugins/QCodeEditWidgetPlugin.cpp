#include "QCodeEditWidgetPlugin.h"


#include "QCodeEditWidget.h"


#include <QtPlugin>
#include <QDebug>

QCodeEditWidgetPlugin::QCodeEditWidgetPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

QCodeEditWidgetPlugin::~QCodeEditWidgetPlugin()
{
}


QWidget* QCodeEditWidgetPlugin::createWidget(QWidget * par)
{
    QCodeEditWidget * wdg = new QCodeEditWidget(par);
    return wdg;
}

void QCodeEditWidgetPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString QCodeEditWidgetPlugin::domXml() const
{
    return "<widget class=\"QCodeEditWidget\" name=\"codeEditWidget\">\n"
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

