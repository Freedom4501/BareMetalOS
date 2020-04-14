//Worked with Mashengjun Li



void printString(char *);
void readString(char *);
void readSector(char*, int);
void readSector(char*, int);
int mod(int, int);
int div(int, int);
int i;
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*);
void writeFile(char*,char*, int);
void executeProgram(char* name, int segment);
void terminate();

  
int main()
{
    char buffer[13312];  /* this is the maximum size of a file */
    makeInterrupt21();
    terminate();
    while(1);
    return 0;
}
  void printString(char *chars){
      while(*chars != 0x0){
        if(*chars == '\n'){
          interrupt(0x10, 0xe*256+0xa, 0, 0, 0);
          interrupt(0x10, 0xe*256+0xd, 0, 0, 0);
        }else{
          interrupt(0x10, 0xe*256+*chars, 0, 0, 0);
        }
        
        chars++;  
      }
  }


    void readString(char *line)
    {
      int i, lineLength, ax;
      char charRead, backSpace, enter;
      lineLength = 80;
      i = 0;
      ax = 0;
      backSpace = 0x8;
      enter = 0xd;
      charRead = interrupt(0x16, ax, 0, 0, 0);
      while (charRead != enter && i < lineLength-2) {
        if (charRead != backSpace) {
          interrupt(0x10, 0xe*256+charRead, 0, 0, 0);
          line[i] = charRead;
          i++;
        } else {
          i--;
          if (i >= 0) {
    	      interrupt(0x10, 0xe*256+charRead, 0, 0, 0);
    	      interrupt(0x10, 0xe*256+'\0', 0, 0, 0);
    	      interrupt(0x10, 0xe*256+backSpace, 0, 0, 0);
          }
          else {
    	      i = 0;
          }
        }
        charRead = interrupt(0x16, ax, 0, 0, 0);  
      }
      line[i] = 0xa;
      line[i+1] = 0x0;
      
      /* correctly prints a newline */
      printString("\r\n");

      return;
    }

    int mod(int a, int b)
    {
      int temp;
      temp = a;
      while (temp >= b) {
        temp = temp-b;
      }
      return temp;
    }

    int div(int a, int b)
    {
      int quotient;
      quotient = 0;
      while ((quotient + 1) * b <= a) {
        quotient++;
      }
      return quotient;
    }

    void readSector(char* buffer, int sector){
      int relative = mod(sector, 18) + 1;
      int head = mod(div(sector, 18),2);
      int track = div(sector, 36);
      int cx = track*256+relative;
      int dx = head*256+0;
      interrupt(0x13, (2*256)+1, buffer, cx, dx);
    }
    void writeSector(char* buffer, int sector){
      int relative = mod(sector, 18) + 1;
      int head = mod(div(sector, 18),2);
      int track = div(sector, 36);
      int cx = track*256+relative;
      int dx = head*256+0;
      interrupt(0x13, (3*256)+1, buffer, cx, dx);
    }

  void readFile(char* filename, char* buffer){
      int index;
      int i;
      int matches;
      int sectorNum;
      char directory[512];
      buffer[0] = '\n';
      buffer[1] = '\0';
      readSector(directory, 2);
      for(index = 0;index < 512; index+=32){
        matches = 1;
        for(i = 0; i < 6&&filename[i] !='\0';i++){
          if(directory[i+index]!=filename[i]){
            matches = 0;
            break;
          }
        }
        if(i < 6 && directory[i + index]!=filename[i]){
          matches = 0;
        }
        if(matches){
          for(i = 0; directory[6+index + i]!=0x00; i++){
            readSector(buffer + 512*i, directory[6+index+i]);
          }
          return;
        }
      }  
  }
  void writeFile(char* name, char* buffer, int numberOfSectors){
    
  }

void executeProgram(char* name, int segment){
    char buffer[13312];
    int i;
    readFile(name, buffer);
    for(i = 0; i < 13312; i++){
      putInMemory(segment, i, buffer[i]);
    }
    launchProgram(segment);
}

  void terminate(){
    char shell[6];
    shell[0] = 's';
    shell[1] = 'h';
    shell[2] = 'e';
    shell[3] = 'l';
    shell[4] = 'l';
    shell[5] = '\0';

    interrupt(0x21, 4, shell, 0x2000, 0);
  }


  void handleInterrupt21(int ax, int bx, int cx, int dx){
    if(ax == 0){
      printString(bx);
      printString("\r\n");

    }
    else if(ax == 1){
      readString(bx);
    }
    else if(ax == 2){
      readSector(bx, cx);
      printString(bx);
    }
    else if(ax == 3){
      readFile((char*)bx, (char*)cx);
    }
    else if(ax == 4){
      executeProgram((char*)bx, cx);
    }

    else if(ax == 5){
      terminate();
    }else if(ax == 6){
      writeSector(bx, cx);
    }
    else{
      printString("error, ax can't be 7 or greater");
    }
  }

