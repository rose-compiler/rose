
#ifndef ROSEGRAPHICSVIEW_H
#define ROSEGRAPHICSVIEW_H


#include <QGraphicsView>
#include <QMimeData>

class SgNode;
class AstFilterInterface;


class RoseGraphicsView : public QGraphicsView
{
    Q_OBJECT

    Q_PROPERTY(bool SgDragEnabled  READ isSgDragEnabled WRITE setSgDragEnabled);
    Q_PROPERTY(bool SgDropEnabled  READ isSgDropEnabled WRITE setSgDropEnabled);


    public:
        RoseGraphicsView(QWidget * parent=0);
        virtual ~RoseGraphicsView();

        void setSgDragEnabled(bool b) { dragEnabled= b; }
        void setSgDropEnabled(bool b) { dropEnabled= b; }

        bool isSgDragEnabled() const { return dragEnabled; }
        bool isSgDropEnabled() const { return dropEnabled; }


    public slots:
        void setNode(SgNode * sgNode);

        // TODO: implement proper goto mechanism
        void gotoNode( SgNode *sgNode )
        { setNode( sgNode ); }

        void setFilter(AstFilterInterface * filter);
        void setFileFilter(int id);


    signals:
        void nodeActivated(SgNode * node);
        void nodeActivatedAlt(SgNode * node);
        void clicked(const QString & filename, int line, int column);
        void nodeDropped(SgNode * node, QPoint pos);


    protected:

        /// Reimplement in derived classes, and use curFilter and curNode
        /// to generate graph/tree
        virtual void updateGraph(SgNode * node, AstFilterInterface * filter)= 0;



        // Zoom
        virtual void wheelEvent(QWheelEvent *event);
        virtual void scaleView(qreal scaleFactor);
        virtual void mousePressEvent(QMouseEvent *event);

        // Drop & Drop
        virtual void dragEnterEvent(QDragEnterEvent * ev);
        virtual void dropEvent(QDropEvent *ev);
        virtual void dragMoveEvent( QDragMoveEvent * ev);
        virtual void mouseMoveEvent(QMouseEvent *event);

        virtual bool acceptMimeData( const QMimeData *mimeData );
        virtual void handleMimeData( const QMimeData *mimeData, const QPoint& pos );


        //Drag and drop members
        QPoint dragStartPosition;  ///< position of mouseclick, used for d&d functions
        bool dragEnabled;          ///< if SgNodes can be dragged out of this view
        bool dropEnabled;          ///< if SgNodes can be dropped at this view


    private:
        AstFilterInterface * curFilter;
        SgNode *             curNode;

};



#endif
