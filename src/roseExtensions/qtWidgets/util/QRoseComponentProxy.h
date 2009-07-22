
#ifndef QROSECOMPONENTPROXY_H
#define QROSECOMPONENTPROXY_H

#include <QObject>

#include <qrose.h>

namespace qrs
{

class QRoseComponentProxy
    : public QObject
{
    Q_OBJECT

    public:
        QRoseComponentProxy( QRoseComponent *comp );

        template< typename ComponentType >
        ComponentType *component() const
        { return dynamic_cast<ComponentType *>( comp_ ); }

    signals:
        void clicked( SgNode *node );

    public slots:
        void setNode( SgNode *node );
        void gotoNode( SgNode *node );

    protected:

    private slots:
        void getBroadcast( QRGroupWidget *emitter, QRMessage *msg );

    private:
        QRoseComponent *comp_;

};

}

#endif
