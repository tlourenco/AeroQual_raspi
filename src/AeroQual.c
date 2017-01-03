#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include "protocol.h"
#include <pthread.h>


//
/* This is our thread function.  It is like main(), but for a thread */
void *Aeroqual1(void *arg)
{
  struct info_sens conn;
  struct termios tio;
  struct termios old_stdio;
  int tty_fd, j = 0;

  tcgetattr(STDOUT_FILENO,&old_stdio);

  memset(&tio,0,sizeof(tio));
 // printf("entrou\n");
  memset(&tio,0,sizeof(tio));
  tio.c_iflag=0;
  tio.c_oflag=0;
  tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
  tio.c_lflag=0;
  tio.c_cc[VMIN]=1;
  tio.c_cc[VTIME]=0;

  tty_fd=open(arg, O_RDWR | O_NOCTTY);

  cfsetospeed(&tio,B19200);            // 115200 baud
  cfsetispeed(&tio,B19200);            // 115200 baud
  tcflush(tty_fd, TCIFLUSH);

  if (tcsetattr(tty_fd, TCSAFLUSH, &tio)!= 0)
  {
      perror("Error message");
      perror("1Error");
  }


  tcsetattr(tty_fd,TCSANOW,&tio);
  conn = connect(tty_fd, conn);

  j = get_othersens(conn.othersens);
  if (j == 0){
      perror("2Error");
  }

  // printf("Type = %d\n", j);
  // printf("$%s\n", os.sens2);
  // printf("$%s\n", os.sens3);
  loggin_on(tty_fd, conn.sensor, j, conn.othersens);

  close(tty_fd);
  tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
  return NULL;

}


// gcc AeroQual.c -o main -fno-builtin-exit
int main(int argc,char** argv)
{

  pthread_t pth1;	// this is our thread identifier
  pthread_t pth2;	// this is our thread identifier
  pthread_t pth3;	// this is our thread identifier

  /* Create worker thread */
  usleep(200000);
  pthread_create(&pth1,NULL,Aeroqual1, argv[1]);
  usleep(400000);
  pthread_create(&pth2,NULL,Aeroqual1, argv[2]);
  usleep(550000);
  pthread_create(&pth3,NULL,Aeroqual1, argv[3]);
  usleep(200000);

  /* wait for our thread to finish before continuing */
  pthread_join(pth1, NULL);
  pthread_join(pth2, NULL);
  pthread_join(pth3, NULL);

        return EXIT_SUCCESS;
}
