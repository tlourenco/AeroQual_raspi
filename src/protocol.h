#ifndef PROTOCOL_H_   /* Include guard */
#define PROTOCOL_H_
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>


#define START     0xAA
#define DLE       0x10
#define SPLIT     0x80
#define STX       0x02
#define SOH       0x01
#define EOT       0x04
#define ACK       0x06
#define SPACE     0x20
#define ERROR     0x1B

#define FALSE 0
#define TRUE  1

struct info_sens{
  unsigned char *serie;
  unsigned char *sensor;
  unsigned char *othersens;
  unsigned char *info1;
  unsigned char *info2;
};


void intHandler(int dummy);
void *Aeroqual1(void *arg);

struct info_sens connect(int tty_fd, struct info_sens conn);
int loggin_on(int tty_fd,  unsigned char *sensor, int type, unsigned char *sens1);

unsigned char *readRX(int tty_fd, int size);
// int sendTX(int tty_fd, char *command);

unsigned char *parse_msg(unsigned char *msg);
int parse_logon(unsigned char *msg, sqlite3 *db);


int get_othersens(unsigned char *othersens);
#endif // PROTOCOL_H_
