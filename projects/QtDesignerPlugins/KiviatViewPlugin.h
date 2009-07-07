

#ifndef KIVIATVIEWPLUGIN_H
#define KIVIATVIEWPLUGIN_H

#include <QtDesigner/QDesignerCustomWidgetInterface>

class KiviatViewPlugin : public QObject, public QDesignerCustomWidgetInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetInterface)

	public:
		KiviatViewPlugin(QObject *parent = 0);
    	virtual ~KiviatViewPlugin();

		bool isContainer() const 	{ return false; }
		bool isInitialized() const	{ return initialized; }
		QIcon icon() const			{ return QIcon(":/QtDesignerPlugins/icons/Kiviat.gif");  }
		QString domXml() const;
		QString group() const		{ return "Rose Generic Widgets"; }
		QString includeFile() const { return "KiviatView.h"; }
		QString name() const		{ return "KiviatView";   }
		QString toolTip() const		{ return tr("Can display a Kiviat Diagram"); }
		QString whatsThis() const	{ return ""; }
		QWidget *createWidget(QWidget *parent);
		void initialize(QDesignerFormEditorInterface *core);

	 private:
	     bool initialized;
};

#endif
