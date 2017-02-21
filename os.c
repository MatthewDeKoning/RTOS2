#include "os.h"
#include "FIFO.h"
#include "SysTick.h"
#include "../inc/tm4c123gh6pm.h"


#define TRUE  1
#define FALSE 0

#define ROUND_ROBIN_SCH   TRUE
#define PRIORITY_SCH      FALSE
#define POSTOFFICESIZE 32
#define FIFOSIZE   128         // size of the FIFOs (must be power of 2)
#define FIFOSUCCESS 1         // return value on success
#define FIFOFAIL    0         // return value on failure
                              // create index implementation FIFO (see FIFO.h)
#define MAILBOXSIZE 30



static uint8_t OS_FIFO_Index;

static Sema4Type FIFO_Free;
static Sema4Type FIFO_Valid;

AddIndexFifo(OS, FIFOSIZE, unsigned long, FIFOSUCCESS, FIFOFAIL)

AddIndexFifo(POSTOFFICE, POSTOFFICESIZE, Mail, FIFOSUCCESS, FIFOFAIL);

static uint8_t OS_MAIL_Index;

static Sema4Type MailFree;
static Sema4Type MailValid;

void OS_Fifo_Init(){
  OSFifo_Init();
  FIFO_Free.Value = 128;
  FIFO_Valid.Value = 0;
}

int OS_Fifo_Put(unsigned long data){
  if(FIFO_Free.Value > 0){
    OS_Wait(&FIFO_Free);
    DisableInterrupts();
    OSFifo_Put(data);
    EnableInterrupts();
    OS_Signal(&FIFO_Valid);
    
    return FIFOSUCCESS;
  }
  return FIFOFAIL;
}

unsigned long OS_Fifo_Get(void){
  
  unsigned long ret;
  OS_Wait(&FIFO_Valid);
  DisableInterrupts();
  OSFifo_Get(&ret);
  OS_FIFO_Index--;
  EnableInterrupts();
  OS_Signal(&FIFO_Free);
  return ret;
}

unsigned long OS_Fifo_Size(void){
  return FIFO_Free.Value;
}

void OS_MailBox_Init(){
  MailFree.Value = 32;
  MailValid.Value = 0;
  POSTOFFICEFifo_Init();
}

void OS_MailBox_Send(int device, int line, char* message){
  int i;
  Mail NextMail;
  NextMail.device = device;
  NextMail.line = line;
  for(i = 0; i < 20; i++){
      NextMail.message[i] = message[i];
  }
  
  OS_Wait(&MailFree);
  DisableInterrupts();
  POSTOFFICEFifo_Put(NextMail);
  EnableInterrupts();
  OS_Signal(&MailValid);
}

Mail OS_MailBox_Recv(int i){
  Mail ret;
  OS_Wait(&MailValid);
  DisableInterrupts();
  POSTOFFICEFifo_Get(&ret);
  EnableInterrupts();
  OS_Signal(&MailFree);
  return ret;
}

int OS_MailBox_Count(){
  return MailValid.Value;
}
TCBType* RunPt;
TCBType* NextPt;

const uint8_t TCB_COUNT = 10;
TCBType tcbs[TCB_COUNT];
static uint32_t periodicTimes[5];
static uint32_t periods[5];
static void (*PERIODICTASKS[5])(void);
static uint8_t periodicIndex;


