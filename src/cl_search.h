#ifndef CL_SEARCH_H
#define CL_SEARCH_H

struct search_positions {
	RECT headerRect;	
}; 

struct search_state {
	struct search_positions pos;
	struct customer_search_results results;	
	
	HWND mainSearch;
	HWND searchText;
	HWND searchSelect;
	HWND searchBtn;
	HWND searchList;
};

#endif
