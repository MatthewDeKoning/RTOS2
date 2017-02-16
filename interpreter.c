#include "string.h"
#include "interpreter.h"
#include "ADCT2ATrigger.h"
#include "ST7735.h"
#include "Timer4A.h"
//#include "Clock.h"
#include "UART.h"
#include "OS.h"

static char strArray[ROWS][COLS];

int8_t interpreter_line = 0;
int8_t interpreter_device = 0;
char interpreter_msg[21];
/* the error message when an invalid command was passed */
const char static * errorMsg = "Invalid Command";

static char string[100];

//The function to run as interpreter thread
void INTERPRETER_Run(){
  while(1){
    UART_OutString(">");
    UART_InString(string, 99);
    INTERPRETER_parseMessage(string);
    OutCRLF();
    if(interpreter_device == -1){
      UART_OutString(interpreter_msg);
    }
    else{
      //UART_OutString(interpreter_msg);
      //ST7735_InitR(INITR_REDTAB); //This resets the SSI0 port (and unfortunately undoes the lines...)
      //if you want to check out the crash, uncomment the line above
      ST7735_ds_SetCursor(interpreter_device, 0, interpreter_line);
      ST7735_ds_OutString(interpreter_device, "                    ");
      ST7735_ds_SetCursor(interpreter_device, 0, interpreter_line);
      ST7735_ds_OutString(interpreter_device, interpreter_msg);
    }
    OutCRLF();
  }
}

void INTERPRETER_initArray(){
  int i;
  for(i = 0; i < ROWS; i++){
    strArray[i][0] = '\0';
  }
}
void INTERPRETER_clearMsg(){
  int i;
  for(i = 0; i < 21; i++)
    interpreter_msg[i] = '\0';
}

