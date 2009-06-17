#include "DebugDialog.h"
#include "CustomListModels.h"

#include <QSortFilterProxyModel>

DebugDialog::DebugDialog(QWidget * par)
    : QDialog(par), heapModel(0), stackModel(0), memModel(0)
{
    ui.setupUi(this);
}


void DebugDialog::setHeapVars(RuntimeVariablesType * arr, int arrSize)
{
    if(heapModel)
    {
        delete heapModel;
        delete heapProxyModel;
    }

    heapModel = new RuntimeVariablesModel(arr,arrSize,this);
    heapProxyModel = new QSortFilterProxyModel(this);
    connect(ui.txtHeapFilter,SIGNAL(textChanged(const QString&)),
            heapProxyModel, SLOT(setFilterWildcard(const QString&)));

    heapProxyModel->setSourceModel(heapModel);

    ui.lstHeap->setModel(heapProxyModel);

}

void DebugDialog::setStackVars(RuntimeVariablesType * arr, int arrSize)
{
    if(stackModel)
    {
        delete stackModel;
        delete stackProxyModel;
    }

    stackModel = new RuntimeVariablesModel(arr,arrSize,this);
    stackProxyModel = new QSortFilterProxyModel(this);
    connect(ui.txtStackFilter,SIGNAL(textChanged(const QString&)),
            stackProxyModel, SLOT(setFilterWildcard(const QString&)));

    stackProxyModel->setSourceModel(stackModel);

    ui.lstStack->setModel(stackProxyModel);
}

void DebugDialog::setMemoryLocations(MemoryType * arr, int arrSize)
{
    if(memModel)
    {
        delete memModel;
        delete memProxyModel;
    }

    memModel = new MemoryTypeModel(arr,arrSize,this);
    memProxyModel = new QSortFilterProxyModel(this);
    connect(ui.txtMemFilter,SIGNAL(textChanged(const QString&)),
            memProxyModel, SLOT(setFilterWildcard(const QString&)));


    memProxyModel->setSourceModel(memModel);

    ui.lstMem->setModel(memProxyModel);
}


void DebugDialog::on_lstHeap_clicked(const QModelIndex & ind)
{
    displayRuntimeVariable(heapModel->getRuntimeVariable(ind),ui.propHeap );
}

void DebugDialog::on_lstStack_clicked(const QModelIndex & ind)
{
    displayRuntimeVariable(stackModel->getRuntimeVariable(ind),ui.propStack );

}

void DebugDialog::on_lstMem_clicked(const QModelIndex & ind)
{
    displayMemoryType(memModel->getMemoryType(ind),ui.propMem);
}


void DebugDialog::displayRuntimeVariable(RuntimeVariablesType * rv, PropertyTreeWidget * w)
{
    w->clear();

    int general = w->addSection("RuntimeVariablesType");

    addVariableSection(w,general,rv);

    if(rv->address != NULL)
    {
        int memory = w->addSection("Memory Type");
        addMemorySection(w,memory,rv->address);
    }

    if(rv->arrays != NULL)
    {
        int array = w->addSection("Array");
        addArraySection(w,array,rv->arrays);
    }
}


void DebugDialog::displayMemoryType(MemoryType * mt, PropertyTreeWidget * w)
{
    w->clear();

    int mem = w->addSection("Memory Type");
    addMemorySection(w,mem,mt);

    if(mt->variables != NULL)
    {
        int var = w->addSection("Variable");
        addVariableSection(w,var,mt->variables->variable);
    }
}




void DebugDialog::addVariableSection(PropertyTreeWidget * w, int id, RuntimeVariablesType * rv)
{
    w->addEntryToSection(id,"Name", QString(rv->name) );
    w->addEntryToSection(id,"Mangled Name", QString(rv->mangled_name) );
    w->addEntryToSection(id,"Type", QString(rv->type) );
    w->addEntryToSection(id,"Initialized", rv->initialized );
    w->addEntryToSection(id,"fileOpen", QString(rv->fileOpen) );
    w->addEntryToSection(id,"Value", static_cast<qulonglong>(rv->value) );
}

void DebugDialog::addMemorySection(PropertyTreeWidget * w, int id, MemoryType * mt)
{
    w->addEntryToSection(id,"Address", static_cast<qulonglong>(mt->address));
    w->addEntryToSection(id,"LastVariablePos", mt->lastVariablePos);
    w->addEntryToSection(id,"MaxNrOfVariables", mt->maxNrOfVariables);
    w->addEntryToSection(id,"Size",mt->size);
}

void DebugDialog::addArraySection(PropertyTreeWidget * w, int id, ArraysType * at)
{
    w->addEntryToSection(id,"Name", QString(at->name));
    w->addEntryToSection(id,"Dimension",at->dim);
    w->addEntryToSection(id,"Size Dim1",at->size1);
    w->addEntryToSection(id,"Size Dim2",at->size2);
    w->addEntryToSection(id,"IsMalloc",at->ismalloc);
}

