/****************************************************************************
 * @file     main.c
 * @version  V2.0
 * $Revision: 8 $
 * $Date: 15/01/16 1:45p $
 * @brief
 *           Implement transmit message in Basic mode.
 *           This sample code needs to work with CAN_BasicMode_Receive.
 * @note
 * @copyright SPDX-License-Identifier: Apache-2.0
 *
 * @copyright Copyright (C) 2014 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NUC1311.h"


#define PLL_CLOCK       48000000

/*---------------------------------------------------------------------------*/
/*  Function Declare                                                         */
/*---------------------------------------------------------------------------*/
extern char GetChar(void);
void CAN_ShowMsg(STR_CANMSG_T* Msg);

/*---------------------------------------------------------------------------------------------------------*/
/* Define global variables and constants                                                                   */
/*---------------------------------------------------------------------------------------------------------*/
/* Declare a CAN message structure */
STR_CANMSG_T rrMsg;

/*---------------------------------------------------------------------------------------------------------*/
/* CAN0 interrupt handler                                                                                  */
/*---------------------------------------------------------------------------------------------------------*/
void CAN0_IRQHandler(void)
{
    uint32_t u8IIDRstatus;

    u8IIDRstatus = CAN0->IIDR;

    if(u8IIDRstatus == 0x00008000)        /* Check Status Interrupt Flag (Error status Int and Status change Int) */
    {
        /**************************/
        /* Status Change interrupt*/
        /**************************/
        if(CAN0->STATUS & CAN_STATUS_RXOK_Msk)
        {
            CAN0->STATUS &= ~CAN_STATUS_RXOK_Msk;   /* Clear RxOK status*/
        }

        if(CAN0->STATUS & CAN_STATUS_TXOK_Msk)
        {
            CAN0->STATUS &= ~CAN_STATUS_TXOK_Msk;    /* Clear TxOK status*/
        }

        /**************************/
        /* Error Status interrupt */
        /**************************/
        if(CAN0->STATUS & CAN_STATUS_BOFF_Msk)
        {
            printf("BOFF INT\n") ;
        }
        else if(CAN0->STATUS & CAN_STATUS_EWARN_Msk)
        {
            printf("EWARN INT\n") ;
        }
        else if((CAN0->ERR & CAN_ERR_TEC_Msk) != 0)
        {
            printf("Transmit error!\n") ;
        }
        else if((CAN0->ERR & CAN_ERR_REC_Msk) != 0)
        {
            printf("Receive error!\n") ;
        }

    }
    else if((u8IIDRstatus >= 0x1) || (u8IIDRstatus <= 0x20))
    {
        CAN_CLR_INT_PENDING_BIT(CAN0, (u8IIDRstatus - 1)); /* Clear Interrupt Pending */
    }
    else if(CAN0->WU_STATUS == 1)
    {
        printf("Wake up\n");

        CAN0->WU_STATUS = 0;    /* Write '0' to clear */
    }

}

/*---------------------------------------------------------------------------------------------------------*/
/* Disable CAN Clock and Reset it                                                                          */
/*---------------------------------------------------------------------------------------------------------*/
void CAN_STOP(void)
{
    /* Disable CAN0 Clock and Reset it */
    SYS_ResetModule(CAN0_RST);
    CLK_DisableModuleClock(CAN0_MODULE);
}

