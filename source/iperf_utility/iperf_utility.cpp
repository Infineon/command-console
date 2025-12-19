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
#ifndef DISABLE_COMMAND_CONSOLE_IPERF
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "command_console.h"
#include "iperf_utility.h"
#include "iperf_sockets.h"
#include "cyabs_rtos.h"
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
extern int iperf( int argc, char *argv[], tlv_buffer_t** data );
int iperf_test(int argc, char *argv[], tlv_buffer_t** data);
/******************************************************
 *               Variables Definitions
 ******************************************************/

 static uint32_t wait_bits = 0x1;

#define IPERF_COMMANDS \
    { "iperf", iperf_test, 0, NULL, NULL, (char *)"", (char *)"Run iperf --help for usage."}, \

#define IPERF_THREAD_STACK_SIZE 4096

#if defined(__ICCARM__)
#define IPERF_WEAK_FUNC            __WEAK
#elif defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM)
#define IPERF_WEAK_FUNC            __attribute__((weak))
#else
#define IPERF_WEAK_FUNC           __attribute__((weak))
#endif

const cy_command_console_cmd_t iperf_command_table[] =
{
    IPERF_COMMANDS
    CMD_TABLE_END
};

#pragma pack(1)
struct cmd_result
{
    char status;
    char flag;
    int  count;
};

struct iperf_args
{
    int argc;
    char **argv;
};

typedef struct
{
    bool    available;
    struct  iperf_args args;
    cy_thread_t  *thread;
} iperf_thread_t;

#ifdef COMPONENT_55900
/* MAX_SIMULTANEOUS_COMMANDS is 1 for 55900 devices. Hence a single thread is sufficient */
cy_thread_t iperf_thread;
#endif

static cy_event_t event;
iperf_thread_t iperf_util_threads[MAX_SIMULTANEOUS_COMMANDS];

/******************************************************
 *               Function Definitions
 ******************************************************/
IPERF_WEAK_FUNC void iperf_utility_init( void *networkInterface)
{
    int i;
    iperf_network_init( networkInterface );
    cy_command_console_add_table( iperf_command_table );

    for( i = 0; i < MAX_SIMULTANEOUS_COMMANDS; i++)
    {
        iperf_util_threads[i].available = true;
    }

    cy_rtos_init_event(&event);
}
int iperf_util_find_free_thread()
{
    int j = -1;

    for(int i = 0; i < MAX_SIMULTANEOUS_COMMANDS; i++)
    {
        if(iperf_util_threads[i].available == true)
        {
            if(iperf_util_threads[i].thread != NULL)
            {
                cy_rtos_join_thread(iperf_util_threads[i].thread);
#ifndef COMPONENT_55900
                delete iperf_util_threads[i].thread;
#endif
                iperf_util_threads[i].thread = NULL;
            }
            iperf_util_threads[i].available = false;
            return i;
        }
    }
    return j;
}

void iperf_util_thread(cy_thread_arg_t arg)
{
    int index;

    index = *((int *)arg);
    cy_rtos_setbits_event(&event, wait_bits, false);
    iperf ( iperf_util_threads[index].args.argc, iperf_util_threads[index].args.argv, NULL);
    iperf_util_threads[index].available = true;
    printf("IPERF[%d] EXITED\n", index);
    cy_rtos_exit_thread();
}

int iperf_test(int argc, char *argv[], tlv_buffer_t** data)
{
    int result = 0;
    int index;
    cy_thread_arg_t args;
    cy_rslt_t res;
    int i = 1;
    bool is_server = false;

    index = iperf_util_find_free_thread();
    if( index == -1 )
    {
        printf("Error: exceeded maximum number of simultaneous commands supported\n");
        return -1;
    }

    iperf_util_threads[index].args.argc = argc;
    iperf_util_threads[index].args.argv = argv;
#ifdef COMPONENT_55900
    iperf_util_threads[index].thread = &iperf_thread;
#else
    iperf_util_threads[index].thread = new cy_thread_t;
#endif

    args = (cy_thread_arg_t)(&index);
    while (i <= (argc - 1))
    {
        if (strcasecmp (argv[i], "-s") == 0)
        {
            is_server = true;
        }
        i = i+1 ;
    }

    if ( is_server ==  true)
    {
        res = cy_rtos_create_thread(iperf_util_threads[index].thread, iperf_util_thread, NULL, NULL, IPERF_THREAD_STACK_SIZE, (cy_thread_priority_t)(CY_RTOS_PRIORITY_HIGH), args);
    }
    else
    {
        res = cy_rtos_create_thread(iperf_util_threads[index].thread, iperf_util_thread, NULL, NULL, IPERF_THREAD_STACK_SIZE, (cy_thread_priority_t)(CY_RTOS_PRIORITY_HIGH - 1), args);
    }

    if(res != CY_RSLT_SUCCESS)
    {
        printf("Error creating thread \n");
        return -1;
    }

    cy_rtos_waitbits_event(&event, &wait_bits, true, true, CY_RTOS_NEVER_TIMEOUT);

    return result;
}

#ifdef __cplusplus
}
#endif
#endif