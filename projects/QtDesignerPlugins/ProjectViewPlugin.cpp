#include "ProjectViewPlugin.h"
#include "Project.h"
#include "ProjectView.h"

#include <QtPlugin>


ProjectViewPlugin::ProjectViewPlugin(QObject *par)
    : QObject(par),
      initialized( false )
{
}

ProjectViewPlugin::~ProjectViewPlugin()
{
}


QWidget* ProjectViewPlugin::createWidget(QWidget * par)
{
    ProjectView * wdg = new ProjectView(par);
    return wdg;
}

void ProjectViewPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;
    
    /*ProjectManager *pm =*/ ProjectManager::instance();

    initialized = true;
}

QString ProjectViewPlugin::domXml() const
{
    return "<widget class=\"ProjectView\" name=\"projectView\">\n"
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
