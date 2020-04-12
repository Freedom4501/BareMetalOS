//Worked with Mashengjun Li

void printString(char *);
void readString(char *);
void readSector(char*, int);
int mod(int, int);
int div(int, int);
void handleInterrupt21(int ax, int bx, int cx, int dx);


int main()
{
    char line[80];
    char *printout; 
    char buffer[512];
    //1. Printing out hello world at the top left using raw memory commands
    putInMemory(0xB000, 0x80A0, 'H');
    putInMemory(0xB000, 0x80A1, 0x7);
    putInMemory(0xB000, 0x80A2, 'e');
    putInMemory(0xB000, 0x80A3, 0x7);
    putInMemory(0xB000, 0x80A4, 'l');
    putInMemory(0xB000, 0x80A5, 0x7);
    putInMemory(0xB000, 0x80A6, 'l');
    putInMemory(0xB000, 0x80A7, 0x7);
    putInMemory(0xB000, 0x80A8, 'o');
    putInMemory(0xB000, 0x80A9, 0x7);
    putInMemory(0xB000, 0x80AA, ' ');
    putInMemory(0xB000, 0x80AB, 0x7);
    putInMemory(0xB000, 0x80AC, 'W');
    putInMemory(0xB000, 0x80AD, 0x7);
    putInMemory(0xB000, 0x80AE, 'o');
    putInMemory(0xB000, 0x80AF, 0x7);
    putInMemory(0xB000, 0x80B0, 'r');
    putInMemory(0xB000, 0x80B1, 0x7);
    putInMemory(0xB000, 0x80B2, 'l');
    putInMemory(0xB000, 0x80B3, 0x7);
    putInMemory(0xB000, 0x80B4, 'd');
    putInMemory(0xB000, 0x80B5, 0x7);
    putInMemory(0xB000, 0x80B6, '!');
    putInMemory(0xB000, 0x80B7, 0x7);
   
    
    
    makeInterrupt21();
    //2. Using printString to display a string to the console
    printout = "Hello World!\0";
    interrupt(0x21,0,printout,0,0);
    //3. Reading and displaying characters entered with a keyboard
    interrupt(0x21,1,line,0,0);
    //4. Displaying the string read back to the console at the start of a new line
    interrupt(0x21,0,line,0,0);
    //5. Using readSector to read the sector that contains message.txt and displays the string to the console
    interrupt(0x21,2, buffer, 30, 0);
    interrupt(0x21,0, buffer, 0, 0);
    while (1){}
    return 0;
}
void printString(char *chars){
    while(*chars != 0x0){
      interrupt(0x10, 0xe*256+*chars, 0, 0, 0);
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
      // printString("\r\n");
      interrupt(0x13, (2*256)+1, buffer, cx, dx);
    }


  void handleInterrupt21(int ax, int bx, int cx, int dx){
    if(ax == 0){
      printString(bx);
      printString("\r\n");

    }
    else if(ax == 1){
      printString("Enter a line: \0");
      readString(bx);
    }
    else if(ax == 2){
      readSector(bx, cx);
      //printString(bx);
    }
    else if(ax >= 3){
      printString("error, ax can't be 3 or greater");
    }
  }
