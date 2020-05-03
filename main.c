/***********************************************************************
* FILENAME :    main.token_start             
*
* DESCRIPTION :
*       Simple record store for a hosptial
*
*
* AUTHOR :    P.T.R. Muthukumarana(170393U)    
*
*
**/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include "md5.h"

#define MAX_ATTRIBUTE_SIZE 256
#define MAX_ATTRIBUTE_COUNT 10
#define PERMISSION_SIZE 100
#define MAX_RECORD_LENGTH ((MAX_ATTRIBUTE_SIZE + 1) * MAX_ATTRIBUTE_COUNT)
#define CONFIG_FILE "config.dat"
#define DATA_FILE "data.dat";

#define DESCRIPTION_LENGTH 256

typedef enum PrivilegeLevelEnum
{
    LEVEL1,
    LEVEL2,
    LEVEL3,
    LEVEL4
} PrivilegeLevel;

typedef enum UserTypeEnum
{
    PATIENT,
    HOSPITAL_STAFF,
    DOCTOR,
    ADMIN
} UserType;

typedef enum RecordTypeEnum
{
    ID_RECORD,
    USER_RECORD,
    PATIENT_INFO_RECORD,
    SICKNESS_INFO_RECORD,
    DRUG_PRESCRIPTION_RECORD,
    LAB_TEST_PRESCRIPTION_RECORD
} RecordType;

typedef struct IdStruct
{
    int last_user_id;
    int last_patient_info_id;
    int last_sickness_info_id;
    int last_drug_prescription_id;
    int last_lab_test_prescription_id;
} Id;
typedef struct UserStruct
{
    int id;
    char username[MAX_ATTRIBUTE_SIZE];
    char password[MAX_ATTRIBUTE_SIZE];
    UserType user_type;
    PrivilegeLevel privilege_level;
} User;

typedef struct PatientInfoStruct
{
    int id;
    int user_id;
    char name[MAX_ATTRIBUTE_SIZE];
    char address[MAX_ATTRIBUTE_SIZE];
    char phone[20];
    char gender;
    int birth_year;
} PatientInfo;

typedef struct SicknessInfoStruct
{
    int id;
    int patient_id;
    int doctor_id;
    char datetime[100];
    char description[DESCRIPTION_LENGTH];

} SicknessInfo;

typedef struct PrescriptionStruct
{
    int id;
    int patient_id;
    int doctor_id;
    char datetime[100];
    char prescription[DESCRIPTION_LENGTH];
} Prescription;

bool search(const char **array, char *item)
{
    for (int i = 0; i < sizeof(array) / sizeof(array[0]); i++)
    {
        if (strncmp(array[i], item, sizeof(char) * (strlen(array[i]) + 1)) == 0)
        {
            return true;
        }
    }
    return false;
}

User *current_user;
const char *level1_privileges[5] = {
    "READ_SELF_PATIENT_RECORD",
    "READ_SELF_LAB_PRESCRIPTION_RECORD",
    "READ_SELF_DRUG_PRESCRIPTION_RECORD",
    "READ_SELF_SICKNESS_INFO_RECORD"};

const char *level2_privileges[5] = {
    "CREATE_PATIENT_RECORD",
    "READ_ANY_PATIENT_RECORD",
    "READ_ANY_LAB_PRESCRIPTION_RECORD",
    "READ_ANY_DRUG_PRESCRIPTION_RECORD",
};

const char *level3_privileges[6] = {
    "CREATE_SICKNESS_INFO_RECORD",
    "CREATE_PRESCRIPTION_RECORD",
    "READ_ANY_PATIENT_RECORD",
    "READ_ANY_LAB_PRESCRIPTION_RECORD",
    "READ_ANY_DRUG_PRESCRIPTION_RECORD",
    "READ_ANY_SICKNESS_INFO_RECORD"};

bool has_privilege(PrivilegeLevel level, char *privilege)
{
    switch (level)
    {
    case LEVEL1:
        return search(level1_privileges, privilege);
    case LEVEL2:
        return search(level2_privileges, privilege);
    case LEVEL3:
        return search(level3_privileges, privilege);
    case LEVEL4:
        return true;
    default:
        return false;
    }
}

