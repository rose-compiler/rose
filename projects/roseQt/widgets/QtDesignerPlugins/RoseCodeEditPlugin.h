#ifndef ROSECODEEDITPLUGIN_H
#define ROSECODEEDITPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class RoseCodeEditPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        RoseCodeEditPlugin(QObject *parent = 0);
        virtual ~RoseCodeEditPlugin();

        bool isContainer() const    { return false; }
        bool isInitialized() const  { return initialized; }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");  }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "RoseCodeEdit.h"; }
        QString name() const        { return "RoseCodeEdit";   }
        QString toolTip() const     { return tr("Displays the file a SgNode points to"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
