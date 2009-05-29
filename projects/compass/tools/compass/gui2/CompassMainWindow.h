
#ifndef COMPASSMAINWINDOW_H
#define COMPASSMAINWINDOW_H


#include <QMainWindow>

namespace Ui {  class CompassMainWindow; }
class CheckerTableModel;
class ViolationsTableModel;
class CompassInterface;
class QModelIndex;
class CheckerFilterModel;
class QSortFilterProxyModel;

#include "qcodeedit.h"

class CompassMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        CompassMainWindow(CompassInterface * ci, QWidget * parent = 0);

        virtual ~CompassMainWindow();



    protected slots:
        void on_tblCheckers_clicked  (const QModelIndex & index);
        void on_tblViolations_clicked(const QModelIndex & index);

        void on_actionRunTests_triggered();
        void on_actionReset_triggered();

        void on_actionSelectAll_triggered();
        void on_actionUnselectAll_triggered();
        void on_actionInvertSelection_triggered();

        void on_actionSave_triggered();
        void on_actionSaveAs_triggered();

        void on_actionEditorSettings_triggered();

        void on_tblCheckers_customContextMenuRequested (const QPoint & pos);

    protected:
        void updateCheckersTable();

        QCodeEdit * editorWrapper;


        CompassInterface * compInterface;

        Ui::CompassMainWindow * ui;

        CheckerTableModel * checkerTableModel;
        CheckerFilterModel * checkerFilterModel;

        ViolationsTableModel * violationsTableModel;
        QSortFilterProxyModel * violationsFilterModel;
};


#endif
