/**************************************************************************//**
 * @file     startup_NUC1311.S
 * @version  V3.00
 * @brief    NUC1311 Series Startup Source File
 *
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2024 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/


    .section .bss.STACK, "aw", %nobits
    .align 3

    .global __initial_sp
#ifndef Stack_Size
// <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
    .equ    Stack_Size, 0x00000400
#endif
Stack_Mem:
    .space   Stack_Size
__initial_sp:


    .section .bss.HEAP, "aw", %nobits
    .align  3
    .global Heap_Mem
    .global __heap_base
    .global __heap_limit
#ifndef Heap_Size
// <o>  Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
    .equ    Heap_Size, 0x00000000
#endif
__heap_base:
Heap_Mem:
    .space  Heap_Size
__heap_limit:

    .eabi_attribute Tag_ABI_align_preserved, 1
    .thumb

//*** <<< end of configuration section >>>    ***


// ; Vector Table Mapped to Address 0 at Reset
    .section RESET, "ax"
    .global     __Vectors
    .global     __Vectors_End
    .global     __Vectors_Size

__Vectors:
    .word     __initial_sp              //; Top of Stack
    .word     Reset_Handler             //; Reset Handler
    .word     NMI_Handler               //; NMI Handler
    .word     HardFault_Handler         //; Hard Fault Handler
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     SVC_Handler               //; SVCall Handler
    .word     0                         //; Reserved
    .word     0                         //; Reserved
    .word     PendSV_Handler            //; PendSV Handler
    .word     SysTick_Handler           //; SysTick Handler

    //; External Interrupts
    .word     BOD_IRQHandler  
    .word     WDT_IRQHandler  
    .word     EINT0_IRQHandler
    .word     EINT1_IRQHandler
    .word     GPAB_IRQHandler 
    .word     GPCDEF_IRQHandler
    .word     Default_Handler 
    .word     Default_Handler 
    .word     TMR0_IRQHandler 
    .word     TMR1_IRQHandler 
    .word     TMR2_IRQHandler 
    .word     TMR3_IRQHandler 
    .word     UART02_IRQHandler
    .word     UART1_IRQHandler
    .word     SPI0_IRQHandler 
    .word     UART3_IRQHandler 
    .word     UART4_IRQHandler 
    .word     UART5_IRQHandler 
    .word     I2C0_IRQHandler 
    .word     I2C1_IRQHandler 
    .word     CAN0_IRQHandler 
    .word     CAN1_IRQHandler
    .word     PWM0_IRQHandler
    .word     PWM1_IRQHandler
    .word     Default_Handler
    .word     Default_Handler
    .word     BRAKE0_IRQHandler
    .word     BRAKE1_IRQHandler
    .word     PWRWU_IRQHandler
    .word     ADC_IRQHandler
    .word     CKD_IRQHandler  
    .word     RTC_IRQHandler  
                
__Vectors_End:
    .equ    __Vectors_Size, __Vectors_End - __Vectors

    .section .text, "ax"

// ; Reset Handler
    .global Reset_Handler
    .global  SystemInit
    .global  __main
    .type   Reset_Handler, "function"
Reset_Handler:
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__main
        BX      R0


// ; Dummy Exception Handlers (infinite loops which can be modified)
    .weak   NMI_Handler
    .type   NMI_Handler, "function"
NMI_Handler:
        B       .

    .weak   HardFault_Handler
    .type   HardFault_Handler, "function"

HardFault_Handler:
        MOV     R0, LR
        MRS     R1, MSP
        MRS     R2, PSP
        LDR     R3, =ProcessHardFault
        BLX     R3
        BX      R0

    .weak   SVC_Handler, "function"
SVC_Handler:
        B       .

    .weak   PendSV_Handler, "function"
PendSV_Handler:
        B       .

    .weak   SysTick_Handler, "function"
SysTick_Handler:
        B       .               


    .weak  BOD_IRQHandler,"function"
    .weak  WDT_IRQHandler,"function"
    .weak  EINT0_IRQHandler,"function"
    .weak  EINT1_IRQHandler,"function"
    .weak  GPAB_IRQHandler,"function"
    .weak  GPCDEF_IRQHandler,"function"
    .weak  TMR0_IRQHandler,"function"
    .weak  TMR1_IRQHandler,"function"
    .weak  TMR2_IRQHandler,"function"
    .weak  TMR3_IRQHandler,"function"
    .weak  UART02_IRQHandler,"function"
    .weak  UART1_IRQHandler,"function"
    .weak  SPI0_IRQHandler,"function"
    .weak  UART3_IRQHandler,"function"
    .weak  UART4_IRQHandler,"function"
    .weak  UART5_IRQHandler,"function"
    .weak  I2C0_IRQHandler,"function"
    .weak  I2C1_IRQHandler,"function"
    .weak  CAN0_IRQHandler,"function"
    .weak  CAN1_IRQHandler,"function" 
    .weak  PWM0_IRQHandler,"function"                               
    .weak  PWM1_IRQHandler,"function"
    .weak  BRAKE0_IRQHandler,"function"
    .weak  BRAKE1_IRQHandler,"function"
    .weak  PWRWU_IRQHandler,"function"
    .weak  ADC_IRQHandler,"function"
    .weak  CKD_IRQHandler,"function"				
    .weak  RTC_IRQHandler,"function"
               
Default_Handler:			   
BOD_IRQHandler:
WDT_IRQHandler:
EINT0_IRQHandler:
EINT1_IRQHandler:
GPAB_IRQHandler:
GPCDEF_IRQHandler:
TMR0_IRQHandler:
TMR1_IRQHandler:
TMR2_IRQHandler:
TMR3_IRQHandler:
UART02_IRQHandler:
UART1_IRQHandler:
SPI0_IRQHandler:
UART3_IRQHandler:
UART4_IRQHandler:
UART5_IRQHandler:
I2C0_IRQHandler:
I2C1_IRQHandler:
CAN0_IRQHandler:
CAN1_IRQHandler:
PWM0_IRQHandler:
PWM1_IRQHandler:
BRAKE0_IRQHandler:
BRAKE1_IRQHandler:
PWRWU_IRQHandler:
ADC_IRQHandler:
CKD_IRQHandler:
RTC_IRQHandler:
DEFAULT_IRQHandler:
        B       .

    .align 2


// ;int32_t SH_DoCommand(int32_t n32In_R0, int32_t n32In_R1, int32_t *pn32Out_R0)
    .global     SH_DoCommand
    .global     SH_Return
    .type       SH_DoCommand, "function"
SH_DoCommand:
                BKPT   0xAB                // ; Wait ICE or HardFault
                LDR    R3, =SH_Return
                MOV    R4, lr
                BLX    R3                  // ; Call SH_Return. The return value is in R0
                BX     R4                  // ; Return value = R0

    .align 2

    .global     __PC
    .type       __PC, "function"
__PC:
                MOV     r0, lr
                BLX     lr