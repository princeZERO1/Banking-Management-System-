#include "database.h"

// Simple key-value store for POST parameters
#define MAX_PARAMS 50
typedef struct {
    char key[50];
    char value[500];
} Param;

Param params[MAX_PARAMS];
int param_count = 0;

// Utility to URL-decode a string
void urldecode2(char *dst, const char *src) {
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
            if (a >= 'a')
                a -= 'a'-'A';
            if (a >= 'A')
                a -= ('A' - 10);
            else
                a -= '0';
            if (b >= 'a')
                b -= 'a'-'A';
            if (b >= 'A')
                b -= ('A' - 10);
            else
                b -= '0';
            *dst++ = 16*a+b;
            src+=3;
        } else if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else {
            *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}

void parse_post_data() {
    char *len_str = getenv("CONTENT_LENGTH");
    if (len_str == NULL) return;
    
    int len = atoi(len_str);
    if (len <= 0) return;
    
    char *post_data = malloc(len + 1);
    fread(post_data, 1, len, stdin);
    post_data[len] = '\0';
    
    // Parse key=value&key2=value2
    char *pair = strtok(post_data, "&");
    while (pair != NULL && param_count < MAX_PARAMS) {
        char *eq = strchr(pair, '=');
        if (eq != NULL) {
            *eq = '\0';
            urldecode2(params[param_count].key, pair);
            urldecode2(params[param_count].value, eq + 1);
            param_count++;
        }
        pair = strtok(NULL, "&");
    }
    free(post_data);
}

char* get_param(const char* key) {
    for (int i = 0; i < param_count; i++) {
        if (strcmp(params[i].key, key) == 0) {
            return params[i].value;
        }
    }
    return "";
}

void send_json_headers() {
    printf("Content-Type: application/json\n\n");
}

char* get_session_cookie() {
    char* cookie = getenv("HTTP_COOKIE");
    if (!cookie) return "";
    
    // Simplistic cookie parsing looking for session_acc=
    char* found = strstr(cookie, "session_acc=");
    if (found) {
        static char acc[20];
        found += 12; // length of "session_acc="
        int i = 0;
        while (found[i] != ';' && found[i] != '\0' && i < 15) {
            acc[i] = found[i];
            i++;
        }
        acc[i] = '\0';
        return acc;
    }
    return "";
}

// --- File Handling Functions ---

int save_customer(Customer *c) {
    FILE *file = fopen(CUSTOMER_FILE, "ab");
    if (!file) {
        // Create directory if it doesn't exist? (Assuming it exists)
        file = fopen(CUSTOMER_FILE, "wb");
        if (!file) return 0;
    }
    fwrite(c, sizeof(Customer), 1, file);
    fclose(file);
    return 1;
}

int get_customer(const char* account_number, Customer *c) {
    FILE *file = fopen(CUSTOMER_FILE, "rb");
    if (!file) return 0;
    
    int found = 0;
    while (fread(c, sizeof(Customer), 1, file)) {
        if (strcmp(c->account_number, account_number) == 0) {
            found = 1;
            break;
        }
    }
    fclose(file);
    return found;
}

int update_customer(Customer *c) {
    FILE *file = fopen(CUSTOMER_FILE, "rb+");
    if (!file) return 0;
    
    Customer temp;
    int found = 0;
    while (fread(&temp, sizeof(Customer), 1, file)) {
        if (strcmp(temp.account_number, c->account_number) == 0) {
            fseek(file, -sizeof(Customer), SEEK_CUR);
            fwrite(c, sizeof(Customer), 1, file);
            found = 1;
            break;
        }
    }
    fclose(file);
    return found;
}

int check_email_exists(const char* email) {
    FILE *file = fopen(CUSTOMER_FILE, "rb");
    if (!file) return 0;
    Customer c;
    while (fread(&c, sizeof(Customer), 1, file)) {
        if (strcmp(c.email, email) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

int check_phone_exists(const char* phone) {
    FILE *file = fopen(CUSTOMER_FILE, "rb");
    if (!file) return 0;
    Customer c;
    while (fread(&c, sizeof(Customer), 1, file)) {
        if (strcmp(c.phone, phone) == 0) {
            fclose(file);
            return 1;
        }
    }
    fclose(file);
    return 0;
}

void generate_account_number(char* acc_num) {
    // Generate sequential account number starting from 100000000001
    FILE *file = fopen(CUSTOMER_FILE, "rb");
    long long next_acc = 100000000001;
    if (file) {
        Customer c;
        while (fread(&c, sizeof(Customer), 1, file)) {
            long long acc = atoll(c.account_number);
            if (acc >= next_acc) {
                next_acc = acc + 1;
            }
        }
        fclose(file);
    }
    sprintf(acc_num, "%lld", next_acc);
}

int save_transaction(Transaction *t) {
    FILE *file = fopen(TRANSACTION_FILE, "ab");
    if (!file) {
        file = fopen(TRANSACTION_FILE, "wb");
        if (!file) return 0;
    }
    fwrite(t, sizeof(Transaction), 1, file);
    fclose(file);
    return 1;
}

void generate_transaction_id(char* txn_id) {
    srand((unsigned int)time(NULL));
    int random_part = rand() % 900000 + 100000;
    sprintf(txn_id, "TXN-%d", random_part);
}

void get_current_datetime(char* date_str, char* time_str) {
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(date_str, "%04d-%02d-%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
    sprintf(time_str, "%02d:%02d:%02d", tm.tm_hour, tm.tm_min, tm.tm_sec);
}
