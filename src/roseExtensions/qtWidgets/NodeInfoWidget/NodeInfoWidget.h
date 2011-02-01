#ifndef NODEINFOVIEW_H
#define NODEINFOVIEW_H

#include "PropertyTreeWidget.h"


class SgNode;
class SgFunctionModifier;
class SgDeclarationModifier;
class SgSpecialFunctionModifier;

/**
 * \brief Shows information about an SgNode (similar to pdf generator)
 *
 * \image html NodeInfoWidget.jpg
 *
 * Shows following information about an SgNode:
 * - Location Info if node is an SgLocatedNode
 * - general Rose-RTI
 * - Annotated (Metric-) Attributes
 *
 *  Usage: just call setNode()
 */
class NodeInfoWidget : public PropertyTreeWidget
{
        Q_OBJECT

        public:
                NodeInfoWidget(QWidget * parent=0);
                virtual ~NodeInfoWidget();

        public slots:
                void setNode(SgNode * node);
                void gotoNode( SgNode *node )
                { setNode( node ); }

        protected:
            SgNode * curNode;

        virtual void dragEnterEvent(QDragEnterEvent * ev);
        virtual void dragMoveEvent(QDragMoveEvent * ev);
        virtual void dropEvent(QDropEvent * ev);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);

        QPoint dragStartPosition;  ///< position of mouseclick, used for d&d functions


                void printDeclModifier(const QModelIndex & par,
                                                          const SgDeclarationModifier & declMod);

                void printFuncModifier(const QModelIndex & par,
                                                           const SgFunctionModifier & funcMod);

                void printSpecialFuncModifier(const QModelIndex & par,
                                                                          const SgSpecialFunctionModifier& funcMod);


                QString getTraversalName(SgNode * node);
};

#endif