char *generate_privilege_string(char *holder, RecordType type, char *action, char *owner)
{
    char *record_string;

    switch (type)
    {
    case USER_RECORD:
        record_string = "USER_RECORD";
        break;
    case PATIENT_INFO_RECORD:
        record_string = "PATIENT_INFO_RECORD";
        break;
    case DRUG_PRESCRIPTION_RECORD:
        record_string = "DRUG_PRESCRIPTION_RECORD";
        break;
    case LAB_TEST_PRESCRIPTION_RECORD:
        record_string = "LAB_TEST_PRESCRIPTION_RECORD";
        break;
    case SICKNESS_INFO_RECORD:
        record_string = "SICKNESS_INFO_RECORD";
        break;
    default:
        printf("Error incorrect record type");
        exit(2);
        break;
    }

    sprintf("%s_%s_%s", action, owner, record_string);
    return holder;
}

char *user_record_to_string(User *record, char *str)
{
    sprintf(str, "%d;%d;%s;%s;%d;%d\n", USER_RECORD,
            record->id,
            record->username,
            record->password,
            record->user_type,
            record->privilege_level);
    return str;
}

char *prescription_to_string(Prescription *record, char *str, RecordType type)
{
    sprintf(str, "%d;%d;%d;%d;%s;%s\n", type,
            record->id,
            record->patient_id,
            record->doctor_id,
            record->datetime,
            record->prescription);
    return str;
}

char *patient_info_to_string(PatientInfo *record, char *str)
{
    sprintf(str, "%d;%d;%d;%s;%s;%s;%c;%d\n",
            PATIENT_INFO_RECORD,
            record->id,
            record->user_id,
            record->name,
            record->address,
            record->phone,
            record->gender,
            record->birth_year);
    return str;
}

char *sickness_info_to_string(SicknessInfo *record, char *str)
{
    sprintf(str, "%d;%d;%d;%d;%s;%s\n",
            SICKNESS_INFO_RECORD,
            record->id,
            record->patient_id,
            record->doctor_id,
            record->datetime,
            record->description);
    return str;
}

char *id_to_string(Id *record, char *str)
{
    sprintf(str, "%d;%d;%d;%d;%d;%d;%d\n",
            ID_RECORD,
            0,
            record->last_user_id,
            record->last_patient_info_id,
            record->last_sickness_info_id,
            record->last_drug_prescription_id,
            record->last_lab_test_prescription_id);
}

char *get_file_name(RecordType record_type)
{
    char *file_name;

    switch (record_type)
    {
    case USER_RECORD:
    case ID_RECORD:
        file_name = CONFIG_FILE;
        break;
    case PATIENT_INFO_RECORD:
    case SICKNESS_INFO_RECORD:
    case LAB_TEST_PRESCRIPTION_RECORD:
    case DRUG_PRESCRIPTION_RECORD:
        file_name = DATA_FILE;
    }
    return file_name;
}

FILE *open_file(RecordType record_type, char *mode)
{
    char *file_name = get_file_name(record_type);
    FILE *file = fopen(file_name, mode);

    if (file == NULL)
    {
        printf("Error: Coudln't open %s file!", file_name);
        exit(1);
    }

    return file;
}

void *allocate_space_for_record(RecordType type)
{
    switch (type)
    {
    case USER_RECORD:
        return malloc(sizeof(User));
    case PATIENT_INFO_RECORD:
        return malloc(sizeof(PatientInfo));
    case DRUG_PRESCRIPTION_RECORD:
    case LAB_TEST_PRESCRIPTION_RECORD:
        return malloc(sizeof(Prescription));
    case SICKNESS_INFO_RECORD:
        return malloc(sizeof(SicknessInfo));
    case ID_RECORD:
        return malloc(sizeof(Id));
    default:
        printf("Error incorrect record type");
        exit(2);
    }
}

