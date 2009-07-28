
#ifndef SUBWINDOWFACTORY_H
#define SUBWINDOWFACTORY_H

#include <QMdiArea>
#include <QMap>

class WidgetCreatorInterface;
class QMenu;

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
