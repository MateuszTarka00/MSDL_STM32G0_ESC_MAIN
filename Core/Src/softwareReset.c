/*
 * softwareReset.c
 *
 *  Created on: 17 lut 2026
 *      Author: mateo
 */

#include "softwareReset.h"
#include "core_cm0plus.h"

void softwareReset(void)
{
    __disable_irq();   // Optional but recommended

    SCB->AIRCR = (0x5FA << SCB_AIRCR_VECTKEY_Pos) |  // Write key
                 SCB_AIRCR_SYSRESETREQ_Msk;          // Request system reset

    while (1) { }  // Wait for reset
}
