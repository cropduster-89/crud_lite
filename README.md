# Crud_Lite - A sample lightweight CRUD app, written in C11 with Win32 and Sqlite3

A simple database program for Windows. Supports various database operations via SQLite3, and can be used with ease thanks to it's Win32 gui.     

## Operation
![Imgur](https://i.imgur.com/mb6l96d.png)

The program opens with the search window, which allows the user to search for record via either their first, or last name. Pressing search with the serch field empty will simply display all records contained within the database.

To select a record, double-click on it in the ListView control. 

![Imgur](https://i.imgur.com/wexBxvs.png)

This will bring up a display of the current record, and all it's stored information (currently in a rather spartan manner).

![Imgur](https://i.imgur.com/zsEqn8r.png)

From here the record can be edited, or deleted. 

![Imgur](https://i.imgur.com/KN9wx3k.png)

The user will be notified if any important information is omitted.


## Building

Win32 supports the concept of a _resource_ (.rc) file, which allows you to add external assets like strings and bitmaps into the .exe file at compile time. While this will result in a bigger binary, it removes the neeed to load assets at runtime, and keep them knocking around on the heap. If your compiler of choice is supported by Visual Studio, the process is very easy and more or less automated through the gui. 
 
The choice of C compilers for windows that support C99/C11 is sadly rather limited. I use MinGW-w64, which happily, comes bundled with a little tool called Windres, which will compile the contents of the resource file to an object file, that you can link to. Thus, the build command is split into 3 stages. 
```
gcc -ggdb -Wall -std=c11 -O3 -Iinclude src/cl_win32.c -c -o build/crud_lite.o
windres -i src/cl_resource.rc -o build/resource.o
gcc -Llib build/crud_lite.o build/resource.o -o crud_lite.exe -luser32 -lgdi32 -lcomctl32 -lsqlite3
```
Currently, the only assets that the .rc file contains are two bitmaps, and two strings. However, there is another, and fairly  important item contained within, the name and path of the _manifest file_. This is a simple XML document, that specifies the version of Common Controls (the bread & butter of a Win32 program) the program will use. If this is included, everything looks fine, but if it isn't, all of the controls take on a distincly retro look that will look out of place on a Windows Vista/7/8/10 desktop.

## Implementation

```c
int CALLBACK WinMain(HINSTANCE hInstance,
		     HINSTANCE hPrevInstance,
		     LPSTR lpCmdLine,
		     int nCmdShow)
{
	INITCOMMONCONTROLSEX commCtrl;
	commCtrl.dwSize = sizeof(INITCOMMONCONTROLSEX);
	commCtrl.dwICC = ICC_TAB_CLASSES|ICC_STANDARD_CLASSES|ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx(&commCtrl);
	
	//	The main window class. cbClsExtra sets space for extra data, 
	//	which we will use to store a pointer to the program's state,
	// 	so we can access it inside WindowProcs without using globals.	
	
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbClsExtra = sizeof(LONG_PTR);
	wc.style = CS_VREDRAW|CS_HREDRAW;
	wc.lpfnWndProc = win32_Proc;
	wc.hInstance = hInstance;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = "WINCLASS_MAINWINDOW";		
	if(!RegisterClassEx(&wc)) {return(1);}	
	
	HWND mainWindow = CreateWindowEx(WS_EX_APPWINDOW, wc.lpszClassName, "Crud Lite", 
		WS_OVERLAPPEDWINDOW|WS_CLIPCHILDREN|WS_VISIBLE, 
		CW_USEDEFAULT, CW_USEDEFAULT, 896, 512, NULL, 0, hInstance, 0);
	
	if(!mainWindow) {PostQuitMessage(0);printf("fail\n");}
		
	MSG msg;	
	while(GetMessage(&msg, NULL, 0, 0) != 0) {				
	
		TranslateMessage(&msg);
		DispatchMessage(&msg);					
	}	
	
	return(0);
}
```
