/*
 * (c) 2025, Infineon Technologies AG, or an affiliate of Infineon
 * Technologies AG. All rights reserved.
 * This software, associated documentation and materials ("Software") is
 * owned by Infineon Technologies AG or one of its affiliates ("Infineon")
 * and is protected by and subject to worldwide patent protection, worldwide
 * copyright laws, and international treaty provisions. Therefore, you may use
 * this Software only as provided in the license agreement accompanying the
 * software package from which you obtained this Software. If no license
 * agreement applies, then any use, reproduction, modification, translation, or
 * compilation of this Software is prohibited without the express written
 * permission of Infineon.
 *
 * Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
 * IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
 * THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
 * SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
 * Infineon reserves the right to make changes to the Software without notice.
 * You are responsible for properly designing, programming, and testing the
 * functionality and safety of your intended application of the Software, as
 * well as complying with any legal requirements related to its use. Infineon
 * does not guarantee that the Software will be free from intrusion, data theft
 * or loss, or other breaches ("Security Breaches"), and Infineon shall have
 * no liability arising out of any Security Breaches. Unless otherwise
 * explicitly approved by Infineon, the Software may not be used in any
 * application where a failure of the Product or any consequences of the use
 * thereof can reasonably be expected to result in personal injury.
 */

/** @file
 *
 */

#include "command_console.h"

#include <stdio.h>
#include <string.h>

#if defined(COMPONENT_MTB_HAL)
#include "mtb_hal_uart.h"
#include "mtb_hal_hw_types.h"
#else
#include "cyhal_uart.h"
#include "cyhal_hw_types.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/******************************************************
 *                 Type Definitions
 ******************************************************/

/******************************************************
 *                    Structures
 ******************************************************/

/******************************************************
 *               Function Declarations
 ******************************************************/

/******************************************************
 *               Variables Definitions
 ******************************************************/

/******************************************************
 *               Function Definitions
 ******************************************************/

bool cy_isreadable( void* serial )
{
#if defined(COMPONENT_MTB_HAL)
    mtb_hal_uart_t *uart = (mtb_hal_uart_t *)serial;
    bool is_readable = mtb_hal_uart_readable(uart) > 0;
#else
    cyhal_uart_t *uart = (cyhal_uart_t *)serial;
    bool is_readable = cyhal_uart_readable(uart) > 0;
#endif
    return is_readable;
}

int cy_read( void* serial )
{
#if defined(COMPONENT_MTB_HAL)
    mtb_hal_uart_t *uart = (mtb_hal_uart_t *)serial;
	uint8_t value;
	mtb_hal_uart_get( uart, &value, 0);
#else
	cyhal_uart_t *uart = (cyhal_uart_t *)serial;
	uint8_t value;
	cyhal_uart_getc( uart, &value, 0);
#endif
    return value;
}

#ifdef __cplusplus
}
#endif
