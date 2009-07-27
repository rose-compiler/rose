#ifndef Q_CODE_EDIT_WIDGET_PLUGIN_H
#define Q_CODE_EDIT_WIDGET_PLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class QCodeEditWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        QCodeEditWidgetPlugin(QObject *parent = 0);
        virtual ~QCodeEditWidgetPlugin();

        bool isContainer() const    { return false; }
        bool isInitialized() const  { return initialized; }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");  }
        QString domXml() const;
        QString group() const       { return "Rose Generic Widgets"; }
        QString includeFile() const { return "QCodeEditWidget.h"; }
        QString name() const        { return "QCodeEditWidget";   }
        QString toolTip() const     { return tr("Small wrapper around QEditor"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
