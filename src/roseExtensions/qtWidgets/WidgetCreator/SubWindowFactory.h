
#ifndef SUBWINDOWFACTORY_H
#define SUBWINDOWFACTORY_H

#include <QMdiArea>
#include <QMap>

class WidgetCreatorInterface;
class QMenu;


/**
 * \brief QMdiArea with ROSE sub-widgets
 *
 * This class implements a QMdiArea which can create ROSE widgets, and link them together.
 * The use can link widgets together by using a custom widgets-menu which is located in the top left corner of a subwidget
 * The link mechanism relies on the fact that the subwidgets, i.e. widget which are created by the WidgetCreatorInterface have the following signals and slots:
 *           - slot \c setNode()   Takes the given node as base for the whole display. TreeView's usually set the new node as root.
 *           - slot \c gotoNode()  Selects the given node (and in tree-view expands until this node is visible), and highlights it, the displayed data set does not change
 *           - signal \c nodeActivated()    This signal is usually sent when the user clicks on some node
 *           - signal \c nodeActivatedAlt() Alternative way of activating a node, mostly double-click (but may vary from view to view)
 * Unfortunately this cannot be enforced by a common abstract base class, since these are signals and slots, this base class whould have to inherit from
 * QObject, which would result in a diamond shaped class hierarchy. If these signals and slots do not exist the link-mechanism just does not work because connect fails
 * which result in a warning on the command line)
 */
class SubWindowFactory
    : public QMdiArea
{
    Q_OBJECT

    public:
        SubWindowFactory( QWidget *parent = NULL )
            : QMdiArea( parent )
        {}

        virtual ~SubWindowFactory();

        void registerSubWindow( WidgetCreatorInterface *winInterface );

        QList<QAction *> getActions() const;

    private slots:
        void addSubWindowAction();
        void linkAction();

    private:
        bool eventFilter( QObject *object, QEvent *event );

        void rebuildSystemMenus();

        QList<WidgetCreatorInterface *> interfaces;

        QMap<QMdiSubWindow *, QWidget *> openWidgets;
        QMap<QPair<QWidget *, QWidget *>, bool> linked;        
};

#endif
