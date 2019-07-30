
/*
*	Counts the number of results given by
*	a SELECT command.
*/
static int database_CallbackCount(void *data,
				  int argc,
			          char **argv,
			          char **azColname)
{
	int32_t *count = (int32_t *)data; 
	(*count) += 1;
	
	return(0);
}

/*
*	Retrieves the data from a list of records.
*/
static int database_CallbackMultipleReturn(void *data,
					   int argc,
			                   char **argv,
			                   char **azColname)
{
	struct customer_search_results *results =
		(struct customer_search_results *)data;
		
	int32_t k = results->added;
	for(int32_t i = 0; i < argc; ++i) {
		
		char *dest = results->records[k].fName + ((i - 1) * FIELD_SIZE);
		char *src = argv[i];
		
		if(i == 0) {
			char *endPtr;
			results->records[k].id = strtoimax(argv[i], &endPtr, 10);	
		} else if(src) {strcpy(dest, src);}	
		
	}
	
	++results->added;
	assert(results->added <= results->hits);
	
	return(0);
}

/*
*	Retrieves the data from a single record.
*/
static int database_CallbackSingleReturn(void *data,
					 int argc,
			                 char **argv,
			                 char **azColname)
{
	struct customer_record *record =
		(struct customer_record *)data;
		
	for(int32_t i = 0; i < argc; ++i) {
		
		char *dest = record->fName + ((i - 1) * FIELD_SIZE);
		char *src = argv[i];
		
		if(i == 0) {
			char *endPtr;
			record->id = strtoimax(argv[i], &endPtr, 10);	
		} else if(src) {strcpy(dest, src);}	
		
	}
	return(0);
}

/*
*	Deletes a record based on it's id.
*/
extern bool database_Delete(int32_t id)
{
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	int32_t openErr = sqlite3_open("data/cl_db.db", &dbHandle);
	if(openErr) {printf("No DB handle %s\n", sqlite3_errmsg(dbHandle));return(result);}
	
	const char *sqlFormatString =
		"DELETE FROM customer WHERE ID = %d";
		
	char sqlString[strlen(sqlFormatString) + 9];	
	
	sprintf(sqlString, sqlFormatString, id);	
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s\n", err);
	} else {result = true;}	
	
	sqlite3_close(dbHandle);
	return(result);	
}

/*
*	Updates an exsisting record. Currently updates every single field.
*/
extern bool database_Update(struct customer_record *record)
{
	
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	int32_t openErr = sqlite3_open("data/cl_db.db", &dbHandle);
	if(openErr) {printf("No DB handle %s\n", sqlite3_errmsg(dbHandle));return(result);}
	
	const char *sqlFormatString = 
		"UPDATE customer SET FNAME = '%s', LNAME = '%s', ADDR1 = '%s',"	\
		"ADDR2 = '%s', ADDR3  = '%s', POSTCODE = '%s',"			\
		"TELEPHONE  = '%s', EMAIL = '%s' "				\
		"WHERE ID = %d;"						;
		
	char sqlString[strlen(sqlFormatString) + (FIELD_SIZE * INPUT_COUNT)];
	
	sprintf(sqlString, sqlFormatString, record->fName, record->lName, 
		record->addr1, record->addr2, record->addr3, 
		record->postcode, record->telephone, record->email, record->id);
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s\n", err);
	} else {result = true;}	
	
	sqlite3_close(dbHandle);
	return(result);		
}

/*
*	Get a record from it's ID.
*/
extern bool database_GetById(int64_t id,
			     struct customer_record *record)
{
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	int32_t openErr = sqlite3_open("data/cl_db.db", &dbHandle);
	if(openErr) {printf("No DB handle %s\n", sqlite3_errmsg(dbHandle));return(result);}
	
	const char *sqlFormatString = 
		"SELECT * FROM customer WHERE ID = %d;";
	char sqlString[strlen(sqlFormatString) + 9];		
	sprintf(sqlString, sqlFormatString, id);
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, database_CallbackSingleReturn, record, 
	   &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s", err);
	} else {result = true;}
	
	sqlite3_close(dbHandle);
	return(result);		
}

/*
*	Searches for a record by name.
*
*	TODO: sqlite3_exec is currently called twice, so that the result 
*	list can be created with a single malloc, as opposed to a linked list. 
*	There should be a better way to do this.
*/
extern bool database_SearchByName(char *term,
				  int32_t len,
				  struct customer_search_results *results) 
{
	bool result = false;
	
	win32_ClearRecords(results);
	
	sqlite3 *dbHandle = 0;	
	int32_t openErr = sqlite3_open("data/cl_db.db", &dbHandle);
	if(openErr) {printf("No DB handle %s\n", sqlite3_errmsg(dbHandle));return(result);}
	
	const char *sqlFormatString = 
		"SELECT * FROM customer WHERE FNAME LIKE '%%%s'"		\
		"OR LNAME LIKE '%%%s';"						;	
		
	char sqlString[strlen(sqlFormatString) + len * 2];		
	sprintf(sqlString, sqlFormatString, term, term);
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, database_CallbackCount, &results->hits, 
	   &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s", err);
	}	
	
	results->records = calloc(1, sizeof(struct customer_record) * results->hits);
	if(sqlite3_exec(dbHandle, sqlString, database_CallbackMultipleReturn, results, 
	   &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s", err);
	} else {result = true;}
	
	sqlite3_close(dbHandle);
	return(result);
}

/*
*	Creates a new record from the data provided.
*/
extern bool database_Commit(struct customer_record *record)
{
	bool result = false;
	
	sqlite3 *dbHandle = 0;	
	int32_t openErr = sqlite3_open("data/cl_db.db", &dbHandle);
	if(openErr) {printf("No DB handle %s\n", sqlite3_errmsg(dbHandle));return(result);}
	
	const char *sqlFormatString = 
		"INSERT INTO customer (FNAME, LNAME, ADDR1, ADDR2,"		\
		"ADDR3, POSTCODE, TELEPHONE, EMAIL)"				\
		"VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s');"	; 		
	
	char sqlString[strlen(sqlFormatString) + (FIELD_SIZE * INPUT_COUNT)];
	
	sprintf(sqlString, sqlFormatString, record->fName, record->lName, 
		record->addr1, record->addr2, record->addr3, 
		record->postcode, record->telephone, record->email);
	
	char *err;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &err) != SQLITE_OK ) {			
		printf("Sqlite3 error: %s", err);
	} else {result = true;}	
	
	sqlite3_close(dbHandle);
	return(result);
}

/*
*	Create database if none exsists, and create tables.
*	Citical error if this fails.
*/
extern bool database_InitDatabase(void)
{
	sqlite3 *dbHandle = 0;	
	sqlite3_open("data/cl_db.db", &dbHandle);	
	if(!dbHandle) {printf("No DB handle\n");return(false);}
	
	const char *sqlString = {
		"CREATE TABLE IF NOT EXISTS customer(\
		 ID INTEGER PRIMARY KEY,\
		 FNAME	TEXT,\
		 LNAME TEXT NOT NULL,\
		 ADDR1 TEXT NOT NULL,\
		 ADDR2 TEXT,\
		 ADDR3 TEXT,\
		 POSTCODE TEXT NOT NULL,\
		 TELEPHONE TEXT NOT NULL,\
		 EMAIL TEXT);"
	};
	
	char *err;
	bool tableExists = true;
	if(sqlite3_exec(dbHandle, sqlString, 0, 0, &err) != SQLITE_OK ) {	
		tableExists = false;
		printf("Sqlite3 error: %s", err);
	}
	
	sqlite3_close(dbHandle);	
	return(dbHandle && tableExists);	
}