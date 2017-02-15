#ifndef BANK_H_
#define BANK_H_

struct BankAcct_
{
  int status;
  char* owner;
  float balance;
  int inSession; //0 is true, 1 is false                                                 
};

//typedef struct BankAcct_* BankAcct;

int open(char* name, struct BankAcct_** AllAccounts);
int start(char* name, struct BankAcct_** AllAccounts);
int credit(char* name, float amount, struct BankAcct_** AllAccounts);
int debit(char* name, float amount, struct BankAcct_** AllAccounts);
int finish(char* name, struct BankAcct_** AllAccounts);
struct BankAcct_* balance(struct BankAcct_** AllAccounts, char* name);
void intializeAcct(struct BankAcct_** AllAccounts);

#endif
