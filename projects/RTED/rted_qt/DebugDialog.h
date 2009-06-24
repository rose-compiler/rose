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

class DebugDialog : public QMainWindow
{
    Q_OBJECT

    public:
        DebugDialog(QWidget * parent =0);
        ~DebugDialog();

        void setHeapVars(RuntimeVariablesType * arr, int arrSize);
        void setStackVars(RuntimeVariablesType * arr, int arrSize);

        void setMemoryLocations(MemoryType * arr, int arrSize);

        void setEditorMark(const QString & file, int row);

    protected slots:
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

        RuntimeVariablesModel * heapModel;
        QSortFilterProxyModel * heapProxyModel;

        RuntimeVariablesModel * stackModel;
        QSortFilterProxyModel * stackProxyModel;

        MemoryTypeModel *  memModel;
        QSortFilterProxyModel * memProxyModel;

        Ui::DebugDialog * ui;

};

#endif

