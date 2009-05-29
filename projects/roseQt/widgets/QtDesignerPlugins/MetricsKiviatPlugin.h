
#ifndef METRICSKIVIAT_PLUGIN_H
#define METRICSKIVIAT_PLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class MetricsKiviatPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        MetricsKiviatPlugin(QObject *parent = 0);
        virtual ~MetricsKiviatPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "MetricsKiviat.h"; }
        QString name() const        { return "MetricsKiviat";   }
        QString toolTip() const     { return tr("Display Metrics Attributes of SgNodes in a Kiviat Diagram"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
