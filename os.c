#include "os.h"
#include "FIFO.h"
#include "../inc/tm4c123gh6pm.h"


#define FIFOSIZE   16         // size of the FIFOs (must be power of 2)
#define FIFOSUCCESS 1         // return value on success
#define FIFOFAIL    0         // return value on failure
                              // create index implementation FIFO (see FIFO.h)
//AddIndexFifo(Rx_OS, FIFOSIZE, char, FIFOSUCCESS, FIFOFAIL)
//AddIndexFifo(Tx_OS, FIFOSIZE, char, FIFOSUCCESS, FIFOFAIL)




TCBType* RunPt;
//TCBType* NextPt;
const uint8_t TCB_COUNT = 3;
TCBType tcbs[TCB_COUNT];

void OS_Wait(Sema4Type *semaPt){
  DisableInterrupts();
  while(semaPt->Value <=0){
    EnableInterrupts();
    DisableInterrupts();
    //OS_Suspend();
  }
  semaPt->Value = semaPt->Value - 1;
  EnableInterrupts();
  
}

void OS_Signal(Sema4Type *semaPt){
  long status;
  status = StartCritical();
  semaPt->Value = semaPt->Value + 1;
  EndCritical(status);
  
}

void OS_Waitb(Sema4Type *semaPt){
  DisableInterrupts();
  while(semaPt->Value != 1){
    EnableInterrupts();
    DisableInterrupts();
    //OS_Suspend();
  }
  semaPt->Value = 0;
  EnableInterrupts();
  
}

void OS_Signalb(Sema4Type *semaPt){
  long status;
  status = StartCritical();
  semaPt->Value = 1;
  EndCritical(status);
}

void OS_InitSemaphore(Sema4Type *semaPt, long value){
  semaPt->Value = value;
}


void OS_Suspend(void){
  //call scheduler
  NVIC_INT_CTRL_R = 0x10000000; // Trigger PendSV
}

uint8_t OS_Id(void){
  return RunPt->id;
}

void OS_Kill(void){
  struct TCB* new_prev = RunPt->prev;
  struct TCB* new_next = RunPt->next;
  
  //properly link the tasks before and after the current
  new_next->prev = new_prev;
  new_prev->next = new_next;
  
  //set current running id to 0 for killed
  RunPt->id = 0;
  
  //call os suspend to context switch to the next tasks
  OS_Suspend();
}

int OS_AddThread(void(*task)(void),  uint8_t priority, uint8_t id){
  uint8_t i;
	for(i = TCB_COUNT; i > 0; i--){
		if(tcbs[i-1].id == 0){
			tcbs[i-1].id = id; //set id
			tcbs[i-1].stackPt = &tcbs[i-1].Regs[0]; //set stack pointer
			tcbs[i-1].PC = task;
			tcbs[i-1].PSR = 0x01000000;
			if(RunPt == 0){
				tcbs[i-1].next = &tcbs[i-1];
        tcbs[i-1].prev = &tcbs[i-1];
				RunPt = &tcbs[i-1];
			}
			else{
				struct TCB* placeHolder = RunPt->next;
				RunPt->next = (&tcbs[i-1]);
				tcbs[i-1].next = placeHolder;
        placeHolder->prev = &tcbs[i-1];
			}
			return 1;
		}
	}
	//no empty tcb found
	return -1;
}

void OS_Init(void){
  uint8_t i;
  RunPt = 0;
	for(i = TCB_COUNT; i > 0; i--){
    tcbs[i-1].PSR = 0x01000000;
    tcbs[i-1].id = 0;
    tcbs[i-1].PC = (void *) 0x15151515;
    tcbs[i-1].Regs[0] = 0x44444444;
    tcbs[i-1].Regs[1] = 0x55555555;
    tcbs[i-1].Regs[2] = 0x66666666;
    tcbs[i-1].Regs[3] = 0x77777777;
    tcbs[i-1].Regs[4] = 0x88888888;
    tcbs[i-1].Regs[5] = 0x99999999;
    tcbs[i-1].Regs[6] = 0x10101010;
    tcbs[i-1].Regs[7] = 0x11111110;
    tcbs[i-1].Regs[8] = 0x00000000;
    tcbs[i-1].Regs[9] = 0x11111111;
    tcbs[i-1].Regs[10] = 0x22222222;
    tcbs[i-1].Regs[11] = 0x33333333;
    tcbs[i-1].Regs[12] = 0x12121212;
    tcbs[i-1].Regs[13] = 0x13370000;
    tcbs[i-1].stackPt = &tcbs[i-1].Regs[0];
  }
  NVIC_SYS_PRI3_R |= 7<<21;
  DisableInterrupts();
}
