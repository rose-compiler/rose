#ifndef DEBUG_DIALOG_H
#define DEBUG_DIALOG_H

#include <QMainWindow>



class RtedDebug;
class PropertyTreeWidget;

#include "CppRuntimeSystem.h"





namespace Ui { class MainWindow; }
class ItemTreeModel;
class QSortFilterProxyModel;

class DbgMainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        DbgMainWindow(RtedDebug * dbg, QWidget * parent =0);
        ~DbgMainWindow();


        void addMessage(const QString & msg);

        /// shows the dialog if a breakpoint was set in instrumented file
        void showIfBreakpoint(int lineNr);


        void updateAllRsData();

    protected slots:
        void on_actionSave_triggered();
        void on_actionSaveAs_triggered();
        void on_actionOpen_triggered();
        void on_actionEditorSettings_triggered();

        void on_actionSingleStep_triggered();
        void on_actionResume_triggered();


        void on_chkShowStack_toggled();
        void on_chkShowHeap_toggled();

    protected:

        void updateTypeDisplay();
        void updateMemoryDisplay();
        void updateStackDisplay();

        RuntimeSystem * rs;
        RtedDebug *     dbgObj;

        QList<int> breakPoints;

        Ui::MainWindow * ui;

        // Type System
        ItemTreeModel * typeModel;
        QSortFilterProxyModel * typeProxyModel;

        // Memory System
        ItemTreeModel * memModel;
        QSortFilterProxyModel * memProxyModel;

        // Stack variables
        ItemTreeModel * stackModel;
        QSortFilterProxyModel * stackProxyModel;


};

#endif

