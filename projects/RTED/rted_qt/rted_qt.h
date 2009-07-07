#ifndef RTED_QT_H
#define RTED_QT_H


#include <stdarg.h>
//#include "RuntimeSystem.h"


/** Transfer control to gui, gui may show dialog (depends on checkpoints)
 *  checkpoints are defined by user through gui
 *  @param filename   filename of non-instrumented file, instrumented filename
 *                    is determined from this filename by "namebefore_rose.c"
 *  @param lineFile1  linenumber in non-instrumented file
 *  @param lineFile2  linenumber in instrumented file
 */
#ifdef __cplusplus
	extern "C"
#endif
void gui_checkpoint(const char* filename, int lineFile1, int lineFile2);


/// Gui is forced to show debug-dialog (independent of checkpoints)
/// user changes sourcecode directly (to call this function)
/// for params see gui_checkpoint()
#ifdef __cplusplus
	extern "C"
#endif
void gui_showDialog(const char* filename, int lineFile1, int lineFile2);



/// Notifies the gui of datastructure change - control stays at runtimesystem
#ifdef __cplusplus
	extern "C"
#endif
void gui_updateDataStructures(struct RuntimeVariablesType * stack,int stackSize,
		                      struct RuntimeVariablesType * vars, int varSize,
							  struct MemoryType * mem, int memSize);





/// ----------------------- Output --------------------------------------------

/// Prints a message in the gui message window
#ifdef __cplusplus
	extern "C"
#endif
void gui_printMessage(const char * format, va_list ap );

/// Prints a warning (=message with different icon) in the gui message window
#ifdef __cplusplus
	extern "C"
#endif
void gui_printWarning(const char * format, va_list ap);

/// Prints an error (=message with different icon) in the gui message window
#ifdef __cplusplus
	extern "C"
#endif
void gui_printError(const char * format, va_list ap);



#endif
