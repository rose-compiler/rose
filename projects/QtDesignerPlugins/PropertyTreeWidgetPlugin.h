#ifndef PROPERTYTREEWIDGETPLUGIN_H
#define PROPERTYTREEWIDGETPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>


class PropertyTreeWidgetPlugin: public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        PropertyTreeWidgetPlugin(QObject *parent = 0);
        virtual ~PropertyTreeWidgetPlugin();

        bool isContainer() const    { return false; }
        bool isInitialized() const  { return initialized; }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/PropTreeWidget.gif");  }
        QString domXml() const;
        QString group() const       { return "Rose Generic Widgets"; }
        QString includeFile() const { return "PropertyTreeWidget.h"; }
        QString name() const        { return "PropertyTreeWidget";   }
        QString toolTip() const     { return tr("TreeWidget like the Property Editor of Qt-Designer"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
