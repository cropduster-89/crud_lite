#ifndef CL_DATABASE_H
#define CL_DATABASE_H

#define FIELD_SIZE 48
/*
*	DB<->program interchange struct. All
*	fields are the same size in order to make 
* 	looping through read/writes easier 	
*/
struct customer_record {
	
	int64_t id;
	
	char fName[FIELD_SIZE];	
	char lName[FIELD_SIZE];	
	char addr1[FIELD_SIZE];	
	char addr2[FIELD_SIZE];	
	char addr3[FIELD_SIZE];	
	char postcode[FIELD_SIZE];	
	char telephone[FIELD_SIZE];	
	char email[FIELD_SIZE];
};

struct customer_search_results {
	
	int32_t hits;
	int32_t added;
	struct customer_record *records;	
};

#endif