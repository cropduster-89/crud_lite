/*
*	Adds strings the edit window's controls. blank if it's a new record,
*	existing stings from the search state if it's an exsisting one.
*/
#define edit_InitInput(gs) (_PopulateIntput(gs, (BITCHECK((gs)->stateFlags, FLAGS_EDIT))))
static void _PopulateIntput(struct gui_state *gs,
			    bool existing)
{
	for(int32_t i = 0; i < INPUT_COUNT; ++i) {
		Edit_SetText(gs->edit.inputText[i], 
			existing ? gs->record.fName + (FIELD_SIZE * i) : "");
	}
}

static void edit_SetRecord(struct gui_state *gs)
{
	
	for(int32_t i = 0; i < INPUT_COUNT; ++i) {
		Edit_GetText(gs->edit.inputText[i], 
			gs->record.fName + (FIELD_SIZE * i), FIELD_SIZE);
	}
}

/*
*	Copies data from Edit controls into the struct
*	sned to the database, commits it, and 
*	checks that the operation was successful
*/
static bool edit_Commit(struct gui_state *gs)
{
	bool result = false;	
	
	struct customer_record *newRecord = 
		calloc(1, sizeof(struct customer_record));
	if(!newRecord) {return(result);}
	
	for(int32_t i = 0; i < INPUT_COUNT; ++i) {		
		char *dest = newRecord->fName + (sizeof(char) * FIELD_SIZE * i);
		Edit_GetText(gs->edit.inputText[i], dest, FIELD_SIZE - 1);
	}	
	if(BITCHECK(gs->stateFlags, FLAGS_EDIT)) {
		newRecord->id = gs->record.id;
		if(database_Update(newRecord)) {result = true;}
		else{printf("Commit failed\n");}
	} else {
		if(database_Commit(newRecord)) {result = true;}
		else{printf("Commit failed\n");}
	}
		
	free(newRecord);			
	return(result);
}

/*
*	Checks that all required fields are not empty,
*	and sets error flags (used for rendering error test)
*	are set & unset accordingly
*/
static bool edit_CheckPreCommit(struct gui_state *gs)
{
	int32_t test1 = 0, test2 = 0, test3 = 0, test4 = 0;
	struct input_state *is = &gs->edit;
	
	test1 = Edit_GetTextLength(is->inputText[INPUT_LNAME]);
	if(!test1) {		
		BITSET(is->inputState, ERROR_NONAME);		
	} else BITCLEAR(is->inputState, ERROR_NONAME);
	
	test2 = Edit_GetTextLength(is->inputText[INPUT_ADDR1]);
	if(!test2) {		
		BITSET(is->inputState, ERROR_NOADDR);		
	} else BITCLEAR(is->inputState, ERROR_NOADDR);
	
	test3 = Edit_GetTextLength(is->inputText[INPUT_POSTCODE]);
	if(!test3) {		
		BITSET(is->inputState, ERROR_NOPOSTCODE);		
	} else BITCLEAR(is->inputState, ERROR_NOPOSTCODE);
	
	test4 = Edit_GetTextLength(is->inputText[INPUT_TELEPHONE]);
	if(!test4) {		
		BITSET(is->inputState, ERROR_NOPHONENO);		
	} else BITCLEAR(is->inputState, ERROR_NOPHONENO);
	
	bool result = (test1 && test2 && test3 && test4);
	if(!result) RedrawWindow(is->mainInput, 0, 0, RDW_INVALIDATE);
		
	return(result);
}

extern void edit_Open(struct gui_state *gs)
{
	edit_InitInput(gs);
	ShowWindow(gs->search.mainSearch, SW_HIDE);
	ShowWindow(gs->display.mainDisplay, SW_HIDE);
	ShowWindow(gs->edit.mainInput, SW_SHOW);
}

static bool edit_Init(HWND window,
		      struct gui_state *gs)
{
	RECT inputRect = {};
	if(!GetClientRect(window, &inputRect)) {
		printf("Could not create window\n");
	}
					
	const POINT rowHeader = {DEF_OFFSET, DEF_OFFSET};	
	gs->edit.pos.headerRect = win32_MakeRect(rowHeader, DEF_HEADERX, DEF_HEADERY);
	
	// 	Column 0 : 3 text labels 
	
