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

/*---------------------------------------------------------------
 * Copyright (c) 1999,2000,2001,2002,2003
 * The Board of Trustees of the University of Illinois
 * All Rights Reserved.
 *---------------------------------------------------------------
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software (Iperf) and associated
 * documentation files (the "Software"), to deal in the Software
 * without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions:
 *
 *
 * Redistributions of source code must retain the above
 * copyright notice, this list of conditions and
 * the following disclaimers.
 *
 *
 * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimers in the documentation and/or other materials
 * provided with the distribution.
 *
 *
 * Neither the names of the University of Illinois, NCSA,
 * nor the names of its contributors may be used to endorse
 * or promote products derived from this Software without
 * specific prior written permission.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * ________________________________________________________________
 * National Laboratory for Applied Network Research
 * National Center for Supercomputing Applications
 * University of Illinois at Urbana-Champaign
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________
 *
 * Mutex.h
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * An abstract class for locking a mutex (mutual exclusion). If
 * threads are not available, this does nothing.
 * ------------------------------------------------------------------- */
#ifndef MUTEX_H
#define MUTEX_H

#include "headers.h"

#if   defined( HAVE_POSIX_THREAD )
    typedef pthread_mutex_t Mutex;
#elif defined( HAVE_WIN32_THREAD )
    typedef HANDLE Mutex;
#elif defined(HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	typedef xMutexHandle Mutex;
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	typedef TX_MUTEX Mutex;
#else
    typedef int Mutex;
#endif

/* ------------------------------------------------------------------- *
class Mutex {
public:*/

    // initialize mutex
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Initialize( MutexPtr ) pthread_mutex_init( MutexPtr, NULL )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Initialize( MutexPtr ) *MutexPtr = CreateMutex( NULL, 0, NULL )
#elif defined(HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	#define Mutex_Initialize( MutexPtr ) \
        *(MutexPtr) = xMutexCreate( )
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	#define Mutex_Initialize( MutexPtr ) \
        tx_mutex_create( MutexPtr, (char*) "mutex", 0)
#else
    #define Mutex_Initialize( MutexPtr )
#endif

    // lock the mutex variable
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Lock( MutexPtr ) pthread_mutex_lock( MutexPtr )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Lock( MutexPtr ) WaitForSingleObject( *MutexPtr, INFINITE )
#elif defined(HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	#define Mutex_Lock( MutexPtr ) \
        xMutexTake( (xMutexHandle) *(MutexPtr), portMAX_DELAY )
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	#define Mutex_Lock( MutexPtr ) \
        tx_mutex_get( MutexPtr, TX_WAIT_FOREVER )
#else
    #define Mutex_Lock( MutexPtr )
#endif

    // unlock the mutex variable
#if   defined( HAVE_POSIX_THREAD )
    #define Mutex_Unlock( MutexPtr ) pthread_mutex_unlock( MutexPtr )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Unlock( MutexPtr ) ReleaseMutex( *MutexPtr )
#elif defined(HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	#define Mutex_Unlock( MutexPtr ) \
        xMutexGive( (xMutexHandle) *(MutexPtr), portMAX_DELAY )
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	#define Mutex_Unlock( MutexPtr ) \
        tx_mutex_put( MutexPtr )
#else
    #define Mutex_Unlock( MutexPtr )
#endif

    // destroy, making sure mutex is unlocked
#if   defined(HAVE_POSIX_THREAD) && !defined(NO_THREADS)
    #define Mutex_Destroy( MutexPtr )  do {         \
        int rc = pthread_mutex_destroy( MutexPtr ); \
        if ( rc == EBUSY ) {                        \
            Mutex_Unlock( MutexPtr );               \
            pthread_mutex_destroy( MutexPtr );      \
        }                                           \
    } while ( 0 )
#elif defined( HAVE_WIN32_THREAD )
    #define Mutex_Destroy( MutexPtr ) CloseHandle( *MutexPtr )
#elif defined(HAVE_FREERTOS_THREAD) && !defined(NO_THREADS)
	#define Mutex_Destroy( MutexPtr )  				    \
        do {           								    \
            int rc = xDoIOwnTheMutex( MutexPtr );       \
            if ( rc ) {                                 \
                Mutex_Unlock( MutexPtr );               \
                xMutexDestroy( MutexPtr );              \
            }                                           \
        } while ( 0 )
#elif defined(HAVE_THREADX_THREAD) && !defined(NO_THREADS)
	#define Mutex_Destroy( MutexPtr )                   \
        tx_mutex_delete( MutexPtr )
#else
    #define Mutex_Destroy( MutexPtr )
#endif

#endif // MUTEX_H
