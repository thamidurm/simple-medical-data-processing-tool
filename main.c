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

#define MAX_ATTRIBUTE_SIZE 256
#define MAX_ATTRIBUTE_COUNT 10
#define CONFIG_FILE "config.dat"

typedef enum PriviledgeEnum
{
    LEVEL1,
    LEVEL2
} Priviledge;

typedef enum UserTypeEnum
{
    PATIENT,
    DOCTOR
} UserType;

typedef enum RecordTypeEnum
{
    USER_RECORD
} RecordType;

typedef struct UserStruct
{
    int id;
    char *username;
    char *password;
    UserType user_type;
    Priviledge priviledge_level;
} User;

char *decrypt(char *string)
{
}

char *encrypt(char *string)
{
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
    fputs(record, file);
    fclose(file);
}

int next_token(char *string, char *token, int *current)
{
    int t = 0;
    while (string[*current] != '\0')
    {
        if (string[*current] == ',' || string[*current] == '\n')
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

User *find_record(RecordType record_type, int search_attribute, char *value)
{

    FILE *file = open_file(record_type, "r");

    char *buffer = NULL;
    size_t len = 0;
    bool found = false;
    User *current = malloc(sizeof(User));
    int token_start, attribute;
    char *token = (char *)malloc(MAX_ATTRIBUTE_SIZE + 1);

    while (getline(&buffer, &len, file) != -1)
    {
        token_start = 0;
        attribute = 1;

        while (next_token(buffer, token, &token_start) != -1)
        {
            switch (attribute)
            {
            case 1:
                (*current).id = atoi(token);
                break;
            case 2:
                (*current).username = malloc((strlen(token) + 1) * sizeof(char));
                strcpy((*current).username, token);
                break;
            case 3:
                (*current).password = malloc((strlen(token) + 1) * sizeof(char));
                strcpy((*current).password, token);
                break;
            case 4:
                (*current).user_type = atoi(token);
                break;
            case 5:
                (*current).priviledge_level = atoi(token);
                break;
            }

            if (attribute == search_attribute && strcmp(value, token) == 0)
            {
                printf("Found!\n");
                found = true;
            }
            attribute++;
        }

        if (found)
        {
            break;
        }
        else
        {
            free((*current).password);
            free((*current).username);
        }
    };

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
void update_record(RecordType record_type, int id, char* new_row)
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
                char row[(MAX_ATTRIBUTE_SIZE + 1) * MAX_ATTRIBUTE_COUNT];
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

int main()
{
    // User* user = find_record(USER_RECORD, 1, "cat");

    // if (user != NULL){
    //     printf("User.name: %s", (*user).username);
    // } else {
    //     printf("Not found!");
    // }

    // free(user);

    update_record(USER_RECORD, 3, "3,cato,passwordzz,1,1");
}
