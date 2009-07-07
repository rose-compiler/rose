#ifndef NODEINFOWIDGETPLUGIN_H_
#define NODEINFOWIDGETPLUGIN_H_

#include <QtDesigner/QDesignerCustomWidgetInterface>

class NodeInfoWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        NodeInfoWidgetPlugin(QObject *parent = 0);
        virtual ~NodeInfoWidgetPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/PropTreeWidget.gif");  }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "NodeInfoWidget.h"; }
        QString name() const        { return "NodeInfoWidget";   }
        QString toolTip() const     { return tr("Displays properties of an SgNode"); }
        QString whatsThis() const   { return tr("displays information of an SgNode (RTTI, Attributes...)"); }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};
#endif
