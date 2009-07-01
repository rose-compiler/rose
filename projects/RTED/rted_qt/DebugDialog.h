#ifndef DEBUG_DIALOG_H
#define DEBUG_DIALOG_H

#include <QMainWindow>

class RuntimeVariablesType;
class MemoryType;
class ArraysType;

class PropertyTreeWidget;

class RuntimeVariablesModel;
class MemoryTypeModel;
class QSortFilterProxyModel;


#include "ui_DebugDialog.h"


namespace Ui { class DebugDialog; }

class RtedDebug;

class DebugDialog : public QMainWindow
{
    Q_OBJECT

    public:
        DebugDialog(RtedDebug * dbg, QWidget * parent =0);
        ~DebugDialog();

        void setHeapVars(RuntimeVariablesType * arr, int arrSize);
        void setStackVars(RuntimeVariablesType * arr, int arrSize);

        void setMemoryLocations(MemoryType * arr, int arrSize);

        void setEditorMark1(const QString & file, int row);
        void setEditorMark2(const QString & file, int row);


        void addMessage(const QString & msg);

        /// shows the dialog if a breakpoint was set in instrumented file
        void showIfBreakpoint(int lineNr);

    protected slots:
        void on_actionSingleStep_triggered();
        void on_actionResume_triggered();

        void on_lstHeap_clicked(const QModelIndex & ind);
        void on_lstStack_clicked(const QModelIndex & ind);
        void on_lstMem_clicked(const QModelIndex & ind);

        void on_actionSave_triggered();
        void on_actionSaveAs_triggered();
        void on_actionOpen_triggered();
        void on_actionEditorSettings_triggered();

    protected:
        void displayRuntimeVariable(RuntimeVariablesType * rv, PropertyTreeWidget * w);
        void displayMemoryType(MemoryType * mt, PropertyTreeWidget * w);

        void addArraySection(PropertyTreeWidget * w, int sectionId, ArraysType * at);
        void addMemorySection(PropertyTreeWidget * w, int sectionId, MemoryType * mt);
        void addVariableSection(PropertyTreeWidget * w, int id, RuntimeVariablesType * rv);


        QList<int> breakPoints;


        RtedDebug * dbgObj;

        RuntimeVariablesModel * heapModel;
        QSortFilterProxyModel * heapProxyModel;

        RuntimeVariablesModel * stackModel;
        QSortFilterProxyModel * stackProxyModel;

        MemoryTypeModel *  memModel;
        QSortFilterProxyModel * memProxyModel;

        Ui::DebugDialog * ui;

};

#endif

