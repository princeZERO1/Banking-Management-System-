#include "database.c" // Include the source directly for simple GCC compilation without makefile

int main() {
    parse_post_data();
    
    char* fullname = get_param("fullname");
    char* dob = get_param("dob");
    char* gender = get_param("gender");
    char* phone = get_param("phone");
    char* email = get_param("email");
    char* address = get_param("address");
    char* aadhaar = get_param("aadhaar");
    char* pan = get_param("pan");
    char* acc_type = get_param("acc_type");
    char* initial_deposit = get_param("initial_deposit");
    char* password = get_param("reg_password");

    send_json_headers();

    // Basic Validation
    if (strlen(fullname) == 0 || strlen(email) == 0 || strlen(password) == 0) {
        printf("{\"status\": \"error\", \"message\": \"All fields are required.\"}");
        return 0;
    }

    if (check_email_exists(email)) {
        printf("{\"status\": \"error\", \"message\": \"Email already registered.\"}");
        return 0;
    }

    if (check_phone_exists(phone)) {
        printf("{\"status\": \"error\", \"message\": \"Phone number already registered.\"}");
        return 0;
    }

    Customer c;
    strcpy(c.name, fullname);
    strcpy(c.dob, dob);
    strcpy(c.gender, gender);
    strcpy(c.phone, phone);
    strcpy(c.email, email);
    strcpy(c.address, address);
    strcpy(c.aadhaar, aadhaar);
    strcpy(c.pan, pan);
    strcpy(c.password, password);
    strcpy(c.account_type, acc_type);
    c.balance = atof(initial_deposit);
    c.status = 1; // Active

    generate_account_number(c.account_number);

    if (save_customer(&c)) {
        printf("{\"status\": \"success\", \"message\": \"Account created!\", \"account_number\": \"%s\"}", c.account_number);
    } else {
        printf("{\"status\": \"error\", \"message\": \"Failed to save customer data.\"}");
    }

    return 0;
}
