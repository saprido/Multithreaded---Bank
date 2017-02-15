/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include "bank.h"

struct params
{
  struct BankAcct_** AllAccounts;
  int newsockfd;
};

void error(char *msg)
{
  perror(msg);
  exit(1);
}

int timer_create(clockid_t clockid, struct sigevent *sevp,
                 timer_t *timerid);


char account [100] = " ";
char accountIS[100]=" ";
//prints the balance of all accounts
//this is to be called by the SERVER ONLY every 20 seconds
void printAll(struct BankAcct_** AllAccounts)
{
  int i;
  for(i = 0; i < 20; i++)
    {
      if (AllAccounts[i] != NULL)
	{
	  printf("%s\t%s\n", AllAccounts[i]->owner, AllAccounts[i]->balance);
	}
    }
}


int parse(char* buffer, struct BankAcct_** AllAccounts)
{
  char* token;
  token = strtok(buffer, " \t\n\f\v\r");
  float amount = 0.0;
  //char account [100];
  int condition = -1;
  //BankAcct temp = NULL;
  struct BankAcct_* temp;
  while(token)
    {
      if(strcmp(token, "open")== 0)
	{
	  token = strtok(NULL, " \t\n\f\v\r");
	  strcpy(account,token);
	  printf("HELLO\n");
	  condition = open(account, AllAccounts);
	  if(condition!=0)
	    return 1;
	  else
	    return 0;
	}
      else if(strcmp(token,"start")==0)
	{
	  int i = 0;
	  char tempAcc[100];
	  strcpy(tempAcc, account);
	  token = strtok(NULL, " \t\n\f\v\r");
	  strcpy(accountIS,token);
	  condition = start(accountIS,AllAccounts);
	  if(condition!=0)
	    strcpy(accountIS," ");
	  return 0;
	}
      else if(strcmp(token,"credit")==0)
	{
	  token = strtok(NULL, " \t\n\f\v\r");
	  amount = atof(token);
	  if(amount == 0)
	    printf("Invalid Amount, Please enter amount > 0.0\n");
	  if(strcmp(accountIS," ")==0)
	    printf("NO ACCOUNT IN SESSION\n");
	  else
	    condition = credit(accountIS,amount, AllAccounts);
	  return 0;
	  
	}
      else if(strcmp(token,"debit")==0)
	{
	  token = strtok(NULL, " \t\n\f\v\r");
	  amount = atof(token);
	  if(amount == 0)
	    printf("Invalid Amount, Please enter amount > 0.0\n");
	  if(strcmp(accountIS," ")==0)
	    printf("NO ACCOUNT IN SESSION\n");
	  else
	    condition = debit(account,amount, AllAccounts);
	  return 0;
	}
      else if(strcmp(token,"balance")==0)
	{
	  if(strcmp(accountIS," ")==0)
	    printf("NO ACCOUNT IN SESSION\n");
	  else
	    temp = balance(AllAccounts,accountIS);
	  if(temp==NULL)
	    return 1;
	  else{
	    printf("%s's BALANCE: %f",temp->owner, temp->balance);
	    return 0;
	  }
	  
	}
      else if(strcmp(token,"finish")==0)
	{
	  if(strcmp(accountIS," ")==0)
	    printf("NO ACCOUNT IN SESSION\n");
	  else{
	    finish(accountIS, AllAccounts);
	    strcpy(accountIS, " ");
	  }
	  return 0;
	}
      else if(strcmp(token,"exit")==0)
	{
	  //send back with value 1
	  //2 signals main to terminate thread
	  return 2;
	}
      
      token = strtok(NULL, " \t\n\f\v\r");
    }
  
  
}


int handleConnection(void * args)
{
  struct params* temp = (struct params*)args;

  struct BankAcct_** AllAccounts = temp->AllAccounts;
  int newsockfd = temp->newsockfd;
  //these two variables have been lifted from main
  int n = -1;
  int c = 0;
  char buffer[256];
  int p = -1;//return value from parse method
  while(c == 0)
    {
      printAccts(temp->AllAccounts);
      sleep(5);

      while (c == 0)
	{
	  // zero out the char buffer to receive a client message
	  bzero(buffer,256);
	  sleep(2);
	  // try to read from the client socket
	  //read is a blocking call, will wait till there is something to read
	  n = read(newsockfd,buffer,255);
	  
	  // printf("Got the message\n");
	  // if the read from the client blew up, complain and exit
	  if (n < 0)
	    {
	  error("ERROR reading from socket");
	    }
	  
	  // printf("Here is the message: %s\n",buffer);
	  
	  // try to write to the client socket
	  n = write(newsockfd,"I got your message",18);
	  
	  //parse
	  p = parse(buffer, temp->AllAccounts); //returns 0 if action is successful, 1 if error or exit
      
	  if (p == 2)
	    {
	      //close(newsockfd);
	      n = write(newsockfd, "exit", 4);
	      return 0;
	    }
	  // if the write to the client below up, complain and exit
      if (n < 0)
	{
	  error("ERROR writing to socket");
	  return 1;
	}
	}
    }
  return 1;
}