void set_attribute_for_record(void *record, RecordType type, int attribute, char *token)
{
    switch (type)
    {
    case USER_RECORD:
    {
        User *user = (User *)record;
        switch (attribute)
        {
        case 1:
            user->id = atoi(token);
            break;
        case 2:
            strcpy(user->username, token);
            break;
        case 3:
            strcpy(user->password, token);
            break;
        case 4:
            user->user_type = atoi(token);
            break;
        case 5:
            user->privilege_level = atoi(token);
            break;
        }
    }
    break;
    case PATIENT_INFO_RECORD:
    {
        PatientInfo *patient_info = (PatientInfo *)record;
        switch (attribute)
        {
        case 1:
            patient_info->id = atoi(token);
            break;
        case 2:
            patient_info->user_id = atoi(token);
        case 3:
            strcpy(patient_info->name, token);
            break;
        case 4:
            strcpy(patient_info->address, token);
            break;
        case 5:
            strcpy(patient_info->phone, token);
            break;
        case 6:
            patient_info->gender = (char)token[0];
            break;
        case 7:
            patient_info->birth_year = atoi(token);
            break;
        }
    }
    break;
    case DRUG_PRESCRIPTION_RECORD:
    case LAB_TEST_PRESCRIPTION_RECORD:
    {
        Prescription *prescription = (Prescription *)record;
        switch (attribute)
        {
        case 1:
            prescription->id = atoi(token);
            break;
        case 2:
            prescription->patient_id = atoi(token);
            break;
        case 3:
            prescription->doctor_id = atoi(token);
            break;
        case 4:
            strcpy(prescription->datetime, token);
            break;
        case 5:
            strcpy(prescription->prescription, token);
            break;
        }
    }
    break;
    case SICKNESS_INFO_RECORD:
    {
        SicknessInfo *sickness_info = (SicknessInfo *)record;
        switch (attribute)
        {
        case 1:
            sickness_info->id = atoi(token);
            break;
        case 2:
            sickness_info->patient_id = atoi(token);
            break;
        case 3:
            sickness_info->doctor_id = atoi(token);
            break;
        case 4:
            strcpy(sickness_info->datetime, token);
            break;
        case 5:
            strcpy(sickness_info->description, token);
            break;
        }
    }
    case ID_RECORD:
    {
        Id *ids = (Id *)record;
        switch (attribute)
        {
        case 2:
            ids->last_user_id = atoi(token);
            break;
        case 3:
            ids->last_patient_info_id = atoi(token);
            break;
        case 4:
            ids->last_sickness_info_id = atoi(token);
            break;
        case 5:
            ids->last_drug_prescription_id = atoi(token);
            break;
        case 6:
            ids->last_lab_test_prescription_id = atoi(token);
            break;
        }
    }
    break;
    default:
        printf("Error incorrect record type");
        exit(2);
    }
}

int next_token(char *string, char *token, int *current)
{
    int t = 0;
    while (string[*current] != '\0')
    {
        if (string[*current] == ';' || string[*current] == '\n')
        {
            (*current)++;
            token[t] = '\0';
            return 1;
        }
        else
        {
            token[t] = string[*current];
            t++;
        }
        (*current)++;
    }
    return -1;
}

void *find_single_record(RecordType record_type, int search_attribute, char *value)
{

    FILE *file = open_file(record_type, "r");

    char *buffer = NULL;
    size_t len = 0;
    bool found = false;
    void *current = allocate_space_for_record(record_type);
    int token_start, attribute;
    char token[MAX_ATTRIBUTE_SIZE];
    bool correct_type;
    while (getline(&buffer, &len, file) != -1)
    {
        token_start = 0;
        attribute = 0;

        while (next_token(buffer, token, &token_start) != -1)
        {
            if (attribute == 0)
            {
                if (atoi(token) != record_type)
                    break;
            }
            // else
            // {
            if (attribute == search_attribute && strncmp(value, token, sizeof token) == 0)
            {
                found = true;
            }
            set_attribute_for_record(current, record_type, attribute, token);
            // }

            attribute++;
        }

        if (found)
        {
            break;
        }
    }

    if (buffer != NULL)
        free(buffer);

    // if (token != NULL)
    //     free(token);

    fclose(file);

    if (found)
        return current;
    else
        return NULL;
}

