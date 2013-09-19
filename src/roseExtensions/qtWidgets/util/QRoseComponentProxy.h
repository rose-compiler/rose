
#ifndef QROSECOMPONENTPROXY_H
#define QROSECOMPONENTPROXY_H

#include <QWidget>

//#include <qrose.h>
#include "sage3basic.h"

class QBoxLayout;

namespace qrs
{

class QRoseComponent;
class QRGroupWidget;
class QRMessage;

/**
 * \brief Proxy class to use QRose widgets in the RoseGui
 * 
 */
class QRoseComponentProxy
    : public QWidget
{
    Q_OBJECT

    public:
        QRoseComponentProxy( QRoseComponent *comp, ::QWidget *parent );

        template< typename ComponentType >
        ComponentType *component() const
        { return dynamic_cast<ComponentType *>( comp_ ); }

    signals:
        void nodeActivated( SgNode *node );
        void nodeActivatedAlt( SgNode *node );

    public slots:
        void setNode( SgNode *node );
        void gotoNode( SgNode *node );

    protected:

    private slots:
        void getBroadcast( QRGroupWidget *emitter, QRMessage *msg );

    private:
        QBoxLayout *layout;
        QRoseComponent *comp_;

        
};

}

#endif
