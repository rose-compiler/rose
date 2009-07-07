#ifndef BASTVIEWPLUGIN_H
#define BASTVIEWPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class BAstViewPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        BAstViewPlugin(QObject *parent = 0);
        virtual ~BAstViewPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "BAstView.h"; }
        QString name() const        { return "BAstView";   }
        QString toolTip() const     { return tr("View of an Abstract Syntax (Sub)Tree, grouped by files,classes,functions.."); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
