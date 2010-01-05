#define WIN32_LEAN_AND_MEAN
#include <windows.h>

  HWND hwndMain;	//Main window handle
 
  // Callback function
  LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);
  // Windows entry point
  int WINAPI
  WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow)
 {
	MSG msg; // MSG structure to store messages
	WNDCLASSEX wcx; // WINDOW class information

	// Initialize the struct to zero
	ZeroMemory(&wcx,sizeof(WNDCLASSEX));
	wcx.cbSize = sizeof(WNDCLASSEX); // Window size. Must always be sizeof(WNDCLASSEX)
	wcx.style = CS_HREDRAW|CS_VREDRAW |CS_DBLCLKS ; // Class styles
	wcx.lpfnWndProc = (WNDPROC)MainWndProc; // Pointer to the callback procedure
	wcx.cbClsExtra = 0; // Extra byte to allocate following the wndclassex structure
	wcx.cbWndExtra = 0; // Extra byte to allocate following an instance of the structure
	wcx.hInstance = hInstance; // Instance of the application
	wcx.hIcon = NULL; // Class Icon
  wcx.hCursor = LoadCursor(NULL, IDC_ARROW); // Class Cursor
	wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW); // Background brush
	wcx.lpszMenuName = NULL; // Menu resource
	wcx.lpszClassName = "Lesson2"; // Name of this class
	wcx.hIconSm = NULL; // Small icon for this class

	// Register this window class with MS-Windows
	if (!RegisterClassEx(&wcx))
		return 0;

	// Create the window
	hwndMain = CreateWindowEx(0, //Extended window style
				"Lesson2", // Window class name
				"Lesson 2 - A simple win32 application", // Window title
				WS_OVERLAPPEDWINDOW, // Window style
				CW_USEDEFAULT,CW_USEDEFAULT, // (x,y) pos of the window
				CW_USEDEFAULT,CW_USEDEFAULT, // Width and height of the window
				HWND_DESKTOP, // HWND of the parent window (can be null also)
				NULL, // Handle to menu
				hInstance, // Handle to application instance
				NULL); // Pointer to window creation data

	// Check if window creation was successful
	if (!hwndMain)
		return 0;

	// Make the window visible
	ShowWindow(hwndMain,SW_SHOW);

	// Process messages coming to this window
	while (GetMessage(&msg,NULL,0,0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// return value to the system
	return msg.wParam;
 }

 LRESULT CALLBACK MainWndProc(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam)
 {
	switch (msg)
	{
		case WM_DESTROY:
			// User closed the window
			PostQuitMessage(0);
			break;
		default:
			// Call the default window handler
			return DefWindowProc(hwnd,msg,wParam,lParam);
	}
	return 0;
 }
