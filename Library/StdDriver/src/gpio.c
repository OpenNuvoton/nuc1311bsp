/**************************************************************************//**
 * @file     gpio.c
 * @version  V3.00
 * @brief    NUC1311 series GPIO driver source file
 *
 * @note
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (C) 2019 Nuvoton Technology Corp. All rights reserved.
*****************************************************************************/

#include "NUC1311.h"

/** @addtogroup Device_Driver NUC1311 Device Driver
  @{
*/

/** @addtogroup GPIO_Driver GPIO Driver
  @{
*/

/** @addtogroup GPIO_EXPORTED_FUNCTIONS GPIO Exported Functions
  @{
*/

/**
 * @brief       Set GPIO operation mode
 *
 * @param[in]   port        GPIO port. It could be PA, PB, PC, PD or PF.
 * @param[in]   u32PinMask  The single or multiple pins of specified GPIO port. \n
 *                          It could be BIT0 ~ BIT15 for PA GPIO port. \n
 *                          It could be BIT0 ~ BIT5, BIT8, BIT12 and BIT15 for PB GPIO port. \n 
 *                          It could be BIT0 ~ BIT3, BIT6 and BIT7 for PC GPIO port. \n
 *                          It could be BIT6, BIT7, BIT14 and BIT15 for PD GPIO port. \n
 *                          It could be BIT0, BIT1 and BIT4 ~ BIT8 for PF GPIO port.
 * @param[in]   u32Mode     Operation mode. It could be \n
 *                          GPIO_PMD_INPUT, GPIO_PMD_OUTPUT, GPIO_PMD_OPEN_DRAIN, GPIO_PMD_QUASI.
 *
 * @return      None
 *
 * @details     This function is used to set specified GPIO operation mode.
 */
void GPIO_SetMode(GPIO_T *port, uint32_t u32PinMask, uint32_t u32Mode)
{
    uint32_t i;

    for(i = 0; i < GPIO_PIN_MAX; i++)
    {
        if(u32PinMask & (1 << i))
        {
            port->PMD = (port->PMD & ~(0x3 << (i << 1))) | (u32Mode << (i << 1));
        }
    }
}

/**
 * @brief       Enable GPIO interrupt
 *
 * @param[in]   port            GPIO port. It could be PA, PB, PC, PD or PF.
 * @param[in]   u32Pin          The pin of specified GPIO port. \n
 *                              It could be 0 ~ 15 for PA GPIO port. \n
 *                              It could be 0 ~ 5, 8, 12 and 15 for PB GPIO port. \n
 *                              It could be 0 ~ 3, 6 and 7 for PC GPIO port. \n
 *                              It could be 6, 7, 14 and 15 for PD GPIO port. \n
 *                              It could be 0, 1 and 4 ~ 8 for PF GPIO port. \n
 * @param[in]   u32IntAttribs   The interrupt attribute of specified GPIO pin. It could be \n
 *                              GPIO_INT_RISING, GPIO_INT_FALLING, GPIO_INT_BOTH_EDGE, GPIO_INT_HIGH, GPIO_INT_LOW.
 *
 * @return      None
 *
 * @details     This function is used to enable specified GPIO pin interrupt.
 */
void GPIO_EnableInt(GPIO_T *port, uint32_t u32Pin, uint32_t u32IntAttribs)
{
    port->IMD |= (((u32IntAttribs >> 24) & 0xFFUL) << u32Pin);
    port->IEN |= ((u32IntAttribs & 0xFFFFFFUL) << u32Pin);
}


/**
 * @brief       Disable GPIO interrupt
 *
 * @param[in]   port      GPIO port. It could be PA, PB, PC, PD or PF.
 * @param[in]   u32Pin    The pin of specified GPIO port. \n
 *                        It could be 0 ~ 15 for PA GPIO port. \n
 *                        It could be 0 ~ 5, 8, 12 and 15 for PB GPIO port. \n
 *                        It could be 0 ~ 3, 6 and 7 for PC GPIO port. \n
 *                        It could be 6, 7, 14 and 15 for PD GPIO port. \n
 *                        It could be 0, 1 and 4 ~ 8 for PF GPIO port. \n
 *
 * @return      None
 *
 * @details     This function is used to enable specified GPIO pin interrupt.
 */
void GPIO_DisableInt(GPIO_T *port, uint32_t u32Pin)
{
    port->IMD &= ~(1UL << u32Pin);
    port->IEN &= ~((0x00010001UL) << u32Pin);
}


/*@}*/ /* end of group GPIO_EXPORTED_FUNCTIONS */

/*@}*/ /* end of group GPIO_Driver */

/*@}*/ /* end of group Device_Driver */


