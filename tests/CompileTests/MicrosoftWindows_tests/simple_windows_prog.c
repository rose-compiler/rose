/* Windows Programming Tutorial Series
 * Lesson 1 - Your first windows program
 * Pravin Paratey (October 08, 2002)
**/
#include <windows.h>

int WINAPI
WinMain(HINSTANCE hInst, 
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
        int nCmdShow)
{
	MessageBox (NULL, "Hello World! This is my first WIN32 program", 
                "Lesson 1", MB_OK);
	return 0;
}