void OS_Wait(Sema4Type *semaPt){
  DisableInterrupts();
  while(semaPt->Value <=0){
    EnableInterrupts();
    DisableInterrupts();
    OS_Suspend();
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

void OS_bWait(Sema4Type *semaPt){
  DisableInterrupts();
  while(semaPt->Value != 1){
    EnableInterrupts();
    DisableInterrupts();
    OS_Suspend();
  }
  semaPt->Value = 0;
  EnableInterrupts();
  
}

void OS_bSignal(Sema4Type *semaPt){
  long status;
  status = StartCritical();
  semaPt->Value = 1;
  EndCritical(status);
}

void OS_InitSemaphore(Sema4Type *semaPt, long value){
  semaPt->Value = value;
}

void OS_Scheduler(void){
#if PRIORITY_SCH
  //write priority scheduler here
#endif
#if ROUND_ROBIN_SCH
  NextPt = 0;
  struct TCB* next = RunPt->next;
  while(NextPt == 0){
    if(next->sleep == 0){
      NextPt = next;
      return;
    }
    else{
      if(SYSTICK_getCount(next->id) >= next->sleep)
      {
        next->sleep = 0;
        NextPt = next;
      }
      else{
        next = next->next;
      }
    }
  }
#endif //ROUND_ROBIN_SCH
}


void OS_Suspend(void){
  //call scheduler
  OS_Scheduler();
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
int OS_AddSW1Task(void(*pushTask)(void), void(*pullTask)(void), unsigned long priority){
  Switch_Init(pushTask, pullTask);
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
        tcbs[i-1].prev = RunPt;
        placeHolder->prev = &tcbs[i-1];
			}
			return 1;
		}
	}
	//no empty tcb found
	return -1;
}

void OS_Sleep(unsigned long sleepTime){
  RunPt->sleep = sleepTime;
  SYSTICK_setCount(RunPt->id);
  OS_Suspend();
}
// ***************** TIMER1_Init ****************
// Activate TIMER1 for OS_Time clock cycles
// Inputs:  none
// Outputs: none
void Timer1_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00000000;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = TIMER_TAMR_TACDIR; // 3) configure for count up
  TIMER1_TAPR_R = 0;            // 5) bus clock resolution
  TIMER1_TAR_R = 0;
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

// ***************** OS_Time ****************
// Runtime in clock cycles
// Inputs:  none
// Outputs: runtime
unsigned long OS_Time(void){
  return TIMER1_TAR_R;
}

unsigned long OS_TimeDifference(unsigned long start, unsigned long stop){
  return stop-start;
}

int Timer2Period;

void Timer2_Init(unsigned long period){
  Timer2Period = period;
  SYSCTL_RCGCTIMER_R |= 0x04;   // 0) activate timer2
  TIMER2_CTL_R = 0x00000000;    // 1) disable timer2A during setup
  TIMER2_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER2_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
  TIMER2_TAILR_R = period-1;    // 4) reload value
  TIMER2_TAPR_R = 0;            // 5) bus clock resolution
  TIMER2_ICR_R = 0x00000001;    // 6) clear timer2A timeout flag
  TIMER2_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R&0x00FFFFFF)|0x80000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 39, interrupt number 23
  NVIC_EN0_R = 1<<23;           // 9) enable IRQ 23 in NVIC
  TIMER2_CTL_R = 0x00000001;    // 10) enable timer2A
}

void Timer2A_Handler(void){
  int i; 
  TIMER2_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER2A timeout
  for(i = 0; i < periodicIndex; i++){
    periodicTimes[i] += Timer2Period;
   
    if(periodicTimes[i] >= periods[i]){
      PERIODICTASKS[i]();
      periodicTimes[i] = 0;
    }
  }
}

int OS_AddPeriodicThread(void(*task)(void), unsigned long period, unsigned long priority){
  if(periodicIndex < 5){
    PERIODICTASKS[periodicIndex] = task;
    periods[periodicIndex] = period;
    periodicTimes[periodicIndex] = 0;
    periodicIndex++;  
    return 1;
  }
  return 0;
}

void OS_Init(void){
  uint8_t i;
  RunPt = 0;
  periodicIndex = 0;
  Timer2_Init(10000); //run the periodic functions ever 0.000125 seconds - 8 KHz
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
  Timer1_Init(); //This is OS time
}

void OS_ClearMsTime(void){
  Timer1_Init();
}
unsigned long OS_MsTime(void){
  return TIMER1_TAR_R/80000;
  
}