	const POINT col0Base = {rowHeader.x, rowHeader.y + DEF_HEADERY + DEF_OFFSET};	
	POINT col0 = col0Base;		
	gs->edit.pos.nameRect = win32_MakeRect(col0, DEF_TEXTX, DEF_EDITY); 
			
	col0 = win32_IncrementPoint(col0, 0, DEF_EDITY + DEF_OFFSET);
	gs->edit.pos.addrRect = win32_MakeRect(col0, DEF_TEXTX, DEF_EDITY);
	
	col0 = win32_IncrementPoint(col0, 0, (DEF_EDITY + DEF_OFFSET) * 3);
	gs->edit.pos.postCodeRect = win32_MakeRect(col0, DEF_TEXTX, DEF_EDITY);
	
	// 	Column 1 : 5 edit controls 
	
	const POINT col1Base = win32_IncrementPoint(col0Base, DEF_TEXTX + DEF_OFFSET, 0);	
	POINT col1 = col1Base;		
	gs->edit.inputText[INPUT_FNAME] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col1.x, col1.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_FNAME]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
			
	col1 = win32_IncrementPoint(col1, 0, DEF_EDITY + DEF_OFFSET);
	gs->edit.inputText[INPUT_ADDR1] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col1.x, col1.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_ADDR1]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}	
	
	col1 = win32_IncrementPoint(col1, 0, DEF_EDITY + DEF_OFFSET);
	gs->edit.inputText[INPUT_ADDR2] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col1.x, col1.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_ADDR2]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	
	col1 = win32_IncrementPoint(col1, 0, DEF_EDITY + DEF_OFFSET);
	gs->edit.inputText[INPUT_ADDR3] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col1.x, col1.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_ADDR3]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	
	col1 = win32_IncrementPoint(col1, 0, DEF_EDITY + DEF_OFFSET);
	gs->edit.inputText[INPUT_POSTCODE] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col1.x, col1.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_POSTCODE]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	
	//	Column 2: 1 edit control and 2 error text labels
	
	const POINT col2Base = win32_IncrementPoint(col1Base, DEF_EDITX + DEF_OFFSET, 0);	
	POINT col2 = col2Base;		
	gs->edit.inputText[INPUT_LNAME] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col2.x, col2.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_FNAME]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	
	col2 = win32_IncrementPoint(col2, 0, DEF_EDITY + DEF_OFFSET);
	gs->edit.pos.addrErrorRect = win32_MakeRect(col2, DEF_TEXTX, DEF_TEXTY);
	
	col2 = win32_IncrementPoint(col2, 0, (DEF_TEXTY + DEF_OFFSET) * 3);
	gs->edit.pos.postCodeErrorRect = win32_MakeRect(col2, DEF_TEXTX, DEF_TEXTY);
	
	const POINT col3Base = win32_IncrementPoint(col2Base, DEF_EDITX + DEF_OFFSET, 0);
	
	POINT col3 = col3Base;
	gs->edit.pos.nameErrorRect = win32_MakeRect(col3, DEF_TEXTX, DEF_TEXTY);
		
	col3 = win32_IncrementPoint(col3, 0, (DEF_TEXTY + DEF_OFFSET) * 3);	
	gs->edit.pos.telephoneRect = win32_MakeRect(col3, DEF_TEXTX, DEF_TEXTY);
	
	col3 = win32_IncrementPoint(col3, 0, DEF_TEXTY + DEF_OFFSET);	
	gs->edit.pos.emailRect = win32_MakeRect(col3, DEF_TEXTX, DEF_TEXTY);
	
	const POINT col4Base = win32_IncrementPoint(col3Base, DEF_TEXTX + DEF_OFFSET, 0);
	POINT col4 = win32_IncrementPoint(col4Base, 0, (DEF_TEXTY + DEF_OFFSET) * 3);		
	
	gs->edit.inputText[INPUT_TELEPHONE] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col4.x, col4.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_TELEPHONE]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	
	col4 = win32_IncrementPoint(col4, 0, DEF_TEXTY + DEF_OFFSET);	
	gs->edit.inputText[INPUT_EMAIL] = CreateWindowEx(0, WC_EDIT, NULL, 
		WS_CHILD|WS_VISIBLE|WS_BORDER, col4.x, col4.y, DEF_EDITX, DEF_EDITY, 
		window, NULL, gs->hInstance, NULL);
	if(!gs->edit.inputText[INPUT_EMAIL]) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}
	
	const POINT col5Base = win32_IncrementPoint(col4Base, DEF_EDITX + DEF_OFFSET, 0);
	POINT col5 = col5Base;		
	col5 = win32_IncrementPoint(col5, 0, (DEF_TEXTY + DEF_OFFSET) * 3);	
	gs->edit.pos.telephoneErrorRect = win32_MakeRect(col5, DEF_TEXTX, DEF_TEXTY);
		
	gs->edit.inputSave = CreateWindowEx(0, WC_BUTTON, "Save", WS_CHILD|WS_VISIBLE, 
		inputRect.right - DEF_OFFSET - DEF_BTN_X, 
		inputRect.bottom - DEF_OFFSET - DEF_BTN_Y, DEF_BTN_X, DEF_BTN_Y, 
		window, (HMENU)WINDOW_INPUT_SAVE, gs->hInstance, NULL);
	if(!gs->edit.inputSave) {
		printf("Error: %x\n", (int32_t)GetLastError());
		return(false);
	}	
	SendMessage(gs->edit.inputSave, WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	
	//	Set the font for all input controls
	
	for(int32_t i = 0; i < INPUT_COUNT; ++i) {
		SendMessage(gs->edit.inputText[i], WM_SETFONT, (WPARAM)gs->stdFont, FALSE);
	}
	
	return(true);
}

