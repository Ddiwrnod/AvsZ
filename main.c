/*
    ChibiOS - Copyright (C) 2006..2016 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "ch_test.h"


// PWM configuration function.
static PWMConfig pwmcfg = {
  10000,                                    /* 10kHz PWM clock frequency.   */
  10000,                                    /* Initial PWM period 10ms.       */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0,
#if STM32_PWM_USE_ADVANCED
  0
#endif
};


// PWM-SERVOS configuration function.
static PWMConfig pwmcfg2 = {
  1000000,                                    /* 1MHz PWM clock frequency.   */
  20000,                                      /* Initial PWM period 20mS.     */
  NULL,
  {
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_ACTIVE_HIGH, NULL},
   {PWM_OUTPUT_DISABLED, NULL},
   {PWM_OUTPUT_DISABLED, NULL}
  },
  0,
  0,
#if STM32_PWM_USE_ADVANCED
  0
#endif
};


// Blinker test.
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;

  chRegSetThreadName("blinker");
  while (true) {
    palSetPad(GPIOD, 2);
    chThdSleepMilliseconds(250);
    palClearPad(GPIOD, 2);
    chThdSleepMilliseconds(250);
  }
}


// Main init
int main(void) {

  halInit();
  chSysInit();
  boardInit();

  sdStart(&SD3, NULL);     // USART1 --> PA9(TX), PA10(RX) : Interno
  sdStart(&SD2, NULL);     // USART2 --> PA2(TX), PA3(RX) : SEN 3 and SEN 4, Bluetooth.

  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO+1, Thread1, NULL);
  

  uint8_t n;                  // Variable de control PWM.


//  PWM Control
  palSetPad(GPIOA, 6);	      // PWM1 --> PA6: SEN7
  palSetPad(GPIOA, 7);        // PWM2 --> PA7: SEN8
  palSetPad(GPIOA, 8);        // PMM1-Servo --> PA8: SERV_1
  palSetPad(GPIOA, 9);        // PWM2-Servo --> PA9: SERV_2

  pwmStart(&PWMD1, &pwmcfg2);
  pwmStart(&PWMD3, &pwmcfg);

  pwmEnablePeriodicNotification(&PWMD1);
  pwmEnablePeriodicNotification(&PWMD3);

  palSetPadMode(GPIOA, 6, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOA, 7, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOA, 8, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  palSetPadMode(GPIOA, 9, PAL_MODE_STM32_ALTERNATE_PUSHPULL);
  chThdSleepMilliseconds(2);


  while (true) {

    n = sdGet(&SD2);
    sdPut(&SD2, n); 

    chprintf((BaseChannel *)&SD3, "\r\n");
    chprintf((BaseChannel *)&SD3, "Boton presionado: %u\r\n", n);
    chThdSleepMilliseconds(2);

  switch(n) {
   //Adelante
   case 70  :
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(1);
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(1);
      break; 
   //Izquierda
   case 76  :
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   //Atras
   case 66  :
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   //Derecha
   case 82  :
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   //Adelante - Izquierda
   case 71  :
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   //Adelante - Derecha
   case 73  :
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   //Atras - Izquierda
   case 72  :
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   //Atras - Derecha
   case 74  :
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 5000));
        chThdSleepMilliseconds(5);
      break; 
   
   
   //Control cambio de velocidad   
   case 48  :
        pwmChangePeriod(&PWMD3, 10000);
        chThdSleepMilliseconds(5);
      break; 
   case 49  :
        pwmChangePeriod(&PWMD3, 9000);
        chThdSleepMilliseconds(5);
      break;
   case 50  :
        pwmChangePeriod(&PWMD3, 8000);
        chThdSleepMilliseconds(5);
      break;	
   case 51  :
        pwmChangePeriod(&PWMD3, 7000);
        chThdSleepMilliseconds(5);
      break;
   case 52  :
        pwmChangePeriod(&PWMD3, 6000);
        chThdSleepMilliseconds(5);
   case 53  :
        pwmChangePeriod(&PWMD3, 5500);
        chThdSleepMilliseconds(5);
      break;
   case 54  :
        pwmChangePeriod(&PWMD3, 5000);
        chThdSleepMilliseconds(5);
   case 55  :
        pwmChangePeriod(&PWMD3, 4500);
        chThdSleepMilliseconds(5);
      break;
   case 56  :
        pwmChangePeriod(&PWMD3, 4000);
        chThdSleepMilliseconds(5);
   case 57  :
        pwmChangePeriod(&PWMD3, 3500);
        chThdSleepMilliseconds(5);
      break;
   case 113  :
        pwmChangePeriod(&PWMD3, 1000);
        chThdSleepMilliseconds(5);
   //STOP
   case 83  :
        pwmEnableChannel(&PWMD3, 0, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 0));
        pwmEnableChannel(&PWMD3, 1, PWM_PERCENTAGE_TO_WIDTH(&PWMD3, 0));
        chThdSleepMilliseconds(1);

   // Control Servos 
  
    case 87  :
        pwmEnableChannel(&PWMD1, 0, 1000);
        chThdSleepMilliseconds(5);
      break; 

    case 119  :
        pwmEnableChannel(&PWMD1, 0, 2000);
        chThdSleepMilliseconds(5);
      break;

    case 85  :
        pwmEnableChannel(&PWMD1, 1, 1000);
        chThdSleepMilliseconds(5);
      break; 

    case 117 :
        pwmEnableChannel(&PWMD1, 1, 2000);
        chThdSleepMilliseconds(5);
      break; 

    default : 
    chprintf((BaseChannel *)&SD3, "\r\n" );
    chprintf((BaseChannel *)&SD3, "Tecla no valida \r\n");
      break;
   }
 
  }
  return 0;
}






