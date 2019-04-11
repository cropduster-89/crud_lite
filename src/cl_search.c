static void win32_AddResultsToList(struct gui_state *gs)
{
	LVITEM lvi = {};
	lvi.mask = LVIF_TEXT;	
		
	struct customer_search_results *results = &gs->search.results;	
	for(int32_t i = 0; i < results->hits; ++i) {		
			
		lvi.iItem = i;
		lvi.iSubItem = 0;
		char idBuffer[10];	
		sprintf(idBuffer, "%i", (int32_t)results->records[i].id);		
		lvi.pszText = idBuffer;
		ListView_InsertItem(gs->search.searchList, &lvi);
		
		lvi.iSubItem = 1;		
		char nameBuffer[100];
		sprintf(nameBuffer, "%s %s", 
			results->records[i].fName, 
			results->records[i].lName);		
		lvi.pszText = nameBuffer;	
		ListView_SetItem(gs->search.searchList, &lvi);		
	}		
}

extern void search_Open(struct gui_state *gs)
{
	win32_ClearRecords(&gs->search.results);
	ListView_DeleteAllItems(gs->search.searchList);		
	ShowWindow(gs->display.mainDisplay, SW_HIDE);
	ShowWindow(gs->edit.mainInput, SW_HIDE);
	ShowWindow(gs->search.mainSearch, SW_SHOW);
}

/*
*	
*/
static bool win32_SearchByName(struct gui_state *gs)
{
	bool result = false;
	
	char searchTerm[FIELD_SIZE]; 
	Edit_GetText(gs->search.searchText, searchTerm, FIELD_SIZE - 1);	
	int32_t count =	Edit_GetTextLength(gs->search.searchText);
	
	if(database_SearchByName(searchTerm, count, &gs->search.results)) {
		result = true;
	}
	return(result);
} 

static bool search_Init(HWND window,
			struct gui_state *gs)
{
	RECT searchRect = {};
	if(!GetClientRect(window, &searchRect)) {
		printf("Could not create window\n");
	}
	
	const POINT rowHeader = {DEF_OFFSET, DEF_OFFSET};	
	gs->search.pos.headerRect = win32_MakeRect(rowHeader, DEF_HEADERX, DEF_HEADERY);
	