void SYS_Init(void)
{
    /*---------------------------------------------------------------------------------------------------------*/
    /* Init System Clock                                                                                       */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Enable Internal RC 22.1184MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_OSC22M_EN_Msk);

    /* Waiting for Internal RC clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_OSC22M_STB_Msk);

    /* Switch HCLK clock source to Internal RC and HCLK source divide 1 */
    CLK_SetHCLK(CLK_CLKSEL0_HCLK_S_HIRC, CLK_CLKDIV_HCLK(1));

    /* Enable external XTAL 12MHz clock */
    CLK_EnableXtalRC(CLK_PWRCON_XTL12M_EN_Msk);

    /* Waiting for external XTAL clock ready */
    CLK_WaitClockReady(CLK_CLKSTATUS_XTL12M_STB_Msk);

    /* Set core clock as PLL_CLOCK from PLL */
    CLK_SetCoreClock(PLL_CLOCK);

    /* Enable UART module clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Enable CAN module clock */
    CLK_EnableModuleClock(CAN0_MODULE);
    //CLK_EnableModuleClock(CAN1_MODULE);

    /* Select UART module clock source */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART_S_PLL, CLK_CLKDIV_UART(1));

    /*---------------------------------------------------------------------------------------------------------*/
    /* Init I/O Multi-function                                                                                 */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFP &= ~(SYS_GPB_MFP_PB0_Msk | SYS_GPB_MFP_PB1_Msk);
    SYS->GPB_MFP |= SYS_GPB_MFP_PB0_UART0_RXD | SYS_GPB_MFP_PB1_UART0_TXD;

    /* Set PD multi-function pins for CANTX0 and CANRX0 */
    SYS->GPD_MFP &= ~(SYS_GPD_MFP_PD6_Msk | SYS_GPD_MFP_PD7_Msk);
    SYS->GPD_MFP |= SYS_GPD_MFP_PD6_CAN0_RXD | SYS_GPD_MFP_PD7_CAN0_TXD;

}

/*---------------------------------------------------------------------------------------------------------*/
/* Init UART                                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void UART0_Init()
{
    /* Reset IP */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 Baudrate */
    UART_Open(UART0, 115200);
}

/*----------------------------------------------------------------------------*/
/*  Some description about how to create test environment                     */
/*----------------------------------------------------------------------------*/
void Note_Configure()
{
    printf("\n\n");
    printf("+--------------------------------------------------------------------------+\n");
    printf("|  About CAN sample code configure                                         |\n");
    printf("+--------------------------------------------------------------------------+\n");
    printf("|   The sample code provide a simple sample code for you study CAN         |\n");
    printf("|   Before execute it, please check description as below                   |\n");
    printf("|                                                                          |\n");
    printf("|   1.CAN_TX and CAN_RX should be connected to your CAN transceiver        |\n");
    printf("|   2.Using two module board and connect to the same CAN BUS               |\n");
    printf("|   3.Check the terminal resistor of bus is connected                      |\n");
    printf("|   4.Using UART0 as print message port                                    |\n");
    printf("|                                                                          |\n");
    printf("|  |--------|       |-----------|   CANBUS  |-----------|       |--------| |\n");
    printf("|  |        |------>|           |<--------->|           |<------|        | |\n");
    printf("|  |        |CAN_TX |    CAN    |   CAN_H   |   CAN     |CAN_TX |        | |\n");
    printf("|  | NUC1311|       |Transceiver|           |Transceiver|       | NUC1311| |\n");
    printf("|  |        |<------|           |<--------->|           |------>|        | |\n");
    printf("|  |        |CAN_RX |           |   CAN_L   |           |CAN_RX |        | |\n");
    printf("|  |--------|       |-----------|           |-----------|       |--------| |\n");
    printf("|  |                                                            |          |\n");
    printf("|  |                                                            |          |\n");
    printf("|  V                                                            V          |\n");
    printf("| UART0                                                         UART0      |\n");
    printf("|(print message)                                          (print message)  |\n");
    printf("+--------------------------------------------------------------------------+\n");
}

/*----------------------------------------------------------------------------*/
/*  Check the real baud-rate                                                  */
/*----------------------------------------------------------------------------*/
void BaudRateCheck(uint32_t u32BaudRate, uint32_t u32RealBaudRate)
{
    /* Get Core Clock Frequency */
    SystemCoreClockUpdate();

    if(u32BaudRate != u32RealBaudRate)
    {
        printf("Set CAN baud-rate is fail\n");
        printf("Real baud-rate value(bps): %d\n", u32RealBaudRate);
        printf("CAN baud-rate calculation equation as below:\n");
        printf("CAN baud-rate(bps) = Fin/(BPR+1)*(Tseg1+Tseg2+3)\n");
        printf("where: Fin: System clock freq.(Hz)\n");
        printf("       BRP: The baud rate prescale. It is composed of BRP (CAN_BTIME[5:0]) and BRPE (CAN_BRPE[3:0]).\n");
        printf("       Tseg1: Time Segment before the sample point. You can set tseg1 (CAN_BTIME[11:8]).\n");
        printf("       Tseg2: Time Segment after the sample point. You can set tseg2 (CAN_BTIME[14:12]).\n");

        if(SystemCoreClock % u32BaudRate != 0)
            printf("\nThe BPR does not calculate, the Fin must be a multiple of the CAN baud-rate.\n");

        else
            printf("\nThe BPR does not calculate, the (Fin/(CAN baud-rate)) must be a multiple of the (Tseg1+Tseg1+3).\n");
    }
    else
        printf("\nReal baud-rate value(bps): %d\n", u32RealBaudRate);
}

