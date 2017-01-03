#include "protocol.h"
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include "sqlite/sql.h"

static volatile int keepRunning = 1;

void intHandler(int dummy) {
    printf("Turn off\n");
    keepRunning = 0;
}
//verify connection
unsigned char MODEL[] =  {0x55, 0xFA, 0x00,0xB1};//model 16
unsigned char SENSORID[] =  {0x55, 0xFB, 0x00,0xB0};//senors16
unsigned char OTHERSEN[] =  {0x55, 0xF9, 0x00,0xB2};//other sensor 28
unsigned char TRASH1[] =  {0x55, 0xF8, 0x00,0xB3};//trash 43
unsigned char TRASH2[] =  {0x55, 0x18, 0x00,0x93};//trash 28

// loggin real time measurment
unsigned char LOGON[] =  {0x55, 0x1B, 0x00,0x90};//log on 69
unsigned char ACKLOG[] =  {0x55, 0x0D, 0x00,0x9E};//dont receive
unsigned char LOGOFF[] =  {0x55, 0x1C, 0x00,0x8F};//see

// Download
unsigned char DOWNLOAD[] =  {0x55, 0x29, 0x00,0x82};
unsigned char ask[] =  {0x55, 0x11, 0x00,0x9A};
unsigned char ack[] =  {0x55, 0x29, 0x00,0x82};

volatile int STOP=FALSE;

struct info_sens connect(int tty_fd, struct info_sens conn){

    int res = 0;
    // struct info_sens conn;

    // printf("MODEL SERIES:\n");
    res = write(tty_fd, MODEL, 4);
    conn.serie = parse_msg(readRX(tty_fd, 16));
    if (res == 0){perror("Conn Error");}
    // printf("*%s\n", conn.serie);
    usleep(500000);
    //
    // printf("SENSOR:\n");
    res = 0;
    res = write(tty_fd, SENSORID, 4);
    conn.sensor = parse_msg(readRX(tty_fd, 16));
    if (res == 0){perror("Conn Error");}
    // if ((strcmp((char *)conn.sensor, "NO2") != 0) || (strcmp((char *)conn.sensor, "O3") != 0) || (strcmp((char *)conn.sensor, "CO") != 0)){
    //   exit(0);
    // }

    // printf("*%s\n", conn.sensor);
    usleep(500000);
    //
    // printf("OTHER SENSOR:\n");
    res = 0;
    res = write(tty_fd, OTHERSEN, 4);
    conn.othersens = parse_msg(readRX(tty_fd, 28));
    if (res == 0){perror("Conn Error");}
    fprintf(stdout,"*%s\n", conn.othersens);
    fflush(stdout);
    usleep(500000);
    // //
    // printf("INFO 1\n");
    res = 0;
    res = write(tty_fd, TRASH1, 4);
    conn.info1 = readRX(tty_fd, 43);
    if (res == 0){perror("Conn Error");}
    // printf("%s\n", conn.info1);
    usleep(1500000);
    // //
    // printf("INFO 2\n");
    res = 0;
    res = write(tty_fd, TRASH2, 4);
    conn.info2 = readRX(tty_fd, 27);
    if (res == 0){perror("Conn Error");}
    // printf("%s\n", conn.info2);
    usleep(500000);
    //

    return conn;
}

int loggin_on(int tty_fd,  unsigned char *sensor, int type, unsigned char *sens1){
    int res = 0, j = 0, fact = 0;
    // char filecsv[30] = {0x00};
    // char header[30] = {0x00};
    sqlite3 *db;


    // FILE *book;
    // sprintf(filecsv, "measurement/%s.csv", sensor);
    // sprintf(header, "seconds,%s\n", sens1);

    db = connect_sqlite();

    // book=fopen(filecsv, "a+");
    // fprintf(book, "%s", header);
    // fclose(book);

    res = write(tty_fd, LOGON, 4);
    //loop
    signal(SIGTERM, intHandler);
    while(keepRunning){
      fprintf(stdout,"sample [%d] ", j);
      if(type == 1){fact = 20;}
      if(type == 3){fact = 69;}
      usleep(50000);
      parse_logon(readRX(tty_fd, fact), db);
      usleep(50000);
      res = write(tty_fd, ACKLOG, 4);
      j++;
    }

    res = 0;
    sqlite3_close(db);
    fprintf(stdout,"DESABLE LOG\n");
    res = write(tty_fd, LOGOFF, 4);
    if (res != 0){fprintf(stdout,"LOG OFF\n");}
    return 1;
}

