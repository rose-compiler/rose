#include "KiviatViewPlugin.h"
#include "KiviatView.h"

#include <QtPlugin>

#include <QDebug>
KiviatViewPlugin::KiviatViewPlugin(QObject *par)
	: QObject(par)
{
	initialized=false;
}

KiviatViewPlugin::~KiviatViewPlugin()
{
}


QWidget* KiviatViewPlugin::createWidget(QWidget * par)
{
	KiviatView * wdg = new KiviatView(par,8);

	// setup some example data
	QVector<float> data;
	data << 0.1 << 0.2 << 0.3 << 0.4
             << 0.0 << 0.0 << 0.0 << 0.0;
        wdg->addData(data,Qt::blue);

	data.clear();
	data << 0 << 0 << 0<< 0
             << 0.1 << 0.5 << 0.8 << 0.4;
        wdg->addData(data,Qt::yellow);

	return wdg;
}

void KiviatViewPlugin::initialize(QDesignerFormEditorInterface * core)
{
    if (initialized)
        return;

    initialized = true;
}

QString KiviatViewPlugin::domXml() const
{
    return "<widget class=\"KiviatView\" name=\"kiviatView\">\n"
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
