/* Note from DQ (11/2/2005): I have examined this code closely to verify that it
   does not appear to have any behavior that would be inappropriate for use as 
   an example code within ROSE.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h> /* memset() */
#include <sys/time.h> /* select() */
#include <stdlib.h>
#include <pthread.h>

#define REMOTE_SERVER_PORT 1500
#define REMOTE_SERVER_TCP_PORT 1501
#define MAX_MSG 100
#define exit(m) _exit(m)

void *thread_func_tcp(void *p);
void *thread_func_udp(void *p);

struct hostent *h;

int main(int argc, char *argv[])
{

  int sd,  rc, i;
  struct sockaddr_in cliAddr, remoteServAddr, remoteTCPServAddr;


  /* check command line args */
  if(argc<4) {
    printf("usage : %s <server> <tcp threads num> <udp threads num> [optional: 1 = shutdown server]\n", argv[0]);
    exit(1);
  }

  /* get server IP address (no check if input is IP address or DNS name */
  h = gethostbyname(argv[1]);
  if(h==NULL) {
    printf("%s: unknown host '%s' \n", argv[0], argv[1]);
    exit(1);
  }

  printf("%s: sending data to '%s' (IP : %s) \n", argv[0], h->h_name,
         inet_ntoa(*(struct in_addr *)h->h_addr_list[0]));


  /* bind any port */
  //cliAddr.sin_family = AF_INET;
  //cliAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  //cliAddr.sin_port = htons(0);

  //rc = bind(sd, (struct sockaddr *) &cliAddr, sizeof(cliAddr));
  //if(rc<0) {
  //  printf("%s: cannot bind port\n", argv[0]);
  //  exit(1);
  //}

  pthread_t *tcp_threadVec = NULL;
  int tcp_threads = atoi(argv[2]);

  if (tcp_threads > 0)
  {
  	tcp_threadVec = (pthread_t *)malloc( sizeof(pthread_t)*tcp_threads);
  	for (i = 0; i < tcp_threads; i++)
  	{
  	  if (pthread_create(&tcp_threadVec[i], NULL,  thread_func_tcp, (void*)(i+1)) != 0)
  	  {
  	      printf("failed while creating threads\n");
  	      exit(1);
  	  }
  	}
  }

  pthread_t *udp_threadVec = NULL;
  int udp_threads = atoi(argv[3]);

  if (udp_threads > 0)
  {
  	udp_threadVec = (pthread_t*)malloc( sizeof(pthread_t)*udp_threads);
  	for (i = 0; i < udp_threads; i++)
  	{
  	  if (pthread_create(&udp_threadVec[i], NULL,  thread_func_udp, (void*)(i+1)) != 0)
  	  {
  	      printf("failed while creating threads\n");
  	      exit(1);
  	  }
  	}
  }

  for (i = 0; i < udp_threads; i++)
  {
    pthread_join(udp_threadVec[i], NULL);
  }


  for (i = 0; i < tcp_threads; i++)
  {
//    printf("joining tcp_threads: %d\n", tcp_threads);
	pthread_join(tcp_threadVec[i], NULL);
  }

  if (argc >= 5)
  {
      // give the server time to finish processing the other requests
	  sleep(1);
	  if (! strcmp(argv[4], "1"))
      {
          int sd, rc, j;
		  struct sockaddr_in remoteServAddr;

		  remoteServAddr.sin_family = h->h_addrtype;
	      memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
	   	  remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  		  /* socket creation */
  		  sd = socket(AF_INET,SOCK_DGRAM,0);
  		  if(sd<0)
		  {
    	      printf("cannot close server \n");
	    	  exit(1);
      	  }
	  	  for (j = 0; j < 10; j++)
	  	  {
		  	  rc = sendto(sd, "quit", strlen("quit")+1, 0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
    	  	  if(rc<0)
			  {
                  printf("cannot send server closing request \n");
        	  }
  		  }
  		  close(sd);
      }
  }

}

void *thread_func_udp(void *p)
{
    int sd, rc, j;
	struct sockaddr_in remoteServAddr;

	remoteServAddr.sin_family = h->h_addrtype;
  	memcpy((char *) &remoteServAddr.sin_addr.s_addr, h->h_addr_list[0], h->h_length);
    remoteServAddr.sin_port = htons(REMOTE_SERVER_PORT);

  	/* socket creation */
  	sd = socket(AF_INET,SOCK_DGRAM,0);
  	if(sd<0)
	{
    	printf("cannot open udp socket \n");
    	exit(1);
  	}
    char pNum[40];
	int i = 1000*(int)p;
	for (j = i; j < i+100; j++)
	{
   	sprintf(pNum, "%d", j);
		rc = sendto(sd, pNum, strlen(pNum)+1, 0, (struct sockaddr *) &remoteServAddr, sizeof(remoteServAddr));
    	if(rc<0)
		{
    		printf("cannot send data to udp socket\n");
    		close(sd);
    		exit(1);
      }
      //usleep(10000);
  	}
  	close(sd);
   return NULL;
}

void *thread_func_tcp(void *p)
{
  struct sockaddr_in remoteTCPServAddr;
  int stcp = socket(AF_INET,SOCK_STREAM,0);
  int rc,j;
  if(stcp<0) {
    printf("cannot open tcp socket \n");
    return NULL;
  }

  remoteTCPServAddr.sin_family = h->h_addrtype;
  memcpy((char *) &remoteTCPServAddr.sin_addr.s_addr,
         h->h_addr_list[0], h->h_length);
  remoteTCPServAddr.sin_port = htons(REMOTE_SERVER_TCP_PORT);
//  printf("before connect \n");
  rc = connect(stcp, (struct sockaddr *)&remoteTCPServAddr, sizeof(remoteTCPServAddr));
  if (rc != 0)
  {
    printf("failed to connect to TCP server\n");
    return NULL;
  }
//  printf("after connect \n");
  char pNum[20];
  int i = 1000*(int)p;

  for (j = i; j < i+100; j++)
  {
    sprintf(pNum, "%d_", j);
	rc = send(stcp, pNum, strlen(pNum), 0); //dont send the terminating NULL.
    if(rc<0)
	{
      printf("cannot send data to tcp server%d \n",i-1);
      close(stcp);
      exit(1);
    }
    else
    {
        printf("TCP send(): %s\n", pNum);
    }
  }
  close(stcp);
}
