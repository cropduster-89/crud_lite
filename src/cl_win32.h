#ifndef CL_WIN32_H
#define CL_WIN32_H

#define DEF_EDITX 150
#define DEF_EDITY 25
#define DEF_TEXTX 75
#define DEF_TEXTY 25
#define DEF_HEADERX 300
#define DEF_HEADERY 50
#define DEF_DISPLAY_NAME_X 305
#define DEF_DISPLAY_NAME_Y 25
#define DEF_DISPLAY_ADDR_X 150
#define DEF_DISPLAY_ADDR_Y 100
#define DEF_BTN_X 50
#define DEF_BTN_Y 40
#define DEF_OFFSET 5

/*
*	These enums are used exclusively for their windows 
*	HMENU parameter, for use during WM_COMMAND and 
*	WM_NOTIFY messages
*/
enum window_ids {	
	WINDOW_TOOLBTN_SEARCH,
	WINDOW_TOOLBTN_NEW,	
	
	WINDOW_INPUT_SAVE,	
	
	WINDOW_SEARCH_SELECT,
	WINDOW_SEARCH_BTN,
	WINDOW_SEARCH_RESULTS,
	
	WINDOW_DISPLAY_DELETE,
	WINDOW_DISPLAY_EDIT,
};

enum state_flags {
	FLAGS_RUNNING = 0,
	FLAGS_EDIT = 8,
};

struct gui_state {
	HINSTANCE hInstance;
	
	HWND mainWindow;
	HWND toolbar;
	
	uint32_t stateFlags;
	struct customer_record record;
	
	struct input_state edit;	
	struct search_state search;
	struct display_state display;
	
	HFONT stdFont;
	HFONT hdrFont;
	HFONT errFont;
};

#endif