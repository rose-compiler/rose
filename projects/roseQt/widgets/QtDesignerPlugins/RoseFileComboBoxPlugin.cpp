#include "RoseFileComboBoxPlugin.h"
#include "RoseFileSelector/RoseFileComboBox.h"


#include <QtPlugin>

RoseFileComboBoxPlugin::RoseFileComboBoxPlugin(QObject *par)
    : QObject(par)
{
    initialized=false;
}

RoseFileComboBoxPlugin::~RoseFileComboBoxPlugin()
{
}


QWidget* RoseFileComboBoxPlugin::createWidget(QWidget * par)
{
    RoseFileComboBox * wdg = new RoseFileComboBox(par);
    return wdg;
}

void RoseFileComboBoxPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString RoseFileComboBoxPlugin::domXml() const
{
    return "<widget class=\"RoseFileComboBox\" name=\"roseFileComboBox\">\n"
           " <property name=\"geometry\">\n"
           "  <rect>\n"
           "   <x>0</x>\n"
           "   <y>0</y>\n"
           "   <width>180</width>\n"
           "   <height>24</height>\n"
           "  </rect>\n"
           " </property>\n"
           "</widget>\n";
}
