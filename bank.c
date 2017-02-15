#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<ctype.h>
#include"bank.h"

//declaring a global var for lock
pthread_mutex_t lock;

//this function creates a new bank account
//cannot be called by a user in a session
//can only be called by someone not in a session
//RETURNS 0 IF SUCCESSFUL, 1 IF NOT
int open(char* name, struct BankAcct_** AllAccounts)
{
  // printf("in function open\n");
  //begin lock for this function
  pthread_mutex_lock(&lock);

  //  printf("name");
  int i = 0;
  for(i = 0; i < strlen(name); i++)
    {
      if(isalpha(name[i]))
	{
	  name[i] = tolower(name[i]);
	}
    }

  for(i = 0; i < 20; i++)
    {
      if(AllAccounts[i]!=NULL)
	{
	  if(strcmp(name, AllAccounts[i]->owner) == 0)
	    {
	      printf("Account for %s already exists!\n", AllAccounts[i]->owner);
	      //unlock before function exit
	      pthread_mutex_unlock(&lock);
	      return 1;
	    }
	}
	else
	{
	  struct BankAcct_* ba = (struct BankAcct_*)malloc(sizeof(struct BankAcct_));
	  ba->owner = (char *)malloc(sizeof(strlen(name))+1);
	  strcpy(ba->owner, name);
	  ba->balance = 0.00;
	  ba->inSession = 1; //0 means true
	  AllAccounts[i] = ba;
	  printf("SUCCESS: Account: %s Opened Succesfully\n",AllAccounts[i]->owner);
	  //unlock before function exit
	  pthread_mutex_unlock(&lock);
	  return 0;
	}
    }

  //unlock before function exit
  pthread_mutex_unlock(&lock);
  printf("FAIL: Accounts all Filled\n");
  return 1;
  
}

//this function sets flag for inSession
//indicates that an account is currently
//being used
/** 1 ACCOUNT CAN ONLY HAVE 1 SESSION **/
int start(char* name, struct BankAcct_** AllAccounts)
{
  //begin lock
  pthread_mutex_lock(&lock);

  int i = 0;
  for(i = 0; i < strlen(name); i++)
    {
      if(isalpha(name[i]))
	{
	  name[i] = tolower(name[i]);
	}
    }
  for(i = 0; i<20; i++){
    if(AllAccounts[i]!=NULL && strcmp(AllAccounts[i]->owner,name)==0){
      if(AllAccounts[i]->inSession == 0){
	printf("FAIL: Account: %s Already in Session\n",AllAccounts[i]->owner);
	//unlock before function exit
	pthread_mutex_unlock(&lock);
	return 1;
      }
      else{
	AllAccounts[i]->inSession = 0;
	printf("SUCCESS: Account: %s Started Succesfully\n", AllAccounts[i]->owner);
	//unlock before function exit
	pthread_mutex_unlock(&lock);
	return 0;
      }
    }
  }

  //unlock before function exit
  pthread_mutex_unlock(&lock);
  printf("FAIL: Accounts does not Exist\n");
  return 1;

}


//changes the balance of an account
//return 0 if successful, 1 if not
/** CAN ONLY BE CALLED IF INSESSION FLAG IS TRUE **/
int credit(char* name, float amount, struct BankAcct_** AllAccounts)
{
  //begin lock
  pthread_mutex_lock(&lock);

  int i = 0;
  for(i = 0; i < strlen(name); i++)
    {
      if(isalpha(name[i]))
	{
	  name[i] = tolower(name[i]);
	}
    }
  for(i = 0; i<20; i++){
    if(AllAccounts[i]!=NULL && strcmp(AllAccounts[i]->owner,name)==0){
      if(AllAccounts[i]->inSession == 0){
	AllAccounts[i]->balance+=amount;
	printf("SUCCESS: Account: %s credited %f for total: %f\n", AllAccounts[i]->owner,amount,AllAccounts[i]->balance);
	//unlock before function exit
	pthread_mutex_unlock(&lock);
	return 0;
      }
      else{
	printf("FAIL: Account: %s Started NOT in Session\n", AllAccounts[i]->owner);
	//unlock before function exit
	pthread_mutex_unlock(&lock);
	return 1;
      }
    }
  }
  
  //unlock before function exit
  pthread_mutex_unlock(&lock);
  printf("FAIL: Accounts does not Exist\n");
  return 1;
  
}

