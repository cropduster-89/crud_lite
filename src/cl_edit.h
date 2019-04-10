#ifndef CL_EDIT_H
#define CL_EDIT_H

enum input_error {
	ERROR_NONAME = 0,
	ERROR_NOADDR = 8,
	ERROR_NOPOSTCODE = 16,
	ERROR_NOPHONENO	= 24
};

enum input_ids {
	INPUT_FNAME,
	INPUT_LNAME,
	INPUT_ADDR1,
	INPUT_ADDR2,
	INPUT_ADDR3,
	INPUT_POSTCODE,
	INPUT_TELEPHONE,
	INPUT_EMAIL,
	
	INPUT_COUNT,
};

struct input_positions {
	RECT headerRect;	
	RECT nameRect;	
	RECT nameErrorRect;	
	RECT addrRect;		
	RECT addrErrorRect;		
	RECT postCodeRect;		
	RECT postCodeErrorRect;		
	RECT telephoneRect;		
	RECT telephoneErrorRect;		
	RECT emailRect;			
};

struct input_state {
	struct input_positions pos;	
	HWND mainInput;
	HWND inputText[INPUT_COUNT];
	HWND inputSave;	
	uint32_t inputState;	
};

#endif