unsigned char *readRX(int tty_fd, int size){
  unsigned char rx = 0x00;
  unsigned char *buf = malloc (sizeof (unsigned char) * 70);
  if (buf == NULL){
      perror("Error:");
  }
  // buf = malloc(size);

  int i = 0;
  STOP=FALSE;

  tcflush(tty_fd, TCIOFLUSH);
  // printf("RX = [");

  while (STOP==FALSE) {
      read(tty_fd, &rx, 1);
      if ((i == 0) && (rx != START)){return (unsigned char *)ERROR;}
      // printf("0x%02X ", rx);
      buf[i] = rx;
      i++;
      if (i == size){STOP=TRUE;}
  }
  // printf("]\n");

  return(buf);
}

unsigned char *parse_msg(unsigned char *msg){
  unsigned char *buf = malloc (sizeof (unsigned char) * 70);
  int i=0, j = 0;

  STOP=FALSE;

  // book=fopen("measurement/example.csv", "w+");
  if (msg[1] == 0xFA){
    i = 6;
    while (msg[i] != 0x00) {
      buf[j] = msg[i];
      j++;
      i++;
    }
    free(msg);
    return buf;
  }
  else if (msg[1] == 0xFB){
    i = 5;
    while (msg[i] != 0x20) {
      buf[j] = msg[i];
      j++;
      i++;
    }
    free(msg);
    return buf;
  }
  else if (msg[1] == 0xF9){
    i = 3;
    int n = 0;
    while (n<2){
      if (msg[i] == 0x00){n++;buf[j] = 0x2C;j++;}
      else if (msg[i] == 0x01){buf[j] = 0x2C;j++;}
      else if ((msg[i] != 0x03) && (msg[i] != 0x02)  && (msg[i] != 0x04)&& (msg[i] != 0x20)){
        buf[j] = msg[i];
        j++;
      }
      i++;
    }
    free(msg);
    return buf;
  }
  else if (msg[6] == 0x10){
    i = 0;
    while (STOP==FALSE){
      buf[j] = msg[i];
      j++;
      i++;
      if (msg[i] == 0xAA){STOP=TRUE;}
    }
    free(msg);
    return buf;
  }
  free(msg);
  return (unsigned char *)ERROR;
}

int parse_logon(unsigned char *msg, sqlite3 *db){
  char query[500];
  char query2[500];
  time_t seconds;
  seconds = time (NULL);
  unsigned char *sensor = NULL;
  unsigned char *measurement = NULL;
  float m1 = 0, m2 = 0, m3 = 0;
  int k = 0, x = 3, i = 0, j = 0;
  STOP=FALSE;

  sensor = malloc (sizeof (unsigned char) * 7);
  measurement = malloc (sizeof (unsigned char) * 5);

  if ((sensor == NULL) || (measurement == NULL) || (msg == NULL)){
    perror("4Error");
  }


  for(x = 0; x < 3; x++){
    for(i = 1+k; i < 8+k; i++){
      if(msg[i] == 0x20){break;}
      sensor[j] = msg[i];
      j++;
    }
    msg[i+1] = '\0';
    i = 0;
    j = 0;
    for(i = 13+k; i < 18+k; i++){
      measurement[j] = msg[i];
      j++;
    }

    if (x == 0){
      m1 = *((float*)measurement);
      fprintf(stdout,"[%s],%f,",sensor, m1);
      k = 23;
    }
    if(x == 1){
      m2 = *((float*)measurement);
      fprintf(stdout,"%f,", m2);
      k = k+23;
    }
    if(x == 2){
      m3 = *((float*)measurement);
      fprintf(stdout,"%f [", m3);
    }

  }
  if (sensor[1] == 'N'){
    sprintf(query, NO2SQL, seconds, m1, m1);
    execute_sql_statement(query, db);
    // printf("SQL = %s\n", query);
  }
  else if (sensor[1] == 'C'){
    sprintf(query, COSQL, seconds, m1, m1);
    execute_sql_statement(query,db);

    // printf("SQL = %s\n", query);
  }
  else if (sensor[1] == 'O'){
    sprintf(query, O3SQL, seconds, m1, m1);
    sprintf(query2, BASICSQL, seconds, m2, m2, m3, m3);
    execute_sql_statement(query, db);
    execute_sql_statement(query2, db);

    // printf("SQL = %s\n", query);
    // printf("SQL = %s\n", query2);
  }
  else{
    perror("SQL_Error");
    return 1;
  }
  free(sensor);
  free(measurement);
  return 0;
}


int get_othersens(unsigned char *othersens){
  int i = 0, j = 0;

  while(i <= 10){
    // printf("entro\n");
    // printf("%s\n", othersens);s
    if((othersens[i] == 0x2C) && (othersens[i+1] != 0x2C)){
      j++;
      i++;
      // printf("%d\n ",j);
     }else{i++;}
   }

  return j;
}