// update_record
void update_record(RecordType record_type, int id, char *new_row)
{

    FILE *file = open_file(record_type, "r");
    char *buffer = NULL;
    char *content = NULL;
    char *token = (char *)malloc(MAX_ATTRIBUTE_SIZE + 1);
    int start;
    size_t len = 0;

    while (getline(&buffer, &len, file) != -1)
    {
        start = 0;
        if (next_token(buffer, token, &start) != -1)
        {
            if (atoi(token) == id)
            {
                // Space is needed for attributes, commas and the null byte
                if (content == NULL)
                {
                    content = malloc((strlen(new_row) + 1) * sizeof(char));
                    strcpy(content, new_row);
                }
                else
                {
                    content = realloc(content, (strlen(content) + strlen(new_row) + 1) * sizeof(char));
                    strcat(content, new_row);
                }
            }
            else
            {

                if (content == NULL)
                {
                    content = malloc((strlen(buffer) + 1) * sizeof(char));
                    strcpy(content, buffer);
                }
                else
                {
                    content = realloc(content, (strlen(content) + strlen(buffer) + 1) * sizeof(char));
                    strcat(content, buffer);
                }
            }
        }
    }
    free(buffer);
    fclose(file);

    char temp_file_name[100];
    strcpy(temp_file_name, get_file_name(record_type));
    strcat(temp_file_name, ".tmp");

    FILE *temp = fopen(temp_file_name, "w");
    fputs(content, temp);
    free(content);
    fclose(temp);
    remove(get_file_name(record_type));
    rename(temp_file_name, get_file_name(record_type));
}

int get_last_id(RecordType type)
{
    Id *ids = (Id *)find_single_record(ID_RECORD, 0, "0");
    int num;
    switch (type)
    {
    case USER_RECORD:
        num = ids->last_user_id;
        break;
    case PATIENT_INFO_RECORD:
        num = ids->last_patient_info_id;
        break;
    case DRUG_PRESCRIPTION_RECORD:
        num = ids->last_drug_prescription_id;
        break;
    case LAB_TEST_PRESCRIPTION_RECORD:
        num = ids->last_lab_test_prescription_id;
        break;
    case SICKNESS_INFO_RECORD:
        num = ids->last_drug_prescription_id;
        break;
    default:
        printf("Error incorrect record type");
        exit(2);
        break;
    }

    return num;
}

void set_last_id(RecordType type, int new_id)
{
    char ids[MAX_RECORD_LENGTH];
    Id *id = (Id *)find_single_record(ID_RECORD, 0, "0");

    switch (type)
    {
    case USER_RECORD:
        id->last_user_id = new_id;
        break;
    case PATIENT_INFO_RECORD:
        id->last_patient_info_id = new_id;
        break;
    case DRUG_PRESCRIPTION_RECORD:
        id->last_drug_prescription_id = new_id;
        break;
    case LAB_TEST_PRESCRIPTION_RECORD:
        id->last_lab_test_prescription_id = new_id;
        break;
    case SICKNESS_INFO_RECORD:
        id->last_drug_prescription_id = new_id;
        break;
    default:
        printf("Error incorrect record type");
        exit(2);
        break;
    }

    id_to_string(id, ids);
    update_record(ID_RECORD, 0, ids);
}

void append_record(RecordType record_type, char *record)
{
    int last_id = get_last_id(record_type);
    FILE *file = open_file(record_type, "a");
    fputs(record, file);
    fclose(file);

    set_last_id(record_type, last_id + 1);
}

size_t get_record_size(RecordType type)
{
    switch (type)
    {
    case USER_RECORD:
        return sizeof(User);
    case PATIENT_INFO_RECORD:
        return sizeof(PatientInfo);
    case DRUG_PRESCRIPTION_RECORD:
    case LAB_TEST_PRESCRIPTION_RECORD:
        return sizeof(Prescription);
    case SICKNESS_INFO_RECORD:
        return sizeof(SicknessInfo);
    default:
        printf("Error incorrect record type");
        exit(2);
    }
}

