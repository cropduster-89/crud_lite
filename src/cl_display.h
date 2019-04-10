#ifndef CL_DISPLAY_H
#define CL_DISPLAY_H

struct display_positions {
	RECT headerRect;	
	RECT nameRect;
	RECT addrRect; 
	RECT telephoneRect;	
	RECT emailRect;	
};

struct display_state {
	struct display_positions pos;
	
	char nameBuf[100];
	char addrBuf[200];
	char telephoneBuf[50];
	char emailBuf[50];
	
	HWND mainDisplay;

	HWND editButton;	
	HWND deleteButton;	
};

#endif
