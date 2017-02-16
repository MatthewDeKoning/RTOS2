#ifndef _STRING_
#define _STRING_
#include <stdint.h>

//up to 10 args
#define ROWS 10
//int16 numbers read in (+-~32500) all commands less than 5 chars
#define COLS 15
//macro to translate the value of a number char to an int
#define charToInt(c) ((int)c-48)

/* compare two strings:
   returns 1 if the first string is "greater"
   returns 0 if they are equal
   returns -1 if the second string is "greater"
*/
int8_t strcomp(const char*, const char*);

/* return the length of a string */
uint16_t LengthOfString(const char*);

/* fill a char array[ROWS][COLS] with the different parts of 
   the passed string, broken into parts deliminated by 
   the last string parameter
*/
void strtoklist(const char*, char[ROWS][COLS], const char*);

/* return the index of the first occurence of the passed parameter */
int8_t firstIndex(const char* , const char* );

/*return the int equal to the value of the number string*/
int16_t strToInt(const char*);

/*return a string loaded with a numerical value:
   param num - number to turn into string
   param str - string to fill with the number
   param base - base of the number to turn into string 
   param offset - offset into the string to place the number
*/
char* itoa(int , char* , int , int);

#endif //String