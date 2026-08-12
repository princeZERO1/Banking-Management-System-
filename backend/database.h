#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

// File paths
#define CUSTOMER_FILE "../database/customers.dat"
#define TRANSACTION_FILE "../database/transactions.dat"

// Data Structures

typedef struct {
    char account_number[15]; // 12-digit account number + null terminator
    char name[100];
    char dob[15];
    char gender[10];
    char phone[20];
    char email[100];
    char address[200];
    char aadhaar[15]; // 12 digits
    char pan[15];     // 10 chars
    char password[50];
    char account_type[20]; // Savings, Current
    double balance;
    int status; // 1 = Active, 0 = Frozen
} Customer;

typedef struct {
    char transaction_id[20]; // e.g. TXN-12345678
    char account_number[15]; // The account this transaction belongs to
    char type[20]; // Deposit, Withdraw, Transfer
    double amount;
    char sender_account[15];
    char receiver_account[15];
    char date[15]; // YYYY-MM-DD
    char time[15]; // HH:MM:SS
    double updated_balance; // Balance after this transaction
    char description[100];
} Transaction;

// CGI Helper Functions
void send_json_headers();
void parse_post_data();
char* get_param(const char* key);
void urldecode2(char *dst, const char *src);
char* get_session_cookie();

// File Handling Helper Functions
int save_customer(Customer *c);
int get_customer(const char* account_number, Customer *c);
int update_customer(Customer *c);
int check_email_exists(const char* email);
int check_phone_exists(const char* phone);
void generate_account_number(char* acc_num);

int save_transaction(Transaction *t);
void generate_transaction_id(char* txn_id);
void get_current_datetime(char* date_str, char* time_str);

#endif