//changes the balance of an account
//return 0 if successful, 1 if not
/** CAN ONLY BE CALLED IF INSESSION FLAG IS TRUE **/
int debit(char* name, float amount, struct BankAcct_** AllAccounts)
{
  int i = 0;
  for(i = 0; i < strlen(name); i++)
    {
      if(isalpha(name[i]))
	{
	  name[i] = tolower(name[i]);
	}
    }

  for(i = 0; i<20; i++){
    if(AllAccounts[i]!=NULL && strcmp(AllAccounts[i]->owner,name)==0){
      if(AllAccounts[i]->inSession == 0){
	//check to see if there are sufficient funds
	if(AllAccounts[i]->balance > amount){
	  AllAccounts[i]->balance-=amount;
	  printf("SUCCESS: Account: %s debited %f for total: %f\n", AllAccounts[i]->owner,amount,AllAccounts[i]->balance);
	  return 0;
	}
	else
	  {
	    printf("FAILED: insufficient funds :(\n");
	    return 1;
	  }
      }
      else{
	printf("FAIL: Account: %s Started NOT in Session\n", AllAccounts[i]->owner);
	return 1;
      }
    }
  }
  
  printf("FAIL: Accounts does not Exist\n");
  return 1;

}

/** CAN ONLY BE CALLED IF INSESSION FLAG IS TRUE **/
//Changes the value for inSession to inactive
int finish(char* name, struct BankAcct_** AllAccounts)
{
  int i = 0;
  for(i = 0; i < strlen(name); i++)
    {
      if(isalpha(name[i]))
	{
	  name[i] = tolower(name[i]);
	}
    }
  for(i = 0; i<20; i++){
    if(AllAccounts[i]!=NULL && strcmp(AllAccounts[i]->owner,name)==0){
      if(AllAccounts[i]->inSession == 0){
	AllAccounts[i]->inSession = 1;
	printf("SUCCESS: Account: %s NOW  NOT in Session\n", AllAccounts[i]->owner);
	return 0;
      }
      else{
	printf("FAIL: Account: %s Already NOT in Session\n", AllAccounts[i]->owner);
	return 1;
      }
    }
  }
  
  printf("FAIL: Accounts does not Exist\n");
  return 1;

}

/** CAN ONLY BE CALLED IF INSESSION FLAG IS TRUE **/
struct BankAcct_* balance(struct BankAcct_** AllAccounts, char* name)
{
  struct BankAcct_* temp = NULL;
  int i = 0;
  for(i = 0; i < strlen(name); i++)
    {
      if(isalpha(name[i]))
	{
	  name[i] = tolower(name[i]);
	}
    }
  for(i = 0; i<20; i++){
    if(AllAccounts[i]!=NULL && strcmp(AllAccounts[i]->owner,name)==0){
      printf("SUCCESS: Account %s Found\n",AllAccounts[i]->owner);
      temp = AllAccounts[i];
      return temp;
    }
  }
  
  printf("FAIL: Accounts does not Exist\n");
  return temp;
  
}

void printAccts(struct BankAcct_** AllAccounts)
{
  int i = 0;
  //  printf("ALL ACCOUNTS:\n");
  for(i = 0; i<20; i++)
    {
      if(AllAccounts[i]!=NULL)
	{
	  printf("%s's balance is:%f\n",AllAccounts[i]->owner, AllAccounts[i]->balance);
	  printf("\n");
	}
    }


}

/** CAN ONLY BE CALLED IF INSESSION FLAG IS FALSE **/
//Ends the thread's connection to the server
int myexit(char* name)
{
  
  /** COMPLAIN IF ba NOT IN SESSION **/

  return 0;
}
