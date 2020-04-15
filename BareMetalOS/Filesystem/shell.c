int getCommand(char *);
void type(char *);
void execute(char *);
void copy(char *);
void dir();
void fillShell(char *);
void fillErrorMessage(char *);

int mod(int a, int b);
int div(int a, int b);

#define MAX_COMMAND_SIZE 80
#define MAX_BUFFER_SIZE 13312

int main() {
    char line[MAX_COMMAND_SIZE], shell[8], error[15];
    int i, segment;
    fillShell(shell);
    fillErrorMessage(error);

    while (1) {
        interrupt(0x21, 0, shell, 0, 0);
        interrupt(0x21, 1, line, 0, 0);
        i = getCommand(line);
        if (i == 1) {
            type(line);
        } else if (i == 2) {
            execute(line);
        } else if (i == 3) {
            dir();
        } else if (i == 4) {
            copy(line);
        } else if (line[0] != 0xa || line[1] != 0x0) {
            interrupt(0x21, 0, error, 0, 0);
        }
    }
    return 0;
}

int getCommand(char *command) {
    if (command[0] == 't' && command[1] == 'y' && command[2] == 'p'
        && command[3] == 'e' && command[4] == ' ') {
        return 1;
    }else if (command[0] == 'e' && command[1] == 'x' && command[2] == 'e'
        && command[3] == 'c' && command[4] == 'u' && command[5] == 't'
        && command[6] == 'e' && command[7] == ' ') {
        return 2;
    }else if (command[0] == 'd' && command[1] == 'i' && command[2] == 'r') {
        return 3;
    }else if (command[0] == 'c' && command[1] == 'o' && command[2] == 'p'
        && command[3] == 'y' && command[4] == ' ') {
        return 4;
    }
    return 0;
}

void type(char *line) {
    char fileName[6], buffer[MAX_BUFFER_SIZE];
    int i;
    for (i = 0; i < 6 && line[5 + i] != 0xa; i++) {
        fileName[i] = line[5 + i];
    }
    for (; i < 6; i++) {
        fileName[i] = 0x0;
    }
    interrupt(0x21, 3, fileName, buffer, 0);
    interrupt(0x21, 0, buffer, 0, 0);
}

void execute(char *line) {
    char fileName[6];
    int i;
    for (i = 0; i < 6 && line[8 + i] != 0xa; i++) {
        fileName[i] = line[8 + i];
    }
    for (; i < 6; i++) {
        fileName[i] = 0x0;
    }
    interrupt(0x21, 4, fileName, 0x2000, 0);
}

void dir() {
    char directory[512], fileName[11];
    int i, j, k, numSectors;

    interrupt(0x21, 2, directory, 2, 0);

    for (i = 0; i < 512; i += 32) {
        if (directory[i] != 0x0) {
        numSectors = 0;
        for (j = i; j < i + 6 && directory[j] != 0x0; j++) {
            fileName[j - i] = directory[j];
        }
        fileName[j - i] = ' ';
        for (k = i + 6; k < i + 32 && directory[k] != 0x0; k++) {
            numSectors++;
        }
        if (numSectors > 9) {
            fileName[j-i+1] = '0' + div(numSectors, 10);
            fileName[j-i+2] = '0' + mod(numSectors, 10);
            fileName[j-i+3] = '\r';
            fileName[j-i+4] = '\n';
            fileName[j-i+5] = '\0';
        } else {
            fileName[j-i+1] = '0' + numSectors;
            fileName[j-i+2] = '\n';
            fileName[j-i+3] = '\r';
            fileName[j-i+4] = '\0';
        }
        
        interrupt(0x21, 0, fileName, 0, 0);
        }
    }
}

void copy(char *line) {
    char file1[6], file2[6], buffer[MAX_BUFFER_SIZE], directory[512];
    int i, j, corrent, sec;
    for (i = 5; line[i] != ' ' && i < 5 + 6; i++) {
        file1[i - 5] = line[i];
    }
    for (j = i; j < 5 + 6; j++) {
        file1[j - 5] = 0x0;
    }
    for (; line[i] != ' '; i++) {}
    i++;
    corrent = i;
    for (; line[i] != 0xa && i < corrent + 6; i++) {
        file2[i - corrent] = line[i];
    }
    for (; i <  corrent + 6; i++) {
        file2[i - corrent] = 0x0;
    }
    interrupt(0x21, 3, file1, buffer, 0); //read file1
    interrupt(0x21, 2, directory, 2); //read sector

    sec = 0;
    for (i = 0; i < 512; i += 32) {
        for (j = 0; j < 6; j++) {
            if (file1[j] != directory[i + j]) {
                break;
            }
        }
        if (j == 6) {
            for (j = 0; j < 26; j++) {
                if ((int) directory[i + 6 + j] == 0) {
                    break;
                }
                sec++;
            }
            break;
        }
    }
    interrupt(0x21, 8, file2, buffer, sec);
}

void fillShell(char *buffer) {
  buffer[0] = 'S';
  buffer[1] = 'H';
  buffer[2] = 'E';
  buffer[3] = 'L';
  buffer[4] = 'L';
  buffer[5] = '>';
  buffer[6] = ' ';
  buffer[7] = '\0';
}

void fillErrorMessage(char *buffer) {
  buffer[0] = 'B';
  buffer[1] = 'a';
  buffer[2] = 'd';
  buffer[3] = ' ';
  buffer[4] = 'c';
  buffer[5] = 'o';
  buffer[6] = 'm';
  buffer[7] = 'm';
  buffer[8] = 'a';
  buffer[9] = 'n';
  buffer[10] = 'd';
  buffer[11] = '!';
  buffer[12] = '\r';
  buffer[13] = '\n';
  buffer[14] = '\0';
}

int mod(int a, int b) {
  for (; a >= b; a -= b) {}
  return a;
}

int div(int a, int b) {
  int quotient;
  for (quotient = 0; (quotient + 1) * b <= a; quotient++) {}
  return quotient;
}