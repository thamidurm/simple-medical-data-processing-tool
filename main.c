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

#define MAX_ATTRIBUTE_SIZE 256
#define MAX_ATTRIBUTE_COUNT 10
#define PERMISSION_SIZE 100
#define MAX_RECORD_LENGTH ((MAX_ATTRIBUTE_SIZE + 1) * MAX_ATTRIBUTE_COUNT)
#define CONFIG_FILE "config.dat"

#define DESCRIPTION_LENGTH 256

typedef enum PriviledgeLevelEnum
{
    PATIENT_LEVEL,  // Patients
    CLERK_LEVEL,    // Hospital Staff - Clerk
    LAB_LEVEL,      // Hosptial Staff - Lab
    MEDICINE_LEVEL, // Hostpital Staff - Medicine
    DOCTOR_LEVEL    // Doctors
} PriviledgeLevel;

typedef enum UserTypeEnum
{
    PATIENT,
    CLERK,
    LAB_STAFF,
    MEDICINE_STAFF,
    DOCTOR
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

typedef struct UserStruct
{
    int id;
    char username[100];
    char password[256];
    UserType user_type;
    PriviledgeLevel priviledge_level;
} User;

typedef struct PatientInfoStruct
{
    int id;
    char name[250];
    char address[250];
    char phone[20];
    char gender[5];
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

const char *patient_level_priviledges[5] = {
    "READ_SELF_PATIENT_RECORD",
    "READ_SELF_LAB_PRESCRIPTION_RECORD",
    "READ_SELF_MEDICINE_PRESCRIPTION_RECORD",
    "READ_SELF_SICKNESS_INFO_RECORD"
    "READ_SELF_USER_RECORD"};

const char *clerk_level_priviledges[3] = {
    "CREATE_PATIENT_RECORD",
    "READ_ANY_PATIENT_RECORD",
    "CREATE_ANY_USER_RECORD"};

const char *lab_level_priviledges[1] = {
    "READ_ANY_LAB_PRESCRIPTION"};

const char *medicine_level_priviledges[1] = {
    "READ_ANY_MEDICINE_PRESCRIPTION_RECORD"};

const char *doctor_level_priviledges[6] = {
    "CREATE_SICKNESS_INFO_RECORD",
    "CREATE_PRESCRIPTION_RECORD",
    "READ_ANY_PATIENT_RECORD",
    "READ_ANY_LAB_PRESCRIPTION_RECORD",
    "READ_ANY_MEDICINE_PRESCRIPTION_RECORD",
    "READ_ANY_SICKNESS_INFO_RECORD"};

bool has_priviledge(PriviledgeLevel level, char *priviledge)
{
    switch (level)
    {
    case PATIENT_LEVEL:
        return search(patient_level_priviledges, priviledge);
    case CLERK_LEVEL:
        return search(clerk_level_priviledges, priviledge);
    case LAB_LEVEL:
        return search(lab_level_priviledges, priviledge);
    case MEDICINE_LEVEL:
        return search(medicine_level_priviledges, priviledge);
    case DOCTOR_LEVEL:
        return search(doctor_level_priviledges, priviledge);
    default:
        return false;
    }
}

char *generate_priviledge_string(char *holder, RecordType type, char *action, char *owner)
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
            record->priviledge_level);
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
    sprintf(str, "%d;%d;%s;%s;%s;%s;%d\n",
            PATIENT_INFO_RECORD,
            record->id,
            record->name,
            record->address,
            record->phone,
            record->gender,
            record->birth_year);
    return str;
}

