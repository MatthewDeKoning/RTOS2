;/*****************************************************************************/
;/* OSasm.s: low-level OS commands, written in assembly                       */
;/* derived from uCOS-II                                                      */
;/*****************************************************************************/
;Jonathan Valvano, Fixed Scheduler, 5/3/2015


        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
        EXPORT  OS_Launch
        EXPORT  PendSV_Handler


NVIC_INT_CTRL   EQU     0xE000ED04                              ; Interrupt control state register.
NVIC_SYSPRI14   EQU     0xE000ED22                              ; PendSV priority register (position 14).
NVIC_SYSPRI15   EQU     0xE000ED23                              ; Systick priority register (position 15).
NVIC_LEVEL14    EQU           0xEF                              ; Systick priority value (second lowest).
NVIC_LEVEL15    EQU           0xFF                              ; PendSV priority value (lowest).
NVIC_PENDSVSET  EQU     0x10000000                              ; Value to trigger PendSV exception.
NVIC_ST_RELOAD  EQU     0xE000E014
NVIC_ST_CURRENT EQU     0xE000E018




OS_Launch
    LDR     R0, =RunPt         ; currently running thread
    LDR     R2, [R0]		   ; R2 = value of RunPt
    LDR     SP, [R2]           ; new thread SP; SP = RunPt->stackPointer;
    POP     {R4-R11}           ; restore regs r4-11 
    POP     {R0-R3}            ; restore regs r0-3 
    POP     {R12}
    POP     {LR}               ; discard LR from initial stack
    POP     {LR}               ; start location
    POP     {R1}               ; discard PSR
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

OSStartHang
    B       OSStartHang        ; Should never get here



;********************************************************************************************************

  
    
PendSV_Handler
  CPSID I         ; Disable interrupts
  PUSH {R4-R11}   ; Store registers on stack
  LDR R0, =RunPt  
  LDR R1, [R0]    ; Load current TCB address
  STR SP, [R1]    ; Store SP in TCB
  LDR R1, [R1, #4] ;load R1 with value of RunPt->Next//does not work right!
  LDR R2, [R1]    ; Load next TCB address
  STR R1, [R0]    ; Store next TCB in RunPt
  LDR SP, [R1]    ; Load new SP
  POP {R4-R11}    ; Load new registers
  CPSIE I         ; Re-enable interrupts
  BX LR           ; Resume new task
  
  ALIGN
  END
