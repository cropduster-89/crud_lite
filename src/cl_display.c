/*
*	Opens the window that displays a record.
*/
extern void display_Open(struct gui_state *gs)
{
	BITSET(gs->stateFlags, FLAGS_EDIT);
	
	struct customer_record *record = &gs->record;
	
	sprintf(gs->display.nameBuf, "%s %s", record->fName, record->lName);
	sprintf(gs->display.addrBuf, "%s\n%s\n%s\n%s", 
		record->addr1, record->addr2, record->addr3, record->postcode);
	sprintf(gs->display.telephoneBuf, "%s", record->telephone);
	sprintf(gs->display.emailBuf, "%s", record->email);
		
	ShowWindow(gs->edit.mainInput, SW_HIDE);
	ShowWindow(gs->search.mainSearch, SW_HIDE);
	ShowWindow(gs->display.mainDisplay, SW_SHOW);
}

static bool display_Init(HWND window, 
			 struct gui_state *gs)
{
	RECT displayRect = {};
	GetClientRect(window, &displayRect);		
			
	const POINT rowHeader = {DEF_OFFSET, DEF_OFFSET};	
	gs->display.pos.headerRect = win32_MakeRect(rowHeader, DEF_HEADERX, DEF_HEADERY);
	
	const POINT col0Base = win32_IncrementPoint(rowHeader, DEF_TEXTX / 2, DEF_HEADERY + DEF_OFFSET);	
	POINT col0 = col0Base;	
	gs->display.pos.nameRect = win32_MakeRect(col0, DEF_DISPLAY_NAME_X, DEF_DISPLAY_NAME_Y);

	col0 = win32_IncrementPoint(col0, 0, DEF_DISPLAY_NAME_Y + DEF_OFFSET);
	gs->display.pos.addrRect = win32_MakeRect(col0, DEF_DISPLAY_ADDR_X, DEF_DISPLAY_ADDR_Y);	
	
	const POINT col1Base = win32_IncrementPoint(col0Base, DEF_EDITX * 2, 0);
	POINT col1 = win32_IncrementPoint(col1Base, 0, (DEF_EDITY + DEF_OFFSET) + 3);
	gs->display.pos.telephoneRect = win32_MakeRect(col1, DEF_EDITX, DEF_EDITY);
			
	col1 = win32_IncrementPoint(col1, 0, DEF_EDITY);
	gs->display.pos.emailRect = win32_MakeRect(col1, DEF_EDITX, DEF_EDITY);
	
	gs->display.deleteButton = CreateWindowEx(0, WC_BUTTON, "Delete", WS_CHILD|WS_VISIBLE, 
		displayRect.right - DEF_OFFSET - DEF_BTN_X, 
		displayRect.bottom - DEF_OFFSET - DEF_BTN_Y, DEF_BTN_X, DEF_BTN_Y, 
		window, (HMENU)WINDOW_DISPLAY_DELETE, gs->hInstance, NULL);
	if(!gs->display.deleteButton) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	SendMessage(gs->display.deleteButton, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	gs->display.editButton = CreateWindowEx(0, WC_BUTTON, "Edit", WS_CHILD|WS_VISIBLE, 
		displayRect.right - (DEF_OFFSET + DEF_BTN_X) * 2, 
		displayRect.bottom - DEF_OFFSET - DEF_BTN_Y, DEF_BTN_X, DEF_BTN_Y, 
		window, (HMENU)WINDOW_DISPLAY_EDIT, gs->hInstance, NULL);
	if(!gs->display.editButton) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	SendMessage(gs->display.editButton, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	return(true);
}

LRESULT display_Proc(HWND window,
		     UINT msg,
		     WPARAM wParam,
		     LPARAM lParam)
{
	LRESULT result = 0;
		
	switch(msg) {	
	case WM_CREATE: {
		
		struct gui_state *gs = win32_GetState(window);	
		assert(gs);
		gs->display.mainDisplay = window;
				
		if(!display_Init(window, gs)) {
			MessageBox(NULL, TEXT("Edit Window creation failed," \
			 "startup aborted.\n"), NULL, MB_ICONERROR);
			return(1);
		}			
		break;		
		
	} case WM_PAINT: {
		
		struct gui_state *gs = win32_GetState(window);
		
		PAINTSTRUCT ps;
		HDC dc = BeginPaint(window, &ps);		
		
		SelectObject(dc, gs->hdrFont);
		DrawTextA(dc, "Customer Details", -1, &gs->display.pos.headerRect, DT_LEFT);
		
		SelectObject(dc, gs->stdFont);
		DrawTextA(dc, gs->display.nameBuf, -1, &gs->display.pos.nameRect, DT_LEFT);
		DrawTextA(dc, gs->display.addrBuf, -1, &gs->display.pos.addrRect, DT_LEFT);
		DrawTextA(dc, gs->display.telephoneBuf, -1, &gs->display.pos.telephoneRect, DT_LEFT);
		DrawTextA(dc, gs->display.emailBuf, -1, &gs->display.pos.emailRect, DT_LEFT);
		
		ReleaseDC(window, dc);
		EndPaint(window, &ps);	
		
		break;
		
	} case WM_COMMAND: {		
		
		if(LOWORD(wParam) == WINDOW_DISPLAY_DELETE) {	

			struct gui_state *gs = win32_GetState(window);
			if(!database_Delete(gs->record.id)){
				MessageBox(NULL, TEXT("Could not delete record!\n"\
					"More useful error messages coming soon."), 
					NULL, MB_ICONERROR);
			} else {
				search_Open(gs);
			}
			
			
		} else if(LOWORD(wParam) == WINDOW_DISPLAY_EDIT) {
			
			struct gui_state *gs = win32_GetState(window);
			BITSET(gs->stateFlags, FLAGS_EDIT);
			edit_Open(gs);
		} 		
		break;
		
	} default: {		

		result = DefWindowProcA(window, msg, wParam, lParam);
		break;
		
	}	
	}
	return(result);
}