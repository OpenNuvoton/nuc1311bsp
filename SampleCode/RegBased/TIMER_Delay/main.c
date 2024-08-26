/**************************************************************************//**
 * @file     main.c
 * @version  V2.00
 * $Revision: 5 $
 * $Date: 15/01/16 11:44a $
 * @brief    Show how to use timer0 to create various delay time.
 * @note
 * @copyright SPDX-License-Identifier: Apache-2.0
 * @copyright Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC1311.h"

#define PLLCON_SETTING      CLK_PLLCON_50MHz_HXT
#define PLL_CLOCK           50000000


/**
  * @brief      Create a specify Delay Time
  *
  * @param[in]  timer       The pointer of the specified Timer module. It could be TIMER0, TIMER1, TIMER2, TIMER3.
  * @param[in]  u32TimerClk The clock frequency of specified Timer module.
  * @param[in]  u32Usec     Delay period in micro seconds. Valid values are between 100~1000000 (100 micro second ~ 1 second).
  *
  * @return     None
  *
  * @details    This API is used to create a delay loop for u32usec micro seconds by using timer one-shot mode.
  * @note       This API overwrites the register setting of the timer used to count the delay time.
  * @note       This API use polling mode. So there is no need to enable interrupt for the timer module used to generate delay.
  */
void SetTimerDelay(TIMER_T *timer, uint32_t u32TimerClk, uint32_t u32Usec)
{
    uint32_t u32Prescale = 0UL, u32Delay = (SystemCoreClock / u32TimerClk) + 1UL;
    uint32_t u32Cmpr, u32NsecPerTick;

    /* Clear current timer configuration */
    timer->TCSR = 0UL;
    timer->TEXCON = 0UL;

    if(u32TimerClk <= 1000000UL)   /* min delay is 1000 us if timer clock source is <= 1 MHz */
    {
        if(u32Usec < 1000UL)
        {
            u32Usec = 1000UL;
        }
        if(u32Usec > 1000000UL)
        {
            u32Usec = 1000000UL;
        }
    }
    else
    {
        if(u32Usec < 100UL)
        {
            u32Usec = 100UL;
        }
        if(u32Usec > 1000000UL)
        {
            u32Usec = 1000000UL;
        }
    }

    if(u32TimerClk <= 1000000UL)
    {
        u32Prescale = 0UL;
        u32NsecPerTick = 1000000000UL / u32TimerClk;
        u32Cmpr = (u32Usec * 1000UL) / u32NsecPerTick;
    }
    else
    {
        u32Cmpr = u32Usec * (u32TimerClk / 1000000UL);
        u32Prescale = (u32Cmpr >> 24);  /* for 24 bits CMPDAT */
        if (u32Prescale > 0UL)
            u32Cmpr = u32Cmpr / (u32Prescale + 1UL);
    }

    timer->TCMPR = u32Cmpr;
    timer->TCSR = TIMER_TCSR_CEN_Msk | TIMER_ONESHOT_MODE | u32Prescale;

    /*
        When system clock is faster than timer clock, it is possible timer active bit cannot set in time while we check it.
        And the while loop below return immediately, so put a tiny delay here allowing timer start counting and raise active flag.
    */
    for(; u32Delay > 0UL; u32Delay--)
    {
        __NOP();
    }

    while(timer->TCSR & TIMER_TCSR_CACT_Msk);
}

void SYS_Init(void)
{
	uint32_t u32TimeOutCnt;

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Enable IRC22M clock */
    CLK->PWRCON |= CLK_PWRCON_IRC22M_EN_Msk;

    /* Waiting for IRC22M clock ready */
    u32TimeOutCnt = __HIRC;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_IRC22M_STB_Msk))
		if(--u32TimeOutCnt == 0) break;

    /* Switch HCLK clock source to HIRC */
    CLK->CLKSEL0 = CLK_CLKSEL0_HCLK_S_HIRC;

    /* Set PLL to Power-down mode and PLL_STB bit in CLKSTATUS register will be cleared by hardware.*/
    CLK->PLLCON |= CLK_PLLCON_PD_Msk;

    /* Enable external 12 MHz XTAL */
    CLK->PWRCON |= CLK_PWRCON_XTL12M_EN_Msk;

    /* Enable PLL and Set PLL frequency */
    CLK->PLLCON = PLLCON_SETTING;

    /* Waiting for clock ready */
    u32TimeOutCnt = __HIRC;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_PLL_STB_Msk))
		if(--u32TimeOutCnt == 0) break;
    u32TimeOutCnt = __HIRC;
    while(!(CLK->CLKSTATUS & CLK_CLKSTATUS_XTL12M_STB_Msk))
		if(--u32TimeOutCnt == 0) break;

    /* Switch HCLK clock source to PLL, STCLK to HCLK/2 */
    CLK->CLKSEL0 = CLK_CLKSEL0_STCLK_S_HCLK_DIV2 | CLK_CLKSEL0_HCLK_S_PLL;

    /* Enable peripheral clock */
    CLK->APBCLK = CLK_APBCLK_UART0_EN_Msk | CLK_APBCLK_TMR0_EN_Msk | CLK_APBCLK_TMR1_EN_Msk;

    /* Peripheral clock source */
    CLK->CLKSEL1 = CLK_CLKSEL1_UART_S_PLL | CLK_CLKSEL1_TMR0_S_HCLK | CLK_CLKSEL1_TMR1_S_HXT;

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CyclesPerUs automatically. */
    SystemCoreClockUpdate();

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Set PB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= (SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD);
}