char *get_file_name(RecordType record_type)
{
    char *file_name;

    switch (record_type)
    {
    case USER_RECORD:
        file_name = CONFIG_FILE;
        break;
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

void append_record(RecordType record_type, char *record)
{
    FILE *file = open_file(record_type, "a");
    char record_init[2];
    sprintf(record_init, "%d;", record_type);
    fputs(record_init, file);
    fputs(record, file);
    fputs("\n", file);
    fclose(file);
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
            token = realloc(token, sizeof(char) * (t + 1));
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

int get_last_id(RecordType record_type)
{
    int line = 1;
    switch (record_type)
    {
    case USER_RECORD:
        line = 1;
        break;

    default:
        break;
    }
    FILE *file = fopen("ids.dat", "r");

    int cur = 0;
    int num;
    while (cur != line)
    {
        char *buffer = NULL;
        size_t len = 0;
        getline(&buffer, &len, file);
        num = atoi(buffer);
        free(buffer);
        cur++;
    }
    fclose(file);
    return num;
}

void set_last_id(RecordType record_type, int new_id)
{
    int line = 1;
    switch (record_type)
    {
    case USER_RECORD:
        line = 1;
        break;

    default:
        break;
    }
    FILE *file = fopen("ids.dat", "r");

    int cur_line = 1;
    char *buffer = NULL;
    size_t len = 0;
    char *content = NULL;
    while (getline(&buffer, &len, file) != -1)
    {
        if (content != NULL)
        {
            if (cur_line == line)
            {
                char num[100];
                sprintf(num, "%d\n", new_id);
                content = realloc(content, sizeof(char) * (strlen(content) + strlen(num) + 1));
                strcat(content, num);
            }
            else
            {
                content = realloc(content, sizeof(char) * (strlen(content) + strlen(buffer) + 1));
                strcat(content, buffer);
            }
        }
        else
        {
            if (cur_line == line)
            {
                char num[100];
                sprintf(num, "%d\n", new_id);
                content = num;
            }
            else
            {
                content = malloc(sizeof(char) * (strlen(buffer) + 1));
                strcat(content, buffer);
            }
        }
        cur_line++;
    }
    free(buffer);
    fclose(file);

    remove("ids.dat");

    FILE *temp = fopen("ids.dat.tmp", "w");
    fputs(content, temp);
    fclose(temp);

    rename("ids.dat.tmp", "ids.dat");
    free(content);
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
            user->priviledge_level = atoi(token);
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
            strcpy(patient_info->name, token);
            break;
        case 3:
            strcpy(patient_info->address, token);
            break;
        case 4:
            strcpy(patient_info->phone, token);
            break;
        case 5:
            strcpy(patient_info->gender, token);
            break;
        case 6:
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
    break;
    default:
        printf("Error incorrect record type");
        exit(2);
    }
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

void* append_record_to_array(void* array, void *item, RecordType type, int size)
{
    int new_size = size + 1;
    switch (type)
    {
    case USER_RECORD:
    {
        User* new_address;
        if(array == NULL){
            new_address = (User*) malloc(new_size * sizeof(User));
            new_address[new_size-1] = *((User*)item);
        } else {
            new_address = (User*) realloc(array, new_size * sizeof(User));
            new_address[new_size-1] = *((User*)item);
        }
        return new_address;
    }
    break;
    // case PATIENT_INFO_RECORD:
    // {
    //     PatientInfo **user_array = (PatientInfo **)array;
    //     if (array == NULL)
    //     {
    //         *array = malloc(sizeof(PatientInfo));
    //         array[0] = (PatientInfo *)item;
    //     }
    //     else
    //     {
    //         int size = sizeof(array) / sizeof(array[0]);
    //         *array = realloc(array, size * sizeof(PatientInfo));
    //         array[size] = (PatientInfo *)item;
    //     }
    // }
    // break;
    // case DRUG_PRESCRIPTION_RECORD:
    // case LAB_TEST_PRESCRIPTION_RECORD:
    // {
    //     Prescription **user_array = (Prescription **)array;
    //     if (array == NULL)
    //     {
    //         *array = malloc(sizeof(Prescription));
    //         array[0] = (Prescription *)item;
    //     }
    //     else
    //     {
    //         int size = sizeof(array) / sizeof(array[0]);
    //         *array = realloc(array, size * sizeof(Prescription));
    //         array[size] = (Prescription *)item;
    //     }
    // }
    // break;
    // case SICKNESS_INFO_RECORD:
    // {
    //     SicknessInfo **user_array = (SicknessInfo **)array;
    //     if (array == NULL)
    //     {
    //         *array = malloc(sizeof(SicknessInfo));
    //         array[0] = (SicknessInfo *)item;
    //     }
    //     else
    //     {
    //         int size = sizeof(array) / sizeof(array[0]);
    //         *array = realloc(array, size * sizeof(SicknessInfo));
    //         array[size] = (SicknessInfo *)item;
    //     }
    // }
    // break;
    default:
        printf("Error incorrect record type");
        exit(2);
    }
}

void *find_all_records(RecordType record_type, int search_attribute, char *value)
{
    void *array = NULL;
    FILE *file = open_file(record_type, "r");
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
        while (next_token(buffer, token, &token_start) != -1)
        {
        
            if (attribute == 0)
            {
                if (atoi(token) != record_type)
                break;
            }
            else
            {
                if (attribute == search_attribute && strcmp(value, token) == 0)
                {
                    printf("Found!\n");
                    found = true;
                } 

                set_attribute_for_record(current, record_type, attribute, token);
            }

            attribute++;

    
        }

        if (found)
        {
            array = append_record_to_array(array, current, record_type, size++);
            // User** arry = (User**)array;
            // User* usr = arry[0];
            printf("%s", "ads");
  
        }
    }
  
    if (buffer != NULL)
        free(buffer);

    if (token != NULL)
        free(token);

    fclose(file);
    if (found)
        return array;
    else
        return NULL;
}

void *find_single_record(RecordType record_type, int search_attribute, char *value)
{

    FILE *file = open_file(record_type, "r");

    char *buffer = NULL;
    size_t len = 0;
    bool found = false;
    void *current = allocate_space_for_record(record_type);
    int token_start, attribute;
    char *token = (char *)malloc(MAX_ATTRIBUTE_SIZE + 1);
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
            else
            {
                if (attribute == search_attribute && strncmp(value, token, sizeof token) == 0)
                {
                    printf("Found!\n");
                    found = true;
                }
                set_attribute_for_record(current, record_type, attribute, token);
            }

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

// delete_record

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
                char row[MAX_RECORD_LENGTH];
                sprintf(row, "%s\n", new_row);
                if (content == NULL)
                {
                    content = malloc((strlen(row) + 1) * sizeof(char));
                    strcpy(content, row);
                }
                else
                {
                    content = realloc(content, (strlen(content) + strlen(row) + 1) * sizeof(char));
                    strcat(content, row);
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

char* generate_hash(char *string)
{
    return string;
}

void prompt_login()
{
    char *username;
    char *password;

    printf("Username: ");
    scanf("%s", username);

    printf("Password: ");
    scanf("%s", password);
    User *user;

    if (user = (User *)find_single_record(USER_RECORD, 2, username))
    {
    }
}

int main()
{
    // prompt_login();
    User **users = (User **)find_all_records(USER_RECORD, 2, "horyang");
    if (users != NULL){
        printf("YOY\n");
        int size = sizeof(users) / sizeof(users[0]);
        for (int i = 0; i < size; i++)
        {   
            printf("ASfsa");
            char string[MAX_RECORD_LENGTH];
            user_record_to_string(users[i], string);
            printf("%s\n", string);
        }
    }
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
