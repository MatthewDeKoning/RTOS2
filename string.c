#include "string.h"

//approximation of the itoa() function, based on an implementation
//from http://www.geeksforgeeks.org/implement-itoa/
// Implementation of itoa()
char* itoa(int num, char* str, int base, int offset)
{
  int i = 0;
  int negative = 0;
  
  //handle 0
  if (num == 0){
    str[i++] = '0';
    str[i] = '\0';
    return str;
  }
 
  // In standard itoa(), negative numbers are handled only with 
  // base 10. Otherwise numbers are considered unsigned.
  if (num < 0 && base == 10)
  {
    negative = 1;
    num = -num;
  }
  
  int copy = num;
  // Process individual digits
  while (copy != 0){  
    i++;
    copy = copy/base;
  }
  
  if (negative){
    str[offset] = '-';
    offset++;
  }
  
  str[i+offset] = '\0';
  i--;
  for(i; i >= 0; i--){
    int rem = num % base;
    num = num/base;
    str[i+offset] = (rem > 9)? (rem-10) + 'a' : rem + '0';
  }
 
  return str;
}

uint16_t LengthOfString(const char *str){
  int i;
  for(i = 0; str[i] != '\0'; ++i);
  return i;
}

int8_t strcomp(const char* str1, const char* str2){
  while(*str1 != '\0'){
    if(*str1 > *str2){
      return 1;
    }
    else if(*str1 < *str2){
      return -1;
    }
    if(*str2 == '\0')
      return 1;
    str1++; str2++;
  }
  if(*str2 != '\0'){
    return -1;
  }
  return 0;
}
int8_t firstIndex(const char* str, const char* delim){
  int i = 0;
  while(*str != '\0'){
    if(*str == *delim){
      return i;
    }
    else
    {
      str++;
      i++;
    }
  }
  return -1;
}

int16_t strToInt(const char* str){
  int i = LengthOfString(str);
  int len = i;
  int r = 0; int j = 0; int pow = 0;
  for(i; i > 0; i--){
    if(str[i-1] == '-'){
      //handle negative
      return ~r +1;    
    }
    else{
      pow = 1;
      for(j=0; j < (len-i); j++){
        pow*=10;
      }
      r+=charToInt(str[i-1])*pow;
      
    }
  }
  return r;
}
void strtoklist(const char* str, char str2[ROWS][COLS] , const char* delim){
  int word = 0;
  int letter = 0;
  while(*str != '\0'){
    if(*str == *delim){
      str2[word][letter] = '\0';
      word++;
      letter = 0;
      if(word > ROWS){
        return;
      }
      *str++;
    }
    else{
      if(letter > COLS){
        return;
      }
      str2[word][letter] = *str;
      letter++;
      *str++;
    }
  }
  str2[word][letter] = '\0';
  word++;
  while(word < ROWS){
	  str2[word][0] = '\0';
	  word++;
  }
}