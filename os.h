// filename **********OS.H***********
// Real Time Operating System for Labs 2 and 3 
// Jonathan W. Valvano 1/27/14, valvano@mail.utexas.edu
// EE445M/EE380L.6 
// You may use, edit, run or distribute this file 
// You are free to change the syntax/organization of this file
// You are required to implement the spirit of this OS

#include <stdint.h>
#ifndef __OS_H
#define __OS_H  1

#define DEBUG_ON 1
#define DEBUG_OFF 0
#define DEBUG DEBUG_ON
#if DEBUG
#define PB2  (*((volatile unsigned long *)0x40005010))
#define PB3  (*((volatile unsigned long *)0x40005020))
#define PB4  (*((volatile unsigned long *)0x40005040))
#define PB5  (*((volatile unsigned long *)0x40005080))
#define PB6  (*((volatile unsigned long *)0x40005100))
#define Debug_Task(TASKNUM){ \
  switch(TASKNUM){           \
    case 0:                  \
      PB3 = 0x00;            \
      PB4 = 0x00;            \
      PB5 = 0x00;            \
      PB6 = 0x00;            \
      break;                 \
    case 1:                  \
      PB3 = 0x08;            \
      PB4 = 0x00;            \
      PB5 = 0x00;            \
      PB6 = 0x00;            \
      break;                 \
    case 2:                  \
      PB3 = 0x00;            \
      PB4 = 0x10;            \
      PB5 = 0x00;            \
      PB6 = 0x00;            \
      break;                 \
    case 3:                  \
      PB3 = 0x08;            \
      PB4 = 0x10;            \
      PB5 = 0x00;            \
      PB6 = 0x00;            \
      break;                 \
    case 4:                  \
      PB3 = 0x00;            \
      PB4 = 0x00;            \
      PB5 = 0x20;            \
      PB6 = 0x00;            \
      break;                 \
    case 5:                  \
      PB3 = 0x08;            \
      PB4 = 0x00;            \
      PB5 = 0x20;            \
      PB6 = 0x00;            \
      break;                 \
    case 6:                  \
      PB3 = 0x00;            \
      PB4 = 0x10;            \
      PB5 = 0x20;            \
      PB6 = 0x00;            \
      break;                 \
    case 7:                  \
      PB3 = 0x08;            \
      PB4 = 0x10;            \
      PB5 = 0x20;            \
      PB6 = 0x00;            \
      break;                 \
    case 8:                  \
      PB3 = 0x00;            \
      PB4 = 0x00;            \
      PB5 = 0x00;            \
      PB6 = 0x40;            \
      break;                 \
    case 9:                  \
      PB3 = 0x08;            \
      PB4 = 0x00;            \
      PB5 = 0x00;            \
      PB6 = 0x40;            \
      break;                 \
    case 10:                 \
      PB3 = 0x00;            \
      PB4 = 0x10;            \
      PB5 = 0x00;            \
      PB6 = 0x40;            \
      break;                 \
    case 11:                 \
      PB3 = 0x08;            \
      PB4 = 0x10;            \
      PB5 = 0x00;            \
      PB6 = 0x40;            \
      break;                 \
    case 12:                 \
      PB3 = 0x00;            \
      PB4 = 0x00;            \
      PB5 = 0x20;            \
      PB6 = 0x40;            \
      break;                 \
    case 13:                 \
      PB3 = 0x08;            \
      PB4 = 0x00;            \
      PB5 = 0x20;            \
      PB6 = 0x40;            \
      break;                 \
    case 14:                 \
      PB3 = 0x00;            \
      PB4 = 0x10;            \
      PB5 = 0x20;            \
      PB6 = 0x40;            \
      break;                 \
    case 15:                 \
      PB3 = 0x08;            \
      PB4 = 0x10;            \
      PB5 = 0x20;            \
      PB6 = 0x40;            \
      break;                 \
  }                          \
  PB2 ^= 0x04;               \
}
#endif //DEBUG

// edit these depending on your clock        
#define TIME_1MS    80000          
#define TIME_2MS    (2*TIME_1MS)  
#define TIME_500US  (TIME_1MS/2)  
#define TIME_250US  (TIME_1MS/5) 


/* ---------------------------------------------------- */
/*                      OS OBJECTS                      */
/* ---------------------------------------------------- */


enum uint8_t{
  screen0, screen1, screen2, screen3
};