void INTERPRETER_parseMessage(char* str){
  INTERPRETER_clearMsg();
  const char* delim = " ";
  interpreter_device = 0; interpreter_line = 0;
  strtoklist((const char*)str, strArray , delim);
  //INTERPRETER_printParams();
  int command = INTERPRETER_handleCommand(INTERPRETER_handleParam1(&interpreter_device, &interpreter_line));
  
  //int screenFlag = 1;
  //if(interpreter_device == -1){
    //screenFlag = 0;
  //}
  //ST7735_ds_SetCursor(interpreter_device, 0, interpreter_line);
  int i = 0;
  switch(command){
    case 1:
      //calculator
      interpreter_msg[0] = 'A';interpreter_msg[1] = 'N';interpreter_msg[2] = 'S';interpreter_msg[3] = ' ';interpreter_msg[4] = '=';interpreter_msg[5] = ' ';
      itoa(INTERPRETER_calculator(), interpreter_msg, 10, 6);
      break;
    case 2:
      //adc
      interpreter_msg[0] = 'A';interpreter_msg[1] = 'D';interpreter_msg[2] = 'C';interpreter_msg[3] = ' ';interpreter_msg[4] = '=';interpreter_msg[5] = ' ';
      //itoa(ADC_GetVoltage(), interpreter_msg, 10, 6);
      break;
    case 3:
      //timer
      interpreter_msg[0] = 'A';interpreter_msg[1] = 'v';interpreter_msg[2] = 'g';interpreter_msg[3] = 'r';interpreter_msg[4] = 't';interpreter_msg[5] = ':';interpreter_msg[6] = ' ';
      //itoa(OS_AvgRunTime(), interpreter_msg, 10, 7);
      break;
    case 4:
      //print string
      for(i = 0; i < LengthOfString(strArray[1]);i++){
        interpreter_msg[i] = strArray[1][i];
      }
      interpreter_msg[i+1] = '\0';
      break;
    case 5:
      //adjust screen
      //ST7735_ds_InitR(INITR_REDTAB, strToInt(strArray[1]), strToInt(strArray[2]), strToInt(strArray[3]),strToInt(strArray[4]));
      //interpreter_msg[0] = '\0';
      break;
    case 6:
      //Toggle Real Time ADC
      //interpreter_msg[0] = '\0';
      //ST7735_ds_SetCursor(interpreter_device, 0, interpreter_line);
      //ST7735_ds_OutString(i, "                    ");
      //ADC_RTVoltageToggle(interpreter_device,interpreter_line);
      break;
    case 7:
      //print time
      //Clock_GetTime(interpreter_msg);
      break;
    case 8:
      //set date
      //Clock_SetDay(strToInt(strArray[2]));
      //Clock_SetMonth(strToInt(strArray[1]));
      //Clock_SetYear(strToInt(strArray[3]));
      //interpreter_msg[0] = 'D'; interpreter_msg[1] = 'a';interpreter_msg[2] = 't';interpreter_msg[3] = 'e';interpreter_msg[4] = ' ';interpreter_msg[5] = 's';interpreter_msg[6] = 'e';interpreter_msg[7] = 't';
      //interpreter_msg[8] = '\0'; 
      break;
    case 9:
      //set time
      //Clock_SetHour(strToInt(strArray[1]));
      //Clock_SetMinute(strToInt(strArray[2]));
      //Clock_SetSecond(strToInt(strArray[3]));
      //interpreter_msg[0] = 'T'; interpreter_msg[1] = 'i';interpreter_msg[2] = 'm';interpreter_msg[3] = 'e';interpreter_msg[4] = ' ';interpreter_msg[5] = 's';interpreter_msg[6] = 'e';interpreter_msg[7] = 't';
      //interpreter_msg[8] = '\0'; 
      break;
    case 10:
      //print date
      //Clock_GetDate(interpreter_msg);
      break;
    case 11:
      //ST7735_ds_SetCursor(interpreter_device, 0, interpreter_line);
      //ST7735_ds_OutString(i, "                    ");
      //Clock_RTClockToggle(interpreter_device, interpreter_line, 1);
      //interpreter_msg[0] = '\0';
      break;
    case 12:
      //ST7735_ds_SetCursor(interpreter_device, 0, interpreter_line);
      //ST7735_ds_OutString(i, "                    ");
      //Clock_RTClockToggle(interpreter_device, interpreter_line, 2);
      //interpreter_msg[0] = '\0';
      break;
    case 13:
      //interpreter_msg[0] = 'L';interpreter_msg[1] = 'n';interpreter_msg[2] = 'g';interpreter_msg[3] = 'r';interpreter_msg[4] = 't';interpreter_msg[5] = ':';interpreter_msg[6] = ' ';
      //itoa(OS_LongRunTime(), interpreter_msg, 10, 7);
      break;
    case 14:
      //interpreter_msg[0] = 'S';interpreter_msg[1] = 'm';interpreter_msg[2] = 'l';interpreter_msg[3] = 'r';interpreter_msg[4] = 't';interpreter_msg[5] = ':';interpreter_msg[6] = ' ';
      //itoa(OS_ShortRunTime(), interpreter_msg, 10, 7);
      break;
    default:
      for(i = 0; i < LengthOfString(errorMsg); i++){
        interpreter_msg[i] = errorMsg[i];
      }
      break;
  }
  
}
static int stack[15];
int32_t INTERPRETER_calculator(){
  int i;int total;int index;
  for(i = 0; i < ROWS; i++){
    if(strArray[i][0] == '\0')
      break;
  }
  total = i;
  i = 0;
  stack[i] = '\0';
  index = 0;
  while(i < total){
    uint8_t op = INTERPRETER_detectOperator(strArray[i][0]);
    if(op == 0xff || ((int)strArray[i][1] >=48 && (int)strArray[i][1] <= 57)){
      stack[index] = strToInt(strArray[i]);
      index++;
    }
    else{
      if(index >= 2){
        int value = 0;
        switch (op){
          case 0:
            value = stack[index-2] + stack[index-1];
            stack[index] = 0;
            index--;
            stack[index-1] = value;
            break;
          case 1:
            value = stack[index-2] - stack[index-1];
            stack[index] = 0;
            index--;
            stack[index-1] = value;
            break;
          case 2:
            value = stack[index-2] * stack[index-1];
            stack[index] = 0;
            index--;
            stack[index-1] = value;
            break;
          case 3:
            value = stack[index-2] / stack[index-1];
            stack[index] = 0;
            index--;
            stack[index-1] = value;
            break;
        }
      }
      else{
        return 0;
      }
    }
    i++;
  }
  return stack[0];
}
void INTERPRETER_fixArray0(uint8_t index){
  int i;
  for(i = 0; i < (LengthOfString(strArray[0]) - index); i++){
    strArray[0][i] = strArray[0][index+i];
  }
  strArray[0][i] = '\0';
}
uint8_t INTERPRETER_handleCommand(uint8_t index){
  int num = 1;
  int i;
  for(i = 0; i < (LengthOfString(strArray[0]) - index); i++){
    if(((int)strArray[0][index+i] <48 || (int)strArray[0][index+i] > 57) && strArray[0][index+i] != '-' ){
      num = 0;
      break;
    }
  }
  if(num){
    INTERPRETER_fixArray0(index);
    return 1;
  }
  switch (LengthOfString(strArray[0]) - index){
    case 3:
      if(strArray[0][index] == 'a' && strArray[0][index+1] == 'd' && strArray[0][index+2] == 'c'){
        return 2;
      }
      return 0xff;
      break;
    case 4:
      if(strArray[0][index] == 'd' && strArray[0][index+1] == 'a' && strArray[0][index+2] == 't' && strArray[0][index+3] == 'e'){
        return 10;
      }
      else if(strArray[0][index] == 't' && strArray[0][index+1] == 'i' && strArray[0][index+2] == 'm' && strArray[0][index+3] == 'e'){
        return 7;
      }
      return 0xff;
      break;
    case 5:
      if(strArray[0][index] == 'p' && strArray[0][index+1] == 'r' && strArray[0][index+2] == 'i' && strArray[0][index+3] == 'n' && strArray[0][index+4] == 't'){
        return 4;
      }
      else if(strArray[0][index] == 'a' && strArray[0][index+1] == 'd' && strArray[0][index+2] == 'c' && strArray[0][index+3] == 'o' && strArray[0][index+4] == 'n'){
        return 6;
      }
      return 0xff;
      break;
    case 6:
      if(strArray[0][index] == 's' && strArray[0][index+1] == 'c' && strArray[0][index+2] == 'r' && strArray[0][index+3] == 'e' && strArray[0][index+4] == 'e'&& strArray[0][index+5] == 'n'){
        return 5;
      }
      if(strArray[0][index] == 'a' && strArray[0][index+1] == 'd' && strArray[0][index+2] == 'c' && strArray[0][index+3] == 'o' && strArray[0][index+4] == 'f'&& strArray[0][index+5] == 'f'){
        return 6;
      }
      return 0xff;
      break;
    case 7:
      if(strArray[0][index] == 's' && strArray[0][index+1] == 'e' && strArray[0][index+2] == 't' && strArray[0][index+3] == 'd' && strArray[0][index+4] == 'a'&& strArray[0][index+5] == 't' && strArray[0][index+6] == 'e'){
        return 8;
      }
      else if(strArray[0][index] == 's' && strArray[0][index+1] == 'e' && strArray[0][index+2] == 't' && strArray[0][index+3] == 't' && strArray[0][index+4] == 'i'&& strArray[0][index+5] == 'm' && strArray[0][index+6] == 'e'){
        return 9;
      }
      else if(strArray[0][index] == 'r' && strArray[0][index+1] == 'u' && strArray[0][index+2] == 'n' && strArray[0][index+3] == 't' && strArray[0][index+4] == 'i'&& strArray[0][index+5] == 'm' && strArray[0][index+6] == 'e'){
        return 3;
      }
      return 0xff;
      break;
    case 8:
      if(strArray[0][index] == 's' && strArray[0][index+1] == 'h' && strArray[0][index+2] == 'o' && strArray[0][index+3] == 'w' && strArray[0][index+4] == 't' && strArray[0][index+5] == 'i'&& strArray[0][index+6] == 'm' && strArray[0][index+7] == 'e'){
        return 11;
      }
      else if(strArray[0][index] == 's' && strArray[0][index+1] == 'h' && strArray[0][index+2] == 'o' && strArray[0][index+3] == 'w' && strArray[0][index+4] == 'd' && strArray[0][index+5] == 'a' && strArray[0][index+6] == 't' && strArray[0][index+7] == 'e'){
        return 12;
      }
      else if(strArray[0][index] == 'r' && strArray[0][index+1] == 'u' && strArray[0][index+2] == 'n' && strArray[0][index+3] == 't' && strArray[0][index+4] == 'i'&& strArray[0][index+5] == 'm' && strArray[0][index+6] == 'e' && strArray[0][index+7] == 'l'){
        return 13;
      }
      else if(strArray[0][index] == 'r' && strArray[0][index+1] == 'u' && strArray[0][index+2] == 'n' && strArray[0][index+3] == 't' && strArray[0][index+4] == 'i'&& strArray[0][index+5] == 'm' && strArray[0][index+6] == 'e' && strArray[0][index+7] == 's'){
        return 14;
      }
      return 0xff;
      break;
      
    default:
      return 0xff;
      break;
  }
}
uint8_t   INTERPRETER_handleParam1(int8_t* interpreter_device, int8_t* interpreter_line){
  
  switch (firstIndex(strArray[0], ":")) {
    case 0:
      *interpreter_device = -1;
      *interpreter_line = -1;
      return 1;
      break;
    case 1:
      //TO SCREEN 0 interpreter_line 0
      return 2;
      break;
    case 2:
      //TO SCREEN interpreter_line[1] interpreter_line 0
      *interpreter_device = charToInt(strArray[0][1]);
      return 3;
      break;
      case 4:
      //TO SCREEN interpreter_line[1] interpreter_line interpreter_line[3]
      *interpreter_device = charToInt(strArray[0][1]);
      *interpreter_line = charToInt(strArray[0][3]);
      return 5;
      break;
  }
  
}

uint8_t INTERPRETER_detectOperator(char c){
  switch(c){
    case '+':
      return 0;
      break;
    case '-':
      return 1;
      break;
    case '*':
      return 2;
      break;
    case '/':
      return 3;
    default:
      return 0xff;
      break;
  }
}
/*void INTERPRETER_printParams(){
  int i =0;
  for(i = 0; i < ROWS; i++){
    if(strArray[i][0] != '\0'){
      printf("%s\n", strArray[i]);
    }
  }
  printf("\n");
}*/