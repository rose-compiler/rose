

#include "rted_qt.h"
//#include "RuntimeSystem.h"
#include <QApplication>
#include "DebugDialog.h"


void showDebugDialog(struct RuntimeVariablesType * stack, int stackSize,
                     struct RuntimeVariablesType * heap, int heapSize,
                     struct MemoryType * mem, int memSize,
                     char* filename, int row)
{
    static QApplication app(0,NULL);
    static DebugDialog * dlg = new DebugDialog();

    QCoreApplication::setOrganizationName("LLNL");
    QCoreApplication::setOrganizationDomain("ROSE");
    QCoreApplication::setApplicationName("rted_qt");


    dlg->setHeapVars(heap,heapSize);
    dlg->setStackVars(stack,stackSize);
    dlg->setMemoryLocations(mem,memSize);

    dlg->setEditorMark(filename,row);

    dlg->showMaximized();
    app.exec();
}

#if 0
// testMain
int main(int argc, char**argv)
{
    RuntimeVariablesType stack[2];
    RuntimeVariablesType heap[2];

    MemoryType mem[2];

    //Stack Setup
    stack[0].name = "StackVar0";
    stack[1].name = "StackVar1";

    stack[0].mangled_name = "StackMangledName0";
    stack[1].mangled_name = "StackMangledName1";

    stack[0].type = "r";
    stack[1].type = "v";

    stack[0].address=NULL;
    stack[1].address=&mem[0];

    stack[0].fileOpen="r";
    stack[1].fileOpen="r";

    stack[0].arrays=NULL;
    stack[1].arrays=NULL;

    //Heap Setup
    heap[0].name = "StackVar0";
    heap[1].name = "StackVar1";

    heap[0].mangled_name = "StackMangledName0";
    heap[1].mangled_name = "StackMangledName1";

    heap[0].fileOpen="r";
    heap[1].fileOpen="r";

    heap[0].type = "r";
    heap[1].type = "v";

    heap[0].address=NULL;
    heap[1].address=&mem[0];

    heap[0].arrays=NULL;
    heap[1].arrays=NULL;


    //Mem Loc Setup
    mem[0].address = 123;
    mem[1].address = 2343;

    mem[0].variables = NULL;
    mem[1].variables = NULL;

    showDebugDialog(stack,2,heap,2,mem,2,"/home/bauer25/test.cpp",1);
}
#endif
