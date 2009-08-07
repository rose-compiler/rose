
#ifndef ROSETREEVIEW_H
#define ROSETREEVIEW_H

#include <QTreeView>

class QDropEvent;
class QDragEnterEvent;
class QDragMoveEvent;

class SgNode;
class ItemTreeModel;
class ItemTreeModel;

class AstFilterInterface;


/**
 * \brief TreeView displaying an ItemTreeModel
 *
 * Features:
 *  - Drag and Drop of SgNodes (when model provides data for SgNodeRole)
 *  - setNode and setFilter for SgNodes
 *  - Signals for clicks on SgNodes
 *
 * Usage:
 *  - re-implement updateModel(), as an example look at BAstView or AsmView
 */
class RoseTreeView : public QTreeView
{
    Q_OBJECT

    Q_PROPERTY(bool SgDragEnabled  READ isSgDragEnabled WRITE setSgDragEnabled);
    Q_PROPERTY(bool SgDropEnabled  READ isSgDropEnabled WRITE setSgDropEnabled);

    public:
        RoseTreeView(QWidget * parent=NULL);
        virtual ~RoseTreeView();

        void setSgDragEnabled(bool b) { dragEnabled= b; }
        void setSgDropEnabled(bool b) { dropEnabled= b; }

        bool isSgDragEnabled() const { return dragEnabled; }
        bool isSgDropEnabled() const { return dropEnabled; }

        void addArrow(const QModelIndex & i1, const QModelIndex & i2);
        void removeAllArrows();

    public slots:
        virtual void setNode(SgNode * node);
        virtual void setFilter(AstFilterInterface * filter);
        virtual bool gotoNode( SgNode *node );

    signals:
        /// Emitted if clicked on an Entry
        /// warning: may be null (appropriate action would be to clear the attached view)
        void nodeActivated   (SgNode * node);
        void nodeActivatedAlt( SgNode * node );

        /// Emitted when clicked, provides the sourcefile and position of the SgNode (can be connected to an editor)
        void clicked(const QString & file, int startRow, int startCol,
                                           int endRow, int endCol);


        void nodeDropped(SgNode * node, const QPoint & p);



    protected slots:
        virtual void viewClicked      (const QModelIndex & ind);
        void         viewDoubleClicked( const QModelIndex & ind );


    protected:
        virtual void dragEnterEvent(QDragEnterEvent * ev);
        virtual void dragMoveEvent(QDragMoveEvent * ev);
        virtual void dropEvent(QDropEvent * ev);
        virtual void mousePressEvent(QMouseEvent *event);
        virtual void mouseMoveEvent(QMouseEvent *event);

        /// handler to decide wether to accept a certain mime data
        /// default is to accept application/SgNode
        /// to support application/SgNode-* overwrite these two
        /// functions in the derived class
        virtual bool acceptMimeData( const QMimeData *mimeData );
        /// extracts and takes proper action for the arrived mimeData
        virtual void handleMimeData( const QMimeData *mimeData, const QPoint& pos );

        // drawing of arrows
        virtual void paintEvent(QPaintEvent * ev);
        void drawArrowBezier(const QPoint & p1, const QPoint & p2, QPainter * painter, float curvature);
        void normalizeVec(QPointF & vec) const;
        QPointF getOrtho(const QPointF & input,bool direction=true) const;

        QList<QPair<QModelIndex,QModelIndex> > arrows;

        virtual void updateModel() {};


        ItemTreeModel * model;

        /// The user calls setNode and filter, the new values are stored here
        /// then updateView is called
        AstFilterInterface * curFilter;
        SgNode * curNode;


        //Drag and drop members
        QPoint dragStartPosition;  ///< position of mouseclick, used for d&d functions
        bool dragEnabled;          ///< if SgNodes can be dragged out of this view
        bool dropEnabled;          ///< if SgNodes can be dropped at this view

};


#endif