/*----------------------------------------------------------------------------*/
/*  Set the CAN speed                                                         */
/*----------------------------------------------------------------------------*/
void SelectCANSpeed(CAN_T  *tCAN)
{
    uint32_t unItem, BaudRate = 0, RealBaudRate = 0;

    printf("Please select CAN speed you desired\n");
    printf("[0] 1000Kbps\n");
    printf("[1]  800Kbps\n");
    printf("[2]  500Kbps\n");
    printf("[3]  250Kbps\n");
    printf("[4]  125Kbps\n");
    printf("[5]  100Kbps\n");
    printf("[6]   50Kbps\n");

    unItem = GetChar();
    printf("%c\n", unItem);

    switch(unItem)
    {
        case '0':
            BaudRate = 1000000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);//Set target baud-rate and operation mode.
            break;

        case '1':
            BaudRate = 800000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);
            break;

        case '2':
            BaudRate = 500000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);
            break;

        case '3':
            BaudRate = 250000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);
            break;

        case '4':
            BaudRate = 125000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);
            break;

        case '5':
            BaudRate = 100000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);
            break;

        case '6':
            BaudRate = 50000;
            RealBaudRate = CAN_Open(tCAN,  BaudRate, CAN_BASIC_MODE);
            break;
    }

    /* Check the real baud-rate is OK */
    BaudRateCheck(BaudRate, RealBaudRate);

}

/*----------------------------------------------------------------------------*/
/*  Test Menu                                                                 */
/*----------------------------------------------------------------------------*/
void TestItem(void)
{
    printf("\n");
    printf("+------------------------------------------------------------------ +\n");
    printf("|  Nuvoton CAN BUS DRIVER DEMO                                      |\n");
    printf("+-------------------------------------------------------------------+\n");
    printf("|                                                                   |\n");
    printf("|     Transmit a message by basic mode                              |\n");
    printf("|     (At first, another module board should be set to              |\n");
    printf("|     [CAN_BasicMode_Receive] waiting for receiving data)           |\n");
    printf("|                                                                   |\n");
    printf("+-------------------------------------------------------------------+\n");
}

