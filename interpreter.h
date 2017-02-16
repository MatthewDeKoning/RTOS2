#ifndef _INTERPRETER_
#define _INTERPRETER_

void INTERPRETER_Run();
/*function to handle input string interpreter command
	params: char* to the string
	output: none
	Intepreter executes a valid command passed as a string. 
	deliminator is " ", display specified as such:
	s#1.#2:cmd 
	#1 = which screen to send the command to
	#2 = which line in the screen to send the command to
	Fills shared variable char interpreter_msg[] with message to print
	Fills shared variables interpreter_line and intepreter_device
	with information about how to output the results
*/
void    INTERPRETER_parseMessage(char*);

/* prints internal variable strArray[][] which contains the contents of 
   the previous string command  */
//void    INTERPRETER_printParams(void);

/* fills internal variable strArray with terminating characters */
void    INTERPRETER_initArray(void);

/* perform post-fix calculation on passed params */
int32_t INTERPRETER_calculator(void);

/* determine the value of intepreter_device and interpreter_line 
   while getting the first argument ready for further processing */
uint8_t INTERPRETER_handleParam1(int8_t*, int8_t*);

/* determines what sort of command is being requested by the command string */
uint8_t INTERPRETER_handleCommand(uint8_t);

/*determines what sort of arithmetic should be done*/
uint8_t INTERPRETER_detectOperator(char);

/*fills shared variable interpreter_msg with terminating characters*/
void INTERPRETER_clearMsg(void);

/* string to print to UART or the LCD screen */
extern char interpreter_msg[];
/* which line of the specified display should be printed*/
extern int8_t interpreter_line;
/* which display the output should go to */
extern int8_t interpreter_device;

#endif