LRESULT edit_Proc(HWND window,
		 UINT msg,
		 WPARAM wParam,
		 LPARAM lParam)
{
	LRESULT result = 0;	
	
	switch(msg) {	
	case WM_CREATE: {
		
		struct gui_state *gs = win32_GetState(window);	
		assert(gs);
		gs->edit.mainInput = window;
		
		if(!edit_Init(window, gs)) {
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
		char *editText = "Edit Customer Details";
		char *newText = "Enter New Customer Details";
		char *displayedText = BITCHECK(gs->stateFlags, FLAGS_EDIT) ? editText : newText;		
		DrawTextA(dc, displayedText, -1, &gs->edit.pos.headerRect, DT_LEFT);
		
		SelectObject(dc, gs->stdFont);
		DrawTextA(dc, "Name:", -1, &gs->edit.pos.nameRect, DT_RIGHT);
		DrawTextA(dc, "Address:", -1, &gs->edit.pos.addrRect, DT_RIGHT);
		DrawTextA(dc, "Postcode:", -1, &gs->edit.pos.postCodeRect, DT_RIGHT);
		DrawTextA(dc, "Contact No:", -1, &gs->edit.pos.telephoneRect, DT_RIGHT);
		DrawTextA(dc, "E-mail:", -1, &gs->edit.pos.emailRect, DT_RIGHT);
		
		if(gs->edit.inputState) {
			
			SelectObject(dc, gs->errFont);			
			char *errMsg = "Required!";
			if(BITCHECK(gs->edit.inputState, ERROR_NONAME)) {
				DrawTextA(dc, errMsg, -1, &gs->edit.pos.nameErrorRect, DT_LEFT);			
			} if(BITCHECK(gs->edit.inputState, ERROR_NOADDR)) {
				DrawTextA(dc, errMsg, -1, &gs->edit.pos.addrErrorRect, DT_LEFT);		
			} if(BITCHECK(gs->edit.inputState, ERROR_NOPOSTCODE)) {
				DrawTextA(dc, errMsg, -1, &gs->edit.pos.postCodeErrorRect, DT_LEFT);			
			} if(BITCHECK(gs->edit.inputState, ERROR_NOPHONENO)) {
				DrawTextA(dc, errMsg, -1, &gs->edit.pos.telephoneErrorRect, DT_LEFT);			
			}
		}		
		
		ReleaseDC(window, dc);
		EndPaint(window, &ps);	
		
		break;
		
	} case WM_COMMAND: {		
		
		if(LOWORD(wParam) == WINDOW_INPUT_SAVE) {
			
			struct gui_state *gs = win32_GetState(window);
			
			if(!edit_CheckPreCommit(gs)) {break;}
			if(!edit_Commit(gs)) {break;}		
			edit_SetRecord(gs)
			display_Open(gs);	
		}
		
		break;
		
	} default: {
		

		result = DefWindowProcA(window, msg, wParam, lParam);
		break;
		
	}	
	}
	return(result);
}