void UART0_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init UART                                                                                               */
    /*---------------------------------------------------------------------------------------------------------*/
    /* Reset UART */
    SYS->IPRSTC2 |=  SYS_IPRSTC2_UART0_RST_Msk;
    SYS->IPRSTC2 &= ~SYS_IPRSTC2_UART0_RST_Msk;

    /* Configure UART0 and set UART0 Baudrate */
    UART0->BAUD = UART_BAUD_MODE2 | UART_BAUD_MODE2_DIVIDER(PllClock, 115200);
    UART0->LCR = UART_WORD_LEN_8 | UART_PARITY_NONE | UART_STOP_BIT_1;
}

/*---------------------------------------------------------------------------------------------------------*/
/*  MAIN function                                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    volatile uint32_t u32DelayTime;
    uint32_t u32TimerClkFreq;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, peripheral clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    printf("CPU @ %d Hz\n", SystemCoreClock);
    printf("+-----------------------------------+\n");
    printf("|    Timer Delay API Sample Code    |\n");
    printf("+-----------------------------------+\n\n");

    printf("# This sample code is using Timer1 to check Timer0 TIMER_Delay API delay time is reasonable or not.\n");
    printf("# Delay time includes 100 ms, 200 ms, 300 ms, 400 ms and 500 ms.\n\n");

    /* The timer clock frequency for timer delay */
    u32TimerClkFreq = SystemCoreClock;

    /* Start Timer1 to measure delay period of TIMER_Delay API is reasonable or not */
    TIMER1->TCSR = TIMER_PERIODIC_MODE | (12 - 1);
    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    TIMER_Start(TIMER1);

    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    SetTimerDelay(TIMER0, u32TimerClkFreq, 100000);
    u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
    printf("    Check DelayTime-1 is %d ms .... ", u32DelayTime);
    if(u32DelayTime == 100)
        printf("PASS.\n");
    else
        printf("FAIL.\n");

    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    SetTimerDelay(TIMER0, u32TimerClkFreq, 200000);
    u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
    printf("    Check DelayTime-2 is %d ms .... ", u32DelayTime);
    if(u32DelayTime == 200)
        printf("PASS.\n");
    else
        printf("FAIL.\n");

    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    SetTimerDelay(TIMER0, u32TimerClkFreq, 300000);
    u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
    printf("    Check DelayTime-3 is %d ms .... ", u32DelayTime);
    if(u32DelayTime == 300)
        printf("PASS.\n");
    else
        printf("FAIL.\n");

    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    SetTimerDelay(TIMER0, u32TimerClkFreq, 400000);
    u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
    printf("    Check DelayTime-4 is %d ms .... ", u32DelayTime);
    if(u32DelayTime == 400)
        printf("PASS.\n");
    else
        printf("FAIL.\n");

    TIMER_SET_CMP_VALUE(TIMER1, 0xFFFFFF);
    SetTimerDelay(TIMER0, u32TimerClkFreq, 500000);
    u32DelayTime = TIMER_GetCounter(TIMER1) / 1000;
    printf("    Check DelayTime-5 is %d ms .... ", u32DelayTime);
    if(u32DelayTime == 500)
        printf("PASS.\n");
    else
        printf("FAIL.\n");

    printf("\n*** Check TIMER_Delay API delay time done ***\n");

    while(1);
}

/*** (C) COPYRIGHT 2013 Nuvoton Technology Corp. ***/
