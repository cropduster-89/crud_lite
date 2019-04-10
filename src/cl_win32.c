#include <assert.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <sqlite3.h>

#include "resource.h"
#include "cl_database.h"
#include "cl_edit.h"
#include "cl_display.h"
#include "cl_search.h"
#include "cl_win32.h"
#include "cl_utils.h"

void win32_ClearRecords(struct customer_search_results *results);
struct gui_state *win32_GetState(HWND window);
POINT win32_IncrementPoint(POINT base, int32_t x, int32_t y);
RECT win32_MakeRect(POINT base, int32_t x, int32_t y);
void display_Open(struct gui_state *gs);
void edit_Open(struct gui_state *gs);
void search_Open(struct gui_state *gs);

#include "cl_database.c"
#include "cl_display.c"
#include "cl_edit.c"
#include "cl_search.c"

extern struct gui_state *win32_GetState(HWND window)
{
	HWND parent = window, temp = window;	
	
	while(1) {
		temp = GetParent(temp);
		if(temp) {
			parent = temp;
		} else {break;}
	}	
	struct gui_state *gs = (struct gui_state *)GetClassLongPtr(parent, 0);
	assert(gs);
	
	return(gs);
}

extern void win32_ClearRecords(struct customer_search_results *results)
{
	if(results->records) {
		free(results->records); 		
	}
	results->hits = 0;
	results->added = 0;
}

/*
*	Turns an XY point into a XYXY (left,top,right,bottom) RECT
*
*	A win32 rect is calculated from the top left 
*	corner, and higher x/y values == further towards
*	the right/bottom:
*
*     lt/xy___________
*	|	     |		
*	|___________rb/xy
*/
extern inline RECT win32_MakeRect(POINT base,
				  int32_t x,
				  int32_t y)
{
	RECT result = {
		base.x, 
		base.y, 
		base.x + x, 
		base.y + y		
	};
	
	return(result);
}

extern inline POINT win32_IncrementPoint(POINT base,
					 int32_t x,
					 int32_t y)
{
	base.x += x;
	base.y += y;
	
	return(base);
}

static bool win32_Init(HWND window,
		       struct gui_state *gs)
{
	//	Load the fonts that will be used
	
	HDC dc = GetDC(window);
	int32_t headerFontSize = -MulDiv(15, GetDeviceCaps(dc, LOGPIXELSY), 72);
	int32_t stdFontSize = -MulDiv(10, GetDeviceCaps(dc, LOGPIXELSY), 72);
	ReleaseDC(window, dc);
	
	gs->hdrFont = CreateFont(headerFontSize, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0 ,0 ,0 ,0, "Tahoma");
	assert(gs->hdrFont);
	gs->stdFont = CreateFont(stdFontSize, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0 ,0 ,0, "Tahoma");
	assert(gs->stdFont);
	gs->errFont = CreateFont(stdFontSize, 0, 0, 0, FW_BOLD, TRUE, 0, 0, 0, 0 ,0 ,0 ,0, "Tahoma");
	assert(gs->errFont);
			
	//	Init the toolbar, always displayed.
			
