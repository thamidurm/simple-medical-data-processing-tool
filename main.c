/***********************************************************************
* FILENAME :    main.token_start             
*
* DESCRIPTION :
*       Simple record store for a hosptial with
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

#define MAX_ATTRIBUE_SIZE 256

int LAST_USER_ID;

typedef enum PriviledgeEnum {
    LEVEL1,
    LEVEL2
} Priviledge;

typedef enum UserTypeEnum {
    PATIENT,
    DOCTOR
} UserType;

typedef enum RecordTypeEnum{
    USER_RECORD
} RecordType;

typedef struct UserStruct{
 char* username;
 char* password;
 UserType user_type;
 Priviledge priviledge_level;
} User;

char* decrypt(char* string){

}

char* encrypt(char* string){

}

FILE* open_file(RecordType record_type, char* mode){
    char* file_name;

    switch (record_type){
        case USER_RECORD:
            file_name = "config.dat";
        break;
    }
    
    FILE* file = fopen(file_name, mode);

    if (file == NULL){
        printf("Error: Coudln't open %s file!", file_name);
        exit(1);
    }
    
    return file;
}

void append_record(RecordType record_type, char* record){
    FILE* file = open_file(record_type, "a");
    fputs(record, file);
    fclose(file);
}

int next_token(char* string, char* token, int* current){
    int* attribute_no = current;
    int t = 0;
    
    while(string[*attribute_no] != '\0'){
        if (string[*attribute_no] == ',' || string[*attribute_no] == '\n'){
            (*attribute_no)++;
            token[t]='\0';
            token = realloc(token, sizeof(char) * (t+1));
            return 1;
        } else {
            token[t] = string[*attribute_no];
            t++;
        }
        (*attribute_no)++;
    }
    return -1;
}

void find_record(RecordType record_type, int attribute, char* value){
    FILE* file = open_file(record_type, "r");
    char *buffer = NULL;
    size_t len = 0;
    ssize_t nread;
    bool found = false;
    User current;
    int token_start, attribute_no;
    char*  token = (char*) malloc(MAX_ATTRIBUE_SIZE + 1);

    while((nread = getline(&buffer, &len,file)) != -1){
         token_start = 0;
         attribute_no = 1; 
         
         while(next_token(buffer, token, &token_start) != -1){
         
            switch (attribute_no){
                case 1:
                    strcpy(current.username, token);
                    break;
                case 2:
                    strcpy(current.password, token);
                    break;
                case 3:
                    current.user_type = atoi(token);
                    break;
                case 4:
                    current.priviledge_level = atoi(token);
                    break;
            }

            if (attribute_no == attribute && strcmp(value, token) == 0){
                printf("Found!\n");
                found = true;
            }
            attribute_no++;
         }
         free(token);
         
             printf("Username: %s\n", current.username);
    };
    
    if (buffer!= NULL)
        free(buffer);

    fclose(file);
    free(buffer);
    
    
   
}

void delete_record(RecordType record_type, int id){

}

// update_record


int main(){
    //char record[256] = "user1, password, type, level\n";
    //append_record(USER_RECORD, record);
    find_record(USER_RECORD, 1, "cat");
}
