#include "RoseWidgetsCollection.h"

#include "AstBrowserWidgetPlugin.h"
#include "AstGraphWidgetPlugin.h"
#include "AsmViewPlugin.h"
#include "BAstViewPlugin.h"
#include "KiviatViewPlugin.h"
#include "NodeInfoWidgetPlugin.h"
#include "MetricsKiviatPlugin.h"
#include "ProjectViewPlugin.h"
#include "PropertyTreeWidgetPlugin.h"
#include "QCodeEditWidgetPlugin.h"
#include "RoseCodeEditPlugin.h"
#include "RoseFileComboBoxPlugin.h"

RoseWidgetsCollection::RoseWidgetsCollection(QObject * par)
    : QObject(par)
{
    widgets.append(new AstGraphWidgetPlugin(this));
    widgets.append(new AsmViewPlugin(this));
    widgets.append(new BAstViewPlugin(this));
    widgets.append(new KiviatViewPlugin(this));
    widgets.append(new NodeInfoWidgetPlugin(this));
    widgets.append(new MetricsKiviatPlugin(this));
    widgets.append(new ProjectViewPlugin(this));
    widgets.append(new PropertyTreeWidgetPlugin(this));
    widgets.append(new QCodeEditWidgetPlugin(this) );
    widgets.append(new RoseCodeEditPlugin(this));
    widgets.append(new RoseFileComboBoxPlugin(this));
}


QList<QDesignerCustomWidgetInterface*> RoseWidgetsCollection::customWidgets() const
{
    return widgets;
}

Q_EXPORT_PLUGIN2(rose_widgets,  RoseWidgetsCollection)