void *find_all_records(RecordType record_type, int search_attribute, char *value)
{
    static long start = 0;

    FILE *file = open_file(record_type, "r");
    fseek(file, start, SEEK_SET);

    char *buffer = NULL;
    size_t len = 0;
    bool found = false;
    void *current = allocate_space_for_record(record_type);
    int token_start, attribute;
    char *token = (char *)malloc(MAX_ATTRIBUTE_SIZE + 1);
    int size = 0;
    while (getline(&buffer, &len, file) != -1)
    {
        token_start = 0;
        attribute = 0;
        start += strlen(buffer);
        while (next_token(buffer, token, &token_start) != -1)
        {

            if (attribute == 0)
            {
                if (atoi(token) != record_type)
                    break;
            }

            if (attribute == search_attribute && strcmp(value, token) == 0)
            {
                found = true;
            }

            set_attribute_for_record(current, record_type, attribute, token);

            attribute++;
        }

        if (found)
        {
            break;
        }
    }

    if (buffer != NULL)
        free(buffer);

    if (token != NULL)
        free(token);

    fclose(file);
    if (found)
        return current;
    else
        return NULL;
}

char *generate_hash(char *string)
{
    unsigned char digest[16];
    MD5_CTX context;
    MD5_Init(&context);
    MD5_Update(&context, string, strlen(string));
    MD5_Final(digest, &context);

    char md5string[33];
    for (int i = 0; i < 16; ++i)
        sprintf(&md5string[i * 2], "%02x", (unsigned int)digest[i]);

    char *copy = malloc(33 * sizeof(char));
    strcpy(copy, md5string);
    return copy;
}

bool prompt_login()
{

    char username[MAX_ATTRIBUTE_SIZE];
    char password[MAX_ATTRIBUTE_SIZE];

    printf("Username: ");
    scanf("%s", username);

    printf("Password: ");
    scanf("%s", password);

    User *user = (User *)find_single_record(USER_RECORD, 2, username);

    if (user != NULL)
    {
        if (strcmp(generate_hash(password), user->password) == 0)
        {
            printf("Login success!\n\nYour user id is %d\n\n", user->id);

            current_user = user;
            return true;
        }
        else
        {
            printf("Wrong password\n");
        }
    }
    else
    {
        printf("Invalid user\n");
    }
    printf("\n");
    return false;
}

void print_menu()
{
    printf("a - Add User (Admin Only)\n");
    printf("b - Add Patient (Staff Only)\n");
    printf("c - Add Sickness Info Record (Doctors Only)\n");
    printf("d - Add Drug Prescription Record (Doctors Only)\n");
    printf("e - Add Lab Test Prescription Record (Doctors Only)\n");
    printf("f - View Patient Info (Patients, Clerks and Doctors Only)\n");
    printf("g - View Patient Drug Prescription Records (Patients (Self),Staff and Doctors Only)\n");
    printf("h - View Patient Lab Test Prescription Records (Patients(Self), Staff and Doctors Only)\n");
    printf("i - View Sickness Info Record (Patients(Self) and Doctors Only)\n\n");
}

