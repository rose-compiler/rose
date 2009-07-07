#ifndef ASMVIEWPLUGIN_H
#define ASMVIEWPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class AsmViewPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        AsmViewPlugin(QObject *parent = 0);
        virtual ~AsmViewPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "AsmView.h"; }
        QString name() const        { return "AsmView";   }
        QString toolTip() const     { return tr("Unparses a binary AST grouped by functions"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};


#endif
