#ifndef ASTGRAPHWIDGETPLUGIN_H
#define ASTGRAPHWIDGETPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class AstGraphWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        AstGraphWidgetPlugin(QObject *parent = 0);
        virtual ~AstGraphWidgetPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Graph.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "AstGraphWidget.h"; }
        QString name() const        { return "AstGraphWidget";   }
        QString toolTip() const     { return tr("GraphicsView which can display AST (like zgrviewer)"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
