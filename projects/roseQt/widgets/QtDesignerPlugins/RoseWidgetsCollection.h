#ifndef ROSEWIDGETSCOLLECTION_H
#define ROSEWIDGETSCOLLECTION_H


#include <QtDesigner/QtDesigner>
#include <QtCore/qplugin.h>


/**
 * Class for grouping all the plugins together
 * to add a new one, just add a new list entry in the constructor
 */
class RoseWidgetsCollection : public QObject, public QDesignerCustomWidgetCollectionInterface
{
    Q_OBJECT
    Q_INTERFACES(QDesignerCustomWidgetCollectionInterface)

    public:
        RoseWidgetsCollection(QObject * parent = 0);
        virtual ~RoseWidgetsCollection() {}


        virtual QList<QDesignerCustomWidgetInterface*> customWidgets() const;

    private:
        QList<QDesignerCustomWidgetInterface*> widgets;
};

#endif
