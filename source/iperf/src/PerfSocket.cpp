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
 * PerfSocket.cpp
 * by Mark Gates <mgates@nlanr.net>
 *    Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * Has routines the Client and Server classes use in common for
 * performance testing the network.
 * Changes in version 1.2.0
 *     for extracting data from files
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <stdio.h>
 *   <string.h>
 *
 *   <sys/types.h>
 *   <sys/socket.h>
 *   <unistd.h>
 *
 *   <arpa/inet.h>
 *   <netdb.h>
 *   <netinet/in.h>
 *   <sys/socket.h>
 * ------------------------------------------------------------------- */


#define HEADERS()

#include "headers.h"

#include "PerfSocket.hpp"
#include "SocketAddr.h"
/* IPERF_MODIFIED Start */
/* As build system looks for all the header files with the name and adding include paths. It includes mutex.h present in some other component
 * Hence files are renamed by appending iperf.
 */
#include "iperf_util.h"
/* IPERF_MODIFIED End */

/* -------------------------------------------------------------------
 * Set socket options before the listen() or connect() calls.
 * These are optional performance tuning factors.
 * ------------------------------------------------------------------- */

void SetSocketOptions( thread_Settings *inSettings ) {
    // set the TCP window size (socket buffer sizes)
    // also the UDP buffer size
    // must occur before call to accept() for large window sizes
    setsock_tcp_windowsize( inSettings->mSock, inSettings->mTCPWin,
                            (inSettings->mThreadMode == kMode_Client ? 1 : 0) );

    if ( isCongestionControl( inSettings ) ) {
#ifdef TCP_CONGESTION
    Socklen_t len = strlen( inSettings->mCongestion ) + 1;
    /* IPERF_MODIFIED Start */
    int rc = iperf_setsockopt( inSettings->mSock, IPPROTO_TCP, TCP_CONGESTION,
                               inSettings->mCongestion, len);
    /* IPERF_MODIFIED End */
    if (rc == SOCKET_ERROR ) {
        fprintf(stderr, "Attempt to set '%s' congestion control failed: %s\n",
            inSettings->mCongestion, strerror(errno));
        exit(1);
    }
#else
    fprintf( stderr, "The -Z option is not available on this operating system\n");
#endif
    }

    // check if we're sending multicast
    if (isMulticast(inSettings)) {
#ifdef HAVE_MULTICAST
    if (!isUDP(inSettings)) {
        FAIL(1, "Multicast requires -u option ", inSettings);
        exit(1);
    }
    // check for default TTL, multicast is 1 and unicast is the system default
    if (inSettings->mTTL == -1) {
        inSettings->mTTL = 1;
    }
    if (inSettings->mTTL > 0) {
        // set TTL
        int val = inSettings->mTTL;
        if ( !isIPV6(inSettings) ) {
        /* IPERF_MODIFIED Start */
        int rc = iperf_setsockopt( inSettings->mSock, IPPROTO_IP, IP_MULTICAST_TTL,
                                   (char*) &val, (Socklen_t) sizeof(val));
        /* IPERF_MODIFIED End */

        WARN_errno( rc == SOCKET_ERROR, "multicast v4 ttl" );
        } else
#  ifdef HAVE_IPV6_MULTICAST
        {
        /* IPERF_MODIFIED Start */
        int rc = iperf_setsockopt( inSettings->mSock, IPPROTO_IPV6, IPV6_MULTICAST_HOPS,
                                   (char*) &val, (Socklen_t) sizeof(val));
        /* IPERF_MODIFIED End */
        WARN_errno( rc == SOCKET_ERROR, "multicast v6 ttl" );
        }
#  else
        FAIL_errno(1, "v6 multicast not supported", inSettings);
#  endif
    }
#endif
    } else if (inSettings->mTTL > 0) {
    int val = inSettings->mTTL;
    /* IPERF_MODIFIED Start */
    int rc = iperf_setsockopt( inSettings->mSock, IPPROTO_IP, IP_TTL,
                               (char*) &val, (Socklen_t) sizeof(val));
    /* IPERF_MODIFIED End */
    WARN_errno( rc == SOCKET_ERROR, "v4 ttl" );
    }

#ifdef IP_TOS
#if HAVE_DECL_IPV6_TCLASS && ! defined HAVE_WINSOCK2_H
    // IPV6_TCLASS is defined on Windows but not implemented.
    if (isIPV6(inSettings)) {
    const int dscp = inSettings->mTOS;
        /* IPERF_MODIFIED Start */
        int rc = iperf_setsockopt(inSettings->mSock, IPPROTO_IPV6, IPV6_TCLASS, (char*) &dscp, sizeof(dscp));
        /* IPERF_MODIFIED End */
        WARN_errno( rc == SOCKET_ERROR, "setsockopt IPV6_TCLASS" );
    } else
#endif
    // set IP TOS (type-of-service) field
    if ( inSettings->mTOS > 0 ) {
        int  tos = inSettings->mTOS;
        /* IPERF_MODIFIED Start */
        if ( tos == -1 )
        {
            tos = 0; // best effort
            WARN_errno( SOCKET_ERROR, ( "Failed to get available IP_TOS.\n" ) );
        }
        /* IPERF_MODIFIED End */
        Socklen_t len = sizeof(tos);
        /* IPERF_MODIFIED Start */
        int rc = iperf_setsockopt( inSettings->mSock, IPPROTO_IP, IP_TOS,
                             (char*) &tos, len );
        /* IPERF_MODIFIED End */
        WARN_errno( rc == SOCKET_ERROR, "setsockopt IP_TOS" );
    }
#endif

    if ( !isUDP( inSettings ) ) {
        // set the TCP maximum segment size
        setsock_tcp_mss( inSettings->mSock, inSettings->mMSS );

#ifdef TCP_NODELAY

        // set TCP nodelay option
        if ( isNoDelay( inSettings ) ) {
            int nodelay = 1;
            Socklen_t len = sizeof(nodelay);
            /* IPERF_MODIFIED Start */
            int rc = iperf_setsockopt( inSettings->mSock, IPPROTO_TCP, TCP_NODELAY,
                                 (char*) &nodelay, len );
            /* IPERF_MODIFIED End */
            WARN_errno( rc == SOCKET_ERROR, "setsockopt TCP_NODELAY" );
        }
#endif
    }

#if HAVE_DECL_SO_MAX_PACING_RATE
    /* If socket pacing is specified try to enable it. */
    if (isFQPacing(inSettings) && inSettings->mFQPacingRate > 0) {
        /* IPERF_MODIFIED Start */
        int rc = iperf_setsockopt(inSettings->mSock, SOL_SOCKET, SO_MAX_PACING_RATE, &inSettings->mFQPacingRate, sizeof(inSettings->mFQPacingRate));
        /* IPERF_MODIFIED End */
        WARN_errno( rc == SOCKET_ERROR, "setsockopt SO_MAX_PACING_RATE" );
    }
#endif /* HAVE_SO_MAX_PACING_RATE */
}

void SetSocketOptionsSendTimeout( thread_Settings *mSettings, int timer) {
    if (timer > 0) {
#ifdef WIN32
    // Windows SO_SNDTIMEO uses ms
    DWORD timeout = (double) timer / 1e3;
#else
    struct timeval timeout;
    timeout.tv_sec = timer / 1000000;
    timeout.tv_usec = timer % 1000000;
#endif
    /* IPERF_MODIFIED Start */
#ifdef COMPONENT_55900
    /* On 55900 devices the TX queue length is not configurable.
     * Hence on slower networks, there is a chance of TX queue overflow.
     * So set higher send timeout */
    if(timeout.tv_sec < 1)
    {
        timeout.tv_sec = 3;
    }
#endif
    if (iperf_setsockopt( mSettings->mSock, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0 ) {
        WARN_errno( mSettings->mSock == SO_SNDTIMEO, "socket" );
    }
    /* IPERF_MODIFIED End */
     }
}


// end SetSocketOptions
