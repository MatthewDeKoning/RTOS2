// TIMER4A.c
// Runs on LM4F120/TM4C123
// Use TIMER4A in periodic mode to request interrupts at a particular
// period.
// Daniel Valvano
// Andrew Lynch
// September 11, 2013
// January 26, 2017

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers"
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2015
   Volume 1, Program 9.8

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Volume 2, Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>
#include "Timer4A.h"
#include "../inc/tm4c123gh6pm.h"



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode
void static (*PeriodicTasks[100])(void);   // user function
static int8_t Index = 0;
static int32_t Time = 0;


// ***************** TIMER4A_Init ****************
// Activate TIMER4 interrupts to run user task periodically
// Inputs:  task is a pointer to a user function
//          period in units (1/clockfreq), 32 bits
// Outputs: none
void Timer4A_Init(uint32_t period, uint8_t priority){long sr;
  sr = StartCritical(); 
  SYSCTL_RCGCTIMER_R |= 0x10;   // 0) activate TIMER4
  TIMER4_CTL_R = 0x00000000;    // 1) disable TIMER4A during setup
  TIMER4_CFG_R = TIMER_CFG_32_BIT_TIMER;    // 2) configure for 32-bit mode
  TIMER4_TAMR_R = TIMER_TAMR_TAMR_PERIOD;   // 3) configure for periodic mode, default down-count settings
  TIMER4_TAILR_R = period-1;    // 4) reload value
  TIMER4_TAPR_R = 0;            // 5) bus clock resolution
  TIMER4_ICR_R = TIMER_ICR_TATOCINT;    // 6) clear TIMER4A timeout flag
  TIMER4_IMR_R = TIMER_IMR_TATOIM;    // 7) arm timeout interrupt
  NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF)|(priority<<21); // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN2_R |= 1<<(70-64);           // 9) enable IRQ 66 in NVIC
  TIMER4_CTL_R = TIMER_CTL_TAEN;    // 10) enable TIMER4A
  //Index = 0;
  EndCritical(sr);
}

void Timer4A_AddPeriodicThread(void(*task)(void)){
  PeriodicTasks[Index] = task;
  Index += 1;
}

void Timer4A_ResetPeriodAndPriority(uint32_t period, uint32_t priority){
  Timer4A_Init(period, priority);
}
  
void Timer4A_ResetPriority(uint32_t priority){

}

void Timer4A_Handler(void){
  TIMER4_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER4A timeout
  for(int i = 0; i < Index; i++){
    (*PeriodicTasks[i])();                // execute user task
  }
  Time++;
}

void Timer4A_ClearPeriodicTime(void){
  Time = 0;
}

uint32_t Timer4A_ReadPeriodicTime(void){
  return Time;
}