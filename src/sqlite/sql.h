#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

int execute_sql_statement(char *sql, sqlite3 *db);
sqlite3 *connect_sqlite();
// static int callback(void *NotUsed, int argc, char **argv, char **azColName);


#define O3SQL "INSERT INTO airquality (deployment_id, seconds, o3_data, o3_calib) VALUES ('1','%ld','%f','%f')"
#define COSQL "INSERT INTO co (deployment_id, seconds, co_data, co_calib) VALUES ('1','%ld','%f','%f')"
#define NO2SQL "INSERT INTO no2 (deployment_id, seconds, no2_data, no2_calib) VALUES ('1','%ld','%f','%f')"
#define BASICSQL "INSERT INTO basic_environment (deployment_id, seconds, temperature_data, temperature_calib, humidity_data, humidity_calib) VALUES ('1','%ld','%f','%f','%f','%f')"
