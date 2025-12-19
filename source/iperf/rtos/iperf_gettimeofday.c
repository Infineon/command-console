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

#ifndef HAVE_GETTIMEOFDAY

#if defined(HAVE_CONFIG_H) && !defined(INCLUDED_CONFIG_H_)
/* NOTE: config.h doesn't have guard includes! */
#define INCLUDED_CONFIG_H_
#include "config.h"
#endif /* defined(HAVE_CONFIG_H) && !defined(INCLUDED_CONFIG_H_) */

#include "headers.h"
#include "gettimeofday.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

int anycloud_gettimeofday(struct timeval* tv, void* timezone)
{
    cy_time_t time_ms;

    (void)timezone; /* Unused parameter */
    (void) cy_rtos_get_time(&time_ms);
    tv->tv_sec =  ( time_ms / 1000 );
    tv->tv_usec = ( time_ms - ( tv->tv_sec * 1000 ) ) * 1000;

    return 0;
}

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* !defined(HAVE_GETTIMEOFDAY) */
