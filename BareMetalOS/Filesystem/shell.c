
#define PRINT_STRING 0
#define READ_STRING 1
#define READ_SECTOR 2
#define READ_FILE 3
#define EXECUTE_PROG 4
#define TERMINATE 5

#define SHELL_OUT "SHELL>\0"
#define BAD_COMMAND "Bad Command!\r\n"
#define CHANGE_LINE "\r\n"

#define MAX_BUFFER_SIZE 13312
#define EXECUTE_SIZE 8
#define TYPE_SIZE 5
#define MAX_COMMAND_SIZE 80


int executeCommand(char *);
void getCommand(char*, char*);
void getArg(char*, char*);
int isEqual(char*, char*);

 
int main(){
    char* command;
    while(1){
        interrupt(0x21, PRINT_STRING, SHELL_OUT, 0, 0);
        interrupt(0x21, READ_STRING, command, 0 ,0);
        interrupt(0x21, PRINT_STRING, CHANGE_LINE,0,0);
        if(!executeCommand(command)){
            interrupt(0x21, PRINT_STRING, BAD_COMMAND, 0, 0);
        } 
    }
}

int executeCommand(char* command){
    char buffer[MAX_BUFFER_SIZE];
    char name[MAX_COMMAND_SIZE];
    char type[TYPE_SIZE];
    char execute[EXECUTE_SIZE];
    char* arg[MAX_COMMAND_SIZE];
    type[0] = 't';
    type[1] = 'y';
    type[2] = 'p';
    type[3] = 'e';
    type[4] = '\0';
    execute[0] = 'e';
    execute[1] = 'x';
    execute[2] = 'e';
    execute[3] = 'c';
    execute[4] = 'u';
    execute[5] = 't';
    execute[6] = 'e';
    execute[7] = '\0';

    getCommand(command, name);
    if(isEqual(name, type)){
        getArg(command, arg);
        interrupt(0x21, READ_FILE, arg, buffer, 0, 0);
        interrupt(0x21, PRINT_STRING, buffer, 0 ,0);
        return 1;
    } else if(isEqual(name, execute)){
        getArg(command, arg);
        interrupt(0x21, EXECUTE_PROG, arg, 0x2000 ,0);
        return 1;
    } 
    return 0;
}

void getCommand(char* command, char* name){
    int i = 0;
    while(command[i]!='\0' && command[i] != ' '){
        name[i] = command[i];
        i++;
    }
    name[i] = '\0';
}

void getArg(char* command, char* arg){
    int i = 0;
    int j = 0;
    int boole = 0;

    while(command[i]!='\0'){
        if(boole){
            arg[j] = command[i];
            j++;
        }
        if(command[i] == ' '){
            boole = 1;
        }
        i++;
    }
    arg[j] = '\0';
}

int isEqual(char* s1, char* s2){
    int i = 0;
    while(s1[i]!='\0' && s2[i] != '\0'){
        if(s1[i] != s2[i]){
            return 0;
        }
        i++;
    }
    return s1[i] == '\0' && s2[i] == '\0';
}