void view_sickness_info_action()
{
  char patient_id[100];
    PatientInfo *pi;

    printf("Enter Patient Id: ");
    if (scanf("%s", patient_id) == 0 ||
        (pi = find_single_record(PATIENT_INFO_RECORD, 1, patient_id)) == NULL)
    {
        printf("Invalid patient id");
        return;
    }

    if (
        (pi->id == current_user->id && has_privilege(current_user->privilege_level, "READ_SELF_SICKNESS_INFO_RECORD")) ||
        has_privilege(current_user->privilege_level, "READ_ANY_SICKNESS_INFO_RECORD"))
    {

        char n;

            printf("Press q to quit; Press n to print next record\n");
        do
        {
            scanf("%c", &n);
            if (n == 'n')
            {
                SicknessInfo *p;
                p = find_all_records(SICKNESS_INFO_RECORD, 2, patient_id);
                if (p == NULL)
                {
                    printf("No more records\n");
                    return;
                }
                else
                {
                    printf("patient id: %d\ndoctor id: %d\ndate time: %s\nprescription: %s\n",
                           p->patient_id,
                           p->doctor_id,
                           p->datetime,
                           p->description);
                }
            }

        } while (n != 'q');
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void view_lab_test_prescription_action()
{
   char patient_id[100];
    PatientInfo *pi;

    printf("Enter Patient Id: ");
    if (scanf("%s", patient_id) == 0 ||
        (pi = find_single_record(PATIENT_INFO_RECORD, 1, patient_id)) == NULL)
    {
        printf("Invalid patient id");
        return;
    }

    if (
        (pi->id == current_user->id && has_privilege(current_user->privilege_level, "READ_SELF_LAB_PRESCRIPTION_RECORD")) ||
        has_privilege(current_user->privilege_level, "READ_ANY_LAB_PRESCRIPTION_INFO_RECORD"))
    {

        char n;

            printf("Press q to quit; Press n to print next record\n");
        do
        {
            scanf("%c", &n);
            if (n == 'n')
            {
                Prescription *p;
                p = find_all_records(LAB_TEST_PRESCRIPTION_RECORD, 2, patient_id);
                if (p == NULL)
                {
                    printf("No more records\n");
                    return;
                }
                else
                {
                    printf("patient id: %d\ndoctor id: %d\ndate time: %s\nprescription: %s\n",
                           p->patient_id,
                           p->doctor_id,
                           p->datetime,
                           p->prescription);
                }
            }

        } while (n != 'q');
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void view_drug_prescription_action()
{
    char patient_id[100];
    PatientInfo *pi;

    printf("Enter Patient Id: ");
    if (scanf("%s", patient_id) == 0 ||
        (pi = find_single_record(PATIENT_INFO_RECORD, 1, patient_id)) == NULL)
    {
        printf("Invalid patient id");
        return;
    }

    if (
        (pi->id == current_user->id && has_privilege(current_user->privilege_level, "READ_SELF_DRUG_PRESCRIPTION_RECORD")) ||
        has_privilege(current_user->privilege_level, "READ_ANY_DRUG_PRESCRIPTION_RECORD"))
    {

        char n;

            printf("Press q to quit; Press n to print next record\n");
        do
        {
            scanf("%c", &n);
            if (n == 'n')
            {
                Prescription *p;
                p = find_all_records(DRUG_PRESCRIPTION_RECORD, 2, patient_id);
                if (p == NULL)
                {
                    printf("No more records\n");
                    return;
                }
                else
                {
                    printf("patient id: %d\ndoctor id: %d\ndate time: %s\nprescription: %s\n",
                           p->patient_id,
                           p->doctor_id,
                           p->datetime,
                           p->prescription);
                }
            }

        } while (n != 'q');
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void view_patient_info_action()
{
    char patient_id[100];
    PatientInfo *pi;

    printf("Enter Patient Id: ");
    if (scanf("%s", patient_id) == 0 ||
        (pi = find_single_record(PATIENT_INFO_RECORD, 1, patient_id)) == NULL)
    {
        printf("Invalid patient id");
        return;
    }

    if (
        (pi->id == current_user->id && has_privilege(current_user->privilege_level, "READ_SELF_PATIENT_INFO_RECORD")) ||
        has_privilege(current_user->privilege_level, "READ_ANY_PATIENT_INFO_RECORD"))
    {
        printf("id: %d\nname: %s\naddress: %s\nphone: %s\ngender: %c\nbirthyear: %d\n",
               pi->id,
               pi->name,
               pi->address,
               pi->phone,
               pi->gender,
               pi->birth_year);
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void add_lab_test_prescription_action()
{
    if (has_privilege(current_user->privilege_level, "CREATE_LAB_TEST_PRESCRIPTION_RECORD"))
    {
        char patient_id[100];
        int doctor_id = current_user->id;
        char datetime[100];
        char description[DESCRIPTION_LENGTH];

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strcpy(datetime, asctime(timeinfo));
        datetime[strlen(datetime) - 1] = '\0';
        printf("Patient Id: ");

        if (scanf("%s", patient_id) == 0 || find_single_record(PATIENT_INFO_RECORD, 1, patient_id) == NULL)
        {
            printf("Invalid patient id");
            return;
        }

        fgets(description, DESCRIPTION_LENGTH, stdin);

        printf("Prescription : ");
        fgets(description, DESCRIPTION_LENGTH, stdin);

        Prescription si;
        si.id = get_last_id(LAB_TEST_PRESCRIPTION_RECORD);
        si.doctor_id = doctor_id;
        si.patient_id = atoi(patient_id);
        strcpy(si.datetime, datetime);
        description[strlen(description) - 1] = '\0';
        strcpy(si.prescription, description);

        char record[MAX_RECORD_LENGTH];
        prescription_to_string(&si, record, LAB_TEST_PRESCRIPTION_RECORD);

        append_record(LAB_TEST_PRESCRIPTION_RECORD, record);
        printf("Record added successfully!\n");
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void add_drug_prescription_action()
{
    if (has_privilege(current_user->privilege_level, "CREATE_DRUG_PRESCRIPTION_RECORD"))
    {

        char patient_id[100];
        int doctor_id = current_user->id;
        char datetime[100];
        char description[DESCRIPTION_LENGTH];

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strcpy(datetime, asctime(timeinfo));
        datetime[strlen(datetime) - 1] = '\0';
        printf("Patient Id: ");

        if (scanf("%s", patient_id) == 0 || find_single_record(PATIENT_INFO_RECORD, 1, patient_id) == NULL)
        {
            printf("Invalid patient id");
            return;
        }

        fgets(description, DESCRIPTION_LENGTH, stdin);

        printf("Prescription : ");
        fgets(description, DESCRIPTION_LENGTH, stdin);

        Prescription si;
        si.id = get_last_id(DRUG_PRESCRIPTION_RECORD);
        si.doctor_id = doctor_id;
        si.patient_id = atoi(patient_id);
        strcpy(si.datetime, datetime);
        description[strlen(description) - 1] = '\0';
        strcpy(si.prescription, description);

        char record[MAX_RECORD_LENGTH];
        prescription_to_string(&si, record, DRUG_PRESCRIPTION_RECORD);

        append_record(DRUG_PRESCRIPTION_RECORD, record);
        printf("Record added successfully!\n");
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void add_sickness_info_action()
{

    if (has_privilege(current_user->privilege_level, "CREATE_SICKNESS_INFO_RECORD"))
    {
        char patient_id[100];
        int doctor_id = current_user->id;
        char datetime[100];
        char description[DESCRIPTION_LENGTH];

        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strcpy(datetime, asctime(timeinfo));
        datetime[strlen(datetime) - 1] = '\0';
        printf("Patient Id: ");

        if (scanf("%s", patient_id) == 0 || find_single_record(PATIENT_INFO_RECORD, 1, patient_id) == NULL)
        {
            printf("Invalid patient id");
            return;
        }

        fgets(description, DESCRIPTION_LENGTH, stdin);

        printf("Description : ");
        fgets(description, DESCRIPTION_LENGTH, stdin);

        SicknessInfo si;
        si.id = get_last_id(SICKNESS_INFO_RECORD);
        si.doctor_id = doctor_id;
        si.patient_id = atoi(patient_id);
        strcpy(si.datetime, datetime);
        description[strlen(description) - 1] = '\0';
        strcpy(si.description, description);

        char record[MAX_RECORD_LENGTH];
        sickness_info_to_string(&si, record);

        append_record(SICKNESS_INFO_RECORD, record);
        printf("Record added successfully!\n");
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void add_patient_action()
{

    if (has_privilege(current_user->privilege_level, "CREATE_PATIENT_RECORD"))
    {
        char user_id[100];
        char name[MAX_ATTRIBUTE_SIZE];
        char address[MAX_ATTRIBUTE_SIZE];
        char phone[20];
        char gender;
        int birth_year;

        printf("Patient User Account Id: ");
        if (scanf("%s", user_id) == 0 ||
            find_single_record(USER_RECORD, 1, user_id) == NULL)
        {
            printf("Non-existant user id\n");
            return;
        }
        fgets(name, MAX_ATTRIBUTE_COUNT, stdin);
        printf("Patient Name: ");
        fgets(name, MAX_ATTRIBUTE_COUNT, stdin);

        if ((strlen(name) > 0) && (name[strlen(name) - 1] == '\n'))
        {
            name[strlen(name) - 1] = '\0';
        }

        printf("Patient Address: ");

        fgets(address, MAX_ATTRIBUTE_COUNT, stdin);
        if ((strlen(address) > 0) && (address[strlen(address) - 1] == '\n'))
        {
            address[strlen(address) - 1] = '\0';
        }

        printf("Patient Phone: ");

        fgets(phone, MAX_ATTRIBUTE_COUNT, stdin);
        if ((strlen(phone) > 0) && (phone[strlen(phone) - 1] == '\n'))
        {
            phone[strlen(phone) - 1] = '\0';
        }
        printf("Gender (M or F): ");

        if (scanf("%c", &gender) == 0 ||
            (gender != 'M' && gender != 'F'))
        {
            printf("Invalid gender\n");
            return;
        }

        printf("Patient Birth Year: ");

        if (scanf("%d", &birth_year) == 0)
        {
            printf("Invalid year\n");
            return;
        }

        PatientInfo pi;
        pi.user_id = atoi(user_id);
        pi.id = get_last_id(PATIENT_INFO_RECORD) + 1;

        strcpy(pi.name, name);
        strcpy(pi.address, address);
        strcpy(pi.phone, phone);
        pi.gender = gender;
        pi.birth_year = birth_year;

        char record[MAX_RECORD_LENGTH];
        patient_info_to_string(&pi, record);
        append_record(PATIENT_INFO_RECORD, record);
        printf("Patient record created successfully!\n");
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void add_user_action()
{
    if (has_privilege(current_user->privilege_level, "CREATE_USER_RECORD"))
    {
        char username[MAX_ATTRIBUTE_SIZE];
        char password[MAX_ATTRIBUTE_SIZE];
        int user_type;
        int privilege_level;

        printf("username: ");
        scanf("%s", username);

        if (find_single_record(USER_RECORD, 2, username) != NULL)
        {
            printf("Username taken\n");
            return;
        }

        printf("password: ");
        scanf("%s", password);

        printf("user type (0 - Patient, 1 - Hostpial Staff, 2- Doctor)\n: ");

        if ((scanf("%i", &user_type) == 0) || user_type < 0 || user_type > 2)
        {
            printf("\nInvalid user type\n");
            return;
        }

        printf("privilege level (1 - Level1, 2 - Level2, 3 - Level3, 4 - Level4)\n: ");

        if (scanf("%d", &privilege_level) == 0 || privilege_level < 1 || privilege_level > 4)
        {
            printf("\nInvalid privilege level\n");
            return;
        }
        privilege_level--;

        char record[MAX_RECORD_LENGTH];
        User user;
        strcpy(user.username, username);

        strcpy(user.password, generate_hash(password));
        user.privilege_level = privilege_level;
        user.user_type = user_type;
        user.id = get_last_id(USER_RECORD) + 1;
        user_record_to_string(&user, record);
        append_record(USER_RECORD, record);
        printf("Record added successfully!\n");
    }
    else
    {
        printf("You do not have permission for this action\n");
    }
}

void do_action(char c)
{
    switch (c)
    {
    case 'a':
        add_user_action();
        break;
    case 'b':
        add_patient_action();
        break;
    case 'c':
        add_sickness_info_action();
        break;
    case 'd':
        add_drug_prescription_action();
        break;
    case 'e':
        add_lab_test_prescription_action();
        break;
    case 'f':
        view_patient_info_action();
        break;
    case 'g':
        view_drug_prescription_action();
        break;
    case 'h':
        view_lab_test_prescription_action();
        break;
    case 'i':
        view_sickness_info_action();
        break;

    default:
        break;
    }
}

int main()
{
    while (!prompt_login())
        ;
    print_menu();
    char c;
    do
    {

        scanf("%c", &c);
        if (c != 'q')
            do_action(c);
    } while (c != 'q');

    // else{
    // User *user = (User *)find_single_record(USER_RECORD, 1, "horyang");
    //      char string[MAX_RECORD_LENGTH];
    //     if (user==NULL) {
    //         printf("ITSNULL");return -1;
    //     }
    //     user_record_to_string(user, string);
    //     printf("%s\n", string);
    // }
}
