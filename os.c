#include "os.h"
#include "../inc/tm4c123gh6pm.h"
struct TCB{
	//stack
	uint32_t *stackPt;
  struct TCB* next;
	uint32_t MoreStack[83];
	uint32_t Regs[14];
	void (*PC)(void);
	uint32_t PSR;
	//state variables
	uint8_t id;
	uint32_t sleep;
	uint8_t priority;
	uint8_t blocked;
};

typedef struct TCB TCBType;

TCBType* RunPt;
//TCBType* NextPt;
const uint8_t TCB_COUNT = 3;
TCBType tcbs[TCB_COUNT];



void OS_Suspend(void){
  //call scheduler
  NVIC_INT_CTRL_R = 0x10000000; // Trigger PendSV
}

uint8_t OS_Id(void){
  return RunPt->id;
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
				RunPt = &tcbs[i-1];
			}
			else{
				struct TCB* placeHolder = RunPt->next;
				RunPt->next = (&tcbs[i-1]);
				tcbs[i-1].next = placeHolder;
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
  
}