struct MailboxMsg{
  uint8_t device;
  uint8_t line;
  char message[20];
};

typedef struct MailboxMsg Mail;

typedef unsigned char OS_objectType;
enum OS_objectType{
  tcb,
  semaphore,
  mutex
};

struct TCB{
	//stack
	uint32_t *stackPt;
  struct TCB* next;
  struct TCB* prev;
	uint32_t MoreStack[83];
	uint32_t Regs[14];
	void (*PC)(void);
	uint32_t PSR;
	//state variables
	uint8_t id;
	unsigned long sleep;
  uint8_t count;
	uint8_t priority;
	uint8_t blocked;
};

typedef struct TCB TCBType;

struct Mutex{
  OS_objectType       Type;
  char               *NamePtr;
  long                Value;
};
typedef struct Mutex MutexType;


// feel free to change the type of semaphore, there are lots of good solutions
struct  Sema4{
  long Value;   // >0 means free, otherwise means busy        
// add other components here, if necessary to implement blocking
};
typedef struct Sema4 Sema4Type;


// ******** OS_Init ************
// initialize operating system, disable interrupts until OS_Launch
// initialize OS controlled I/O: serial, ADC, systick, LaunchPad I/O and timers 
// input:  none
// output: none
void OS_Init(void); 

// ******** OS_InitSemaphore ************
// initialize semaphore 
// input:  pointer to a semaphore
// output: none
void OS_InitSemaphore(Sema4Type *semaPt, long value); 

// ******** OS_Wait ************
// decrement semaphore 
// Lab2 spinlock
// Lab3 block if less than zero
// input:  pointer to a counting semaphore
// output: none
void OS_Wait(Sema4Type *semaPt); 

// ******** OS_Signal ************
// increment semaphore 
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a counting semaphore
// output: none
void OS_Signal(Sema4Type *semaPt); 

// ******** OS_bWait ************
// Lab2 spinlock, set to 0
// Lab3 block if less than zero
// input:  pointer to a binary semaphore
// output: none
void OS_bWait(Sema4Type *semaPt); 

// ******** OS_bSignal ************
// Lab2 spinlock, set to 1
// Lab3 wakeup blocked thread if appropriate 
// input:  pointer to a binary semaphore
// output: none
void OS_bSignal(Sema4Type *semaPt); 

//******** OS_AddThread *************** 
// add a foregound thread to the scheduler
// Inputs: pointer to a void/void foreground task
//         number of bytes allocated for its stack
//         priority, 0 is highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// stack size must be divisable by 8 (aligned to double word boundary)
// In Lab 2, you can ignore both the stackSize and priority fields
// In Lab 3, you can ignore the stackSize fields
int OS_AddThread(void(*task)(void),  uint8_t priority, uint8_t id);

//******** OS_Id *************** 
// returns the thread ID for the currently running thread
// Inputs: none
// Outputs: Thread ID, number greater than zero 
uint8_t OS_Id(void);

//******** OS_AddPeriodicThread *************** 
// add a background periodic task
// typically this function receives the highest priority
// Inputs: pointer to a void/void background function
//         period given in system time units (12.5ns)
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// You are free to select the time resolution for this function
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal	 OS_AddThread
// This task does not have a Thread ID
// In lab 2, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, this command will be called 0 1 or 2 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddPeriodicThread(void(*task)(void), 
   unsigned long period, unsigned long priority);

//******** OS_AddSW1Task *************** 
// add a background task to run whenever the SW1 (PF4) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is the highest, 5 is the lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed that the user task will run to completion and return
// This task can not spin, block, loop, sleep, or kill
// This task can call OS_Signal  OS_bSignal	 OS_AddThread
// This task does not have a Thread ID
// In labs 2 and 3, this command will be called 0 or 1 times
// In lab 2, the priority field can be ignored
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddSW1Task(void(*pushTask)(void), void(*pullTask)(void), unsigned long priority);
/*
//******** OS_AddSW2Task *************** 
// add a background task to run whenever the SW2 (PF0) button is pushed
// Inputs: pointer to a void/void background function
//         priority 0 is highest, 5 is lowest
// Outputs: 1 if successful, 0 if this thread can not be added
// It is assumed user task will run to completion and return
// This task can not spin block loop sleep or kill
// This task can call issue OS_Signal, it can call OS_AddThread
// This task does not have a Thread ID
// In lab 2, this function can be ignored
// In lab 3, this command will be called will be called 0 or 1 times
// In lab 3, there will be up to four background threads, and this priority field 
//           determines the relative priority of these four threads
int OS_AddSW2Task(void(*task)(void), unsigned long priority);
*/
// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// You are free to select the time resolution for this function
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(unsigned long sleepTime); 