	const POINT row0Base = {rowHeader.x, rowHeader.y + DEF_HEADERY + DEF_OFFSET};
	gs->search.searchText = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, row0Base.x, row0Base.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->search.searchText) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	SendMessage(gs->search.searchText, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	POINT row0 = win32_IncrementPoint(row0Base, DEF_EDITX + DEF_OFFSET, 0);
	gs->search.searchSelect = CreateWindowEx(0, WC_COMBOBOX, "", 
		WS_CHILD|WS_VISIBLE|WS_BORDER|CBS_HASSTRINGS|CBS_DROPDOWNLIST, 
		row0.x, row0.y, DEF_TEXTX, DEF_EDITY, 
		window, (HMENU)WINDOW_SEARCH_SELECT, gs->hInstance, NULL);
	if(!gs->search.searchSelect) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	SendMessage(gs->search.searchSelect, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	char *fields[2][10] = {
		{"By Name"}, {"By ID"}
	};
	for(int32_t i = 0; i < 2; ++i) {
		SendMessage(gs->search.searchSelect, CB_ADDSTRING, 0, (LPARAM)fields[i][0]);
	}
	SendMessage(gs->search.searchSelect, CB_SETCURSEL, 0, 0);
	
	row0 = win32_IncrementPoint(row0, DEF_TEXTX + DEF_OFFSET, 0);
	gs->search.searchBtn = CreateWindowEx(0, WC_BUTTON, "Search", 
		WS_CHILD|WS_VISIBLE, 
		row0.x, row0.y, DEF_TEXTX, DEF_EDITY, 
		window, (HMENU)WINDOW_SEARCH_BTN, gs->hInstance, NULL);
	if(!gs->search.searchBtn) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	SendMessage(gs->search.searchBtn, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	//	Create the listview that holds search results
	
	POINT row1 = win32_IncrementPoint(row0Base, 0, DEF_TEXTY + DEF_OFFSET);
	gs->search.searchList = CreateWindowEx(0 , WC_LISTVIEW, "Search", 
		WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT, 
		row1.x, row1.y, 
		searchRect.right - row1.x - DEF_OFFSET,
		searchRect.bottom - row1.y - DEF_OFFSET , 
		window, (HMENU)WINDOW_SEARCH_RESULTS, gs->hInstance, NULL);
	if(!gs->search.searchList) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	SendMessage(gs->search.searchList, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	//	Extended styles for commctrl controls CANNOT be given to createwindowex,
	//	like extended window styles are. They must be manualy set via msg (or, 
	// 	by helper macro as below)
	
	ListView_SetExtendedListViewStyle(gs->search.searchList, 
		LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);
	
	char listBuffer[97];
	LVCOLUMN lvc;
	int32_t columnCount = 2;
	lvc.mask = LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
	
	for(int32_t i = 0; i < columnCount; ++i) {
		lvc.iSubItem = i;
		lvc.pszText = listBuffer;
		lvc.fmt = LVCFMT_LEFT;
		lvc.cx = (!i) ? 100 : searchRect.right - 110; // Will stop working if more cols are added

		LoadString(gs->hInstance, SCOL_ID + i, listBuffer, ARRAY_COUNT(listBuffer));
		ListView_InsertColumn(gs->search.searchList, i, &lvc);	
	}
	return(true);
}

LRESULT search_Proc(HWND window,
		    UINT msg,
		    WPARAM wParam,
		    LPARAM lParam)
{
	LRESULT result = 0;
		
	switch(msg) {	
	case WM_CREATE: {
		
		struct gui_state *gs = win32_GetState(window);	
		assert(gs);
		gs->search.mainSearch = window;
				
		if(!search_Init(window, gs)) {
			MessageBox(NULL, TEXT("Search Window creation failed," \
			 "startup aborted.\n"), NULL, MB_ICONERROR);
			return(1);
		}			
		break;		
		
	} case WM_PAINT: {
		
		struct gui_state *gs = win32_GetState(window);
		
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(window, &ps);		
		
		SelectObject(dc, gs->hdrFont);
		DrawTextA(dc, "Search Existing Customers", -1, &gs->edit.pos.headerRect, DT_LEFT);
				
		ReleaseDC(window, dc);
		EndPaint(window, &ps);	
		
		break;
		
	} case WM_COMMAND: {
		
		if(LOWORD(wParam) == WINDOW_SEARCH_BTN) {
			
			struct gui_state *gs = win32_GetState(window);			
			ListView_DeleteAllItems(gs->search.searchList);			
			if(win32_SearchByName(gs)) {
				assert(gs->search.results.records);
				win32_AddResultsToList(gs);
			}
		}
		
		break;
		
	} case WM_NOTIFY: {
		
		//	Get record by id from the listview, and display it in the 
		// 	display window	
		
		if(LOWORD(wParam) == WINDOW_SEARCH_RESULTS && 
		   ((LPNMHDR)lParam)->code == NM_DBLCLK) {
			
			LPNMITEMACTIVATE lpnmia = (LPNMITEMACTIVATE)lParam;			
			if(lpnmia->iItem > -1) {
				
				int32_t row = lpnmia->iItem; 
				struct gui_state *gs = win32_GetState(window);				
				char str[9];
				ListView_GetItemText(gs->search.searchList, row, 0, str, 9);
				char *endPtr;
				int32_t id = strtoimax(str, &endPtr, 10);
				
				memset(&gs->record, 0, sizeof(struct customer_record));
				database_GetById(id, &gs->record);	
				
				display_Open(gs);							
			}		
		}				
		break;
		
	} default: {		

		result = DefWindowProcA(window, msg, wParam, lParam);
		break;
		
	}	
	}
	return(result);
}