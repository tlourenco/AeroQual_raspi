#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
   int i;
   for(i=0; i<argc; i++){
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
      fflush(stdout);
   }
   printf("\n");
   return 0;
}

int execute_sql_statement(char *sql, sqlite3 *db){
   char *zErrMsg = 0;
   const char* data = "Callback function called";
   int  rc;

   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      perror("RC Error");
   }else{
      fprintf(stdout, "Save db]\n");
      fflush(stdout);
   }
   return 0;
}


sqlite3 *connect_sqlite(){
   sqlite3 *db;
   int  rc;

   rc = sqlite3_open("/home/pi/UrbansenseClient/urbansense", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      fflush(stderr);
      perror("RC Error");
      exit(0);
   }
   else{
      fprintf(stdout, "Opened database successfully\n");
   }
  //  execute_sql_statement(sql, db);

   return db;
}