// ******** OS_Kill ************
// kill the currently running thread, release its TCB and stack
// input:  none
// output: none
void OS_Kill(void); 

// ******** OS_Suspend ************
// suspend execution of currently running thread
// scheduler will choose another thread to execute
// Can be used to implement cooperative multitasking 
// Same function as OS_Sleep(0)
// input:  none
// output: none
void OS_Suspend(void);

// ******** OS_Fifo_Init ************
// Initialize the Fifo to be empty
// Inputs: size
// Outputs: none 
// In Lab 2, you can ignore the size field
// In Lab 3, you should implement the user-defined fifo size
// In Lab 3, you can put whatever restrictions you want on size
//    e.g., 4 to 64 elements
//    e.g., must be a power of 2,4,8,16,32,64,128
void OS_Fifo_Init();

// ******** OS_Fifo_Put ************
// Enter one data sample into the Fifo
// Called from the background, so no waiting 
// Inputs:  data
// Outputs: true if data is properly saved,
//          false if data not saved, because it was full
// Since this is called by interrupt handlers 
//  this function can not disable or enable interrupts
int OS_Fifo_Put(unsigned long data);  

// ******** OS_Fifo_Get ************
// Remove one data sample from the Fifo
// Called in foreground, will spin/block if empty
// Inputs:  none
// Outputs: data 
unsigned long OS_Fifo_Get(void);

// ******** OS_Fifo_Size ************
// Check the status of the Fifo
// Inputs: none
// Outputs: returns the number of elements in the Fifo
//          greater than zero if a call to OS_Fifo_Get will return right away
//          zero or less than zero if the Fifo is empty 
//          zero or less than zero if a call to OS_Fifo_Get will spin or block
unsigned long OS_Fifo_Size(void);

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void);
int OS_MailBox_Count();
// ******** OS_MailBox_Send ************
// enter mail into the MailBox
// Inputs:  data to be sent
// Outputs: none
// This function will be called from a foreground thread
// It will spin/block if the MailBox contains data not yet received 
void OS_MailBox_Send(int device, int line, char* message);

// ******** OS_MailBox_Recv ************
// remove mail from the MailBox
// Inputs:  none
// Outputs: data received
// This function will be called from a foreground thread
// It will spin/block if the MailBox is empty 
Mail OS_MailBox_Recv();

// ******** OS_Time ************
// return the system time 
// Inputs:  none
// Outputs: time in 12.5ns units, 0 to 4294967295
// The time resolution should be less than or equal to 1us, and the precision 32 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_TimeDifference have the same resolution and precision 
unsigned long OS_Time(void);

// ******** OS_TimeDifference ************
// Calculates difference between two times
// Inputs:  two times measured with OS_Time
// Outputs: time difference in 12.5ns units 
// The time resolution should be less than or equal to 1us, and the precision at least 12 bits
// It is ok to change the resolution and precision of this function as long as 
//   this function and OS_Time have the same resolution and precision 
unsigned long OS_TimeDifference(unsigned long start, unsigned long stop);

// ******** OS_ClearMsTime ************
// sets the system time to zero (from Lab 1)
// Inputs:  none
// Outputs: none
// You are free to change how this works
void OS_ClearMsTime(void);

// ******** OS_MsTime ************
// reads the current time in msec (from Lab 1)
// Inputs:  none
// Outputs: time in ms units
// You are free to select the time resolution for this function
// It is ok to make the resolution to match the first call to OS_AddPeriodicThread
unsigned long OS_MsTime(void);

//******** OS_Launch *************** 
// start the scheduler, enable interrupts
// Inputs: number of 12.5ns clock cycles for each time slice
//         you may select the units of this parameter
// Outputs: none (does not return)
// In Lab 2, you can ignore the theTimeSlice field
// In Lab 3, you should implement the user-defined TimeSlice field
// It is ok to limit the range of theTimeSlice to match the 24-bit SysTick
void OS_Launch(unsigned long theTimeSlice);

#endif