int main(int argc, char *argv[])
{
  //int sockfd, newsockfd, portno, clilen;
  //struct sockaddr_in serv_addr;
  //struct sockaddr_in cli_addr;
  
  
  int sockfd = -1;// file descriptor for our server socket
  int newsockfd = -1;// file descriptor for a client socket
  int portno = -1;// server port to connect to
  int clilen = -1;// utility variable - size of clientAddressInfo below
  //int n = -1;// utility variable - for monitoring reading/writing from/to the socket
  //char buffer[256];// char array to store data going to and coming from the socket
  
  struct sockaddr_in serverAddressInfo;// Super-special secret C struct that holds address info for building our server socket
  struct sockaddr_in clientAddressInfo;// Super-special secret C struct that holds address info about our client socket
  
  
  
  // If the user didn't enter enough arguments, complain and exit
  if (argc != 1)
    {
      //fprintf(stderr,"ERROR, no port provided\n");
      fprintf(stderr,"ERROR, too many aruments\n");
      exit(1);
    }
  
  
  
  /** If the user gave enough arguments, try to use them to get a port number and address **/
  
  // convert the text representation of the port number given by the user to an int  
  //portno = atoi(argv[1]);
  portno = 9000;

  // try to build a socket .. if it doesn't work, complain and exit
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd < 0)
    {
      error("ERROR opening socket");
    }
  
  
  
  /** We now have the port to build our server socket on .. time to set up the address struct **/
  
  // zero out the socket address info struct .. always initialize!
  bzero((char *) &serverAddressInfo, sizeof(serverAddressInfo));
  
  // set the remote port .. translate from a 'normal' int to a super-special 'network-port-int'
  serverAddressInfo.sin_port = htons(portno);
  
  // set a flag to indicate the type of network address we'll be using  
  serverAddressInfo.sin_family = AF_INET;
  
  // set a flag to indicate the type of network address we'll be willing to accept connections from
  serverAddressInfo.sin_addr.s_addr = INADDR_ANY;
  
  
  
  /** We have an address struct and a socket .. time to build up the server socket **/
  
  // bind the server socket to a specific local port, so the client has a target to connect to      
  if (bind(sockfd, (struct sockaddr *) &serverAddressInfo, sizeof(serverAddressInfo)) < 0)
    {
      error("ERROR on binding");
    }
  
  // set up the server socket to listen for client connections
  listen(sockfd,20);

  struct BankAcct_** AllAccounts = (struct BankAcct_**)malloc(sizeof(struct BankAcct_*)*20);

 
  //initializes AllAccounts in bank.c
  int i;
  for(i = 0; i < 20; i++)
    {
      AllAccounts[i] = NULL;
    }
  
  // determine the size of a clientAddressInfo struct
  clilen = sizeof(clientAddressInfo);
  
  // block until a client connects, when it does, create a client socket
  while(newsockfd = accept(sockfd, (struct sockaddr *) &clientAddressInfo, &clilen))
    {
      /** If we're here, a client tried to connect **/
      
      // if the connection blew up for some reason, complain and exit
      if (newsockfd < 0) 
	{
	  error("ERROR on accept");
	}
      
      pthread_t newConn;
      /*
	struct sockaddr_in* copy_client_sock = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	*copy_client_sock = clientAddressInfo;
	*/
      int * copy_client = (int *)malloc(sizeof(int));
      *copy_client = newsockfd;
      struct params * temp = (struct params *)malloc(sizeof(struct params));
      temp->newsockfd = newsockfd;
      temp->AllAccounts = AllAccounts;


      //making sure that the thread was successfully created
      if(pthread_create(&newConn, NULL, (void *)handleConnection, (void *)temp) < 0){
	printf("could not create thread\n");
	return 0;
      }
      else
	{
	  while(1){
	    printAccts(temp->AllAccounts);
	    sleep(20);
	  }
	}
	//terminate the thread
	pthread_join(newConn, NULL);
    }
  
  printf("Server is shutting down\n");
  return 0; 
}
