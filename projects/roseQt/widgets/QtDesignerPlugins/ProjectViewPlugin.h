
#ifndef PROJECTVIEWPLUGIN_H
#define PROJECTVIEWPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class ProjectViewPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

    public:
        ProjectViewPlugin(QObject *parent = 0);
        virtual ~ProjectViewPlugin();

        bool isContainer() const    { return false;         }
        bool isInitialized() const  { return initialized;   }
        QIcon icon() const          { return QIcon(":/QtDesignerPlugins/icons/Rose.gif");       }
        QString domXml() const;
        QString group() const       { return "Rose Specific Widgets"; }
        QString includeFile() const { return "ProjectView.h"; }
        QString name() const        { return "ProjectView";   }
        QString toolTip() const     { return tr("Manage an SgProject, add/remove Files etc"); }
        QString whatsThis() const   { return ""; }
        QWidget *createWidget(QWidget *parent);
        void initialize(QDesignerFormEditorInterface *core);

     private:
         bool initialized;
};

#endif
