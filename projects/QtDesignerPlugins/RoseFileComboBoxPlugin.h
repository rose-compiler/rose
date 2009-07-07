#ifndef ROSEFILECOMBOBOXPLUGIN_H
#define ROSEFILECOMBOBOXPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>


class RoseFileComboBoxPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        RoseFileComboBoxPlugin(QObject *parent = 0);
        virtual ~RoseFileComboBoxPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "RoseFileComboBox.h"; }
        QString name() const        { return "RoseFileComboBox";   }
        QString toolTip() const     { return tr("ComboBox where you can select files belonging to an SgProject"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