	gs->toolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL,
		WS_CHILD|WS_VISIBLE|WS_BORDER|
		TBSTYLE_TRANSPARENT|TBSTYLE_FLAT, 0, 0, 0, 0,
		window, NULL, gs->hInstance, NULL);
	if(!gs->toolbar) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}	
	
	const int32_t buttonCount = 2;
	const int32_t bmpSize = 32;		
	
	HIMAGELIST imageList = ImageList_Create(bmpSize, bmpSize, 
		ILC_COLOR24|ILC_MASK|ILC_COLORDDB, 2, 0);
	assert(imageList);		
							
	TBBUTTON buttons[2] = {};
	for(int32_t i = 0; i < buttonCount; ++i) {	
		
		HBITMAP bmp = LoadBitmap(gs->hInstance, 
			MAKEINTRESOURCE(TBTN_SEARCH + i));
		assert(bmp);	
		ImageList_Add(imageList, bmp, 0);		
		buttons[i].iBitmap = i;
		buttons[i].idCommand = WINDOW_TOOLBTN_SEARCH + i;
		buttons[i].fsState = TBSTATE_ENABLED;
		buttons[i].fsStyle = BTNS_BUTTON;
		DeleteObject(bmp); 
	}	
	SendMessage(gs->toolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)imageList);			
	SendMessage(gs->toolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);		
	SendMessage(gs->toolbar, TB_ADDBUTTONS, (WPARAM)buttonCount, (LPARAM)&buttons);			
	SendMessage(gs->toolbar, TB_AUTOSIZE, 0, 0);	
	
	//	Create secondary window panels 
	
	WNDCLASSEX wcPanel = {};
	wcPanel.cbSize = sizeof(WNDCLASSEX);		
	wcPanel.style = CS_VREDRAW|CS_HREDRAW|CS_PARENTDC;		
	wcPanel.hInstance = gs->hInstance;				
	wcPanel.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	
	RECT clientRect;
	GetClientRect(window, &clientRect);
	
	RECT pRect = {
		clientRect.left += DEF_OFFSET,
		clientRect.top += 50,
		clientRect.right -= DEF_OFFSET * 2,
		clientRect.bottom -= DEF_OFFSET + DEF_HEADERY,
	};			
	
	WNDCLASSEX wcEdit = wcPanel;
	wcEdit.lpfnWndProc = edit_Proc;
	wcEdit.lpszClassName = "WINCLASS_EDIT";
	if(!RegisterClassEx(&wcEdit)) {printf("Register\n");}	
	gs->edit.mainInput = CreateWindowEx(0, wcEdit.lpszClassName, NULL, 
		WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER, 		  
		pRect.left, pRect.top, pRect.right, pRect.bottom, 
		gs->mainWindow, 0, gs->hInstance, 0);
	if(!gs->edit.mainInput) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}		
	
	//	Initialisation completed in the panels
	//	stated window Procs	
	
	WNDCLASSEX wcSearch = wcPanel;
	wcSearch.lpfnWndProc = search_Proc;
	wcSearch.lpszClassName = "WINCLASS_SEARCH";
	if(!RegisterClassEx(&wcSearch)) {printf("Register\n");}	
	gs->search.mainSearch = CreateWindowEx(0, wcSearch.lpszClassName, NULL,
		WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|WS_VISIBLE, 		    
		pRect.left, pRect.top, pRect.right, pRect.bottom, 
		gs->mainWindow, 0, gs->hInstance, 0);
	if(!gs->search.mainSearch) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}		
	
	WNDCLASSEX wcDisplay = wcPanel;		
	wcDisplay.lpfnWndProc = display_Proc;
	wcDisplay.lpszClassName = "WINCLASS_DISPLAY";
	if(!RegisterClassEx(&wcDisplay)) {printf("Register\n");}	
	gs->display.mainDisplay = CreateWindowEx(0, wcDisplay.lpszClassName, NULL, 
		WS_CHILD|WS_CLIPSIBLINGS|WS_BORDER|WS_VISIBLE, 		  
		pRect.left, pRect.top, pRect.right, pRect.bottom, 
		gs->mainWindow, 0, gs->hInstance, 0);
	if(!gs->display.mainDisplay) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}	
	
	
	return(true);
}

LRESULT CALLBACK win32_Proc(HWND window,
			    UINT msg,
			    WPARAM wParam,
			    LPARAM lParam)
{
	LRESULT result = 0;
	
	switch(msg) {	
	case WM_DESTROY: {
		
		struct gui_state *gs = (struct gui_state *)GetClassLongPtr(window, 0);
		free(gs);	
		PostQuitMessage(0);			
		
		break;
		
	} case WM_CREATE: {
		
		// 	The WM_CREATE message is called when the main window is created,
		//	and all subsequent initialisation tasks are performed either here
		// 	or in subclassed WM_CREATE procs.	
		
		struct gui_state *gs = calloc(1, sizeof(struct gui_state));		
		SetClassLongPtr(window, 0, (LONG_PTR)gs);		
		gs->mainWindow = window;
		gs->hInstance = (HINSTANCE)GetModuleHandle(NULL);
		if(!database_InitDatabase()) {printf("db connection error\n");}				
		
		if(!win32_Init(window, gs)) {
			MessageBox(NULL, TEXT("Edit Window creation failed," \
			 "startup aborted.\n"), NULL, MB_ICONERROR);
			return(1);
		}	
		
		BITSET(gs->stateFlags, FLAGS_RUNNING);
		
		break;	
		
	} case WM_COMMAND: {
		
		if(LOWORD(wParam) == WINDOW_TOOLBTN_NEW) {
			
			struct gui_state *gs = win32_GetState(window);	
			BITCLEAR(gs->stateFlags, FLAGS_EDIT);
			edit_Open(gs);	
			
		} else if(LOWORD(wParam) == WINDOW_TOOLBTN_SEARCH) {
			
			struct gui_state *gs = win32_GetState(window);		
			ListView_DeleteAllItems(gs->search.searchList);	
			ShowWindow(gs->edit.mainInput, SW_HIDE);
			ShowWindow(gs->display.mainDisplay, SW_HIDE);
			ShowWindow(gs->search.mainSearch, SW_SHOW);
		}		
		break;
		
	} default: {
		result = DefWindowProcA(window, msg, wParam, lParam);
	}	
	}
	
	return(result);
}
		
/*
*	Entry point, and message loop.
*/
		
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