/*----------------------------------------------------------------------------*/
/*  Send Tx Msg by Basic Mode Function (Without Message RAM)                  */
/*----------------------------------------------------------------------------*/
void Test_BasicMode_Tx(CAN_T  *tCAN)
{
    int32_t delaycount;

    /* Declare a CAN message structure */
    STR_CANMSG_T msg1;
    delaycount = 1000;

    /* Enable CAN interrupt */
    CAN_EnableInt(tCAN, CAN_CON_IE_Msk | CAN_CON_SIE_Msk);
    /* Set Interrupt Priority */
    NVIC_SetPriority(CAN0_IRQn, (1 << __NVIC_PRIO_BITS) - 2);
    /* Enable External Interrupt */
    NVIC_EnableIRQ(CAN0_IRQn);

    /* Send Message No.1 */
    msg1.FrameType = CAN_DATA_FRAME;
    msg1.IdType   = CAN_STD_ID;
    msg1.Id       = 0x001;
    msg1.DLC      = 2;
    msg1.Data[0]  = 0x00;
    msg1.Data[1]  = 0x2;
    CAN_Transmit(tCAN, 0, &msg1);//Send CAN message
    printf("Send STD_ID:0x1,Data[0,2]\n");
    CLK_SysTickDelay(delaycount);   /* Generate the Delay Time by Systick */

    /* Send Message No.2 */
    msg1.FrameType = CAN_DATA_FRAME;
    msg1.IdType   = CAN_STD_ID;
    msg1.Id       = 0x1AC;
    msg1.DLC      = 8;
    msg1.Data[0]  = 0x11;
    msg1.Data[1]  = 0x12;
    msg1.Data[2]  = 0x13;
    msg1.Data[3]  = 0x14;
    msg1.Data[4]  = 0x15;
    msg1.Data[5]  = 0x16;
    msg1.Data[6]  = 0x17;
    msg1.Data[7]  = 0x18;
    CAN_Transmit(tCAN, 0, &msg1);//Send CAN message
    printf("Send STD_ID:0x1AC,Data[11,12,13,14,15,16,17,18]\n");
    CLK_SysTickDelay(delaycount);   /* Generate the Delay Time by Systick */

    /* Send Message No.3 */
    msg1.FrameType = CAN_DATA_FRAME;
    msg1.IdType   = CAN_STD_ID;
    msg1.Id       = 0x310;
    msg1.DLC      = 8;
    msg1.Data[0]  = 0x21;
    msg1.Data[1]  = 0x22;
    msg1.Data[2]  = 0x23;
    msg1.Data[3]  = 0x24;
    msg1.Data[4]  = 0x25;
    msg1.Data[5]  = 0x26;
    msg1.Data[6]  = 0x27;
    msg1.Data[7]  = 0x28;
    CAN_Transmit(tCAN, 0, &msg1);//Send CAN message
    printf("Send STD_ID:0x310,Data[21,22,23,24,25,26,27,28]\n");
    CLK_SysTickDelay(delaycount);   /* Generate the Delay Time by Systick */

    /* Send Message No.4 */
    msg1.FrameType = CAN_DATA_FRAME;
    msg1.IdType   = CAN_EXT_ID;
    msg1.Id       = 0x3377;
    msg1.DLC      = 8;
    msg1.Data[0]  = 0x31;
    msg1.Data[1]  = 0x32;
    msg1.Data[2]  = 0x33;
    msg1.Data[3]  = 0x34;
    msg1.Data[4]  = 0x35;
    msg1.Data[5]  = 0x36;
    msg1.Data[6]  = 0x37;
    msg1.Data[7]  = 0x38;
    CAN_Transmit(tCAN, 0, &msg1);//Send CAN message
    printf("Send EXT_ID:0x3377,Data[31,32,33,34,35,36,37,38]\n");
    CLK_SysTickDelay(delaycount);   /* Generate the Delay Time by Systick */

    /* Send Message No.5 */
    msg1.FrameType = CAN_DATA_FRAME;
    msg1.IdType   = CAN_EXT_ID;
    msg1.Id       = 0x7755;
    msg1.DLC      = 8;
    msg1.Data[0]  = 0x41;
    msg1.Data[1]  = 0x42;
    msg1.Data[2]  = 0x43;
    msg1.Data[3]  = 0x44;
    msg1.Data[4]  = 0x45;
    msg1.Data[5]  = 0x46;
    msg1.Data[6]  = 0x47;
    msg1.Data[7]  = 0x48;

    CAN_Transmit(tCAN, 0, &msg1);//Send CAN message
    printf("Send EXT_ID:0x7755,Data[41,42,43,44,45,46,47,48]\n");
    CLK_SysTickDelay(delaycount);   /* Generate the Delay Time by Systick */

    printf("\nCheck the receive host received data\n");

}

/*---------------------------------------------------------------------------------------------------------*/
/* MAIN function                                                                                           */
/*---------------------------------------------------------------------------------------------------------*/
int main(void)
{
    CAN_T *tCAN;
    tCAN = (CAN_T *) CAN0;

    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Init System, IP clock and multi-function I/O */
    SYS_Init();

    /* Lock protected registers */
    SYS_LockReg();

    /* Init UART0 for printf */
    UART0_Init();

    /*---------------------------------------------------------------------------------------------------------*/
    /* SAMPLE CODE                                                                                             */
    /*---------------------------------------------------------------------------------------------------------*/

    /* Some description about how to create test environment */
    Note_Configure();

    /* configuring the Bit Timing */
    SelectCANSpeed(tCAN);

    /* Test Menu */
    TestItem();

    printf("Transmit a message by basic mode\n\n");
    printf("Please confirm receiver is ready.\n");
    printf("Press any key to continue ...\n\n");
    GetChar();

    /* Send Tx Msg by Basic Mode Function (Without Message RAM) */
    Test_BasicMode_Tx(tCAN);

    /* Disable CAN */
    CAN_Close(tCAN);

    /* Disable CAN Clock and Reset it */
    CAN_STOP();

    while(1);

}



