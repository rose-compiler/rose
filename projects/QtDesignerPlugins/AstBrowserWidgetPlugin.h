#ifndef ASTBROWSERWIDGETPLUGIN_H
#define ASTBROWSERWIDGETPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class AstBrowserWidgetPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        AstBrowserWidgetPlugin(QObject *parent = 0);
        virtual ~AstBrowserWidgetPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "AstBrowserWidget.h"; }
        QString name() const        { return "AstBrowserWidget";   }
        QString toolTip() const     { return tr("Displays a TreeWidget of an Rose-AST"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
