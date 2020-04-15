void printString(char *);
void readString(char *);
void readSector(char *, int);
void handleInterrupt21(int, int, int, int);
int readFile(char *, char *);
void executeProgram(char *, int);
void terminate();
void writeSector(char *, int );
void deleteFile(char *);
void writeFile(char* , char* , int);
int mod(int, int);
int div(int, int);
void loadFileNotFound(char *);

int main() {
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';

  makeInterrupt21();
  interrupt(0x21, 4, shell, 0x2000, 0);

  while (1) {}
  return 0;
}

void printString(char *chars) {
  for (; *chars != '\0'; chars++) {
    interrupt(0x10, 0xe * 256 + *chars, 0, 0, 0);
  }
}

void readString(char *buffer) {
    char key;
    int i;
    key = interrupt(0x16, 0, 0, 0, 0);
    i = 0;
    while (key != 0xd && i < 78) {
      if (key == 0x8) {
        if (i > 0) {
          interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
          interrupt(0x10, 0xe * 256 + ' ', 0, 0, 0);
          interrupt(0x10, 0xe * 256 + 0x8, 0, 0, 0);
          i--;
        }
        key = interrupt(0x16, 0, 0, 0, 0);
        continue;
      }
      interrupt(0x10, 0xe * 256 + key, 0, 0, 0);
      *(buffer + i) = key;
      i++;
      key = interrupt(0x16, 0, 0, 0, 0);
    }
    interrupt(0x10, 0xe * 256 + 0xd, 0, 0, 0);
    interrupt(0x10, 0xe * 256 + 0xa, 0, 0, 0);
    *(buffer + i) = 0xa;
    *(buffer + i + 1) = 0x0;
}

void readSector(char *buffer, int sector) {
    int relative = mod(sector, 18) + 1;
    int head = mod(div(sector, 18),2);
    int track = div(sector, 36);
    int cx = track*256+relative;
    int dx = head*256+0;
    interrupt(0x13, (2*256)+1, buffer, cx, dx);
  }


void handleInterrupt21(int ax, int bx, int cx, int dx) {
    if (ax == 0) {
        printString((char *) bx);
    } else if (ax == 1) {
        readString((char *) bx);
    } else if (ax == 2) {
        readSector((char *) bx, cx);
    } else if (ax == 3) {
        readFile((char *) bx, (char *) cx);
    } else if (ax == 4) {
        executeProgram((char *) bx, cx);
    } else if (ax == 5) {
        terminate();
    } else if (ax == 6) {
        writeSector((char *) bx, cx);
    } else if (ax == 8) {
        writeFile((char *) bx, (char *) cx, dx);
    } else {
        printString("Error\r\n\0");
    }
}

int readFile(char *fileName, char *buffer) {
  char directory[512];
  int i, j, sec;
  readSector(directory, 2);

  for (i = 0; i < 512; i += 32) {
    for (j = 0; j < 6; j++) {
      if (fileName[j] != directory[i + j]) {
        break;
      }
    }
    if (j == 6) {
      for (j = 0; j < 26; j++) {
        sec = (int) directory[i + 6 + j];
        if (sec == 0) {
          break;
        }
        readSector(buffer+512*j, sec);
      }
      return 1;
    }
  }
  loadFileNotFound(buffer);
  return 0;
}

void executeProgram(char *name, int segment) {
  char buffer[13312];
  int i;
  if (readFile(name, buffer) == 0) {
    return;
  }

  for (i = 0; i < 13312; i++) {
    putInMemory(segment, i, buffer[i]);
  }

  launchProgram(segment);
}

void terminate() {
  char shell[6];
  shell[0] = 's';
  shell[1] = 'h';
  shell[2] = 'e';
  shell[3] = 'l';
  shell[4] = 'l';
  shell[5] = '\0';
  interrupt(0x21, 4, shell, 0x2000, 0);
}

void writeSector(char *buffer, int sector) {
  int relative = mod(sector, 18) + 1;
  int head = mod(div(sector, 18),2);
  int track = div(sector, 36);
  int cx = track*256+relative;
  int dx = head*256+0;
  interrupt(0x13, (3*256)+1, buffer, cx, dx);
}

void writeFile(char* name, char* buffer, int numberOfSectors) {
  char directory[512], map[512];
  int i, j, sector, sectorsWritten, directoryIndex;
  readSector(map, 1);
  readSector(directory, 2);

  for (i = 0; i < 512; i += 32) {
    if (directory[i] == 0x00) {
      directoryIndex = i;
      for (j = 0; j < 6; j++) {
        directory[i + j] = name[j];
      }
      break;
    }
  }

  sectorsWritten = 0;
  for (i = 0; i < 512 && sectorsWritten < numberOfSectors; i += 1) {
    if (map[i] == 0x00) {
      map[i] = 0xFF;
      directory[directoryIndex + 6 + sectorsWritten] = i;
      writeSector(buffer + sectorsWritten * 512, i);
      sectorsWritten++;
    }
  }
  for (i = sectorsWritten + 6; i < 32; i++) {
    directory[directoryIndex + i] = 0x00;
  }

  writeSector(map, 1);
  writeSector(directory, 2);
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

void loadFileNotFound(char *buffer) {
  buffer[0] = 'F';
  buffer[1] = 'i';
  buffer[2] = 'l';
  buffer[3] = 'e';
  buffer[4] = ' ';
  buffer[5] = 'n';
  buffer[6] = 'o';
  buffer[7] = 't';
  buffer[8] = ' ';
  buffer[9] = 'f';
  buffer[10] = 'o';
  buffer[11] = 'u';
  buffer[12] = 'n';
  buffer[13] = 'd';
  buffer[14] = '\r';
  buffer[15] = '\n';
  buffer[16] = '\0';
}