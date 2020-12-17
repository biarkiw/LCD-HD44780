/*
*  This file is for writing and testing the functions that operate the
*  LCD prior to being added to the header file for use in future projects
*/

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "cmsis/cmsis_device.h"

// ----------------------------------------------------------------------------

/*general opperation bit definition for current project*/
#define LCD_CLEAR     ((uint16_t)0x0100)    //clear display
#define LCD_RHOME     ((uint16_t)0x0200)    //return home
#define LCD_EMS       ((uint16_t)0x0600)    //entry mode set
#define LCD_PCONT     ((uint16_t)0x0C00)    //display ON/OFF control
#define LCD_CURSOR    ((uint16_t)0x1000)    //cursor/display shift
#define LCD_FUNCTSET  ((uint16_t)0x3800)    //function set (0X3C00) to change font
#define LCD_BUSY      ((uint16_t)0x8000)    //busy flag from lcd
#define LCD_READBF    ((uint16_t)0x0040)    //set to read busy flag
#define LCD_LINE1     ((uint8_t)0x00)       //DDRAM address for line 1
#define LCD_LINE2     ((uint8_t)0x40)       //DDRAM address for line 2
#define LCD_SETDRAM   ((uint16_t)0x8000)    //set DDRAM address
#define LCD_WRITE     ((uint16_t)0x0020)    //write do lcd, must be or'd with data
#define LCD_ENABLE    ((uint16_t)0x0080)    //write do lcd, must be or'd with data

/* port defines */
#define IOOUT     ((uint32_t)0x55555555)    //GPIO MODDER register bits for output
#define IOIN      ((uint32_t)0x00005555)    //GPIO MODDER register bits for input

/* Clock prescaler for TIM2 timer: no prescaling */
#define myTIM2_PRESCALER  ((uint16_t)0x0000)
/* Maximum possible setting for overflow */
#define myTIM2_PERIOD     ((uint32_t)0xFFFFFFFF)

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"

/* funtion pre-initializers */
void myClock_Init(void);
void myTIM2_Init(void);
void myGPIOB_Init(void);
void TIM2_IRQHandler(void);

// ----- main() ---------------------------------------------------------------
int main(int argc, char* argv[]){
  // At this stage the system clock should have already been configured
  // at high speed.

  // Infinite loop
  while (1){
       // Add your code here.
    }
}

/* I/O and Timer initializations */
void myClock_Init(){

  //enable clock for ports A,B & C
  RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN;

  //enable clock for TIM2
  RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

}

void myTIM2_Init(){

	/* Configure TIM2: buffer auto-reload, count down, stop on underflow,
	 * enable update events, interrupt on underflow only */
	TIM2->CR1 = ((uint16_t)0x009C);

	/* Set clock prescaler value */
	TIM2->PSC = myTIM2_PRESCALER;

	/* Set auto-reloaded delay */
	TIM2->ARR = myTIM2_PERIOD;

	/* Update timer registers */
	TIM2->EGR = 0x00;

	/* Assign TIM2 interrupt priority = 0 in NVIC */
	NVIC_SetPriority(TIM2_IRQn, 0);

	/* Enable TIM2 interrupts in NVIC */
	NVIC_EnableIRQ(TIM2_IRQn);

	/* Enable update interrupt generation */
	TIM2->DIER = 0x1;
}

void myGPIOB_Init(){

	// Configure PB15-PB8 in alternet function mode and PB7-PB0 as output
	GPIOB->MODER = IOOUT;
	//ensure no pull up, no pull down
	GPIOB->PUPDR = 0x0;

}

/* IRQ Handlers */
void TIM2_IRQHandler(){
	/* Check if update interrupt flag is indeed set */
	if ((TIM2->SR & TIM_SR_UIF) != 0){
		trace_printf("\n*** Overflow! ***\n");

		/* Clear update interrupt flag */
		// Relevant register: TIM2->SR
		TIM2->SR &= ~(TIM_SR_UIF);

		/* Restart stopped timer */
		// Relevant register: TIM2->CR1
		TIM2->CR1 |= TIM_CR1_CEN;

	}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
